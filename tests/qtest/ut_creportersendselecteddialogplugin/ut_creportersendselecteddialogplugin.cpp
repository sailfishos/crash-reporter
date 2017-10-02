/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Raimo Gratseff <ext-raimo.gratseff@nokia.com>
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
#include <MApplication>

#include "ut_creportersendselecteddialogplugin.h"
#include "creportersendselecteddialogplugin.h"
#include "creportersendselecteddialog.h"
#include "creporterdialogserver_stub.h"
#include "creporternamespace.h"
#include "creporternotification.h"

static bool fileSizeToStringCalled;
static bool removeFileCalled;
static bool fileRemoved;

static QString ntfEventType;
static QString ntfSummary;
static QString ntfBody;
static QString ntfImageName;
static bool notificationCreated;
static bool notificationUpdated;
static bool notificationTimeoutSet;

// ----------------------------------------------------------------------------
// CReporterNotification mock object.
// ----------------------------------------------------------------------------

CReporterNotification::CReporterNotification(const QString &eventType,
        const QString &summary, const QString &body,
        const QString &imageName, QObject *parent)
{
    ntfEventType = eventType;
    ntfSummary = summary;
    ntfBody = body;
    ntfImageName = imageName;
    notificationCreated = true;
    Q_UNUSED(parent);
}

CReporterNotification::~CReporterNotification()
{}

void CReporterNotification::update(const QString &summary, const QString &body)
{
    notificationUpdated = true;
    ntfSummary = summary;
    Q_UNUSED(body);
}

void CReporterNotification::setTimeout(int timeout)
{
    notificationTimeoutSet = true;
    Q_UNUSED(timeout);
}

void CReporterNotification::remove()
{}

int CReporterNotification::timeout() const
{
    return 0;
}

// ----------------------------------------------------------------------------
// CReporterUtils mock object.
// ----------------------------------------------------------------------------
class CReporterUtils
{
    static QString fileSizeToString(const quint64 size);
    static bool appendToLzo(const QString &text, const QString &filePath);
    static bool removeFile(const QString &path);
    static QStringList parseCrashInfoFromFilename( const QString &filePath );
};

QString CReporterUtils::fileSizeToString(const quint64 size)
{
    Q_UNUSED(size);
    fileSizeToStringCalled = true;
    return "1 MB";
}

bool CReporterUtils::removeFile(const QString &path)
{
    Q_UNUSED(path);
    removeFileCalled = true;
    return fileRemoved;
}

// ----------------------------------------------------------------------------
// CReporterSendSelectedDialog mock object.
// ----------------------------------------------------------------------------
CReporterSendSelectedDialog::CReporterSendSelectedDialog(const QStringList &files,
        const QString &server)
{
    Q_UNUSED(files);
    Q_UNUSED(server);
}

CReporterSendSelectedDialog::~CReporterSendSelectedDialog()
{
}

void CReporterSendSelectedDialog::createcontent()
{
}

QStringList CReporterSendSelectedDialog::getSelectedFiles()
{
    QStringList selectedFiles;
    selectedFiles << "file1";
    return selectedFiles;
}

// ----------------------------------------------------------------------------
//  UNIT TESTS
// ----------------------------------------------------------------------------
void Ut_CReporterSendSelectedDialogPlugin::init()
{
    fileSizeToStringCalled = false;
    removeFileCalled = false;
    fileRemoved = false;
    ntfEventType.clear();
    ntfSummary.clear();
    ntfBody.clear();
    ntfImageName.clear();
    notificationCreated = false;
    notificationUpdated = false;
    notificationTimeoutSet = false;

    static MApplication *app = 0;

    if (app == 0) {
        int argc = 2;
        const char *argv[] = {"./ut_creportersendselecteddialogplugin", "-software", 0};
        app = new MApplication(argc, (char **)argv);
    }

    m_Server = new TestDialogServer();
    m_Subject = new CReporterSendSelectedDialogPlugin();
}

void Ut_CReporterSendSelectedDialogPlugin::initTestCase()
{

}

void Ut_CReporterSendSelectedDialogPlugin::testInitializePlugin()
{
    // Test plugin initialized successfully.
    m_Subject->initialize(m_Server);
    QVERIFY(m_Subject->name() == CReporter::SendSelectedDialogType );
    QVERIFY(m_Subject->isInitialized() == true);
}

void Ut_CReporterSendSelectedDialogPlugin::testRequestPluginNotInitialized()
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

void Ut_CReporterSendSelectedDialogPlugin::testDestroyPlugin()
{
    // Test destroying plugin.
    m_Subject->initialize(m_Server);
    m_Subject->destroy();
    QVERIFY(m_Subject->isInitialized() == false);
}

void Ut_CReporterSendSelectedDialogPlugin::testRequestPluginInvalidArguments()
{
    // Test request rejected, when arguments are not valid.
    m_Subject->initialize(m_Server);

    QVariantList noFiles; // Arguments list with emply filename list
    QVariantList noShowNtfBool; // Arguments list without notification boolean

    noFiles << QStringList();
    noFiles << bool(true); // "Show notification"

    QStringList files;
    files << "file1" << "file2";
    noShowNtfBool << files;

    QVERIFY(m_Subject->requestDialog(QVariantList()) == false);
    QVERIFY(m_Subject->requestDialog(QVariantList()) == false);

}

void Ut_CReporterSendSelectedDialogPlugin::testRequestHandledFilesSent()
{
    // Test successfull request handling - Send all.
    QSignalSpy requestCompletedSpy(m_Subject, SIGNAL(requestCompleted()));

    m_Subject->initialize(m_Server);
    QVariantList args;
    QStringList files;
    files << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo") <<
          QString("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo");
    args << files;
    // Open via notification
    args << bool(true);

    // Request plugin and show notification.
    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == true);
    QVERIFY(notificationCreated = true);
    QVERIFY(notificationTimeoutSet = true);

    // User interacts with the notification and dialog is displayed.
    m_Subject->notificationActivated();
    QVERIFY(m_Server->showDialogCalled == true);
    QVERIFY(m_Subject->isActive() == true);

    // Fake "Send" -button clicked from dialog.
    int button = static_cast<int>(CReporter::SendSelectedButton);
    m_Subject->actionPerformed(button);

    QVERIFY(m_Server->createRequestCalled == true);

    // Now request is not completed until the dialog has disappeared so this cannot be tested here.
    //QVERIFY(requestCompletedSpy.count() == 1);
}


void Ut_CReporterSendSelectedDialogPlugin::testRequestHandledFilesDeleted()
{
    // Test successful request handling - Delete selected.
    QSignalSpy requestCompletedSpy(m_Subject, SIGNAL(requestCompleted()));

    m_Subject->initialize(m_Server);
    QVariantList args;
    QStringList files;
    files << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo") <<
          QString("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo");
    args << files;
    // Open via notification
    args << bool(true);

    // Request plugin and show notification.
    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == true);
    QVERIFY(notificationCreated = true);
    QVERIFY(notificationTimeoutSet = true);

    // User interacts with the notification and dialog is displayed.
    m_Subject->notificationActivated();
    QVERIFY(m_Server->showDialogCalled == true);
    QVERIFY(m_Subject->isActive() == true);

    // Fake "Send" -button clicked from dialog.
    int button = static_cast<int>(CReporter::DeleteSelectedButton);
    m_Subject->actionPerformed(button);
    QVERIFY(removeFileCalled == true);

    // Now request is not completed until the dialog has disappeared so this cannot be tested here.
    //QVERIFY(requestCompletedSpy.count() == 1);
}

void Ut_CReporterSendSelectedDialogPlugin::testRejectDialog()
{
    // Test rejecting dialog.
    QSignalSpy requestCompletedSpy(m_Subject, SIGNAL(requestCompleted()));

    m_Subject->initialize(m_Server);
    QVariantList args;
    QStringList files;
    files << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo") <<
          QString("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo");
    args << files;
    // Do not open via notification
    args << bool(false);

    // Request plugin to show the dialog
    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == true);
    QVERIFY(m_Subject->isActive() == true);

    m_Server->dialog->reject();
    // Now request is not completed until the dialog has disappeared so this cannot be tested here.
    //QVERIFY(requestCompletedSpy.count() == 1);
}

void Ut_CReporterSendSelectedDialogPlugin::cleanupTestCase()
{

}

void Ut_CReporterSendSelectedDialogPlugin::cleanup()
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


QTEST_APPLESS_MAIN(Ut_CReporterSendSelectedDialogPlugin)

