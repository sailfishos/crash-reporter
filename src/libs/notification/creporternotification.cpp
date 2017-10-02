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

#include <QDebug>

#include "../notification_interface.h" // generated
#include "creporternotification.h"
#include "creporternotification_p.h"
#include "creporterutils.h"

using CReporter::LoggingCategory::cr;

CReporterNotificationPrivate::CReporterNotificationPrivate(const QString &eventType, CReporterNotification *q)
    : id(0), timeout(-1), callWatcher(0), category(eventType),
      proxy(new OrgFreedesktopNotificationsInterface("org.freedesktop.Notifications",
                                                     "/org/freedesktop/Notifications",
                                                     QDBusConnection::sessionBus(), this)),
      q_ptr(q)
{
    connect(proxy, &OrgFreedesktopNotificationsInterface::NotificationClosed,
            this, &CReporterNotificationPrivate::onNotificationRemoved);
}

CReporterNotificationPrivate::~CReporterNotificationPrivate()
{
}

void CReporterNotificationPrivate::sendDBusNotify()
{
    QStringList actions;
    if (!body.isEmpty()) {
        //: Action name for crash reporter notifications
        //% "Show settings"
        actions << QStringLiteral("default") << qtTrId("crash_reporter-show-settings");
    }

    auto encodeArgument = [](const QString & argument) {
        QByteArray a;
        QDataStream ds(&a, QIODevice::WriteOnly);
        ds << QVariant(argument);
        return QString::fromLatin1(a.toBase64());
    };
    QString defaultAction = QStringLiteral("com.jolla.settings "
                                           "/com/jolla/settings/ui "
                                           "com.jolla.settings.ui "
                                           "showPage ") +
                            encodeArgument(QStringLiteral("system_settings/system/crash_reporter"));

    QVariantMap hints;
    hints.insert("category", category);
    hints.insert("x-nemo-preview-summary", summary);
    hints.insert("x-nemo-preview-body", body);
    hints.insert("x-nemo-item-count", count);
    hints.insert("x-nemo-remote-action-default", defaultAction);

    QDBusPendingReply<quint32> reply =
        //: Group name for crash reporter notifications
        //% "Crash reporter"
        proxy->Notify(qtTrId("crash_reporter-notify-app_name"), id, QString(), summary, body,
                      actions, hints, -1);

    qCDebug(cr) << "Sending Notify for notification" << id
                << "of category" << category
                << "with summary" << summary << "and body" << body;

    callWatcher = new QDBusPendingCallWatcher(reply, this);
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
        if (id == 0) {
            // Notification didn't exist, create a new one.
            sendDBusNotify();
            retrieveNotificationId();
            return;
        }
        qCDebug(cr) << "Create notification with id: " << id;
    } else if (reply.isError()) {
        QDBusError error = reply.error();
        qCDebug(cr) << "Failed to create notification: " << error.name() << " - " << error.message();
    }
}

void CReporterNotificationPrivate::removeAfterTimeout()
{
    retrieveNotificationId();

    if (id != 0) {
        Q_Q(CReporterNotification);
        q->remove();
    }
}

void CReporterNotificationPrivate::onNotificationRemoved(quint32 id)
{
    retrieveNotificationId();

    if ((id == this->id) && (id != 0)) {
        Q_Q(CReporterNotification);

        this->id = 0;
        emit q->timeouted();
    }
}

CReporterNotification::CReporterNotification(const QString &eventType, const QString &summary, const QString &body,
                                             QObject *parent)
    : QObject(parent),
      d_ptr(new CReporterNotificationPrivate(eventType, this))
{
    update(summary, body);
}

CReporterNotification::CReporterNotification(const QString &eventType, int id, QObject *parent)
    : QObject(parent),
      d_ptr(new CReporterNotificationPrivate(eventType, this))
{
    d_ptr->id = id;
}

CReporterNotification::~CReporterNotification()
{
    delete d_ptr;
    d_ptr = 0;
}

int CReporterNotification::id()
{
    Q_D(CReporterNotification);

    d->retrieveNotificationId();

    return d->id;
}

void CReporterNotification::setTimeout(int ms)
{
    Q_D(CReporterNotification);

    d->timeout = ms;
}

bool CReporterNotification::operator==(const CReporterNotification &other) const
{
    return (d_ptr == other.d_ptr);
}

void CReporterNotification::update(const QString &summary, const QString &body,
                                   int count)
{
    Q_D(CReporterNotification);

    d->retrieveNotificationId();
    d->summary = summary;
    d->body = body;
    d->count = count;

    d->sendDBusNotify();

    if (d->timeout > 0) {
        QTimer::singleShot(d->timeout, this, SLOT(removeAfterTimeout()));
    }
}

void CReporterNotification::remove()
{
    Q_D(CReporterNotification);

    d->retrieveNotificationId();

    if (d->id != 0) {
        d->proxy->CloseNotification(d->id);
    }
}

bool CReporterNotification::isPublished() const
{
    // TODO: Re-implement for Sailfish
    return true;
}

void CReporterNotification::removeAll()
{
    // TODO: Re-implement for Sailfish
}

#include "moc_creporternotification.cpp"
