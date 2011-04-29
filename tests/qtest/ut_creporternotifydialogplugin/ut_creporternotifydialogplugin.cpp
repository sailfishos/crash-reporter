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

#include <MApplication>

#include "ut_creporternotifydialogplugin.h"
#include "creporternotifydialogplugin.h"
#include "creporternotificationdialog.h"
#include "creporterdialogserver_stub.h"
#include "creporterinfobanner.h"
#include "creporternamespace.h"
#include "creporternotification.h"

// Defined in duicontrolpanelif_stub.cpp
extern bool gDcpAppletPageCalled;
extern QString gDcpRequestedAppletPage;
extern bool gDcpSuccess;

static QString ntfEventType;
static QString ntfSummary;
static QString ntfBody;
static QString ntfImageName;
static bool notificationCreated;
static bool notificationUpdated;
static bool notificationTimeoutSet;

// CReporterNotification mock object.
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

bool CReporterNotification::isPublished() const
{
    return notificationCreated || notificationUpdated;
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

static bool parseCrashInfoFromFilenameCalled;
static bool fileSizeToStringCalled;
static bool appendToLzoCalled;
static bool removeFileCalled;
static bool lzoAppended;
static bool fileRemoved;

// CReporterUtils mock object.
class CReporterUtils
{
    static QString fileSizeToString(const quint64 size);
    static bool appendToLzo(const QString &text, const QString &filePath);
    static bool removeFile(const QString &path);
    static QStringList parseCrashInfoFromFilename( const QString& filePath );
};

QString CReporterUtils::fileSizeToString(const quint64 size)
{
    Q_UNUSED(size);
    fileSizeToStringCalled = true;
    return "1 MB";
}

bool CReporterUtils::appendToLzo(const QString &text, const QString &filePath)
{
    Q_UNUSED(text);
    Q_UNUSED(filePath);
    appendToLzoCalled = true;
    return lzoAppended;
}

bool CReporterUtils::removeFile(const QString &path)
{
    Q_UNUSED(path);
    removeFileCalled = true;
    return fileRemoved;
}

QStringList CReporterUtils::parseCrashInfoFromFilename(const QString &filePath)
{
    Q_UNUSED(filePath);
    QStringList list;
    list << "test-app" << "1111" << "9" << "2222";
    parseCrashInfoFromFilenameCalled = true;
    return list;
}

static bool userCommentsCalled;
static QString userCommentsText;

// CReporterNotificationDialog mock object.
CReporterNotificationDialog::CReporterNotificationDialog(const QStringList &details,
                                                          const QString &server, const QString &filesize)
{
    Q_UNUSED(details);
    Q_UNUSED(server);
    Q_UNUSED(filesize);
}

CReporterNotificationDialog::~CReporterNotificationDialog()
{

}

QString CReporterNotificationDialog::userComments() const
{
    userCommentsCalled = true;

    return userCommentsText;
}

void CReporterNotificationDialog::setContactInfo(QString contactInfo)
{
    Q_UNUSED(contactInfo);
}

QString CReporterNotificationDialog::getContactInfo() const
{
    return QString();
}

void CReporterNotificationDialog::createcontent()
{

}

void CReporterNotificationDialog::setContactChecked()
{

}

// Unit test.
void Ut_CReporterNotifyDialogPlugin::init()
{
    parseCrashInfoFromFilenameCalled = false;
    fileSizeToStringCalled = false;
    appendToLzoCalled = false;
    removeFileCalled = false;
    userCommentsCalled = false;
    userCommentsText.clear();
    lzoAppended = false;
    fileRemoved = false;
    gDcpAppletPageCalled = false;
    gDcpRequestedAppletPage.clear();
    gDcpSuccess = false;
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
            const char *argv[] = {"./ut_creporternotifydialogplugin", "-software", 0};
            app = new MApplication(argc, (char **)argv);
        }

    m_Server = new TestDialogServer();
    m_Subject = new CReporterNotifyDialogPlugin();
}

void Ut_CReporterNotifyDialogPlugin::initTestCase()
{
}

void Ut_CReporterNotifyDialogPlugin::cleanup()
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

void Ut_CReporterNotifyDialogPlugin::cleanupTestCase()
{
}

void Ut_CReporterNotifyDialogPlugin::testInitializePlugin()
{
   // Test plugin initialized successfully.
   m_Subject->initialize(m_Server);
   QVERIFY(m_Subject->name() == CReporter::NotifyNewDialogType);
   QVERIFY(m_Subject->isInitialized() == true);
}

void Ut_CReporterNotifyDialogPlugin::testRequestPluginNotInitialized()
{
    // Test request rejected, if plugin is not initialized.
    QVariantList args;
    args << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo") << true;

    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == false);
    QVERIFY(m_Subject->isInitialized() == false);
}

void Ut_CReporterNotifyDialogPlugin::testDestroyPlugin()
{
    // Test destroying plugin.
    m_Subject->initialize(m_Server);
    m_Subject->destroy();
    QVERIFY(m_Subject->isInitialized() == false);
}

void Ut_CReporterNotifyDialogPlugin::testRequestPluginInvalidNumberOfArguments()
{
    // Test request rejected, when argument count is not valid.
    m_Subject->initialize(m_Server);
    QVariantList args;
    args << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo")
            << QString("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo");

    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == false);
}

void Ut_CReporterNotifyDialogPlugin::testRequestPluginEmptyFilePath()
{
    // Test request rejected, when arguments are not valid.
    m_Subject->initialize(m_Server);
    QVariantList args;
    args << QString();

    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == false);
}

void Ut_CReporterNotifyDialogPlugin::testRequestPluginInvalidArgumentType()
{
    // Test request rejected, when arguments are not valid.
    m_Subject->initialize(m_Server);
    QVariantList args;
    args << 1234;

    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == false);
}


void Ut_CReporterNotifyDialogPlugin::testRequestHandledFileSent()
{
    // Test successfull request handling - Send.
    QSignalSpy requestCompletedSpy(m_Subject, SIGNAL(requestCompleted()));
    userCommentsText = "Some text";
    lzoAppended = true;

    m_Subject->initialize(m_Server);
    QVariantList args;
    args << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");

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
    int button = static_cast<int>(CReporter::SendButton);
    m_Subject->actionPerformed(button);

    QVERIFY(userCommentsCalled == true);
    QVERIFY(appendToLzoCalled == true);
    QVERIFY(m_Server->createRequestCalled == true);

    // Now request is not completed until the dialog has disappeared so this cannot be tested here.
    //QVERIFY(requestCompletedSpy.count() == 1);
}

void Ut_CReporterNotifyDialogPlugin::testRequestHandledFileDeleted()
{
    // Test successfull request handling - Delete.
    QSignalSpy requestCompletedSpy(m_Subject, SIGNAL(requestCompleted()));

    m_Subject->initialize(m_Server);
    QVariantList args;
    args << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");

    // Request plugin and show notification.
    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == true);

    QVERIFY(notificationCreated = true);
    QVERIFY(notificationTimeoutSet = true);

    // User interacts with the notification and dialog is displayed.
    m_Subject->notificationActivated();
    QVERIFY(m_Server->showDialogCalled == true);
    QVERIFY(m_Subject->isActive() == true);

    // Fake "Delete" -button clicked from dialog.
    int button = static_cast<int>(CReporter::DeleteButton);
    m_Subject->actionPerformed(button);

    QVERIFY(removeFileCalled == true);
    // Now request is not completed until the dialog has disappeared so this cannot be tested here.
    //QVERIFY(requestCompletedSpy.count() == 1);
}

void Ut_CReporterNotifyDialogPlugin::testRequestHandledSettingsOpened()
{
    // Test successfull request handling - Options.
    QSignalSpy requestCompletedSpy(m_Subject, SIGNAL(requestCompleted()));
    gDcpSuccess = true;

    m_Subject->initialize(m_Server);
    QVariantList args;
    args << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");

    // Request plugin and show notification.
    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == true);

    QVERIFY(notificationCreated = true);
    QVERIFY(notificationTimeoutSet = true);

    // User interacts with the notification and dialog is displayed.
    m_Subject->notificationActivated();
    QVERIFY(m_Server->showDialogCalled == true);
    QVERIFY(m_Subject->isActive() == true);

    // Fake "Options" -button clicked from dialog.
    int button = static_cast<int>(CReporter::OptionsButton);
    m_Subject->actionPerformed(button);

    QVERIFY(gDcpAppletPageCalled == true);

    // Clicking "Options" should no longer close the dialog
    QVERIFY(requestCompletedSpy.count() == 0);
}

void Ut_CReporterNotifyDialogPlugin::testNotificationTimeout()
{
    // Test notification timeout.
    QSignalSpy requestCompletedSpy(m_Subject, SIGNAL(requestCompleted()));

    m_Subject->initialize(m_Server);
    QVariantList args;
    args << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");

    // Request plugin and show notification.
    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == true);

    QVERIFY(notificationCreated = true);
    QVERIFY(notificationTimeoutSet = true);

    m_Subject->notificationTimeout();

    QVERIFY(m_Subject->isActive() == false);
    // Now request is not completed until the dialog has disappeared so this cannot be tested here.
    //QVERIFY(requestCompletedSpy.count() == 1);
}

void Ut_CReporterNotifyDialogPlugin::testUpdateNotification()
{
    // Test notification update.
    QSignalSpy requestCompletedSpy(m_Subject, SIGNAL(requestCompleted()));

    m_Subject->initialize(m_Server);
    QVariantList args;
    args << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");

    // Request plugin and show notification.
    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == true);

    QVERIFY(notificationCreated = true);
    QVERIFY(notificationTimeoutSet = true);

    notificationTimeoutSet = false;

    args.clear();
    args << QString("/media/mmc1/core-dumps/application-1234-9-4321.rcore.lzo");

    // Request plugin and show notification.
    retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == true);

    // Notifications are no longer updated but removed and replaced with a new one
    //QVERIFY(notificationUpdated == true);
    QVERIFY(notificationTimeoutSet == true);

    m_Subject->notificationTimeout();

    QVERIFY(m_Subject->isActive() == false);
    // Now request is not completed until the dialog has disappeared so this cannot be tested here.
    //QVERIFY(requestCompletedSpy.count() == 1);
}

void Ut_CReporterNotifyDialogPlugin::testDialogRejected()
{
    // Test successfull request handling - Delete.
    QSignalSpy requestCompletedSpy(m_Subject, SIGNAL(requestCompleted()));

    m_Subject->initialize(m_Server);
    QVariantList args;
    args << QString("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo");

    // Request plugin and show notification.
    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == true);

    QVERIFY(notificationCreated = true);
    QVERIFY(notificationTimeoutSet = true);

    // User interacts with the notification and dialog is displayed.
    m_Subject->notificationActivated();
    QVERIFY(m_Server->showDialogCalled == true);
    QVERIFY(m_Subject->isActive() == true);

    // Reject dialog.
    m_Server->dialog->reject();
    // Now request is not completed until the dialog has disappeared so this cannot be tested here.
    //QVERIFY(requestCompletedSpy.count() == 1);
}

QTEST_APPLESS_MAIN(Ut_CReporterNotifyDialogPlugin)
