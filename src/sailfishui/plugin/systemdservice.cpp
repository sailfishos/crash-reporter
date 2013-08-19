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

#include <QDBusPendingCallWatcher>

#include "propertiesproxy.h"
#include "systemdmanagerproxy.h"
#include "systemdunitproxy.h"

SystemdService::SystemdService(const QString& serviceName):
  manager(new SystemdManagerProxy("org.freedesktop.systemd1",
          "/org/freedesktop/systemd1", QDBusConnection::sessionBus(), this)),
  unit(0) {
    // Calling subscribe allows us to receive DBus signals from systemd
    QDBusPendingCall reply = manager->Subscribe();
    reply.waitForFinished();
    if (reply.isError()) {
        qDebug() << "Couldn't subscribe to systemd manager";
    }

    reply = manager->GetUnit(serviceName);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &SystemdService::gotUnitPath);
}

void SystemdService::gotUnitPath(QDBusPendingCallWatcher *call) {
    QDBusPendingReply<QDBusObjectPath> reply = *call;
    if (reply.isError()) {
        qDebug() << "Failed to get crash-reporter.service DBus path"
                 << reply.error().name() << reply.error().message();
    } else {
        QString path = reply.argumentAt<0>().path();

        unit = new SystemdUnitProxy("org.freedesktop.systemd1",
                path, QDBusConnection::sessionBus(), this);

        PropertiesProxy *crashReporterProperties = new PropertiesProxy(
                "org.freedesktop.systemd1", path, QDBusConnection::sessionBus(),
                this);

        connect(crashReporterProperties, &PropertiesProxy::PropertiesChanged,
                this, &SystemdService::propertiesChanged);

        /* Before we create a unit proxy, we 'guess' service is not running. Now
         * when we can actually query its status, notify about the change if our
         * assumption was wrong. */
        if (running() != false) {
            emit runningChanged();
        }
    }

    call->deleteLater();
}

void SystemdService::propertiesChanged(const QString &interface,
                                       const QVariantMap &changedProperties,
                                       const QStringList &invalidatedProperties) {
    if (interface != SystemdUnitProxy::staticInterfaceName())
        return;

    changedProperties.contains("ActiveState");
    QVariant state(changedProperties.value("ActiveState"));
    if (!state.isValid()) {
        if (!invalidatedProperties.contains("ActiveState"))
            return;
    }

    Q_ASSERT(unit);
    qDebug() << "State changed to:" << unit->activeState();

    emit runningChanged();
}

bool SystemdService::running() const {
    return (unit && unit->activeState() == "active");
}

void SystemdService::setRunning(bool state) {
    QDBusPendingCallWatcher *watcher;

    if (!unit) {
        qDebug() << "Systemd unit proxy not initialized!";
        return;
    }

    if (state) {
        watcher = new QDBusPendingCallWatcher(
                unit->Start("replace"), this);
    } else {
        watcher = new QDBusPendingCallWatcher(
                unit->Stop("replace"), this);
    }

    connect(watcher, &QDBusPendingCallWatcher::finished,
            this, &SystemdService::stateChanged);
}

void SystemdService::stateChanged(QDBusPendingCallWatcher *call) {
    QDBusPendingReply<QDBusObjectPath> reply = *call;
    if (reply.isError()) {
        qDebug() << "Couldn't change systemd service state"
                 << reply.error().name() << reply.error().message();
    }

    call->deleteLater();
}

SystemdService::~SystemdService() {
    /* Properly unsubscribe from receiving DBus signals in order to stop systemd
     * pointlessly sending them if we were the sole listener. */
    manager->Unsubscribe();
}
