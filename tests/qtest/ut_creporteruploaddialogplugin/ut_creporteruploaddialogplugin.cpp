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
#include <QStringList>

#include <MApplication>

#include "ut_creporteruploaddialogplugin.h"
#include "creporteruploaddialogplugin.h"
#include "creporteruploaddialog.h"
#include "creporterdialogserver_stub.h"
#include "creporternamespace.h"

// CReporterUploadItem mock object.
CReporterUploadItem::CReporterUploadItem(const QString &file)
{
    Q_UNUSED(file);
}

CReporterUploadItem::~CReporterUploadItem()
{
}

// CReporterUploadQueue mock object.
static int itemsAddedCount;

CReporterUploadQueue::CReporterUploadQueue(QObject *parent)
{
    Q_UNUSED(parent);
    itemsAddedCount = 0;
}

CReporterUploadQueue::~CReporterUploadQueue()
{
}

void CReporterUploadQueue::enqueue(CReporterUploadItem *item)
{
  Q_UNUSED(item);
  itemsAddedCount++;
}

static QString errorString;
static bool cancelAllCalled;
static bool lastErrorCalled;

// CReporterUploadEngine mock object.
CReporterUploadEngine::CReporterUploadEngine(CReporterUploadQueue *queue, QObject *parent)
{
    Q_UNUSED(queue);
    Q_UNUSED(parent);

    errorString.clear();
    cancelAllCalled = false;
    lastErrorCalled = false;
}

CReporterUploadEngine::~CReporterUploadEngine()
{
}

QString CReporterUploadEngine::lastError() const
{
    lastErrorCalled = true;
    return errorString;
}

void CReporterUploadEngine::cancelAll()
{
    cancelAllCalled = true;
}

// CReporterUploadDialog mock object.
CReporterUploadDialog::CReporterUploadDialog(CReporterUploadQueue *queue,
                                             QGraphicsItem *parent)
{
    Q_UNUSED(parent);
    Q_UNUSED(queue);
}

CReporterUploadDialog::~CReporterUploadDialog()
{
}

void CReporterUploadDialog::createcontent()
{

}

void CReporterUploadDialog::updateProgress(int done)
{
     Q_UNUSED(done);
}

void CReporterUploadDialog::uploadFinished()
{
}

void CReporterUploadDialog::handleClicked()
{

}

void CReporterUploadDialog::updateContent(CReporterUploadItem *item)
{
    Q_UNUSED(item);
}

void CReporterUploadDialog::handleItemAdded(CReporterUploadItem *item)
{
    Q_UNUSED(item);
}

// CReporterEngine mock object.

// Unit test.
void Ut_CReporterUploadDialogPlugin::init()
{
    static MApplication *app = 0;

    if (app == 0) {
            int argc = 2;
            const char *argv[] = {"./ut_creporteruploaddialogplugin", "-software", 0};
            app = new MApplication(argc, (char **)argv);
        }

    m_Server = new TestDialogServer();
    m_Subject = new CReporterUploadDialogPlugin();
}

void Ut_CReporterUploadDialogPlugin::initTestCase()
{
}

void Ut_CReporterUploadDialogPlugin::cleanup()
{
    if (m_Server != 0) {
        delete m_Server;
        m_Server = 0;
    }

    if (m_Subject != 0) {
        if (m_Subject->isInitialized()) {
           m_Subject->destroy();
        }
        delete m_Subject;
        m_Subject = 0;
    }
}

void Ut_CReporterUploadDialogPlugin::cleanupTestCase()
{
}

void Ut_CReporterUploadDialogPlugin::testInitializePlugin()
{
   // Test plugin initialized successfully.
   m_Subject->initialize(m_Server);
   QVERIFY(m_Subject->name() == CReporter::UploadDialogType);
   QVERIFY(m_Subject->isInitialized() == true);
}

void Ut_CReporterUploadDialogPlugin::testRequestPluginNotInitialized()
{
    // Test request rejected, if plugin is not initialized.
    QVariantList args;
    QStringList files;
    files << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");
    args << files;

    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == false);
    QVERIFY(m_Subject->isInitialized() == false);
}

void Ut_CReporterUploadDialogPlugin::testDestroyPlugin()
{
    // Test destroying plugin.
    m_Subject->initialize(m_Server);
    m_Subject->destroy();
    QVERIFY(m_Subject->isInitialized() == false);
}

void Ut_CReporterUploadDialogPlugin::testInvalidRequestArguments1()
{
    // Test plugin request with too many arguments.
    m_Subject->initialize(m_Server);
    QVariantList args;
    args << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo") <<
            QString("/media/mmc2/core-dumps/test-1234-11-4321.rcore.lzo");

    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == false);
}

void Ut_CReporterUploadDialogPlugin::testInvalidRequestArguments2()
{
    // Test plugin request with invalid argument type.
    m_Subject->initialize(m_Server);
    QVariantList args;
    args << true;

    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == false);
}

void Ut_CReporterUploadDialogPlugin::testInvalidRequestArguments3()
{
    // Test plugin request with no files
    m_Subject->initialize(m_Server);
    QVariantList args;
    args << QStringList();

    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == false);
}

void Ut_CReporterUploadDialogPlugin::testUploadFilesSuccessfully()
{
    QSignalSpy requestedCompletedSpy(m_Subject, SIGNAL(requestCompleted()));
    m_Subject->initialize(m_Server);
    QVariantList args;
    QStringList files;
    files << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo")
            << QString("/media/mmc1/core-dumps/test-1234-9-4321.rcore.lzo");
    args << files;

   QVERIFY(m_Subject->requestDialog(args) == true);

   m_Subject->dialogAppeared();
   QVERIFY(itemsAddedCount == 2);

   m_Subject->engineFinished(static_cast<int>(CReporterUploadEngine::NoError), 2, 2);

   QVERIFY(m_Server->createRequestCalled == true);
   QVERIFY(lastErrorCalled == false);
   QVERIFY(requestedCompletedSpy.count() == 1);
}

void Ut_CReporterUploadDialogPlugin::testUploadFilesFailed()
{
    QSignalSpy requestedCompletedSpy(m_Subject, SIGNAL(requestCompleted()));
    m_Subject->initialize(m_Server);
    QVariantList args;
    QStringList files;
    files << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo")
            << QString("/media/mmc1/core-dumps/test-1234-9-4321.rcore.lzo");
    args << files;

   QVERIFY(m_Subject->requestDialog(args) == true);

   m_Subject->dialogAppeared();
   QVERIFY(itemsAddedCount == 2);

   m_Subject->engineFinished(static_cast<int>(CReporterUploadEngine::ProtocolError), 1, 2);

   errorString = "Error";
   QVERIFY(requestedCompletedSpy.count() == 1);
   QVERIFY(lastErrorCalled == true);
   QVERIFY(m_Server->createRequestCalled == true);
}

void Ut_CReporterUploadDialogPlugin::testUploadCancelled()
{
    QSignalSpy requestedCompletedSpy(m_Subject, SIGNAL(requestCompleted()));
    m_Subject->initialize(m_Server);
    QVariantList args;
    QStringList files;
    files << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo")
            << QString("/media/mmc1/core-dumps/test-1234-9-4321.rcore.lzo");
    args << files;

   QVERIFY(m_Subject->requestDialog(args) == true);

   m_Subject->dialogAppeared();
   QVERIFY(itemsAddedCount == 2);

   m_Subject->actionPerformed(CReporter::CancelButton);

   QVERIFY(cancelAllCalled == true);
   m_Subject->engineFinished(static_cast<int>(CReporterUploadEngine::ProtocolError), 1, 2);

   errorString = "Cancelled";
   QVERIFY(requestedCompletedSpy.count() == 1);
   QVERIFY(lastErrorCalled == true);
   QVERIFY(m_Server->createRequestCalled == true);
}

void Ut_CReporterUploadDialogPlugin::testDialogRejected()
{
    QSignalSpy requestedCompletedSpy(m_Subject, SIGNAL(requestCompleted()));
    m_Subject->initialize(m_Server);
    QVariantList args;
    QStringList files;
    files << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo")
            << QString("/media/mmc1/core-dumps/test-1234-9-4321.rcore.lzo");
    args << files;

   QVERIFY(m_Subject->requestDialog(args) == true);
   m_Subject->dialogAppeared();
   QVERIFY(itemsAddedCount == 2);

   m_Subject->dialogRejected();

   QVERIFY(cancelAllCalled == true);
   m_Subject->engineFinished(static_cast<int>(CReporterUploadEngine::ProtocolError), 1, 2);

   errorString = "Cancelled";
   QVERIFY(requestedCompletedSpy.count() == 1);
   QVERIFY(lastErrorCalled == true);
   QVERIFY(m_Server->createRequestCalled == true);
}

QTEST_APPLESS_MAIN(Ut_CReporterUploadDialogPlugin)
