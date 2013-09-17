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

#include "propertiesproxy.h"
#include "systemdmanagerproxy.h"
#include "systemdunitproxy.h"

class SystemdServicePrivate {
public:
    QString serviceName;
    SystemdManagerProxy *manager;
    SystemdUnitProxy *unit;

    void gotUnitPath(QDBusPendingCallWatcher *call);
    void propertiesChanged(const QString &interface,
                           const QVariantMap &changedProperties,
                           const QStringList &invalidatedProperties);
    void unitFileStateChanged(QDBusPendingCallWatcher *call);
    void maskingChanged(QDBusPendingCallWatcher *call);
    void reloaded(QDBusPendingCallWatcher *call);
    void stateChanged(QDBusPendingCallWatcher *call);

private:
    Q_DECLARE_PUBLIC(SystemdService)
    SystemdService *q_ptr;
};

void SystemdServicePrivate::gotUnitPath(QDBusPendingCallWatcher *call) {
    Q_Q(SystemdService);

    QDBusPendingReply<QDBusObjectPath> reply = *call;
    if (reply.isError()) {
        qDebug() << "Failed to get crash-reporter.service DBus path"
                 << reply.error().name() << reply.error().message();
    } else {
        QString path = reply.argumentAt<0>().path();

        unit = new SystemdUnitProxy("org.freedesktop.systemd1", path,
                QDBusConnection::sessionBus(), q);

        PropertiesProxy *crashReporterProperties = new PropertiesProxy(
                "org.freedesktop.systemd1", path, QDBusConnection::sessionBus(),
                q);

        QObject::connect(crashReporterProperties,SIGNAL(PropertiesChanged(const QString &, const QVariantMap &, const QStringList &)),
                         q, SLOT(propertiesChanged(const QString &, const QVariantMap &, const QStringList &)));

        /* Before we create a unit proxy, we 'guess' service is not running. Now
         * when we can actually query its status, notify about the change if our
         * assumption was wrong. Same applies to enabled state and masking. */
        if (q->running() != false) {
            emit q->runningChanged();
        }
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

    if (interface != SystemdUnitProxy::staticInterfaceName())
        return;

    Q_ASSERT(unit);

    if (changedProperties.contains("ActiveState") ||
        invalidatedProperties.contains("ActiveState")) {
        qDebug() << "ActiveState changed to:" << unit->activeState();
        emit q->runningChanged();
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
    if (changedProperties.contains("NeedDaemonReload") ||
        invalidatedProperties.contains("NeedDaemonReload")) {
        QDBusPendingCallWatcher *watcher =
                new QDBusPendingCallWatcher(manager->Reload(), q);

        QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
                         q, SLOT(reloaded(QDBusPendingCallWatcher *)));
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
    }

    call->deleteLater();
}

void SystemdServicePrivate::reloaded(QDBusPendingCallWatcher *call) {
    Q_Q(SystemdService);

    QDBusPendingCall reply = *call;
    if (reply.isError()) {
        qDebug() << "Couldn't reload a unit file"
                 << reply.error().name() << reply.error().message();
    } else {
        /* Seems systemd won't notify when UnitFileState or LoadState
         * properties change. We have to emit our change notifications
         * ourselves. */
        emit q->enabledChanged();
        emit q->maskedChanged();
    }

    call->deleteLater();
}

SystemdService::SystemdService(const QString& serviceName):
  d_ptr(new SystemdServicePrivate) {
    Q_D(SystemdService);
    d->q_ptr = this;
    d->serviceName = serviceName;

    d->manager = new SystemdManagerProxy("org.freedesktop.systemd1",
            "/org/freedesktop/systemd1", QDBusConnection::sessionBus(), this);
    d->unit = 0;

    qDBusRegisterMetaType<UnitFileChange>();
    qDBusRegisterMetaType<QList<UnitFileChange> >();

    // Calling subscribe allows us to receive DBus signals from systemd
    QDBusPendingCall reply = d->manager->Subscribe();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "Couldn't subscribe to systemd manager";
    }

    reply = d->manager->GetUnit(d->serviceName);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher *)),
            this, SLOT(gotUnitPath(QDBusPendingCallWatcher *)));
}

bool SystemdService::running() const {
    Q_D(const SystemdService);

    return (d->unit && d->unit->activeState() == "active");
}

void SystemdService::setRunning(bool state) {
    Q_D(SystemdService);

    if (running() == state)
        return;

    QDBusPendingCallWatcher *watcher;

    if (!d->unit) {
        qDebug() << "Systemd unit proxy not initialized!";
        return;
    }

    if (state) {
        watcher = new QDBusPendingCallWatcher(
                d->unit->Start("replace"), this);
    } else {
        watcher = new QDBusPendingCallWatcher(
                d->unit->Stop("replace"), this);
    }

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
