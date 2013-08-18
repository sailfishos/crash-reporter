/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Riku Halonen <riku.halonen@nokia.com>
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

#ifndef CREPORTERNOTIFICATION_H
#define CREPORTERNOTIFICATION_H

// System includes.

#include <QObject>
#include <QString>
#include <QVariant>

// User includes.

#include "creporterexport.h"

// Forward declarations.

class CReporterNotificationPrivate;

/*!
  * @class CReporterNotification
  * @brief Convenience class for showing out-of-process system notifications (MNotification).
  *
  */
class CREPORTER_EXPORT CReporterNotification : public QObject
{
    Q_OBJECT

    public:
        /*!
          * @brief Class constructor.
          *
          * @param eventType Type of the notification.
          * @param summary Summary text to be used in the notification. Defaults to no summary text.
          * @param body Body text to be used in the notification. Defaults to no body text.
          * @param parent Parent object.
          */
        CReporterNotification(const QString &eventType,
                              const QString &summary = QString(),
                              const QString &body = QString(),
                              QObject *parent = 0);

        /*!
          * @brief Class destructor.
          *
          */
        virtual ~CReporterNotification();

        /*!
          * @brief Returns true if @a other is a @e copy of this notification; otherwise returns false.
          *
          */
        bool operator==(const CReporterNotification &other) const;

        /*!
          * @brief Returns true if @a other is a @e not copy of this notification; otherwise returns false.
          *
          */
        inline bool operator!=(const CReporterNotification &other) const
        { return !operator==(other); }

        /*!
          * @brief Updates notification.
          *
          * @param summary Summary text to be used in the notification. Defaults to no summary text.
          * @param body Body text to be used in the notification. Defaults to no body text.
          */
        void update(const QString &summary = QString(),
                              const QString &body = QString());

        /*!
          * @brief Returns whether notification is published or not.
          *
          * @return True, if published; otherwise false.
          */
        bool isPublished() const;

        /*!
          * @brief Removes all notifications not dismissed.
          *
          */
        static void removeAll();

    public Q_SLOTS:

        /*!
          * @brief Removes the notification.
          *
          */
        void remove();

    Q_SIGNALS:
        /*!
         * @brief Emitted, when user interacts with the notification.
         *
         */
        void activated();

       /*!
        * @brief Emitted, when notification timeout occurred and the notification was removed.
        *
        */
        void timeouted();

    private:
        Q_DECLARE_PRIVATE(CReporterNotification);
        //! @arg Pointer to private class.
        CReporterNotificationPrivate *d_ptr;
};

#endif // CREPORTERNOTIFICATION_H

// End of file.
