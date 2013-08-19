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

#ifndef SYSTEMDSERVICE_H
#define SYSTEMDSERVICE_H

#include <QObject>

class QDBusPendingCallWatcher;
class SystemdManagerProxy;
class SystemdUnitProxy;

class SystemdService: public QObject {
    Q_OBJECT

    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)

public:
    SystemdService(const QString& serviceName);

    bool running() const;
    void setRunning(bool state);

    ~SystemdService();
signals:
    void runningChanged();

private:
    Q_DISABLE_COPY(SystemdService)

    SystemdManagerProxy *manager;
    SystemdUnitProxy *unit;

private slots:
    void gotUnitPath(QDBusPendingCallWatcher *call);
    void stateChanged(QDBusPendingCallWatcher *call);
    void propertiesChanged(const QString &interface,
                           const QVariantMap &changedProperties,
                           const QStringList &invalidatedProperties);
};

#endif // SYSTEMDSERVICE_H
