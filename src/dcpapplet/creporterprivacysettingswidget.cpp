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

// System includes.

#include <QFile>
#include <QDBusPendingCall>
#include <QDBusPendingReply>
#include <QDBusPendingCallWatcher>

#include <MLayout>
#include <MLabel>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MButton>

// User includes.

#include "creporterprivacysettingswidget.h"
#include "creporterprivacydisclaimerdlg.h"
#include "creportermonitorsettingscontainer.h"
#include "creporterincludesettingscontainer.h"
#include "creporterprivacysettingsmodel.h"
#include "creporterinfobanner.h"
#include "creporterdaemonproxy.h"
#include "creporterdialogserverproxy.h"
#include "creporternamespace.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsWidget::CReporterPrivacySettingsWidget
// ----------------------------------------------------------------------------
CReporterPrivacySettingsWidget::CReporterPrivacySettingsWidget()
    : waitingForCoreFilesReply(false)
{
	initWidget();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsWidget::~CReporterPrivacySettingsWidget
// ----------------------------------------------------------------------------
CReporterPrivacySettingsWidget::~CReporterPrivacySettingsWidget()
{
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsWidget::back
// ----------------------------------------------------------------------------
bool CReporterPrivacySettingsWidget::back()
{
    // Save settings, update view and go back.
    CReporterPrivacySettingsModel::instance()->writeSettings();

    return DcpWidget::back();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsWidget::initWidget
// ----------------------------------------------------------------------------
void CReporterPrivacySettingsWidget::initWidget()
{    
    //% "<b>Crash Reporter %1</b>"
    MLabel *titleLabel = new MLabel(qtTrId("qtn_dcp_crash_reporter_%1").arg(QString(CREPORTERVERSION)));

    // Create containers.
    CReporterMonitorSettingsContainer *monitorSettings =
            new CReporterMonitorSettingsContainer(this);

    CReporterIncludeSettingsContainer *includeSettings =
            new CReporterIncludeSettingsContainer(this);

	// Button for showing privacy statement.
    MButton *showPrivacyButton = new MButton(this);
    showPrivacyButton->setObjectName("ShowPrivacyButton");
    //% "Privacy Disclaimer"
    showPrivacyButton->setText(qtTrId("qtn_dcp_privacy_statement_button"));
    connect(showPrivacyButton, SIGNAL(clicked()), this, SLOT(openPrivacyDisclaimerDialog()));

	// Button for sending all cores.
    //MButton *sendAllButton = new MButton(this);
    //sendAllButton->setObjectName("SendAllButton");
    ////% "Send all"
    //sendAllButton->setText(qtTrId("qtn_dcp_send_all_button_text"));
    //connect(sendAllButton, SIGNAL(clicked()), this, SLOT (handleSendAllButtonClicked()));

    // Button for sending cores via selection
    MButton *sendSelectButton = new MButton(this);
    sendSelectButton->setObjectName("SendSelectButton");
    //% "Send/Delete Reports"
    sendSelectButton->setText(qtTrId("qtn_dcp_send_delete_button_text"));
    connect(sendSelectButton, SIGNAL(clicked()), this, SLOT (handleSendSelectButtonClicked()), Qt::DirectConnection);
    connect(sendSelectButton, SIGNAL(clicked()), monitorSettings, SLOT(updateButtons()), Qt::QueuedConnection);

    // Create bottom layout and policy for buttons.
    MLayout *bottomLayout = new MLayout(0);
    MLinearLayoutPolicy *bottomLayoutPolicy =
            new MLinearLayoutPolicy(bottomLayout, Qt::Horizontal);
    bottomLayoutPolicy->setObjectName("InnerLinearLayout");
    bottomLayout->setPolicy(bottomLayoutPolicy);

    bottomLayoutPolicy->addItem(showPrivacyButton);//, Qt::AlignLeft);
    // bottomLayoutPolicy->addItem(sendAllButton,  Qt::AlignRight);
    bottomLayoutPolicy->addItem(sendSelectButton);//,  Qt::AlignRight);

    // Create main layout and policy.
    MLayout *mainLayout = new MLayout(this);
    MLinearLayoutPolicy *mainLayoutPolicy =
            new MLinearLayoutPolicy(mainLayout, Qt::Vertical);
    mainLayoutPolicy->setObjectName("PageMainLayout");
    mainLayout->setPolicy(mainLayoutPolicy);

    mainLayoutPolicy->addItem(titleLabel, Qt::AlignLeft);
    mainLayoutPolicy->addItem(monitorSettings, Qt::AlignCenter);
    mainLayoutPolicy->addItem(includeSettings, Qt::AlignCenter);
    mainLayoutPolicy->addItem(bottomLayout, Qt::AlignCenter);
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsWidget::openPrivacyDisclaimerDialog
// ----------------------------------------------------------------------------
void CReporterPrivacySettingsWidget::openPrivacyDisclaimerDialog()
{
    if (QFile::exists(CReporter::PrivacyDisclaimerFile)) {

        CReporterPrivacyDisclaimerDlg *dlg =
                new CReporterPrivacyDisclaimerDlg(CReporter::PrivacyDisclaimerFile);
        dlg->setObjectName("PrivacyDisclaimerDlg");
        // Show dialog.
        dlg->exec();
    }
    else {
        // File not found.
        // % "File not found!"
        CReporterInfoBanner::show(qtTrId("qtn_dcp_file_not_found."));
    }
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsWidget::handleSendAllButtonClicked
// ----------------------------------------------------------------------------
void CReporterPrivacySettingsWidget::handleSendAllButtonClicked()
{
    // Request cores from daemon.
    CReporterDaemonProxy daemonProxy(CReporter::DaemonServiceName,
                                         CReporter::DaemonObjectPath, QDBusConnection::sessionBus(), this);
    QDBusPendingReply<QStringList> fetchReply = daemonProxy.getAllCoreFiles();
    // This blocks, but operation shouldn't take too much time.
    fetchReply.waitForFinished();

    if (fetchReply.isError()) {
        // Show error, if failed to communicate with the daemon.
        //% "Failed to fetch crash reports from the system."
        CReporterInfoBanner::show(qtTrId("qtn_dcp_failed_to_fetch_crash_reports_text"));
        return;
    }

    QStringList files = files = fetchReply.argumentAt<0>();

    if (files.isEmpty()) {
        // No unsent crash reports.
        //% "This system has not stored crash reports."
        CReporterInfoBanner::show(qtTrId("qtn_dcp_no_stored_reports_text"));
        return;
    }

    // Pass files to dialog server for uploading.
    QVariantList arguments;
    arguments << files;

    CReporterDialogServerProxy dialogServerProxy(CReporter::DialogServerServiceName,
                                         CReporter::DialogServerObjectPath, QDBusConnection::sessionBus(), this);

    QDBusPendingReply<bool> uploadReply =
            dialogServerProxy.requestDialog(CReporter::UploadDialogType, arguments);

    uploadReply.waitForFinished();

    if (uploadReply.isError()) {
        // Show error, if failed to communicate with the daemon.
        //% "Dialog request failed."
        CReporterInfoBanner::show(qtTrId("qtn_dcp_daemon_communication_error_text."));
        return;
    }

}

void CReporterPrivacySettingsWidget::handleSendSelectButtonClicked()
{
    // Prevent multiple requests caused by user tapping the button many times
    if (waitingForCoreFilesReply)
        return;
    CReporterPrivacySettingsModel::instance()->setAutomaticSendingEnabled(false);
    CReporterPrivacySettingsModel::instance()->setSendingEnabled(true);

    // Request cores from daemon.
    CReporterDaemonProxy daemonProxy(CReporter::DaemonServiceName,
                                     CReporter::DaemonObjectPath, QDBusConnection::sessionBus(), this);
    QDBusPendingReply<QStringList> fetchReply = daemonProxy.getAllCoreFiles();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(fetchReply, this);

    QObject::connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)),
                     this, SLOT(gotCoreFilesReply(QDBusPendingCallWatcher*)));
    waitingForCoreFilesReply = true;
}

void CReporterPrivacySettingsWidget::gotCoreFilesReply(QDBusPendingCallWatcher *call)
{
    if (!waitingForCoreFilesReply)
        return;
    waitingForCoreFilesReply = false;
    QDBusPendingReply<QStringList> fetchReply = *call;

    if (fetchReply.isError()) {
        // Show error, if failed to communicate with the daemon.
        //% "Failed to fetch crash reports from the system."
        CReporterInfoBanner::show(qtTrId("qtn_dcp_failed_to_fetch_crash_reports_text"));
        call->deleteLater();
        return;
    }

    QStringList files = fetchReply.argumentAt<0>();

    if (files.isEmpty()) {
        // No unsent crash reports.
        //% "This system has not stored crash reports."
        CReporterInfoBanner::show(qtTrId("qtn_dcp_no_stored_reports_text"));
        call->deleteLater();
        return;
    }

    // Pass files to dialog server for selection.
    QVariantList arguments;
    arguments << files;

    bool openViaNotification = false; // Open the dialog immediately
    arguments << openViaNotification;

    CReporterDialogServerProxy dialogServerProxy(CReporter::DialogServerServiceName,
                            CReporter::DialogServerObjectPath, QDBusConnection::sessionBus(), this);

    QDBusPendingReply<bool> selectReply =
            dialogServerProxy.requestDialog(CReporter::SendSelectedDialogType, arguments);

    selectReply.waitForFinished();

    if (selectReply.isError()) {
        // Show error, if failed to communicate with the daemon.
        //% "Dialog request failed."
        CReporterInfoBanner::show(qtTrId("qtn_dcp_daemon_communication_error_text."));
        call->deleteLater();
        return;
    }
    call->deleteLater();
}

// End of file
