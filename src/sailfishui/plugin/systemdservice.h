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

class SystemdServicePrivate;

class SystemdService: public QObject {
    Q_OBJECT

    Q_PROPERTY(bool running READ running WRITE setRunning NOTIFY runningChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(bool masked READ masked WRITE setMasked NOTIFY maskedChanged)

public:
    SystemdService(const QString& serviceName);

    bool running() const;
    void setRunning(bool state);

    bool enabled() const;
    void setEnabled(bool state);

    bool masked() const;
    void setMasked(bool state);

    ~SystemdService();
signals:
    void runningChanged();
    void enabledChanged();
    void maskedChanged();

private:
    Q_DISABLE_COPY(SystemdService)
    Q_DECLARE_PRIVATE(SystemdService)
    QScopedPointer<SystemdServicePrivate> d_ptr;

    Q_PRIVATE_SLOT(d_func(), void gotUnitPath(QDBusPendingCallWatcher *));
    Q_PRIVATE_SLOT(d_func(), void unitFileStateChanged(QDBusPendingCallWatcher *));
    Q_PRIVATE_SLOT(d_func(), void maskingChanged(QDBusPendingCallWatcher *));
    Q_PRIVATE_SLOT(d_func(), void reloaded(QDBusPendingCallWatcher *));
    Q_PRIVATE_SLOT(d_func(), void stateChanged(QDBusPendingCallWatcher *));
    Q_PRIVATE_SLOT(d_func(), void propertiesChanged(const QString &,
                                                    const QVariantMap &,
                                                    const QStringList &));
};

#endif // SYSTEMDSERVICE_H
