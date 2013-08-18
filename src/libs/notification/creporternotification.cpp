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
  id(0), category(eventType)
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
