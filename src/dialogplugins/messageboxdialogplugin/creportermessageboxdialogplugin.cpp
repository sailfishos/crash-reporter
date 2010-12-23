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

#include <MTheme>
#include <MMessageBox>

// User includes.

#include "creportermessageboxdialogplugin.h"
#include "creporternamespace.h"

/*!
  * @class CReporterMessageBoxDialogPluginPrivate
  * @brief Private CReporterMessageBoxDialogPlugin class.
  *
  */
class CReporterMessageBoxDialogPluginPrivate
{
    public:
        //! @arg Pointer to dialog server instance.
        CReporterDialogServerInterface *server;
        //! @arg Plugin state.
        bool active;
        //! @arg Dialog reference.
        MMessageBox *dialog;
};

// *** Class CReporterMessageBoxDialogPlugin ****

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterMessageBoxDialogPlugin::initialize
// ----------------------------------------------------------------------------
void CReporterMessageBoxDialogPlugin::initialize(CReporterDialogServerInterface *server)
{
    d_ptr = new CReporterMessageBoxDialogPluginPrivate();
    d_ptr->server = server;
    d_ptr->active =false;
}

// ----------------------------------------------------------------------------
// CReporterMessageBoxDialogPlugin::destroy
// ----------------------------------------------------------------------------
void CReporterMessageBoxDialogPlugin::destroy()
{
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterMessageBoxDialogPlugin::isInitialized
// ----------------------------------------------------------------------------
bool CReporterMessageBoxDialogPlugin::isInitialized() const
{
    return d_ptr != 0;
}

// -----------------------------------------------------------------------------
// CReporterMessageBoxDialogPlugin::name
// -----------------------------------------------------------------------------
QString CReporterMessageBoxDialogPlugin::name() const
{
    return CReporter::MessageBoxDialogType;
}

// -----------------------------------------------------------------------------
// CReporterMessageBoxDialogPlugin::requestDialog
// -----------------------------------------------------------------------------
bool CReporterMessageBoxDialogPlugin::requestDialog(const QVariantList &arguments)
{
    // Sanity check.
    if (d_ptr == 0 || d_ptr->active == true) return false;

    // Resolve arguments.
    if (arguments.count() != 1 || arguments.at(0).type() != QVariant::String) {
        qDebug() << __PRETTY_FUNCTION__ << "Invalid number of arguments or type:"
                << "Count:" << arguments.count() << "Type:" << arguments.at(0).type();
        return false;
    }

    // Show message box.
    //% "Crash Reporter"
    d_ptr->dialog = new MMessageBox(qtTrId("qtn_crash_reporter_text"),
                                      arguments.at(0).toString(), M::OkButton);
    d_ptr->server->showDialog(d_ptr->dialog);
    connect(d_ptr->dialog, SIGNAL(disappeared()), this, SLOT(dialogFinished()));

    // Set active.
    d_ptr->active = true;
    return true;
}

// -----------------------------------------------------------------------------
// CReporterMessageBoxDialogPlugin::isActive
// -----------------------------------------------------------------------------
bool CReporterMessageBoxDialogPlugin::isActive() const
{
    return d_ptr->active;
}

// -----------------------------------------------------------------------------
// CReporterMessageBoxDialogPlugin::dialogFinished
// -----------------------------------------------------------------------------
void CReporterMessageBoxDialogPlugin::dialogFinished()
{
    qDebug() << __PRETTY_FUNCTION__ << "Dialog disappeared; close window";

    d_ptr->active = false;
    d_ptr->dialog = 0;

    // Complete the request.
    emit requestCompleted();
}

Q_EXPORT_PLUGIN2(crashreportnotifydialogplugin, CReporterMessageBoxDialogPlugin)

// End of file.

