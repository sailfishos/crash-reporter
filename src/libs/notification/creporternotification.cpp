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

// System includes.

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

    // TODO: Re-implement notifications for Sailfish
    Q_UNUSED(eventType);
    Q_UNUSED(summary);
    Q_UNUSED(body);
    Q_UNUSED(imageName);
}

// ----------------------------------------------------------------------------
// CReporterNotificationPrivate::~CReporterNotificationPrivate
// ----------------------------------------------------------------------------
CReporterNotificationPrivate::~CReporterNotificationPrivate() {}

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

    // Removing a notification tends to fail when dbus is not
    // up and running (in special cases) and apparently this will not be fixed in MeeGo Touch.
    // When dbus is not available the system is most likely being shut down and we'll just ignore
    // this here to avoid a core dump.
    try
    {
        // TODO: Re-implement reaction on timeout for Sailfish
    }
    catch (...)
    {
        // Do nothing
    }

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
	// TODO: Re-implement for Sailfish
}

// ----------------------------------------------------------------------------
// CReporterNotification::remove
// ----------------------------------------------------------------------------
void CReporterNotification::remove()
{
    // TODO: Re-implement notification removal for Sailfish

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
	// TODO: Re-implement for Sailfish
	return true;
}

// ----------------------------------------------------------------------------
// CReporterNotification::removeAll()
// ----------------------------------------------------------------------------
void CReporterNotification::removeAll()
{
	// TODO: Re-implement for Sailfish
}

// End of file.
