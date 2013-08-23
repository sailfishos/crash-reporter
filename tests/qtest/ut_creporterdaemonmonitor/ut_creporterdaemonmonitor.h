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

#ifndef UT_CREPORTERDAEMONMONITOR_H
#define UT_CREPORTERDAEMONMONITOR_H

#include <QTest>
#include <QStringList>
#include <QDBusError>
#include <QVariantList>
#include <QDBusMessage>

class CReporterDaemonMonitor;
class CReporterCoreRegistry;

class TestDialogServer : public QObject
{
    Q_OBJECT
    public:
        TestDialogServer();

        ~TestDialogServer();
    public Q_SLOTS:
        QDBusError::ErrorType callReceived(const QString &dialogName,
                                           const QVariantList &arguments,
                                           const QDBusMessage &message);

         void quit();

    public:
        bool callReceivedCalled;
        bool quitCalled;
        QVariantList callArguments;
        QDBusMessage requestMessage;
        QString requestedDialog;
};

class Ut_CReporterDaemonMonitor : public QObject
{
	Q_OBJECT

private slots:

	void initTestCase();
	void init();

    void testNewCoreFileFoundNotified();
    void testNewCoreFileFoundInvalidFile();
    void testNewCoreFileFoundByTheSameName();
    void testDirectoryDeletedNotNotified();
    void testAutoDeleteDublicateCores();
    void testUIFailedToLaunch();

	void cleanupTestCase();
	void cleanup();

private:

    QStringList *paths;
    CReporterDaemonMonitor *monitor;
    CReporterCoreRegistry *registry;
    TestDialogServer *testDialogServer;
};

#endif // UT_CREPORTERDAEMONMONITOR_H
