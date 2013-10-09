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

#include <QSignalSpy>
#include <QDir>
#include <QSettings>
#include <QCoreApplication>
#include <QProcess>
#include <QDBusConnection>
#include <QDBusConnectionInterface>

#include "ut_creporterdaemon.h"
#include "creportercoreregistry.h"
#include "creportertestutils.h"
#include "creporterdaemon.h"
#include "creporterdaemon_p.h"
#include "creporterprivacysettingsmodel.h"
#include "creporternamespace.h"
#include "creporterdialogserverdbusadaptor.h"
#include "creportersettingsinit_p.h"
#include "creporternotification.h"

static const char* test_files1[] = { 
	"test_core.rcore.lzo",
	"test_1234.rcore.lzo",
	"test_5678_core.rcore.lzo",
	};

static const char* test_files_invalid1[] = { 
	".test_core.rcore.lzo",
    /*"crash-reporter-daemon.rcore.lzo",
    "crash-reporter-ui.rcore.lzo",*/
	};

static bool serviceRegisteredReply;
// Overridden here to able to fake UI launch failing. Otherwise the Qt implementation seems
// to return true always in unit tests.
QDBusReply<bool> QDBusConnectionInterface::isServiceRegistered(const QString &serviceName) const
{
    Q_UNUSED(serviceName);
    QDBusMessage reply;
    reply << serviceRegisteredReply;
    return reply;
}

static QString ntfEventType;
static QString ntfSummary;
static QString ntfBody;
static bool notificationCreated;
static bool notificationUpdated;

// CReporterNotification mock object.
CReporterNotification::CReporterNotification(const QString &eventType,
                                             const QString &summary, const QString &body,
                                             QObject *parent)
{
    ntfEventType = eventType;
    ntfSummary = summary;
    ntfBody = body;
    notificationCreated = true;
    Q_UNUSED(parent);
}

CReporterNotification::~CReporterNotification()
{}

void CReporterNotification::update(const QString &summary, const QString &body,
                                   int count)
{
    notificationUpdated = true;
    ntfSummary = summary;
    Q_UNUSED(body);
    Q_UNUSED(count)
}

void CReporterNotification::remove()
{}

const QString testSettingsFile("/tmp/crash-reporter-tests/user_settings/crash-reporter-settings/crash-reporter-privacy.conf");

TestDialogServer::TestDialogServer()
{
    callReceivedCalled = false;
    quitCalled = false;

    new CReporterDialogServerDBusAdaptor(this);
    QDBusConnection::sessionBus().registerService(CReporter::DialogServerServiceName);
    QDBusConnection::sessionBus().registerObject(CReporter::DialogServerObjectPath, this);
}

TestDialogServer::~TestDialogServer()
{
}

QDBusError::ErrorType TestDialogServer::callReceived(const QString &dialogName,
                                const QVariantList &arguments, const QDBusMessage &message) {

    requestedDialog = dialogName;
    callArguments = arguments;
    requestMessage = message;
    callReceivedCalled = true;

    return QDBusError::NoError;
}

void TestDialogServer::quit()
{
    quitCalled = true;
}

void Ut_CReporterDaemon::initTestCase()
{
	CReporterTestUtils::createTestMountpoints();
    daemon = 0;
    paths = 0;
    settings = 0;

    QDir dir;
    dir.mkpath("/tmp/crash-reporter-tests/user_settings");
}

void Ut_CReporterDaemon::init()
{
    ntfEventType.clear();
    ntfSummary.clear();
    ntfBody.clear();
    notificationCreated = false;
    notificationUpdated = false;
    serviceRegisteredReply = true;

    static QCoreApplication *app = 0;

    if (app == 0) {
        int argc = 1;
        const char *argv[] = {"./ut_creporterdaemon", 0};
        app = new QCoreApplication(argc, (char **)argv);
    }

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

    testDialogServer = new TestDialogServer();
    creporterSettingsInit(QString(), "/tmp/crash-reporter-tests/user_settings");
}

void Ut_CReporterDaemon::testInitiateDaemon()
{
    QStringList compareFiles;

    // Check that daemon is initiated successfully, registered to D-Bus and UI is launched.
    daemon = new CReporterDaemon;

    paths = daemon->d_ptr->registry->getCoreLocationPaths();
    CReporterTestUtils::createTestDataFiles(*paths, compareFiles, test_files1);

    QVERIFY(daemon->initiateDaemon() == true);

    QProcess dbusSend;
    dbusSend.start(QString("dbus-send --session --print-reply --dest=%1 %2 %3 string:%4")
                   .arg("org.freedesktop.DBus").arg("/").arg("org.freedesktop.DBus.GetConnectionUnixProcessID")
                   .arg(CReporter::DaemonServiceName));

    dbusSend.waitForFinished();
    QVERIFY(dbusSend.exitCode() == 0);

    daemon->stopService();

    dbusSend.start(QString("dbus-send --session --print-reply --dest=%1 %2 %3 string:%4")
                   .arg("org.freedesktop.DBus").arg("/").arg("org.freedesktop.DBus.GetConnectionUnixProcessID")
                   .arg(CReporter::DaemonServiceName));

    dbusSend.waitForFinished();
    QVERIFY(dbusSend.exitCode() != 0);

    QVERIFY(testDialogServer->callReceivedCalled == true);
    QVERIFY(testDialogServer->callArguments.at(0).type() == QVariant::StringList);
    QVERIFY(testDialogServer->requestedDialog == CReporter::SendSelectedDialogType);
    QStringList files = testDialogServer->callArguments.at(0).toStringList();
    QCOMPARE(files, compareFiles);

}

void Ut_CReporterDaemon::testDelayedStartup()
{
    // Check that daemon is initiated successfully after delay. UI is not launched, since no
    // cores are found.
    daemon = new CReporterDaemon;
    daemon->setDelayedStartup(5000);
    QVERIFY(daemon->d_ptr->timerId != 0);

    QTest::qWait(6000);

    QProcess dbusSend;
    dbusSend.start(QString("dbus-send --session --print-reply --dest=%1 %2 %3 string:%4")
                   .arg("org.freedesktop.DBus").arg("/").arg("org.freedesktop.DBus.GetConnectionUnixProcessID")
                   .arg(CReporter::DaemonServiceName));

    dbusSend.waitForFinished();
    QVERIFY(dbusSend.exitCode() == 0);

    daemon->stopService();

    dbusSend.start(QString("dbus-send --session --print-reply --dest=%1 %2 %3 string:%4")
                   .arg("org.freedesktop.DBus").arg("/").arg("org.freedesktop.DBus.GetConnectionUnixProcessID")
                   .arg(CReporter::DaemonServiceName));

    dbusSend.waitForFinished();
    QVERIFY(dbusSend.exitCode() != 0);
    QVERIFY(testDialogServer->callReceivedCalled == false);
}

void Ut_CReporterDaemon::testCollectAllCoreFiles()
{
    QStringList compareFiles;
	
    daemon = new CReporterDaemon;
    QVERIFY(daemon->initiateDaemon() == true);

    paths = daemon->d_ptr->registry->getCoreLocationPaths();

    CReporterTestUtils::createTestDataFiles(*paths, compareFiles, test_files1);

    QStringList files = daemon->collectAllCoreFiles();

    QCOMPARE(files.count(), compareFiles.count());

    QCOMPARE(files, compareFiles);

    CReporterTestUtils::removeDirectories(*paths);
}

void Ut_CReporterDaemon::testCollectAllCoreFilesNotValidFiles()
{
	QStringList compareFiles;
	QFile file;

    daemon = new CReporterDaemon;
    QVERIFY(daemon->initiateDaemon() == true);

    paths = daemon->d_ptr->registry->getCoreLocationPaths();

    CReporterTestUtils::createTestDataFiles(*paths, compareFiles, test_files_invalid1);

    QStringList files = daemon->collectAllCoreFiles();

    QCOMPARE(files.count(), 0);

    CReporterTestUtils::removeDirectories(*paths);
}

void Ut_CReporterDaemon::testMonitoringEnabledFromSettings()
{
    daemon = new CReporterDaemon;
    QVERIFY(daemon->initiateDaemon() == true);

    paths = daemon->d_ptr->registry->getCoreLocationPaths();

    QString filePath(paths->at(0));
    filePath.append("/foobar_core.rcore.lzo");

    QDir::setCurrent(paths->at(0));

    QFile file;
    file.setFileName("foobar_core.rcore.lzo");
    file.open(QIODevice::ReadWrite);
    file.close();

    QTest::qWait(50);

    QVERIFY(testDialogServer->callReceivedCalled == true);
    QVERIFY(testDialogServer->callArguments.at(0).type() == QVariant::String);
    QVERIFY(testDialogServer->requestedDialog == CReporter::NotifyNewDialogType);
    QString argument = testDialogServer->callArguments.at(0).toString();
    QCOMPARE(argument, filePath);

    QFile::remove(testSettingsFile);

    CReporterTestUtils::removeDirectories(*paths);
}

void Ut_CReporterDaemon::testMonitoringDisabledFromSettings()
{
    // Disable monitoring.
    settings->setValue(Settings::ValueNotifications, false);
    settings->sync();

    daemon = new CReporterDaemon;
    QVERIFY(daemon->initiateDaemon() == true);

    paths = daemon->d_ptr->registry->getCoreLocationPaths();

    QString filePath(paths->at(0));
    filePath.append("/foobar_core.rcore.lzo");

    QDir::setCurrent(paths->at(0));

    QFile file;
    file.setFileName("foobar_core.rcore.lzo");
    file.open(QIODevice::ReadWrite);
    file.close();

    QTest::qWait(50);

    QVERIFY(testDialogServer->callReceivedCalled == false);
    QFile::remove(testSettingsFile);

    CReporterTestUtils::removeDirectories(*paths);
}

void Ut_CReporterDaemon::testLaunchingUIFailed()
{
    // Test situation, when UI is tried to launch, but it fails.
    // In this case error notification is created.
    QStringList compareFiles;
    daemon = new CReporterDaemon;
    paths = daemon->d_ptr->registry->getCoreLocationPaths();
    CReporterTestUtils::createTestDataFiles(*paths, compareFiles, test_files1);

    // Destroy UI.
    delete testDialogServer;
    testDialogServer = 0;

    serviceRegisteredReply = false;

    QVERIFY(daemon->initiateDaemon() == true);

    QTest::qWait(100);

    QVERIFY(notificationCreated == true);
}

void Ut_CReporterDaemon::cleanupTestCase()
{
    CReporterTestUtils::removeTestMountpoints();

    UNUSED_RESULT(system("rm -rf /tmp/crash-reporter-tests"));
}

void Ut_CReporterDaemon::cleanup()
{
    if (daemon != 0) {
        delete daemon;
        daemon = 0;
    }

    if (paths != 0) {
        CReporterTestUtils::removeDirectories(*paths);
        delete paths;
        paths = 0;
    }

    if (settings != 0) {
        delete settings;
        settings = 0;
    }

    if (testDialogServer != 0) {
        delete testDialogServer;
        testDialogServer = 0;
    }
}

QTEST_APPLESS_MAIN(Ut_CReporterDaemon)

