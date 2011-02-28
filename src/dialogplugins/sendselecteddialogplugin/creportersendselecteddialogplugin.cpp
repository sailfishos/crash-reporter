/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Carol Rus <carol.rus@digia.com>
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
#include <MWindow>
#include <QDebug>
#include <QFileInfo>

// User includes.

#include "creportersendselecteddialogplugin.h"
#include "creportersendselecteddialog.h"
#include "creporterutils.h"
#include "creporterapplicationsettings.h"
#include "creporternamespace.h"
#include "creporternotification.h"

/*!
  * @class CReporterSendSelectedDialogPluginPrivate
  * @brief Private CReporterSendSelectedDialogPlugin class.
  *
  */
class CReporterSendSelectedDialogPluginPrivate
{
    public:
        //! @arg Pointer to dialog server instance.
        CReporterDialogServerInterface* server;
        //! @arg List of files.
        QStringList files;
        //! @arg Pointer to dialog instance.
        CReporterSendSelectedDialog *dialog;
        //! @arg Plugin state.
        bool active;
        //! @arg Notification reference.
        CReporterNotification *notification;
};

// *** Class CReporterSendSelectedDialogPlugin ****

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterSendSelectedDialogPlugin::initialize
// ----------------------------------------------------------------------------
void CReporterSendSelectedDialogPlugin::initialize(CReporterDialogServerInterface* server)
{
    d_ptr = new CReporterSendSelectedDialogPluginPrivate();
    d_ptr->server = server;
    d_ptr->active = false;
    d_ptr->notification = 0;
    d_ptr->dialog = 0;
}

// ----------------------------------------------------------------------------
// CReporterSendSelectedDialogPlugin::destroy
// ----------------------------------------------------------------------------
void CReporterSendSelectedDialogPlugin::destroy()
{
    CReporterApplicationSettings::freeSingleton();

    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterSendSelectedDialogPlugin::isInitialized
// ----------------------------------------------------------------------------
bool CReporterSendSelectedDialogPlugin::isInitialized() const
{
    return d_ptr != 0;
}

// -----------------------------------------------------------------------------
// CReporterSendSelectedDialogPlugin::name
// -----------------------------------------------------------------------------
QString CReporterSendSelectedDialogPlugin::name() const
{
    return CReporter::SendSelectedDialogType;
}

// -----------------------------------------------------------------------------
// CReporterSendSelectedDialogPlugin::requestDialog
// -----------------------------------------------------------------------------
bool CReporterSendSelectedDialogPlugin::requestDialog(const QVariantList &arguments)
{
    // Sanity check.
    if (d_ptr == 0 || d_ptr->active == true) return false;

    if (arguments.count() !=  2) {
        qDebug() << __PRETTY_FUNCTION__ << "Invalid number of arguments. ";
        return false;
    }
    
    // Save file paths.
    d_ptr->files.clear();
    d_ptr->files = arguments.at(0).toStringList();

    if (d_ptr->files.isEmpty()) {
        qDebug() << __PRETTY_FUNCTION__ << "Request contained no files.";
        return false;
    }

    bool openViaNotification = arguments.at(1).toBool();

    if (openViaNotification) {

        //% "This system has stored crash reports."
        QString notificationSummary = qtTrId("qtn_this_system_has_stored_crash_reports_text");
        //% "Tap to send or delete."
        QString notificationBody = qtTrId("qtn_tap_to_send_or_delete_text");

        // Create new notification.
        d_ptr->notification = new CReporterNotification("crash-reporter", notificationSummary,
                                                                  notificationBody);
        d_ptr->notification->setTimeout(60);
        d_ptr->notification->setParent(this);

        connect(d_ptr->notification, SIGNAL(timeouted()), SLOT(notificationTimeout()));
        connect(d_ptr->notification, SIGNAL(activated()), SLOT(notificationActivated()));

        d_ptr->active = true;

    } else {
        d_ptr->active = true;
        notificationActivated();
    }


    return true;
}

// -----------------------------------------------------------------------------
// CReporterSendSelectedDialogPlugin::isActive
// -----------------------------------------------------------------------------
bool CReporterSendSelectedDialogPlugin::isActive() const
{
    return d_ptr->active;
}

// -----------------------------------------------------------------------------
// CReporterSendSelectedDialogPlugin::isVisible
// -----------------------------------------------------------------------------
bool CReporterSendSelectedDialogPlugin::isVisible() const
{
    return (d_ptr->dialog != 0);
}

// -----------------------------------------------------------------------------
// CReporterSendSelectedDialogPlugin::actionPerformed
// -----------------------------------------------------------------------------
void CReporterSendSelectedDialogPlugin::actionPerformed(int buttonId)
{
    qDebug() << __PRETTY_FUNCTION__ << "Button id:" << buttonId;
    QVariantList arguments;
    QString errorMessage;
    int deletedFiles = 0;
    int failedFiles = 0;


    switch (buttonId) {
    case CReporter::SendSelectedButton:
        qDebug() << __PRETTY_FUNCTION__ << "User requested to send selected. Create new upload request.";
        d_ptr->files = d_ptr->dialog->getSelectedFiles();
        qDebug() << __PRETTY_FUNCTION__ << "# of files selected: " << d_ptr->files.size();
        arguments << QVariant(d_ptr->files);
        // Create event to upload files.
        d_ptr->server->createRequest(CReporter::UploadDialogType, arguments);
        break;

    case CReporter::SendAllButton:
        qDebug() << __PRETTY_FUNCTION__ << "User requested to send all. Create new upload request.";
        arguments << QVariant(d_ptr->files);
        // Create event to upload files.
        d_ptr->server->createRequest(CReporter::UploadDialogType, arguments);
        break;

    case CReporter::DeleteSelectedButton:
        qDebug() << __PRETTY_FUNCTION__ << "User requested to delete selected files.";
        d_ptr->files = d_ptr->dialog->getSelectedFiles();
        qDebug() << __PRETTY_FUNCTION__ << "# of files selected: " << d_ptr->files.size();
        // "Delete" -button was pressed. Remove selected rich cores from the system.
        foreach(QString file, d_ptr->files) {
            if (!CReporterUtils::removeFile(file)) {
                qDebug() << __PRETTY_FUNCTION__ << "Unable to remove file:"  << file;
                failedFiles++;
            }
            deletedFiles++;
        }
        if (failedFiles) {
            if (deletedFiles == 1) {
                //% "Failed to delete 1 out of 1 file."
                errorMessage = qtTrId("qtn_failed_to_delete_1_out_of_1_file");
            } else {
                //% "Failed to delete %1 out of %2 files."
                errorMessage = qtTrId("qtn_failed_to_delete_%1_out_of_%2_files").arg(failedFiles).arg(deletedFiles);
            }
        } else {
            if (deletedFiles == 1) {
                //% "Successfully deleted 1 file."
                errorMessage = qtTrId("qtn_successfully_deleted_1_file");
            } else {
                //% "Successfully deleted %1 files."
                errorMessage = qtTrId("qtn_successfully_deleted_%1_files").arg(deletedFiles);
            }
        }
        arguments.clear();
        arguments << errorMessage;
        d_ptr->server->createRequest(CReporter::MessageBoxDialogType, arguments);

        break;

        default:
        // Unknown button.
        break;
    };
}


// -----------------------------------------------------------------------------
// CReporterSendSelectedDialogPlugin::dialogFinished
// -----------------------------------------------------------------------------
void CReporterSendSelectedDialogPlugin::dialogFinished()
{
    qDebug() << __PRETTY_FUNCTION__ << "Dialog was closed.";

    d_ptr->active = false;
    d_ptr->dialog->deleteLater();
    d_ptr->dialog = 0;

    // Complete the request.
    emit requestCompleted();
}

// -----------------------------------------------------------------------------
// CReporterSendSelectedDialogPlugin::notificationTimeout
// -----------------------------------------------------------------------------
void CReporterSendSelectedDialogPlugin::notificationTimeout()
{
    qDebug() << __PRETTY_FUNCTION__ << "Notification timeouted.";
    
    delete d_ptr->notification;
    d_ptr->notification = 0;
    d_ptr->active = false;
    
    emit requestCompleted();
}

// -----------------------------------------------------------------------------
// CReporterSendSelectedDialogPlugin::notificationActivated
// -----------------------------------------------------------------------------
void CReporterSendSelectedDialogPlugin::notificationActivated()
{
    qDebug() << __PRETTY_FUNCTION__ << "Notification activated.";

    if (d_ptr->notification)
    {
        delete d_ptr->notification;
        d_ptr->notification = 0;
    }

    // Create dialog.
    d_ptr->dialog = new CReporterSendSelectedDialog(d_ptr->files,
                                               CReporterApplicationSettings::instance()->
                                               value(Server::ValueServerAddress,
                                               QVariant(DefaultApplicationSettings::ValueServerAddressDefault)).toString());

    connect(d_ptr->dialog, SIGNAL(actionPerformed(int)), this, SLOT(actionPerformed(int)));
    connect(d_ptr->dialog, SIGNAL(disappeared()), this, SLOT(dialogFinished()));

    // Become visible.
    d_ptr->server->showDialog(d_ptr->dialog);
}

void CReporterSendSelectedDialogPlugin::setFileList(QStringList files)
{
    d_ptr->files = files;
}


Q_EXPORT_PLUGIN2(sendselectedreportsdialogplugin, CReporterSendSelectedDialogPlugin)

// End of file.

