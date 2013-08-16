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

// System includes.

#include <QObject>
#include <QString>
#include <QVariant>

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

    /*!
      * @brief Class constructor.
      *
      * @param eventType Type of the notification.
      * @param summary Summary text to be used in the notification. Defaults to no summary text.
      * @param body Body text to be used in the notification. Defaults to no body text.
      * @param action Remote action to invoke. Uses default action if 0
      */
        CReporterNotificationPrivate(const QString &eventType,
                                     const QString &summary = QString(),
                                     const QString &body = QString());

        /*!
          * @brief Class destructor.
          *
          */
        virtual ~CReporterNotificationPrivate();

    public Q_SLOTS:

        /*!
          * @brief Called to emit activated signal.
          *
          */
        void activate();

    protected:
        /*!
          * @brief Called, when timer elapses.
          *
          * @param event Pointer to timer event.
          */
        virtual void timerEvent(QTimerEvent *event);

    public:
        //! @arg Timeout value.
        int timeout;
        //! @arg Value for identifying elapsed timer.
        int timerId;
        //! @arg Pointer to public class.
        CReporterNotification *q_ptr;
};

#endif // CREPORTERNOTIFICATION_P_H

 // End of file.
