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

// User includes.

#include "creporterautouploaderdbusadaptor.h"

// ======== MEMBER FUNCTIONS ========

//-----------------------------------------------------------------------------
// CReporterAutoUploaderDBusAdaptor::CReporterAutoUploaderDBusAdaptor
// ----------------------------------------------------------------------------
CReporterAutoUploaderDBusAdaptor::CReporterAutoUploaderDBusAdaptor(QObject *parent) :
        QDBusAbstractAdaptor(parent)
{
}

//-----------------------------------------------------------------------------
// CReporterAutoUploaderDBusAdaptor::~CReporterAutoUploaderDBusAdaptor
// ----------------------------------------------------------------------------
CReporterAutoUploaderDBusAdaptor::~CReporterAutoUploaderDBusAdaptor()
{
}

//-----------------------------------------------------------------------------
// CReporterAutoUploaderDBusAdaptor::uploadFiles
// ----------------------------------------------------------------------------
bool CReporterAutoUploaderDBusAdaptor::uploadFiles(const QStringList &fileList, const QDBusMessage &message)
{
    qDebug() << __PRETTY_FUNCTION__ << "Received D-Bus call 'uploadFiles'.";
    bool success;
    QDBusMessage reply;

    // Handle method call com.nokia.Maemo.CrashReporter.AutoUploader.uploadFiles
    QMetaObject::invokeMethod(parent(), "uploadFiles",
                              Q_RETURN_ARG(bool, success),
                              Q_ARG(QStringList, fileList));

    if (success) {
        reply = message.createReply();
        reply << true;
    }
    else {
        // Create error reply.
        reply = message.createErrorReply(QDBusError::InternalError, "Failed to add files to the upload queue.");
        reply << false;
    }

    // Reply to the sender.
    QDBusConnection::sessionBus().send(reply);
    // Return arbitrary value to satisfy compiler.
    return success;
}

//-----------------------------------------------------------------------------
// CReporterAutoUploaderDBusAdaptor::quit
// ----------------------------------------------------------------------------
void CReporterAutoUploaderDBusAdaptor::quit()
{
    qDebug() << __PRETTY_FUNCTION__ << "Received D-Bus call 'quit'.";
    // Handle method call com.nokia.Maemo.CrashReporter.AutoUploader.quit
    QMetaObject::invokeMethod(parent(), "quit");
}

