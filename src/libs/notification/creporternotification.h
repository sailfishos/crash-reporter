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
     * Creates new CReporterNotification to represent an existing operating
     * system notification element.
     *
     * Use this constructor if you want the object to reuse and update
     * existing notification already shown in OS notification area. New
     * notification element is opened only when a notification with given id
     * doesn't exist. How the notification is presented to the user is
     * system specific.
     *
     * Just creating a CReporterNotification won't trigger any update of
     * the notification element, call update() to change its summary and
     * body.
     *
     * @param eventType Type of the notification.
     * @param id Handle of existing element in system notification area.
     * @param parent Parent object.
     */
    CReporterNotification(const QString &eventType, int id,
                          QObject *parent = 0);

    /*!
      * @brief Class destructor.
      *
      */
    virtual ~CReporterNotification();

    /*!
     * @return Handle to a notification element in the operating system.
     */
    int id();

    /*!
     * Sets the period after which the notification is removed from system
     * notification area.
     *
     * @param ms Expiration timeout in milliseconds.
     */
    void setTimeout(int ms);

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
    {
        return !operator==(other);
    }

    /*!
      * @brief Updates notification.
      *
      * @param summary Summary text to be used in the notification. Defaults to no summary text.
      * @param body Body text to be used in the notification. Defaults to no body text.
      * @param count Number of events this notification represents.
      */
    void update(const QString &summary = QString(),
                const QString &body = QString(),
                int count = 1);

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

#ifndef CREPORTER_UNIT_TEST
    Q_PRIVATE_SLOT(d_func(), void removeAfterTimeout());
#endif
};

#endif // CREPORTERNOTIFICATION_H

// End of file.
