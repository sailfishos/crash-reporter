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

#include <QQmlParserStatus>

class SystemdServicePrivate;

class SystemdService: public QObject, public QQmlParserStatus {
    Q_OBJECT

    Q_ENUMS(State)
    Q_ENUMS(ManagerType)

    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QString serviceName READ serviceName WRITE setServiceName NOTIFY serviceNameChanged)
    Q_PROPERTY(ManagerType managerType READ managerType WRITE setManagerType NOTIFY managerTypeChanged)
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool masked READ masked WRITE setMasked NOTIFY maskedChanged)

public:
    enum ManagerType {
        SystemManager,
        UserManager
    };

    enum State {
        Inactive,
        Activating,
        Active,
        Deactivating
    };

    SystemdService(QObject *parent = 0);
    ~SystemdService();

    QString serviceName() const;
    void setServiceName(const QString& serviceName);

    ManagerType managerType() const;
    void setManagerType(ManagerType managerType);

    State state() const;

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();

    bool enabled() const;
    void setEnabled(bool state);

    bool masked() const;
    void setMasked(bool state);

    void classBegin();
    void componentComplete();

signals:
    void serviceNameChanged();
    void managerTypeChanged();
    void stateChanged();
    void enabledChanged();
    void maskedChanged();

private:
    Q_DISABLE_COPY(SystemdService)
    Q_DECLARE_PRIVATE(SystemdService)
    QScopedPointer<SystemdServicePrivate> d_ptr;

    Q_PRIVATE_SLOT(d_func(), void gotUnitPath(QDBusPendingCallWatcher *))
    Q_PRIVATE_SLOT(d_func(), void unitFileStateChanged(QDBusPendingCallWatcher *))
    Q_PRIVATE_SLOT(d_func(), void maskingChanged(QDBusPendingCallWatcher *))
    Q_PRIVATE_SLOT(d_func(), void reloaded(QDBusPendingCallWatcher *))
    Q_PRIVATE_SLOT(d_func(), void stateChanged(QDBusPendingCallWatcher *))
    Q_PRIVATE_SLOT(d_func(), void propertiesChanged(const QString &,
                                                    const QVariantMap &,
                                                    const QStringList &))
    Q_PRIVATE_SLOT(d_func(), void handleUnitNew(const QString &, const QDBusObjectPath &))
    Q_PRIVATE_SLOT(d_func(), void handleUnitRemoved(const QString &, const QDBusObjectPath &))
    Q_PRIVATE_SLOT(d_func(), void checkUnitState())
};

#endif // SYSTEMDSERVICE_H
