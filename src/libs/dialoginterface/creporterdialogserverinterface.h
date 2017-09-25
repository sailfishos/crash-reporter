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

#ifndef CREPORTERDIALOGSERVERINTERFACE_H
#define CREPORTERDIALOGSERVERINTERFACE_H

// System includes.

#include <QVariantList>

// Forward declarations.

class MRemoteAction;
class MDialog;

class CReporterDialogRequest;
class CReporterNotification;

/*!
  * @namespace DialogServerIf
  * @brief Dialog server interface namespace
  *
  */
namespace DialogServerIf {

/*!
  * @enum DialogEventType
  * @brief Event types handled by  dialog plugins
  *
  */
enum DialogEventType {
    //! Unkown event.
    eventNone = 0,
    //! Event to notify new crash report.
    eventNotifyCrashReport,
    //! Event to notify about unsent crash reports.
    eventSendAllReports,
    //! Event to upload crash reports.
    eventUploadReports,
} typedef DialogEventType;
}

/*!
  * @class CReporterDialogServerInterface
  * @brief Public interface class implemented by CReporterDialogServer.
  *
  * @note This class is pure virtual.
  * @sa CReporterDialogServer
  */
class CReporterDialogServerInterface
{
public:

    /*!
      * @brief Creates a request to dialog plugin with name @a dialogName.
      *
      * @param dialogName Name of the dialog.
      * @param arguments Request arguments passed to the plugin.
      * @return True, if request was created; otherwise false.
      *
      * @sa CReporter::SendAllDialogType
      * @sa CReporter::SendSelectedDialogType
      * @sa CReporter::NotifyNewDialogType
      * @sa CReporter::UploadDialogType
      */
    virtual bool createRequest(const QString &dialogName, const QVariantList &arguments) = 0;

    /*!
      * @brief Show @a dialog on active window.
      *
      * @note @a dialog will be application modal.
      * @param dialog MDialog to show.
      */
    virtual void showDialog(MDialog *dialog) = 0;

    /*!
      * @brief Hides and distroys @a dialog.
      *
      * @param dialog MDialog to hide.
      */
    virtual void hideDialog(MDialog *dialog) = 0;
};

Q_DECLARE_INTERFACE(CReporterDialogServerInterface,
                    "com.nokia.CReporterDialogServerInterface");

#endif // CREPORTERDIALOGSERVERINTERFACE_H

// End of file.
