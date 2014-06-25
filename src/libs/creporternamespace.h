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

        //! Crash Reporter daemon binary name.
        const QString DaemonBinaryName = "crash-reporter-daemon";
        
        //! Crash Reporter Auto Uploader binary name.
        const QString AutoUploaderBinaryName = "crash-reporter-autouploader";

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

       //! Prefix for message and core packages created by Quick Feedback
       const QString QuickFeedbackPrefix = "Quickie";

       //! Prefix for pack of endurance snapshots
       const QString EndurancePackagePrefix = "Endurance";

       //! Prefix for system log packages created on power excess uevent.
       const QString PowerExcessPrefix = "PowerExcess";

       /// Prefix for system log packages created when oneshot script failure is
       /// detected.
       const QString OneshotFailurePrefix = "OneshotFailure";

       /// Prefix for system log packages created when shutdown from overheating
       /// is detected.
       const QString OverheatShutdownPrefix = "OverheatShutdown";

       // Prefixes for HW reboot logs.
       const QString HWrebootPrefix = "HWreboot";
       const QString HWSMPLPrefix = "HWSMPL";

       //! Event type for crash reporter notifications
       const QString ApplicationNotificationEventType = "x-nokia.crash-reporter.notification";

       //! Event type for autouploader notifications
       const QString AutoUploaderNotificationEventType = "x-nokia.crash-reporter.autouploader";
}

#endif // CREPORTERNAMESPACE_H

// End of file.
