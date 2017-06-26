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

#include <MApplicationWindow>
#include <MApplicationPage>

// User includes.

#include "creporteruploaddialogplugin.h"
#include "creporteruploaddialog.h"
#include "creporteruploadqueue.h"
#include "creporteruploaditem.h"
#include "creporterutils.h"
#include "creporternamespace.h"
#include "creporteruploadengine.h"

using CReporter::LoggingCategory::cr;

const QString messageBoxTitle = "Upload finished";

/*! @class CReporterUploadDialogPluginPrivate
  * @brief Private CReporterUploadDialogPlugin class.
  *
  * @sa CReporterUploadDialogPlugin
  */
class CReporterUploadDialogPluginPrivate
{
    public:
        //! @arg Dialog server reference.
        CReporterDialogServerInterface *server;
        //! @arg Upload engine reference.
        CReporterUploadEngine *engine;
        //! @arg Upload queue.
        CReporterUploadQueue queue;
        //! @arg Dialog reference.
        CReporterUploadDialog *dialog;
        //! @arg Plugin state.
        bool active;
        //! @arg List of files to be added to upload queue
        QStringList files;
};

// *** Class CReporterUploadDialogPlugin ****

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterUploadDialogPlugin::initialize
// ----------------------------------------------------------------------------
void CReporterUploadDialogPlugin::initialize(CReporterDialogServerInterface* server)
{
    d_ptr = new CReporterUploadDialogPluginPrivate();
    d_ptr->server = server;

    d_ptr->dialog = 0;
    d_ptr->active =false;
    d_ptr->engine = 0;
}

// ----------------------------------------------------------------------------
// CReporterUploadDialogPlugin::destroy
// ----------------------------------------------------------------------------
void CReporterUploadDialogPlugin::destroy()
{
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterUploadDialogPlugin::isInitialized
// ----------------------------------------------------------------------------
bool CReporterUploadDialogPlugin::isInitialized() const
{
    return d_ptr != 0;
}

// -----------------------------------------------------------------------------
// CReporterUploadDialogPlugin::name
// -----------------------------------------------------------------------------
QString CReporterUploadDialogPlugin::name() const
{
    return CReporter::UploadDialogType;
}

// -----------------------------------------------------------------------------
// CReporterUploadDialogPlugin::requestDialog
// -----------------------------------------------------------------------------
bool CReporterUploadDialogPlugin::requestDialog(const QVariantList &arguments)
{
    // Sanity check.
    if (d_ptr == 0) return false;

    if (arguments.count() != 1) {
        qCDebug(cr) << "Invalid number of arguments.";
        return false;
    }

    QStringList files;
    // Validate argument type.
    if (arguments.at(0).type() == QVariant::String) {
        files << arguments.at(0).toString();
    }
    else if (arguments.at(0).type() == QVariant::StringList) {
        files = arguments.at(0).toStringList();
    }
    else {
        qCDebug(cr) << "Invalid argument type";
        return false;
    }

    if (files.isEmpty()) {
        qCDebug(cr) << "Request contained no files.";
        return false;
    }

    d_ptr->active =true;

    if (d_ptr->dialog == 0) {
        // Create dialog.
        d_ptr->dialog = new CReporterUploadDialog(&d_ptr->queue);
        connect(d_ptr->dialog, SIGNAL(actionPerformed(int)), this, SLOT(actionPerformed(int)));
        connect(d_ptr->dialog, SIGNAL(rejected()), this, SLOT(dialogRejected()));
        connect(d_ptr->dialog, SIGNAL(appeared()), this, SLOT(dialogAppeared()));
    }

    if (d_ptr->engine == 0) {
        // Create engine.
        d_ptr->engine = new CReporterUploadEngine(&d_ptr->queue);

        connect(d_ptr->engine, SIGNAL(finished(int, int, int)),
               this, SLOT(engineFinished(int, int, int)));
    }

    // Save files to be upload. These are queued after dialog has finished its
    // appearance transition.
    d_ptr->files = files;
    d_ptr->server->showDialog(d_ptr->dialog);

    return true;
}

// -----------------------------------------------------------------------------
// CReporterUploadDialogPlugin::isActive
// -----------------------------------------------------------------------------
bool CReporterUploadDialogPlugin::isActive() const
{
    return d_ptr->active;
}

// -----------------------------------------------------------------------------
// CReporterUploadDialogPlugin::isVisible
// -----------------------------------------------------------------------------
bool CReporterUploadDialogPlugin::isVisible() const
{
    return isActive();
}

// -----------------------------------------------------------------------------
// CReporterUploadDialogPlugin::actionPerformed
// -----------------------------------------------------------------------------
void CReporterUploadDialogPlugin::actionPerformed(int buttonId)
{
    qCDebug(cr) << "Button id:" << buttonId;

    switch (buttonId) {
        case CReporter::CancelButton:
            qCDebug(cr) << "User requested to cancel upload.";
            if (d_ptr->engine) d_ptr->engine->cancelAll();
            break;
        default:
            // Unknown button.
           break;
    };
}

// -----------------------------------------------------------------------------
// CReporterUploadDialogPlugin::dialogRejected
// -----------------------------------------------------------------------------
void CReporterUploadDialogPlugin::dialogRejected()
{
    qCDebug(cr) << "User has rejected the dialog.";
    if (d_ptr->engine) d_ptr->engine->cancelAll();
}

// -----------------------------------------------------------------------------
// CReporterUploadDialogPlugin::engineFinished
// -----------------------------------------------------------------------------
void CReporterUploadDialogPlugin::engineFinished(int error, int sent, int total)
{
    QString message;

    // Construct message.
    switch (error) {
        case CReporterUploadEngine::NoError:
            //% "%1 crash report(s) uploaded successfully."
            message = qtTrId("qtn_%1_crash_reports_uploaded_successfully_text").arg(total);
            break;
        case CReporterUploadEngine::ProtocolError:
        case CReporterUploadEngine::ConnectionNotAvailable:
        case CReporterUploadEngine::ConnectionClosed:
            //% "Failed to upload crash report(s)."
            message = qtTrId("qtn_failed_to_send_crash_reports_text");
            //% "<br>Upload results: %1 files attempted %2 files succeeded."
            message += qtTrId("qtn_%1_files_attempted_%2_files_succeeded_text").arg(total).arg(sent);
            break;
        default:
        // We should never enter here.
        break;
    };

    if (error != CReporterUploadEngine::NoError) {
        // Add error reason.
        if (error == CReporterUploadEngine::ConnectionNotAvailable) {
            //% "<br>Reason: Failed to create internet connection."
            message += qtTrId("qtn_reason_failed_to_create_internet_connection_text");
        }
        else {
            QString reason = d_ptr->engine->lastError();
            if (!reason.isEmpty()) {
                // If reason is available, append it.
                //% "<br>Reason: %1."
                message += qtTrId("qtn_reason_%1_text").arg(reason);
            }
            else {
                // Use default error.
                //% "<br>Reason: Unknown."
                message += qtTrId("qtn_reason_unknown_text");
            }
        }
    }

    QVariantList arguments;
    arguments << message;

    d_ptr->server->createRequest(CReporter::MessageBoxDialogType, arguments);
    connect(d_ptr->dialog, SIGNAL(disappeared()), this, SLOT(dialogFinished()));
    d_ptr->server->hideDialog(d_ptr->dialog);

    d_ptr->engine->deleteLater();
    d_ptr->engine = 0;
}

// -----------------------------------------------------------------------------
// CReporterUploadDialogPlugin::dialogFinished
// -----------------------------------------------------------------------------
void CReporterUploadDialogPlugin::dialogFinished()
{
    qCDebug(cr) << "Dialog disappeared.";

    d_ptr->dialog = 0;
    d_ptr->active = false;

    // Complete the request.
    emit requestCompleted();
}

// -----------------------------------------------------------------------------
// CReporterUploadDialogPlugin::dialogAppeared
// -----------------------------------------------------------------------------
void CReporterUploadDialogPlugin::dialogAppeared()
{
    foreach(QString file, d_ptr->files) {
        // CReporterUploadQueue class will own the CReporterUploadItem instance.
       d_ptr->queue.enqueue(new CReporterUploadItem(file));
    }

    d_ptr->files.empty();
}

Q_EXPORT_PLUGIN2(uploadprogressdialogplugin, CReporterUploadDialogPlugin)

// End of file.

