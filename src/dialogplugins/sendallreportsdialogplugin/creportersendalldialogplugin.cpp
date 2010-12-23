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
#include <MWindow>
#include <QDebug>
#include <QFileInfo>
#include <MTheme>

// User includes.

#include "creportersendalldialogplugin.h"
#include "creportersendalldialog.h"
#include "creporterutils.h"
#include "creporterapplicationsettings.h"
#include "creporternamespace.h"
#include "creporternotification.h"

/*!
  * @class CReporterSendAllDialogPluginPrivate
  * @brief Private CReporterSendAllDialogPlugin class.
  *
  */
class CReporterSendAllDialogPluginPrivate
{
    public:
        //! @arg Pointer to dialog server instance.
        CReporterDialogServerInterface* server;
        //! @arg List of files.
        QStringList files;
        //! @arg Pointer to dialog instance.
        CReporterSendAllDialog *dialog;
        //! @arg Plugin state.
        bool active;
        //! @arg Notification reference.
        CReporterNotification *notification;
};

// *** Class CReporterSendAllDialogPlugin ****

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterSendAllDialogPlugin::initialize
// ----------------------------------------------------------------------------
void CReporterSendAllDialogPlugin::initialize(CReporterDialogServerInterface* server)
{
    d_ptr = new CReporterSendAllDialogPluginPrivate();
    d_ptr->server = server;
    d_ptr->active = false;
    d_ptr->notification = 0;
}

// ----------------------------------------------------------------------------
// CReporterSendAllDialogPlugin::destroy
// ----------------------------------------------------------------------------
void CReporterSendAllDialogPlugin::destroy()
{
    CReporterApplicationSettings::freeSingleton();

    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterSendAllDialogPlugin::isInitialized
// ----------------------------------------------------------------------------
bool CReporterSendAllDialogPlugin::isInitialized() const
{
    return d_ptr != 0;
}

// -----------------------------------------------------------------------------
// CReporterSendAllDialogPlugin::name
// -----------------------------------------------------------------------------
QString CReporterSendAllDialogPlugin::name() const
{
    return CReporter::SendAllDialogType;
}

// -----------------------------------------------------------------------------
// CReporterSendAllDialogPlugin::requestDialog
// -----------------------------------------------------------------------------
bool CReporterSendAllDialogPlugin::requestDialog(const QVariantList &arguments)
{
    // Sanity check.
    if (d_ptr == 0 || d_ptr->files.isEmpty() == false || d_ptr->active == true) return false;

    if (arguments.count() !=  1) {
        qDebug() << __PRETTY_FUNCTION__ << "Invalid number of arguments.";
        return false;
    }
    
    // Save file paths.
    d_ptr->files.clear();
    d_ptr->files = arguments.at(0).toStringList();

    if (d_ptr->files.isEmpty()) {
        qDebug() << __PRETTY_FUNCTION__ << "Request contained no files.";
        return false;
    }

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

    return true;
}

// -----------------------------------------------------------------------------
// CReporterSendAllDialogPlugin::isActive
// -----------------------------------------------------------------------------
bool CReporterSendAllDialogPlugin::isActive() const
{
    return d_ptr->active;
}

// -----------------------------------------------------------------------------
// CReporterSendAllDialogPlugin::actionPerformed
// -----------------------------------------------------------------------------
void CReporterSendAllDialogPlugin::actionPerformed(int buttonId)
{
    qDebug() << __PRETTY_FUNCTION__ << "Button id:" << buttonId;
    QVariantList arguments;

    switch (buttonId) {
        case CReporter::SendAllButton:
            qDebug() << __PRETTY_FUNCTION__ << "User requested to send all. Create new upload request.";
            // "Send" -button was pressed.        
            arguments << QVariant(d_ptr->files);
            // Create event to upload file.
            d_ptr->server->createRequest(CReporter::UploadDialogType, arguments);
            break;
        case CReporter::DeleteAllButton:
            qDebug() << __PRETTY_FUNCTION__ << "User requested to delete all files.";
            // "Delete" -button was pressed. Remove all rich cores from the system.
            foreach(QString file, d_ptr->files) {
                if (!CReporterUtils::removeFile(file)) {
                    qDebug() << __PRETTY_FUNCTION__ << "Unable to remove file:"  << file;
                }
            }
            break;
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
// CReporterSendAllDialogPlugin::dialogFinished
// -----------------------------------------------------------------------------
void CReporterSendAllDialogPlugin::dialogFinished()
{
    qDebug() << __PRETTY_FUNCTION__ << "Dialog was rejected.";

    d_ptr->active = false;
    d_ptr->dialog = 0;

    // Complete the request.
    emit requestCompleted();
}

// -----------------------------------------------------------------------------
// CReporterSendAllDialogPlugin::notificationTimeout
// -----------------------------------------------------------------------------
void CReporterSendAllDialogPlugin::notificationTimeout()
{
    qDebug() << __PRETTY_FUNCTION__ << "Notification timeouted.";
    
    delete d_ptr->notification;
    d_ptr->notification = 0;
    
    emit requestCompleted();
}

// -----------------------------------------------------------------------------
// CReporterSendAllDialogPlugin::notificationActivated
// -----------------------------------------------------------------------------
void CReporterSendAllDialogPlugin::notificationActivated()
{
    qDebug() << __PRETTY_FUNCTION__ << "Notification activated.";

    delete d_ptr->notification;
    d_ptr->notification = 0;

    // Count total kB's.
    qint64 total = 0;
    foreach(QString file, d_ptr->files) {
        QFileInfo fi(file);
         total += fi.size();
    }
    // Create dialog.
    d_ptr->dialog = new CReporterSendAllDialog(d_ptr->files,
                                               CReporterApplicationSettings::instance()->
                                               value(Server::ValueServerAddress,
                                                     QVariant(DefaultApplicationSettings::ValueServerAddressDefault)).toString(),
                                               CReporterUtils::fileSizeToString(total));

    connect(d_ptr->dialog, SIGNAL(actionPerformed(int)), this, SLOT(actionPerformed(int)));
    connect(d_ptr->dialog, SIGNAL(rejected()), this, SLOT(dialogFinished()));

    // Become visible.
    d_ptr->server->showDialog(d_ptr->dialog);
}

Q_EXPORT_PLUGIN2(sendallreportsdialogplugin, CReporterSendAllDialogPlugin)

// End of file.

