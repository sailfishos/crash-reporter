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

#include <QSignalSpy>
#include <QDBusInterface>
#include <QDBusReply>
#include <QDebug>

#include <MApplication>
#include <MApplicationService>

#include "ut_creporterdialogserver.h"
#include "creporterdialogserverinterface.h"
#include "creporternotification.h"
#include "creporterdialogserver.h"
#include "creporterdialogserver_p.h"
#include "creporternamespace.h"
#include "creporterdialogserverproxy.h"
#include "creporterinfobanner.h"

// Plugin mock object.

TestPlugin::TestPlugin()
{
    initialized = true;
    initializeCalled = false;
    destroyCalled = false;
    pluginName = QString();
    server = 0;
    active = false;
    requestHandled = false;
    arguments = QVariantList();
    accepted = true;
}

void TestPlugin::initialize(CReporterDialogServerInterface *server)
{
    initializeCalled = true;
    this->server = server;
}

void TestPlugin::destroy()
{
    destroyCalled = true;
}

bool TestPlugin::isInitialized() const
{
    return initialized;
}

QString TestPlugin::name() const
{
    return pluginName;
}

bool TestPlugin::requestDialog(const QVariantList &arguments)
{
    this->arguments = arguments;
    requestHandled = true;
    return accepted;
}

bool TestPlugin::isActive() const
{
    return active;
}

bool TestPlugin::isVisible() const
{
    return active;
}

void TestPlugin::emitRequestCompleted()
{
    emit requestCompleted();
}

// CReporterInfobanner mock object.
CReporterInfoBanner* CReporterInfoBanner::show(const QString &message, const QString &bannerType,
                                               const QString &iconID)
{
    Q_UNUSED(message);
    Q_UNUSED(iconID);
    CReporterInfoBanner *b = new CReporterInfoBanner(bannerType);
    return b;
}

CReporterInfoBanner::CReporterInfoBanner(QString bannerType) :
        MBanner()
{
    Q_UNUSED(bannerType);
}

CReporterInfoBanner::~CReporterInfoBanner()
{
}

void CReporterInfoBanner::notificationTimeout()
{

}

// Unit tests.

void Ut_CReporterDialogServer::init()
{
    static MApplication *app = 0;
    if (app == 0) {
            int argc = 2;
            const char *argv[] = {"./ut_creporterdialogserver", "-software", 0};
            app = new MApplication(argc, (char **)argv);
        }
}

void Ut_CReporterDialogServer::initTestCase()
{
}

void Ut_CReporterDialogServer::cleanup()
{
    if (m_Subject != 0) {
        delete m_Subject;
        m_Subject = 0;
    }
}

void Ut_CReporterDialogServer::cleanupTestCase()
{
}

void Ut_CReporterDialogServer::testDialogPluginLoadingSucceeds()
{
    // Load plugin successfully.
    m_Subject = new CReporterDialogServer("", new MApplicationService("testservice"));
    TestPlugin *plugin = new TestPlugin();
    plugin->pluginName = "test-plugin";
    m_Subject->d_ptr->loadPlugin(plugin);

    QVERIFY(plugin->initializeCalled == true);
    QVERIFY(plugin->server == m_Subject);
    QVERIFY(m_Subject->d_ptr->m_plugins.count() == 1);
}

void Ut_CReporterDialogServer::testDialogPluginLoadingFails()
{
    // Pluging loading fails.
    m_Subject = new CReporterDialogServer("", new MApplicationService("testservice"));
    TestPlugin *plugin = new TestPlugin();
    plugin->initialized = false;
    plugin->pluginName = "test-plugin";
    m_Subject->d_ptr->loadPlugin(plugin);

    QVERIFY(plugin->initializeCalled == true);
    QVERIFY(plugin->server == m_Subject);
    QVERIFY(m_Subject->d_ptr->m_plugins.count() == 0);
}

void Ut_CReporterDialogServer::testDialogPluginDestroyed()
{
    // Pluging destroyed.
    m_Subject = new CReporterDialogServer("", new MApplicationService("testservice"));
    TestPlugin *plugin = new TestPlugin();
    plugin->pluginName = "test-plugin";
    m_Subject->d_ptr->loadPlugin(plugin);

    delete m_Subject;
    m_Subject = 0;
    QVERIFY(plugin->destroyCalled == true);
}

void Ut_CReporterDialogServer::testSendAllCrashReportsRequested()
{
    // Test unsent crash reports are notified and request passed to the correct plugin.
    TestPlugin *plugin = new TestPlugin();
    plugin->pluginName = CReporter::SendAllDialogType;
    m_Subject = new CReporterDialogServer("", new MApplicationService("testservice"));
    m_Subject->d_ptr->loadPlugin(plugin);

    QStringList files;
    files << "/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo" <<
    "/media/mmc1/core-dumps/daemon-9873-9-1000.rcore.lzo";

    QVariantList arguments;
    arguments << files;

    CReporterDialogServerProxy proxy(CReporter::DialogServerServiceName,
                                     CReporter::DialogServerObjectPath, QDBusConnection::sessionBus(), this);

    QDBusPendingReply<bool> reply = proxy.requestDialog(CReporter::SendAllDialogType, arguments);

    // Block
    reply.waitForFinished();

    // validate reply
    QCOMPARE(reply.isValid(), true);
    QCOMPARE(reply.isError(), false);
    QCOMPARE(reply.count(), 1);
    QVERIFY(reply.value() == true);

    QTest::qWait(100);
    QVERIFY(plugin->requestHandled == true);
    QVERIFY(plugin->arguments.count() == 1);

    QStringList args = plugin->arguments.at(0).toStringList();
    QVERIFY(args.count() == 2);

    QVERIFY(args.at(0) == "/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");
    QVERIFY(args.at(1) == "/media/mmc1/core-dumps/daemon-9873-9-1000.rcore.lzo");

}

void Ut_CReporterDialogServer::testNewCrashReportNotified()
{
    // Test notification about new crash report is received and request is passed to the correct plugin.
    TestPlugin *plugin = new TestPlugin();
    plugin->pluginName = CReporter::NotifyNewDialogType;
    m_Subject = new CReporterDialogServer("", new MApplicationService("testservice"));
    m_Subject->d_ptr->loadPlugin(plugin);

    QVariantList arguments;
    arguments << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");

    CReporterDialogServerProxy proxy(CReporter::DialogServerServiceName,
                                     CReporter::DialogServerObjectPath, QDBusConnection::sessionBus(), this);

    QDBusPendingReply<bool> reply = proxy.requestDialog(CReporter::NotifyNewDialogType,
                                                        arguments);

    reply.waitForFinished();
    QCOMPARE(reply.isValid(), true);
    QCOMPARE(reply.isError(), false);
    QCOMPARE(reply.count(), 1);
    QVERIFY(reply.value() == true);

    QTest::qWait(100);
    QVERIFY(plugin->requestHandled == true);
    QVERIFY(plugin->arguments.count() == 1);

    QString file = plugin->arguments.at(0).toString();

    QVERIFY(file == "/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");
}

void Ut_CReporterDialogServer::testRequestInvalidPlugin()
{
    // Test request invalid plugin. Request is ignored.
    TestPlugin *plugin = new TestPlugin();
    plugin->pluginName = CReporter::NotifyNewDialogType;
    plugin->accepted = false;
    m_Subject = new CReporterDialogServer("", new MApplicationService("testservice"));
    m_Subject->d_ptr->loadPlugin(plugin);

    QVariantList arguments;
    arguments << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");

    CReporterDialogServerProxy proxy(CReporter::DialogServerServiceName,
                                     CReporter::DialogServerObjectPath, QDBusConnection::sessionBus(), this);

    QDBusPendingReply<bool> reply = proxy.requestDialog("Invalid-plugin",
                                                        arguments);

    reply.waitForFinished();
    QCOMPARE(reply.isValid(), true);
    QCOMPARE(reply.isError(), false);
    QCOMPARE(reply.count(), 1);

    QTest::qWait(100);
    QVERIFY(plugin->requestHandled == false);

    QVERIFY(reply.value() == false);
}

void Ut_CReporterDialogServer::testRequestActivePlugin()
{
    // Test requesting active plugin. Request is handled by showing previously opened dialog
    // Previously this request would have failed.
    TestPlugin *plugin = new TestPlugin();
    plugin->pluginName = CReporter::NotifyNewDialogType;
    plugin->active = true;
    m_Subject = new CReporterDialogServer("", new MApplicationService("testservice"));
    m_Subject->d_ptr->loadPlugin(plugin);

    QVariantList arguments;
    arguments << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");

    CReporterDialogServerProxy proxy(CReporter::DialogServerServiceName,
                                     CReporter::DialogServerObjectPath, QDBusConnection::sessionBus(), this);

    QDBusPendingReply<bool> reply = proxy.requestDialog(CReporter::NotifyNewDialogType,
                                                        arguments);

    reply.waitForFinished();
    QCOMPARE(reply.isValid(), true);
    QCOMPARE(reply.isError(), false);
    QCOMPARE(reply.count(), 1);

    QTest::qWait(100);
    QVERIFY(plugin->requestHandled == true);
    QVERIFY(reply.value() == true);
}

void Ut_CReporterDialogServer::testDialogRequestedByAnotherPlugin()
{
    TestPlugin *plugin1 = new TestPlugin();
    plugin1->pluginName = CReporter::NotifyNewDialogType;
    TestPlugin *plugin2 = new TestPlugin();
    plugin2->pluginName = CReporter::UploadDialogType;

    m_Subject = new CReporterDialogServer("", new MApplicationService("testservice"));
    m_Subject->d_ptr->loadPlugin(plugin1);
    m_Subject->d_ptr->loadPlugin(plugin2);

    QVariantList arguments;
    arguments << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");


    CReporterDialogServerProxy proxy(CReporter::DialogServerServiceName,
                                     CReporter::DialogServerObjectPath, QDBusConnection::sessionBus(), this);

    QDBusPendingReply<bool> reply = proxy.requestDialog(CReporter::NotifyNewDialogType,
                                                        arguments);

    reply.waitForFinished();
    QCOMPARE(reply.isValid(), true);
    QCOMPARE(reply.isError(), false);
    QCOMPARE(reply.count(), 1);
    QVERIFY(reply.value() == true);

    QTest::qWait(100);
    QVERIFY(plugin1->requestHandled == true);

    QVariantList requestArguments;
    requestArguments << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");

    bool ret = m_Subject->createRequest(CReporter::UploadDialogType, requestArguments);
    QVERIFY(ret == true);
    plugin1->emitRequestCompleted();

    QVERIFY(plugin2->requestHandled == true);
    plugin2->emitRequestCompleted();
}

void Ut_CReporterDialogServer::testInvalidPluginRequestedByAnotherPlugin()
{
    TestPlugin *plugin = new TestPlugin();
    plugin->pluginName = CReporter::NotifyNewDialogType;

    m_Subject = new CReporterDialogServer("", new MApplicationService("testservice"));
    m_Subject->d_ptr->loadPlugin(plugin);

    QVariantList arguments;
    arguments << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");


    CReporterDialogServerProxy proxy(CReporter::DialogServerServiceName,
                                     CReporter::DialogServerObjectPath, QDBusConnection::sessionBus(), this);

    QDBusPendingReply<bool> reply = proxy.requestDialog(CReporter::NotifyNewDialogType,
                                                        arguments);

    reply.waitForFinished();
    QCOMPARE(reply.isValid(), true);
    QCOMPARE(reply.isError(), false);
    QCOMPARE(reply.count(), 1);
    QVERIFY(reply.value() == true);

    QTest::qWait(100);
    QVERIFY(plugin->requestHandled == true);

    QVariantList requestArguments;
    requestArguments << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");

    bool ret = m_Subject->createRequest("Invalid", requestArguments);
    QVERIFY(ret == false);
    QVERIFY(m_Subject->d_ptr->m_requestQueue.count() == 0);
    plugin->emitRequestCompleted();
}

QTEST_APPLESS_MAIN(Ut_CReporterDialogServer)

