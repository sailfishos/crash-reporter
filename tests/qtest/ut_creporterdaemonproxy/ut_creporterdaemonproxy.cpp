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

#include "stdlib.h"

#include <QDBusConnection>
#include <QSignalSpy>
#include <QDBusPendingReply>

#include "ut_creporterdaemonproxy.h"
#include "creportercoreregistry.h"
#include "creportertestutils.h"
#include "creporterdaemon.h"
#include "creporterdaemon_p.h"
#include "creporterdaemonproxy.h"
#include "creporterprivacysettingsmodel.h"
#include "creporternamespace.h"
#include "creportersettingsinit_p.h"
#include "creporternotification.h"

const QString testSettingsFile("/tmp/crash-reporter-tests/user_settings/crash-reporter-settings/crash-reporter-privacy.conf");

static const char* test_files1[] = { 
	"test_core.rcore.lzo",
	"test_1234.rcore.lzo",
	"test_5678_core.rcore.lzo",
	};

// CReporterNotification mock object.
CReporterNotification::CReporterNotification(const QString &eventType,
                                             const QString &summary, const QString &body,
                                             QObject *parent)
{
    Q_UNUSED(eventType);
    Q_UNUSED(summary);
    Q_UNUSED(body);
    Q_UNUSED(parent);
}

CReporterNotification::~CReporterNotification()
{}

void CReporterNotification::update(const QString &summary, const QString &body,
                                   int count)
{
    Q_UNUSED(summary);
    Q_UNUSED(body);
    Q_UNUSED(count);
}

void CReporterNotification::remove()
{}

void Ut_CReporterDaemonProxy::initTestCase()
{
	CReporterTestUtils::createTestMountpoints();
    proxy = 0;
    daemon = 0;
    paths = 0;

    QDir dir;
    dir.mkpath("/tmp/crash-reporter-tests/user_settings");
}

void Ut_CReporterDaemonProxy::init()
{
    settings = new QSettings(testSettingsFile, QSettings::NativeFormat);

    // Create test settings.
    settings->setValue(Settings::ValueNotifications, true);
    settings->setValue(Settings::ValueCoreDumping, true);
    settings->setValue(Settings::ValueAutoDeleteDuplicates, true);
    settings->setValue(Settings::ValueAutomaticSending, false);
    settings->setValue(Settings::ValueLifelog, false);

    settings->setValue(Privacy::ValueIncludeCore, true);
    settings->setValue(Privacy::ValueIncludeSysLog, true);
    settings->setValue(Privacy::ValueIncludePkgList, true);
    settings->sync();

    creporterSettingsInit(QString(), "/tmp/crash-reporter-tests/user_settings");
}

void Ut_CReporterDaemonProxy::testProxyCollectAllCoreFiles()
{
	QStringList files;
	QStringList compareFiles;

    daemon = new CReporterDaemon;

    paths = daemon->d_ptr->registry->getCoreLocationPaths();
    CReporterTestUtils::createTestDataFiles(*paths, compareFiles, test_files1);

    QVERIFY(daemon->initiateDaemon() == true);

    // Connect to service.
    proxy = new CReporterDaemonProxy(CReporter::DaemonServiceName,
                                     CReporter::DaemonObjectPath,
                                     QDBusConnection::sessionBus());

    // Not really testing anything... just to check the connection is valid.
    QDBusConnection conn = proxy->connection();
    QCOMPARE(conn.isConnected(), true);
    QCOMPARE(proxy->isValid(), true);
    QCOMPARE(proxy->path(), CReporter::DaemonObjectPath);

	QDBusPendingReply<QStringList> reply = proxy->getAllCoreFiles();
	// block
	reply.waitForFinished();
	// validate reply
    QCOMPARE(reply.isValid(), true);
    QCOMPARE(reply.isError(), false);
    QCOMPARE(reply.count(), 1);

	files = reply.argumentAt<0>();

    QCOMPARE(files.count(), compareFiles.count());

    QCOMPARE(files, compareFiles);
	
}

void Ut_CReporterDaemonProxy::cleanupTestCase()
{
    CReporterTestUtils::removeTestMountpoints();

    UNUSED_RESULT(system("rm -rf /tmp/crash-reporter-tests"));
}

void Ut_CReporterDaemonProxy::cleanup()
{
    if (proxy != 0) {
        delete proxy;
        proxy = NULL;
    }

    if (daemon != 0) {
        delete daemon;
        daemon = NULL;
    }

    if (paths != 0) {
        CReporterTestUtils::removeDirectories(*paths);
        delete paths;
        paths = NULL;
    }

    if (settings != 0) {
        delete settings;
        settings = 0;
    }
}

QTEST_MAIN(Ut_CReporterDaemonProxy)

