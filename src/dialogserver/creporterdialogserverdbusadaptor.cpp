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

// User includes.

#include "creporterdialogserverdbusadaptor.h"
#include <QDebug>
// ======== MEMBER FUNCTIONS ========

//-----------------------------------------------------------------------------
// CReporterDialogServerDBusAdaptor::CReporterDialogServerDBusAdaptor
// ----------------------------------------------------------------------------
CReporterDialogServerDBusAdaptor::CReporterDialogServerDBusAdaptor(QObject *parent) :
        QDBusAbstractAdaptor(parent)
{
}

//-----------------------------------------------------------------------------
// CReporterDialogServerDBusAdaptor::~CReporterDialogServerDBusAdaptor
// ----------------------------------------------------------------------------
CReporterDialogServerDBusAdaptor::~CReporterDialogServerDBusAdaptor()
{
}

//-----------------------------------------------------------------------------
// CReporterDialogServerDBusAdaptor::requestDialog
// ----------------------------------------------------------------------------
bool CReporterDialogServerDBusAdaptor::requestDialog(const QString &dialogName,
                                                const QList<QVariant> &arguments, const QDBusMessage &message)
{
    QDBusError::ErrorType error;
    QDBusMessage reply;

    // Handle method call com.nokia.Maemo.CrashReporter.DialogServer.requestDialog
    QMetaObject::invokeMethod(parent(), "callReceived",
                              Q_RETURN_ARG(QDBusError::ErrorType, error),
                              Q_ARG(QString, dialogName),
                              Q_ARG(QVariantList, arguments),
                              Q_ARG(QDBusMessage, message));

    if (error == QDBusError::NoError) {
        reply = message.createReply();
        reply << true;
    }
    else {
        // Create error reply.
        reply = message.createErrorReply(error, "");
        reply << false;
    }

    // Reply to the sender.
    QDBusConnection::sessionBus().send(reply);
    // Return arbitrary value to satisfy compiler.
    return error == QDBusError::NoError ? true : false;
}

//-----------------------------------------------------------------------------
// CReporterDialogServerDBusAdaptor::quit()
// ----------------------------------------------------------------------------
void CReporterDialogServerDBusAdaptor::quit()
{
    // Handle method call com.nokia.Maemo.CrashReporter.DialogServer.quit
    QMetaObject::invokeMethod(parent(), "quit");
}

// End of file.
