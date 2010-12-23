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

#ifndef CREPORTERNAMESPACE_H
#define CREPORTERNAMESPACE_H

#include <QString>

/*!
  * @namespace CReporter
  * @brief Common public definitions for Crash Reporter.
  *
  */
namespace CReporter {

    // Dialog button Id's.
    /*!
      * @enum DialogButton
      * @brief Identifiers for the dialog buttons.
      */
      enum DialogButton{
        //! "Delete" button.
        DeleteButton = 0,
        //! "Send" button.
        SendButton,
        //! "Options" button.
        OptionsButton,
        //! "Delete All" button.
        DeleteAllButton,
        //! "Cancel" button.
        CancelButton,
        //! "Send All" button.
        SendAllButton,
        //! "Delete selected" button.
        DeleteSelectedButton,
        //! "Send selected" button.
        SendSelectedButton,
        };

        //! User location for application settings. WRITABLE by user. First part of the path is composed in code.
        const QString UserSettingsLocation = "/.config/crash-reporter-settings";

        //! System (default) location for application settings. NOT writable by user.
        const QString SystemSettingsLocation = "/usr/share";

        //! User privacy settings file. First part of the path is composed in code.
        const QString PrivacySettingsFileUser =
                "/.config/crash-reporter-settings/crash-reporter-privacy.conf";

        //! System privacy settings file.
        const QString PrivacySettingsFileSystem =
                "/usr/share/crash-reporter-settings/crash-reporter-privacy.conf";

        //! Privacy disclaimer file.
        const QString PrivacyDisclaimerFile =  "/usr/share/crash-reporter-settings/privacy.txt";

        //! Default log file.
        const QString DefaultLogFile = "/tmp/crash-reporter.log";

#ifndef CREPORTER_UNIT_TEST
        //! Dialog server service name
        const QString DialogServerServiceName = "com.nokia.CrashReporter.DialogServer";

        //! Dialog server object path.
        const QString DialogServerObjectPath =  "/com/nokia/crashreporter/dialogserver";

        //! Daemon service name
        const QString DaemonServiceName = "com.nokia.CrashReporter.Daemon";

        //! Daemon object path.
        const QString DaemonObjectPath =  "/com/nokia/crashreporter/daemon";

        //! Auto uploader service name
        const QString AutoUploaderServiceName = "com.nokia.CrashReporter.AutoUploader";

        //! Auto uploader object path.
        const QString AutoUploaderObjectPath =  "/com/nokia/crashreporter/autouploader";
#else
        //! Dialog server service name
        const QString DialogServerServiceName = "com.nokia.CrashReporter.DialogServer.Ut";

        //! Dialog server object path.
        const QString DialogServerObjectPath =  "/com/nokia/crashreporter/dialogserver/ut";

        //! Daemon service name
        const QString DaemonServiceName = "com.nokia.CrashReporter.Daemon.Ut";

        //! Daemon object path.
        const QString DaemonObjectPath =  "/com/nokia/crashreporter/daemon/ut";

        //! Auto uploader service name
        const QString AutoUploaderServiceName = "com.nokia.CrashReporter.AutoUploader.Ut";

        //! Auto uploader object path.
        const QString AutoUploaderObjectPath =  "/com/nokia/crashreporter/autouploader/ut";
#endif // CREPORTER_UNIT_TEST
        
        //! Crash Reporter UI binary name.
        const QString UIBinaryName = "crash-reporter-ui";

        //! Crash Reporter daemon binary name.
        const QString DaemonBinaryName = "crash-reporter-daemon";
        
        //! Crash Reporter Auto Uploader binary name.
        const QString AutoUploaderBinaryName = "crash-reporter-autouploader";

        //! Dialog name for sending all crash reports.
        const QString SendAllDialogType = "send-all-dialog";

        //! Dialog name for sending selected crash reports.
        const QString SendSelectedDialogType = "send-selected-dialog";

        //! Dialog name for notifying new crash reports.
        const QString NotifyNewDialogType = "notify-new-dialog";

        //! Dialog name for uploading crash reports.
        const QString UploadDialogType = "upload-dialog";

        //! Dialog name for showing message box.
        const QString MessageBoxDialogType = "msgbox-dialog";

        /*!
          * @enum LogType
          * @brief Logging method.
          */
       typedef enum {
            //! No logging.
            LogNone = 0,
            //! Use syslog for logging.
            LogSyslog,
            //! Log to a file.
            LogFile,
        } LogType;
}

#endif // CREPORTERNAMESPACE_H

// End of file.
