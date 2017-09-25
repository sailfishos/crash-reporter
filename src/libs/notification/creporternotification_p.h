/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Riku Halonen <riku.halonen@nokia.com>
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
 *
 */

#ifndef CREPORTERNOTIFICATION_P_H
#define CREPORTERNOTIFICATION_P_H

#include <QDBusPendingCall>

/*!
  * @def CREPORTER_DBUS_NTF_OBJECT_PATH
  * D-Bus object path for the notification.
  */
#define CREPORTER_DBUS_NTF_OBJECT_PATH  "/com/nokia/crashreporter/notification/"

/*!
  * @def CREPORTER_DBUS_NTF_INTERFACE
  * D-Bus interface name for the notification.
  */
#define CREPORTER_DBUS_NTF_INTERFACE    "com.nokia.CrashReporter.Notification"

// Forward declarations.

class CReporterNotification;
class OrgFreedesktopNotificationsInterface;

/*!
  * @class CReporterNotificationPrivate
  * @brief Private implementation of CReporterNotification.
  *
  * @sa CReporterNotification
  */
class CReporterNotificationPrivate : public QObject
{
    Q_OBJECT

public:

    /**
     * @brief Class constructor.
     *
     * @param eventType Type of the notification.
     * @param q Owner CReporterNotification instance.
     */
    CReporterNotificationPrivate(const QString &eventType, CReporterNotification *q);

    /*!
      * @brief Class destructor.
      *
      */
    virtual ~CReporterNotificationPrivate();

    void sendDBusNotify();
    void retrieveNotificationId();
    void removeAfterTimeout();

private slots:
    void onNotificationRemoved(quint32 id);

public:
    //! Id of the notification assigned by the notification manager.
    quint32 id;
    //! Notification's expiration time in milliseconds.
    int timeout;
    //! Pending reply on the notification creation from the notification manager.
    QDBusPendingCallWatcher *callWatcher;
    //! Category of the notification, e.g. "x-nemo.crash-reporter.autouploader".
    QString category;
    //! Notification's summary string.
    QString summary;
    //! Notification's body.
    QString body;
    /**
     * Number of events this notification represents, i.e.
     * "x-nemo-item-count" hint.
     */
    int count;
    //! DBus proxy of org.freedesktop.Notification
    OrgFreedesktopNotificationsInterface *proxy;

private:
    Q_DECLARE_PUBLIC(CReporterNotification)
    //! @arg Pointer to public class.
    CReporterNotification *q_ptr;
};

#endif // CREPORTERNOTIFICATION_P_H

// End of file.
