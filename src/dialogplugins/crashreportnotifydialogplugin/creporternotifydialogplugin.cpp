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

#include <QtPlugin> // For Q_EXPORT_PLUGIN2
#include <QDebug>
#include <QFileInfo>

#include <MWindow>
#include <DuiControlPanelIf>

// User includes.

#include "creporternotifydialogplugin.h"
#include "creporternotificationdialog.h"
#include "creporterutils.h"
#include "creporterapplicationsettings.h"
#include "creporternotification.h"
#include "creporternamespace.h"
#include "creporterinfobanner.h"

// Local constants.

const QString CrashReporterDcpApplet = "Crash Reporter";

/*!
  * @class CReporterNotifyDialogPluginPrivate
  * @brief Private CReporterNotifyDialogPlugin class.
  *
  */
class CReporterNotifyDialogPluginPrivate
{
    public:
        //! @arg Pointer to dialog server instance.
        CReporterDialogServerInterface *server;
        //! @arg Absolute path to rich-core file.
        QString filePath;
        //! @arg Pointer to dialog.
        CReporterNotificationDialog *dialog;
        //! @arg Notification instance.
        CReporterNotification *notification;
        //! @arg Plugin state.
        bool active;
        //! @arg Details about crash (parsed from rich-core filename).
        QStringList details;
};

// *** Class CReporterNotifyDialogPlugin ****

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::initialize
// ----------------------------------------------------------------------------
void CReporterNotifyDialogPlugin::initialize(CReporterDialogServerInterface *server)
{
    d_ptr = new CReporterNotifyDialogPluginPrivate();
    d_ptr->server = server;
    d_ptr->notification = 0;
    d_ptr->active = false;
}

// ----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::destroy
// ----------------------------------------------------------------------------
void CReporterNotifyDialogPlugin::destroy()
{
    CReporterApplicationSettings::freeSingleton();

    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::isInitialized
// ----------------------------------------------------------------------------
bool CReporterNotifyDialogPlugin::isInitialized() const
{
    return d_ptr != 0;
}

// -----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::name
// -----------------------------------------------------------------------------
QString CReporterNotifyDialogPlugin::name() const
{
    return CReporter::NotifyNewDialogType;
}

// -----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::requestDialog
// -----------------------------------------------------------------------------
bool CReporterNotifyDialogPlugin::requestDialog(const QVariantList &arguments)
{
    // Sanity check.
    if (d_ptr == 0 || d_ptr->active == true) return false;

    // Resolve arguments.
    // arguments[0] = Path to rich-core (argument received from daemon).
    if (arguments.count() != 1 || arguments.at(0).type() != QVariant::String) {
        qDebug() << __PRETTY_FUNCTION__ << "Invalid number of arguments or type:"
                << "Count:" << arguments.count() << "Type:" << arguments.at(0).type();
        return false;
    }

    // Save file path.
    d_ptr->filePath.clear();
    d_ptr->filePath = arguments.at(0).toString();

    if (d_ptr->filePath.isEmpty()) {
        qDebug() << __PRETTY_FUNCTION__ << "Invalid file path.";
        return false;
    }

    // Parse crash report details from file name.
    d_ptr->details.clear();	
    d_ptr->details = CReporterUtils::parseCrashInfoFromFilename(d_ptr->filePath);

    // Construct message for the notification.

    //% "Application %1 crashed."
    QString notificationSummary = qtTrId("qtn_application_%1_crashed_text").arg(d_ptr->details[0]);

    if (d_ptr->notification != 0) {
        // Update notification.
        d_ptr->notification->update(notificationSummary);
        d_ptr->notification->setTimeout(60);
        return true;
    }

    // Create new notification.
    //% "Tap to send crash report."
    d_ptr->notification = new CReporterNotification("crash-reporter", notificationSummary,
                                                              qtTrId("qtn_tab_to_send_crash_report_text"));
    d_ptr->notification->setTimeout(60);
    d_ptr->notification->setParent(this);

    connect(d_ptr->notification, SIGNAL(timeouted()), SLOT(notificationTimeout()));
    connect(d_ptr->notification, SIGNAL(activated()), SLOT(notificationActivated()));

    return true;
}

// -----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::isActive
// -----------------------------------------------------------------------------
bool CReporterNotifyDialogPlugin::isActive() const
{
    return d_ptr->active;
}

// -----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::actionPerformed
// -----------------------------------------------------------------------------
void CReporterNotifyDialogPlugin::actionPerformed(int buttonId)
{
    qDebug() << __PRETTY_FUNCTION__ << "Button id:" << buttonId;

    QVariantList arguments;
    QString comments;
    DuiControlPanelIf *dcp;

    switch (buttonId) {
        case CReporter::SendButton:
            // "Send" -button was pressed. Get the user comments, if any.
            comments =  d_ptr->dialog->userComments();
            if (!comments.isEmpty()) {
                // Append comments to *.lzo.
                if (!CReporterUtils::appendToLzo(comments, d_ptr->filePath)) {
                    qDebug() << __PRETTY_FUNCTION__ << "Unable to add user comments to:"
                           << d_ptr->filePath;
                }
            }
            arguments << QVariant(d_ptr->filePath);
            // Create event to upload file.
            d_ptr->server->createRequest(CReporter::UploadDialogType, arguments);
            break;
        case CReporter::DeleteButton:
            // "Delete" -button was pressed. Remove rich core from the system.
            if (!CReporterUtils::removeFile(d_ptr->filePath)) {
                qDebug() << __PRETTY_FUNCTION__ << "Unable to remove file:"  << d_ptr->filePath;
            }
            break;
         case CReporter::OptionsButton:
            // Get handle to remote Dcp interface.
            dcp = new DuiControlPanelIf("", this);

            if (dcp != 0) {
                // If valid interface, try launching Crash Reporter Settings applet.
                if (!dcp->appletPage(CrashReporterDcpApplet)) {
                    //% "Could not open Crash Reporter settings!"
                    CReporterInfoBanner::show(qtTrId("qtn_could_not_open_crash_reporter_settings_text"));
                }
            }
            else {
                // Show error.
                //% "Could not open Crash Reporter settings!"
                CReporterInfoBanner::show(qtTrId("qtn_could_not_open_crash_reporter_settings_text"));
             }
            return;
         default:
            // Unknown button.
            break;
    };

    d_ptr->active = false;
    d_ptr->dialog = 0;

    // Complete the request.
    emit requestCompleted();
    
}

// -----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::dialogFinished
// -----------------------------------------------------------------------------
void CReporterNotifyDialogPlugin::dialogFinished()
{
    qDebug() << __PRETTY_FUNCTION__ << "Dialog was rejected.";    

    d_ptr->active = false;
    d_ptr->dialog = 0;

    // Complete the request.
    emit requestCompleted();
}

// -----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::notificationTimeout
// -----------------------------------------------------------------------------
void CReporterNotifyDialogPlugin::notificationTimeout()
{
    qDebug() << __PRETTY_FUNCTION__ << "Notification timeouted.";

    delete d_ptr->notification;
    d_ptr->notification = 0;

    emit requestCompleted();
}

// -----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::notificationActivated
// -----------------------------------------------------------------------------
void CReporterNotifyDialogPlugin::notificationActivated()
{
    qDebug() << __PRETTY_FUNCTION__ << "Notification activated.";

    delete d_ptr->notification;
    d_ptr->notification = 0;

    QFileInfo fi(d_ptr->filePath);

    // Display dialog.
   d_ptr->dialog = new CReporterNotificationDialog(d_ptr->details,
                                                   CReporterApplicationSettings::instance()->
                                                   value(Server::ValueServerAddress,
                                                         QVariant(DefaultApplicationSettings::ValueServerAddressDefault)).toString(),
                                                   CReporterUtils::fileSizeToString(fi.size()));
   connect(d_ptr->dialog, SIGNAL(actionPerformed(int)), this, SLOT(actionPerformed(int)));
   connect(d_ptr->dialog, SIGNAL(rejected()), this, SLOT(dialogFinished()));

    // Become visible.
    d_ptr->server->showDialog(d_ptr->dialog);
    d_ptr->active = true;
}

Q_EXPORT_PLUGIN2(crashreportnotifydialogplugin, CReporterNotifyDialogPlugin)

// End of file.

