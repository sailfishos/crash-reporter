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
    Q_PROPERTY(bool journalSpy READ journalSpyEnabled WRITE setJournalSpyEnabled NOTIFY journalSpyEnabledChanged)
    Q_PROPERTY(bool notifications READ notificationsEnabled WRITE setNotificationsEnabled NOTIFY notificationsEnabledChanged)
    Q_PROPERTY(bool autoDeleteDuplicates READ autoDeleteDuplicates WRITE setAutoDeleteDuplicates NOTIFY autoDeleteDuplicatesChanged)
    Q_PROPERTY(bool includeStackTrace READ includeStackTrace WRITE setIncludeStackTrace NOTIFY includeStackTraceChanged)
    Q_PROPERTY(bool downloadDebuginfo READ downloadDebuginfo WRITE setDownloadDebuginfo NOTIFY downloadDebuginfoChanged)
    Q_PROPERTY(bool privacyNoticeAccepted READ privacyNoticeAccepted WRITE setPrivacyNoticeAccepted NOTIFY privacyNoticeAcceptedChanged)
    Q_PROPERTY(bool allowMobileData READ allowMobileData WRITE setAllowMobileData NOTIFY allowMobileDataChanged)
    Q_PROPERTY(bool restrictWhenLowBattery READ restrictWhenLowBattery WRITE setRestrictWhenLowBattery NOTIFY restrictWhenLowBatteryChanged)
    Q_PROPERTY(bool restrictWhenDischarging READ restrictWhenDischarging WRITE setRestrictWhenDischarging NOTIFY restrictWhenDischargingChanged)
    Q_PROPERTY(int dischargingThreshold READ dischargingThreshold WRITE setDischargingThreshold NOTIFY dischargingThresholdChanged)

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

    /**
     * Checks whether the journal spy daemon is enabled.
     *
     * @return @c true if journal spy is enabled, otherwise @c false.
     */
    bool journalSpyEnabled() const;

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
     * Checks whether data transfers through a mobile network are allowed.
     *
     * @return @c true if mobile data usage is allowed; otherwise false.
     */
    bool allowMobileData() const;

    /*!
     * Controls whether to restrict execution of power-intensive tasks when battery is low regardless
     * of charger presence.
     *
     * @return @c true when so
     */
    bool restrictWhenLowBattery() const;

    /*!
     * Controls whether to restrict execution of power-intensive tasks when discharging and bttery is
     * below dischargingThreshold.
     *
     * @return @c true when so
     */
    bool restrictWhenDischarging() const;

    /*!
     * The minimum battery capacity required when restrictWhenLowBattery is active.
     *
     * @return percentage of maximum battery capacity
     */
    int dischargingThreshold() const;

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

    /**
     * Enables or disables journal spy daemon.
     *
     * @param value @c true to enable, @c false to disable.
     */
    void setJournalSpyEnabled(bool value);

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
     * Allows or disables data transfers through a mobile network.
     *
     * @param value @c true to allow mobile data; @c false to block.
     */
    void setAllowMobileData(bool value);

    /*!
     * Controls whether to restrict execution of power-intensive tasks when battery is low regardless
     * of charger presence.
     *
     * @param value to restrict, or not to restrict
     */
    void setRestrictWhenLowBattery(bool value);

    /*!
     * Controls whether to restrict execution of power-intensive tasks when discharging and bttery is
     * below dischargingThreshold.
     *
     * @param value to restrict, or not to restrict
     */
    void setRestrictWhenDischarging(bool value);

    /*!
     * Sets the minimum battery capacity required when restrictWhenLowBattery is active.
     *
     * @param value percentage of maximum battery capacity
     */
    void setDischargingThreshold(int value);

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
    void journalSpyEnabledChanged();
    void notificationsEnabledChanged();
    void autoDeleteDuplicatesChanged();
    void automaticSendingEnabledChanged();
    void includeStackTraceChanged();
    void downloadDebuginfoChanged();
    void privacyNoticeAcceptedChanged();
    void allowMobileDataChanged();
    void restrictWhenLowBatteryChanged();
    void restrictWhenDischargingChanged();
    void dischargingThresholdChanged();

protected:
    CReporterPrivacySettingsModel();

private:
    Q_DISABLE_COPY(CReporterPrivacySettingsModel)

    static QString enduranceCollectMark();
    static QString journalSpyMark();

    //! @arg Static class reference.
    static CReporterPrivacySettingsModel *sm_Instance;
};

#endif // CREPORTERPRIVACYSETTINGSMODEL_H
