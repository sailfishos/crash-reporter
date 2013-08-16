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
                                     const QString &summary, const QString &body) :
  id(0), category(eventType), timeout(0), timerId(0)
{
    QDBusPendingReply<quint32> reply = sendDBusNotify(summary, body);
    callWatcher = new QDBusPendingCallWatcher(reply, this);
}

// ----------------------------------------------------------------------------
// CReporterNotificationPrivate::~CReporterNotificationPrivate
// ----------------------------------------------------------------------------
CReporterNotificationPrivate::~CReporterNotificationPrivate() {}

QDBusPendingReply<quint32>
CReporterNotificationPrivate::sendDBusNotify(const QString &summary,
                                             const QString &body)
{
    QDBusMessage message =
            QDBusMessage::createMethodCall("org.freedesktop.Notifications",
                    "/org/freedesktop/Notifications",
                    "org.freedesktop.Notifications", "Notify");

    QVariantHash hints;
    hints.insert("category", category);
    hints.insert("x-nemo-preview-summary", summary);
    hints.insert("x-nemo-preview-body", body);

    message.setArguments(QVariantList()
            << QString() << id << QString() << summary << body << QStringList()
            << hints << -1);

    qDebug() << __PRETTY_FUNCTION__
             << "Sending Notify for notification" << id
             << "of category" << category
             << "with summary" << summary << "and body" << body;

    return QDBusConnection::sessionBus().asyncCall(message);
}

void CReporterNotificationPrivate::retrieveNotificationId()
{
    if (callWatcher == 0)
        return;

    callWatcher->waitForFinished();

    QDBusPendingReply<quint32> reply = *callWatcher;
    callWatcher->deleteLater();
    callWatcher = 0;

    if (reply.isValid()) {
        id = reply.argumentAt<0>();
        qDebug() << __PRETTY_FUNCTION__
                 << "Create notification with id: " << id;
    } else if (reply.isError()) {
        QDBusError error = reply.error();
        qDebug() << __PRETTY_FUNCTION__ << "Failed to create notification: "
                 << error.name() << " - " << error.message();
    }
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
                                             QObject *parent) :
    QObject(parent),
    d_ptr(new CReporterNotificationPrivate(eventType, summary, body))
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
    Q_D(CReporterNotification);

    d->retrieveNotificationId();

    if (d->id != 0) {
        d->sendDBusNotify(summary, body);
    } else {
        qDebug() << __PRETTY_FUNCTION__ << "Couldn't update notification.";
    }
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
