/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Jakub Adam <jakub.adam@jollamobile.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

#include "systemdservice.h"

#include "unitfilechange.h"
#include "manager_interface.h" // generated
#include "manager_interface.cpp" // generated
#include "moc_manager_interface.cpp" // generated
#include "properties_interface.h" // generated
#include "unit_interface.h" // generated

class SystemdServicePrivate {
public:
    QString serviceName;
    OrgFreedesktopSystemd1ManagerInterface *manager;
    OrgFreedesktopSystemd1UnitInterface *unit;

    void gotUnitPath(QDBusPendingCallWatcher *call);
    void propertiesChanged(const QString &interface,
                           const QVariantMap &changedProperties,
                           const QStringList &invalidatedProperties);
    void unitFileStateChanged(QDBusPendingCallWatcher *call);
    void maskingChanged(QDBusPendingCallWatcher *call);
    void stateChanged(QDBusPendingCallWatcher *call);

    SystemdService::State state;

private:
    Q_DECLARE_PUBLIC(SystemdService)
    SystemdService *q_ptr;

    void reload();
    void reloaded(QDBusPendingCallWatcher *call);

    void changeState(const QString &state);
};

void SystemdServicePrivate::gotUnitPath(QDBusPendingCallWatcher *call) {
    Q_Q(SystemdService);

    QDBusPendingReply<QDBusObjectPath> reply = *call;
    if (reply.isError()) {
        qDebug() << "Failed to get crash-reporter.service DBus path"
                 << reply.error().name() << reply.error().message();
    } else {
        QString path = reply.argumentAt<0>().path();

        unit = new OrgFreedesktopSystemd1UnitInterface("org.freedesktop.systemd1",
                path, QDBusConnection::sessionBus(), q);

        OrgFreedesktopDBusPropertiesInterface *crashReporterProperties =
                new OrgFreedesktopDBusPropertiesInterface("org.freedesktop.systemd1",
                        path, QDBusConnection::sessionBus(), q);

        QObject::connect(crashReporterProperties,SIGNAL(PropertiesChanged(const QString &, const QVariantMap &, const QStringList &)),
                         q, SLOT(propertiesChanged(const QString &, const QVariantMap &, const QStringList &)));

        /* Before we create a unit proxy, we 'guess' service is not running. Now
         * when we can actually query its status, notify about the change if our
         * assumption was wrong. Same applies to enabled state and masking. */
        changeState(unit->activeState());
        if (q->enabled() != false) {
            emit q->enabledChanged();
        }
        if (q->masked() != false) {
            emit q->maskedChanged();
        }
    }

    call->deleteLater();
}

void SystemdServicePrivate::propertiesChanged(const QString &interface,
                                              const QVariantMap &changedProperties,
                                              const QStringList &invalidatedProperties) {
    Q_Q(SystemdService);

    if (interface != OrgFreedesktopSystemd1UnitInterface::staticInterfaceName())
        return;

    Q_ASSERT(unit);

    if (changedProperties.contains("ActiveState") ||
        invalidatedProperties.contains("ActiveState")) {
        QString state = unit->activeState();
        qDebug() << "ActiveState changed to:" << state;
        changeState(state);
    }
    if (changedProperties.contains("UnitFileState") ||
        invalidatedProperties.contains("UnitFileState")) {
        qDebug() << "UnitFileState changed to:" << unit->unitFileState();
        emit q->enabledChanged();
    }
    if (changedProperties.contains("LoadState") ||
        invalidatedProperties.contains("LoadState")) {
        qDebug() << "LoadState changed to:" << unit->loadState();
        emit q->maskedChanged();
    }
}

void SystemdServicePrivate::stateChanged(QDBusPendingCallWatcher *call) {
    QDBusPendingReply<QDBusObjectPath> reply = *call;
    if (reply.isError()) {
        qDebug() << "Couldn't change systemd service state"
                 << reply.error().name() << reply.error().message();
    }

    call->deleteLater();
}

void SystemdServicePrivate::unitFileStateChanged(QDBusPendingCallWatcher *call) {
    QDBusPendingCall reply = *call;
    if (reply.isError()) {
        qDebug() << "Couldn't enable or disable a unit file"
                 << reply.error().name() << reply.error().message();
    }

    call->deleteLater();
}

void SystemdServicePrivate::maskingChanged(QDBusPendingCallWatcher *call) {
    QDBusPendingCall reply = *call;
    if (reply.isError()) {
        qDebug() << "Couldn't mask or unmask a unit file"
                 << reply.error().name() << reply.error().message();
    } else {
        reload();
    }

    call->deleteLater();
}

void SystemdServicePrivate::reload() {
    Q_Q(SystemdService);

    QDBusPendingCallWatcher *watcher =
            new QDBusPendingCallWatcher(manager->Reload(), q);

    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
                     q, SLOT(reloaded(QDBusPendingCallWatcher *)));
}

void SystemdServicePrivate::reloaded(QDBusPendingCallWatcher *call) {
    Q_Q(SystemdService);

    QDBusPendingCall reply = *call;
    if (reply.isError()) {
        qDebug() << "Couldn't reload a unit file"
                 << reply.error().name() << reply.error().message();
    } else {
        /* Seems systemd won't notify when LoadState property changes.
         * We have to emit our change notifications ourselves. */
        emit q->maskedChanged();
    }

    call->deleteLater();
}

void SystemdServicePrivate::changeState(const QString &state) {
    Q_Q(SystemdService);

    SystemdService::State newState;
    if (state == "active" || state == "reloading" || state == "deactivating") {
        newState = SystemdService::Active;
    } else if (state == "activating") {
        newState = SystemdService::Activating;
    } else /* "inactive", "failed", default */ {
        newState = SystemdService::Inactive;
    }

    if (this->state != newState) {
        this->state = newState;
        emit q->stateChanged();
    }
}

SystemdService::SystemdService(const QString& serviceName):
  d_ptr(new SystemdServicePrivate) {
    Q_D(SystemdService);
    d->q_ptr = this;
    d->serviceName = serviceName;
    d->state = Inactive;

    d->manager = new OrgFreedesktopSystemd1ManagerInterface("org.freedesktop.systemd1",
            "/org/freedesktop/systemd1", QDBusConnection::sessionBus(), this);
    d->unit = 0;

    /* Ensure systemd configuration is up to date with unit files, for example
     * after change by package update. */
    QDBusPendingCall reply = d->manager->Reload();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "Couldn't reload systemd unit files";
    }

    qDBusRegisterMetaType<UnitFileChange>();
    qDBusRegisterMetaType<QList<UnitFileChange> >();

    // Calling subscribe allows us to receive DBus signals from systemd
    reply = d->manager->Subscribe();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "Couldn't subscribe to systemd manager";
    }

    reply = d->manager->LoadUnit(d->serviceName);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
            this, SLOT(gotUnitPath(QDBusPendingCallWatcher *)));
}

SystemdService::State SystemdService::state() const {
    Q_D(const SystemdService);

    return d->state;
}

void SystemdService::start() {
    Q_D(SystemdService);

    if (!d->unit) {
        qDebug() << "Systemd unit proxy not initialized!";
        return;
    }

    if (state() == Active || state() == Activating)
        return;

    QDBusPendingCallWatcher *watcher =
            new QDBusPendingCallWatcher(d->unit->Start("replace"), this);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
            this, SLOT(stateChanged(QDBusPendingCallWatcher *)));
}

void SystemdService::stop() {
    Q_D(SystemdService);

    if (!d->unit) {
        qDebug() << "Systemd unit proxy not initialized!";
        return;
    }

    if (state() == Inactive)
        return;

    QDBusPendingCallWatcher *watcher =
            new QDBusPendingCallWatcher(d->unit->Stop("replace"), this);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
            this, SLOT(stateChanged(QDBusPendingCallWatcher *)));
}

bool SystemdService::enabled() const {
    Q_D(const SystemdService);

    return (d->unit && d->unit->unitFileState() == "enabled");
}

void SystemdService::setEnabled(bool state) {
    Q_D(SystemdService);


    QDBusPendingCallWatcher *watcher;

    if (!d->unit) {
        qDebug() << "Systemd unit proxy not initialized!";
        return;
    }

    QStringList services;
    services.append(d->serviceName);

    if (state) {
        watcher = new QDBusPendingCallWatcher(
                d->manager->EnableUnitFiles(services, false, true), this);
    } else {
        watcher = new QDBusPendingCallWatcher(
                d->manager->DisableUnitFiles(services, false), this);
    }

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
            this, SLOT(unitFileStateChanged(QDBusPendingCallWatcher *)));
}

void SystemdService::setMasked(bool state) {
    Q_D(SystemdService);

    if (masked() == state)
        return;

    QDBusPendingCallWatcher *watcher;

    if (!d->unit) {
        qDebug() << "Systemd unit proxy not initialized!";
        return;
    }

    QStringList services;
    services.append(d->serviceName);

    if (state) {
        watcher = new QDBusPendingCallWatcher(
                d->manager->MaskUnitFiles(services, false, true), this);
    } else {
        watcher = new QDBusPendingCallWatcher(
                d->manager->UnmaskUnitFiles(services, false), this);
    }

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
            this, SLOT(maskingChanged(QDBusPendingCallWatcher *)));
}

bool SystemdService::masked() const {
    Q_D(const SystemdService);

    return (d->unit && d->unit->loadState() == "masked");
}

SystemdService::~SystemdService() {
    Q_D(SystemdService);

    /* Properly unsubscribe from receiving DBus signals in order to stop systemd
     * pointlessly sending them if we were the sole listener. */
    d->manager->Unsubscribe();
}

#include "moc_systemdservice.cpp"
