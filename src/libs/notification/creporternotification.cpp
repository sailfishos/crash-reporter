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

// System includes.

#include <MNotification>
#include <MRemoteAction>
#include <QDebug>

// User includes.

#include "creporternotification.h"
#include "creporternotification_p.h"
#include "creporternotificationadaptor.h"

// ======== Class CReporterNotificationPrivate ========

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterNotificationPrivate::CReporterNotificationPrivate
// ----------------------------------------------------------------------------
CReporterNotificationPrivate::CReporterNotificationPrivate(const QString &eventType,
                                     const QString &summary, const QString &body,
                                     const QString &imageName) :
   timeout(0), timerId(0)
{

    new CReporterNotificationAdaptor(this);

    // Register to unique path.
    QString uniqueId = QUuid::createUuid().toString();
    uniqueId = uniqueId.remove('{').remove('}').remove('-');
    QString objPath = QString(CREPORTER_DBUS_NTF_OBJECT_PATH) + uniqueId;
    QDBusConnection::sessionBus().registerObject(objPath, this);

    action = new MRemoteAction(QDBusConnection::sessionBus().baseService(),
                               objPath, CREPORTER_DBUS_NTF_INTERFACE,
                               "activate", QList<QVariant>(), this);

    // Create MNotification.
    notification = new MNotification(eventType, summary, body);
    notification->setImage(imageName);
    notification->setAction(*action);
    notification->publish();
}

// ----------------------------------------------------------------------------
// CReporterNotificationPrivate::~CReporterNotificationPrivate
// ----------------------------------------------------------------------------
CReporterNotificationPrivate::~CReporterNotificationPrivate()
{
    notification->remove();
    delete notification;
    notification = 0;
}

// ----------------------------------------------------------------------------
// CReporterNotificationPrivate::activate
// ----------------------------------------------------------------------------
void CReporterNotificationPrivate::activate()
{
    emit q_ptr->activated();
}

// ----------------------------------------------------------------------------
// CReporterNotificationPrivate::timerEvent
// ----------------------------------------------------------------------------
void CReporterNotificationPrivate::timerEvent(QTimerEvent *event)
{
    if (event->timerId() != timerId) return;

    // kill timer and remove the notification.
    killTimer(timerId);
    timerId = 0;
    timeout = 0;
    notification->remove();

    emit q_ptr->timeouted();
}

// ======== Class CReporterNotification ========

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterNotification::CReporterNotification
// ----------------------------------------------------------------------------
CReporterNotification::CReporterNotification(const QString &eventType,
                                             const QString &summary, const QString &body,
                                             const QString &imageName, QObject *parent) :
    QObject(parent),
    d_ptr(new CReporterNotificationPrivate(eventType, summary, body, imageName))
{
    d_ptr->q_ptr = this;
}

// ----------------------------------------------------------------------------
// CReporterNotification::~CReporterNotification
// ----------------------------------------------------------------------------
CReporterNotification::~CReporterNotification()
{
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterNotification::operator==
// ----------------------------------------------------------------------------
bool CReporterNotification::operator==(const CReporterNotification &other) const
{
    return (d_ptr == other.d_ptr);
}

// ----------------------------------------------------------------------------
// CReporterNotification::update
// ----------------------------------------------------------------------------
void CReporterNotification::update(const QString &summary, const QString &body)
{
    if (!summary.isEmpty()) {
        d_ptr->notification->setSummary(summary);
    }

    if (!body.isEmpty()) {
        d_ptr->notification->setBody(body);
    }

    d_ptr->notification->publish();
}

// ----------------------------------------------------------------------------
// CReporterNotification::remove
// ----------------------------------------------------------------------------
void CReporterNotification::remove()
{
    d_ptr->notification->remove();
    if (d_ptr->timerId != 0) {
        // Remove existing timeout, if notification was removed.
        d_ptr->killTimer(d_ptr->timerId);
        d_ptr->timerId = 0;
    }
}

// ----------------------------------------------------------------------------
// CReporterNotification::setTimeout
// ----------------------------------------------------------------------------
void CReporterNotification::setTimeout(int timeout)
{
    d_ptr->timeout = timeout;

    if (d_ptr->timerId != 0) {
        // Remove previous timeout.
        d_ptr->killTimer(d_ptr->timerId);
        d_ptr->timerId = 0;
    }

    if (d_ptr->timeout > 0) {
        d_ptr->timerId = d_ptr->startTimer(d_ptr->timeout * 1000);
    }
}

// ----------------------------------------------------------------------------
// CReporterNotification::timeout
// ----------------------------------------------------------------------------
int CReporterNotification::timeout() const
{
    return d_ptr->timeout;
}

// ----------------------------------------------------------------------------
// CReporterNotification::isPublished
// ----------------------------------------------------------------------------
bool CReporterNotification::isPublished() const
{
    return d_ptr->notification->isPublished();
}

// ----------------------------------------------------------------------------
// CReporterNotification::removeAll()
// ----------------------------------------------------------------------------
void CReporterNotification::removeAll()
{
    QList <MNotification*> notifications = MNotification::notifications();

    foreach (MNotification* ntf, notifications)
    {
        // this is prone to crashing so we just try and don't care
        try
        {
            ntf->remove();
        }
        catch (...)
        {
        }
    }
    qDeleteAll(notifications);

}

// End of file.
