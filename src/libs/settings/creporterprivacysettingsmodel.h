/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Riku Halonen <riku.halonen@nokia.com>
 *
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Jakub Adam <jakub.adam@jollamobile.com>
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

#ifndef CREPORTERPRIVACYSETTINGSMODEL_H
#define CREPORTERPRIVACYSETTINGSMODEL_H

#include "creportersettingsbase.h"

/*!
  * @namespace Settings
  * @brief Settings affecting to application behiour.
  *
  */
namespace Settings
{
    //! This setting affects to sp-rich-core. If set to true, rich-core dumps are produced.
    const QString ValueCoreDumping("Settings/coredumping");
    //! This setting affects to crash-reporter. If set to true, user notifications are displayed.
    const QString ValueNotifications("Settings/notifications");
    //! This setting affects to crash-reporter. If set to true, duplicate rich-core are deleted automatically.
    const QString ValueAutoDeleteDuplicates("Settings/avoid-dups");
    //! Number of how many similar core dumps are kept when AutoDeleteDuplicates is enabled
    const QString ValueAutoDeleteMaxSimilarCores("Settings/maxsimilarcores");
    //! This setting affects to crash-reporter. If set to true, crash-reporter tries to upload rich-core dumps automatically.
    const QString ValueAutomaticSending("Settings/automaticsending");
    //! If set to true, user has accepted crash reporter's privacy notice.
    const QString ValueNoticeAccepted("Settings/privacy-notice-accepted");
}

/*!
  * @namespace Privacy
  * @brief Settings affecting, what information is included in the crash reports.
  *
  */
namespace Privacy {
    //! If set to true, core dump is included in the crash report.
    const QString ValueIncludeCore("Privacy/INCLUDE_CORE");
     //! If set to true, syslog is included in the crash reporter.
    const QString ValueIncludeSysLog("Privacy/INCLUDE_SYSLOG");
     //! If set to true, package list is included in the crash reporter.
    const QString ValueIncludePkgList("Privacy/INCLUDE_PKGLIST");
    //! If set to true, stack trace is included in the crash report.
    const QString ValueIncludeStackTrace("Privacy/INCLUDE_STACK_TRACE");
     //! If set to true, core dump size is reduced before included in the crash reporter.
    const QString ValueReduceCore("Privacy/REDUCE_CORE");
    /*! If set to true, rich-core-dumper will attempt to download missing debug
     * symbols before generating a stack trace. */
    const QString ValueDownloadDebuginfo("Privacy/DOWNLOAD_DEBUGINFO");
}

/*!
  * @class CReporterPrivacySettingsModel
  * @brief This a singleton class which reads and writes crash-reporter privacy settings.
  *
  * System (default) settings are stored in /usr/share/crash-reporter-settings/crash-reporter-privacy.conf
  * and not writable by user. User defined settings are stored in
  * $HOME/.config/crash-reporter-settings/crash-reporter-privacy.conf and editable from
  * control panel settings applet.
  */
class CREPORTER_EXPORT CReporterPrivacySettingsModel : public CReporterSettingsBase
{
    Q_OBJECT

    Q_PROPERTY(bool coreDumping READ coreDumpingEnabled WRITE setCoreDumpingEnabled NOTIFY coreDumpingEnabledChanged)
    Q_PROPERTY(bool endurance READ enduranceEnabled WRITE setEnduranceEnabled NOTIFY enduranceEnabledChanged)
    Q_PROPERTY(bool notifications READ notificationsEnabled WRITE setNotificationsEnabled NOTIFY notificationsEnabledChanged)
    Q_PROPERTY(bool autoDeleteDuplicates READ autoDeleteDuplicates WRITE setAutoDeleteDuplicates NOTIFY autoDeleteDuplicatesChanged)
    Q_PROPERTY(bool includeStackTrace READ includeStackTrace WRITE setIncludeStackTrace NOTIFY includeStackTraceChanged)
    Q_PROPERTY(bool downloadDebuginfo READ downloadDebuginfo WRITE setDownloadDebuginfo NOTIFY downloadDebuginfoChanged)
    Q_PROPERTY(bool privacyNoticeAccepted READ privacyNoticeAccepted WRITE setPrivacyNoticeAccepted NOTIFY privacyNoticeAcceptedChanged)

    public:
       /*!
          * @brief Creates a new instance of this class if it doesn't exist and returns it.
          *
          * @return Class reference.
          */
        static CReporterPrivacySettingsModel *instance();

        /*!
          * @brief Frees the class instance.
          *
          */
        static void freeSingleton();

        /*!
          * @brief Class destructor.
          *
          */
        ~CReporterPrivacySettingsModel();

        /*!
          * @brief Reads setting value for core dumping and returns it.
          *
          * @note This setting used by the rich-core.
          * @return Returns true, if dumping is enabled; otherwise false. If value doesn't
          *     exist, default value is returned.
          */
        bool coreDumpingEnabled() const;

        /**
         * Checks whether endurance snapshot collection is enabled.
         *
         * @return @c true if endurance is enabled, otherwise @c false.
         */
        bool enduranceEnabled() const;

        /*!
          * @brief Reads setting value for notifications and returns it.
          *
          * @note This setting used by the crash-reporter.
          * @return Returns true, if notifications are enabled; otherwise false. If value doesn't
          *     exist, default value is returned.
          */
        bool notificationsEnabled() const;

        /*!
          * @brief Reads setting value for auto deleting dublicates and returns it.
          *
          * @note This setting used by the crash-reporter.
          * @return Returns true, if deleting is enabled; otherwise false. If value doesn't
          *     exist, default value is returned.
          */
        bool autoDeleteDuplicates() const;

        /*!
          * @brief Reads setting value for how many similar core dumps are kept when auto delete is enabled
          *
          * @note This setting used by the crash-reporter.
          * @return The amount of similar cores kept
          */
        int autoDeleteMaxSimilarCores() const;

        /*!
          * @brief Reads setting value for automatic sending and returns it.
          *
          * @note This setting used by the crash-reporter.
          * @return Returns true, if automatic sending is enabled; otherwise false. If value doesn't
          *     exist, default value is returned.
          */
        bool automaticSendingEnabled() const;

        /*!
          * @brief Reads setting value for including core and returns it.
          *
          * @note This setting used by the rich-core.
          * @return Returns true, if core should be included to the crash report; otherwise false.
          *    If value doesn't exist, default value is returned.
          */
        bool includeCore() const;
        
        /*!
          * @brief Reads setting value for including syslog and returns it.
          *
          * @note This setting used by the rich-core.
          * @return Returns true, if syslog should be included to the crash report; otherwise false.
          *    If value doesn't exist, default value is returned.
          */
        bool includeSystemLog() const;
        
        /*!
          * @brief Reads setting value for reducing core size.
          *
          * @note This setting used by the rich-core.
          * @return Returns true, if core-dump size should be shrinked.
          *    If value doesn't exist, default value is returned.
          */
        bool reduceCore() const;

        /*!
          * @brief Reads setting value for including list of installed packages and returns it.
          *
          * @note This setting used by the rich-core.
          * @return Returns true, if package list should be included to the crash report; otherwise false.
          *    If value doesn't exist, default value is returned.
          */
        bool includePackageList() const;

        /*!
         * @brief Reads setting for including stack trace into crash report.
         *
         * @note This setting is used by rich-core-dumper.
         * @return true if stack trace should be included; otherwise false.
         */
        bool includeStackTrace() const;

        /*!
         * @brief Reads setting for automatic download of debug symbols.
         *
         * @note This setting is used by rich-core-dumper.
         * @return true if debug symbols should be downloaded; otherwise false.
         */
        bool downloadDebuginfo() const;

        /*!
         * Checks whether user accepted a notice about possible privacy breach
         * from using crash reporter application.
         *
         * @return @c true if notice was accepted; otherwise false.
         */
        bool privacyNoticeAccepted() const;

          /*!
          * @brief Enables or disables core dumping.
          *
          * @note This setting used by the rich-core.
          * @param True to enable feature; false to disable.
          */
       void setCoreDumpingEnabled(bool value);

       /**
        * Enables or disables endurance snapshot collection.
        *
        * @param value @c true to enable, @c false to disable.
        */
       void setEnduranceEnabled(bool value);

       /*!
          * @brief Enables or disables notifications.
          *
          * @note This setting used by the crash-reporter.
          * @param True to enable feature; false to disable.
          */
        void setNotificationsEnabled(bool value);

        /*!
          * @brief Enables or disables auto deleting crash reports.
          *
          * @note This setting used by the crash-reporter.
          * @param True to enable feature; false to disable.
          */
        void setAutoDeleteDuplicates(bool value);

        /*!
          * @brief Set the amount of cores kept when auto delete is enabled
          *
          * @note This setting used by the crash-reporter.
          * @param value New value
          */
        void setAutoDeleteMaxSimilarCores(int value);

        /*!
           * @brief Enables or disables automatic sending.
           *
           * @note This setting used by the crash-reporter.
           * @param True to enable feature; false to disable.
           */
         void setAutomaticSendingEnabled(bool value);

        /*!
          * @brief Enables or disables for including core in the crash report.
          *
          * @note This setting used by the rich-core.
          * @param True to enable feature; false to disable.
          */
        void setIncludeCore(bool value);

        /*!
          * @brief Enables or disables for including syslog in the crash report.
          *
          * @note This setting used by the rich-core.
          * @param True to enable feature; false to disable.
          */
        void setIncludeSystemLog(bool value);

        /*!
          * @brief Enables or disables for including list of installed packages in the crash reports.
          *
          * @note This setting used by the rich-core.
          * @param True to enable feature; false to disable.
          */
        void setIncludePackageList(bool value);

        /*!
         * @brief Enables or disables stack trace inclusion into crash reports.
         *
         * @note This setting is used by rich-core-dumper.
         * @param value true to enable feature; false to disable.
         */
        void setIncludeStackTrace(bool value);

        /*!
         * @brief Enables or disables automatic download of debug symbols.
         *
         * @note This setting is used by rich-core-dumper.
         * @param value true to enable feature; false to disable.
         */
        void setDownloadDebuginfo(bool value);

        /*!
         * Accepts or declines crash reporter's privacy notice.
         *
         * @param value true to accept the notice; false to decline.
         */
        void setPrivacyNoticeAccepted(bool value);

        /*!
          * @brief Enables or disables core-dump size shrinking.
          *
          * @note This setting used by the rich-core.
          * @param True to enable feature; false to disable.
          */
        void setReduceCore(bool value);

    signals:
        void coreDumpingEnabledChanged();
        void enduranceEnabledChanged();
        void notificationsEnabledChanged();
        void autoDeleteDuplicatesChanged();
        void includeStackTraceChanged();
        void downloadDebuginfoChanged();
        void privacyNoticeAcceptedChanged();

    protected:
        CReporterPrivacySettingsModel();

    private:
        Q_DISABLE_COPY(CReporterPrivacySettingsModel)

        static QString enduranceCollectMark();

        //! @arg Static class reference.
        static CReporterPrivacySettingsModel *sm_Instance;
};

#endif // CREPORTERPRIVACYSETTINGSMODEL_H

// End of file.
