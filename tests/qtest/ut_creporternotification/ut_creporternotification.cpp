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

#include <QUuid>
#include <QSignalSpy>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>

#include <MApplication>
#include <MNotification>

#include "creporternotification.h"
#include "creporternotification_p.h"
#include "ut_creporternotification.h"

QUuid QUuid::createUuid()
{
    return QUuid();
}

void Ut_CReporterNotification::initTestCase()
{
}

void Ut_CReporterNotification::init()
{
    static MApplication *app = 0;

    if (app == 0) {
        int argc = 2;
        const char *argv[] = {"./ut_creporternotification", "-software", 0};
        app = new MApplication(argc, (char **)argv);
    }

    notification = 0;
}

void Ut_CReporterNotification::testNotificationActivated()
{
    notification = new CReporterNotification("crash-reporter", "test-summary",
            "test-body", "icon-test");
    // Spy activated signal from notification.
    QSignalSpy activatedSpy(notification, SIGNAL(activated()));

    // Wait for notification.
    QTest::qWait(1000);

    QVERIFY(notification->isPublished() == true);

    QString objPath(CREPORTER_DBUS_NTF_OBJECT_PATH);
    objPath.append("00000000000000000000000000000000");

    // Interact with the notification via D-Bus and block.
    QDBusMessage reply = QDBusConnection::sessionBus().call(QDBusMessage::createMethodCall(
                             QDBusConnection::sessionBus().baseService(), objPath,
                             CREPORTER_DBUS_NTF_INTERFACE, "activate"));

    if (reply.type() == QDBusMessage::ErrorMessage) {
        qDebug() << reply.errorMessage();
        QFAIL("Error message received from D-Bus.");
    }

    QVERIFY(activatedSpy.count() == 1);
}

void Ut_CReporterNotification::testNotificationTimeOut()
{
    notification = new CReporterNotification("crash-reporter", "test-summary",
            "test-body", "icon-test");

    // Wait for notification.
    QTest::qWait(1000);

    QVERIFY(notification->isPublished() == true);

    QSignalSpy timeoutedSpy(notification, SIGNAL(timeouted()));
    // Set timeout for the notification.
    notification->setTimeout(2);
    QVERIFY(notification->timeout() == 2);

    // Wait for notification to timeout.
    QTest::qWait(3000);

    QVERIFY(timeoutedSpy.count() == 1);
    QVERIFY(notification->isPublished() == false);
}

void Ut_CReporterNotification::testNotificationOperators()
{
    notification = new CReporterNotification("crash-reporter", "File uploaded");

    CReporterNotification notification2("crash-reporter");
    QVERIFY(*notification != notification2);
}

void Ut_CReporterNotification::testNotificationRemove()
{
    notification = new CReporterNotification("crash-reporter");
    // Wait for notification.
    QTest::qWait(1000);
    QVERIFY(notification->isPublished() == true);

    notification->remove();

    QVERIFY(notification->isPublished() == false);
}

void Ut_CReporterNotification::testNotificationUpdate()
{
    notification = new CReporterNotification("crash-reporter", "test-summary", "test-body");

    // Wait for notification.
    QTest::qWait(1000);

    QVERIFY(notification->isPublished() == true);

    notification->update("test-summary", "test-body");
    QVERIFY(notification->isPublished() == true);

    notification->remove();
    QVERIFY(notification->isPublished() == false);
}

void Ut_CReporterNotification::testRemoveAll()
{
    // Test removing all notifications, which have not been dismissed.
    notification = new CReporterNotification("crash-reporter", "test-summary", "test-body");

    // Wait for notification.
    QTest::qWait(1000);

    CReporterNotification::removeAll();

    QVERIFY(MNotification::notifications().isEmpty() == true);
}

void Ut_CReporterNotification::cleanupTestCase()
{
}

void Ut_CReporterNotification::cleanup()
{
    if (notification != 0) {
        delete notification;
        notification = 0;
    }
}

QTEST_APPLESS_MAIN(Ut_CReporterNotification)
