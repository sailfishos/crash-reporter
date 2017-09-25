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

// System includes.

#include <QFile>
#include <QDir>
#include <QDebug>

// User includes.

#include "creportersettingsbase.h"
#include "creporterprivacysettingsmodel.h"
#include "creporternamespace.h"
#include "../coredir/creportercoreregistry.h"

/* TODO: move what doesn't have to be shared with rich-core-dumper to
 * application settings. */
namespace Settings {
//! When true, rich-core dumps are produced.
const QString CoreDumping("Settings/coredumping");
//! When true, user notifications are displayed.
const QString Notifications("Settings/notifications");
//! When true, duplicate rich cores are deleted automatically.
const QString AutoDeleteDuplicates("Settings/avoid-dups");
//! Stores how many similar core dumps are kept when avoid-dups is enabled.
const QString AutoDeleteMaxSimilarCores("Settings/maxsimilarcores");
//! When true, crash-reporter tries to upload rich-core dumps automatically.
const QString AutomaticSending("Settings/automaticsending");
//! True when user has accepted crash reporter's privacy notice.
const QString NoticeAccepted("Settings/privacy-notice-accepted");
//! When true, crash-reporter can use mobile connection for data transfers.
const QString AllowMobileData("Settings/allow-mobile-data");
}

/*!
 * Settings affecting what information is included in the crash reports.
 */
namespace Privacy {
//! If set to true, core dump is included in the crash report.
const QString IncludeCore("Privacy/INCLUDE_CORE");
//! If set to true, syslog is included in the crash reporter.
const QString IncludeSysLog("Privacy/INCLUDE_SYSLOG");
//! If set to true, package list is included in the crash reporter.
const QString IncludePkgList("Privacy/INCLUDE_PKGLIST");
//! If set to true, stack trace is included in the crash report.
const QString IncludeStackTrace("Privacy/INCLUDE_STACK_TRACE");
/*!
 * If set to true, core dump size is reduced before it's included in
 * the crash reporter.
 */
const QString ReduceCore("Privacy/REDUCE_CORE");
/*!
 * If set to true, rich-core-dumper will attempt to download missing debug
 * symbols before generating a stack trace.
 */
const QString DownloadDebuginfo("Privacy/DOWNLOAD_DEBUGINFO");
}

CReporterPrivacySettingsModel *CReporterPrivacySettingsModel::sm_Instance = 0;

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::instance
// ----------------------------------------------------------------------------
CReporterPrivacySettingsModel *CReporterPrivacySettingsModel::instance()
{
    if (sm_Instance == 0) {
        // Get default settings, if not overriden by the user.
        QString home = QDir::homePath();
        if (!QFile::exists(home + CReporter::PrivacySettingsFileUser)) {
            QDir dir;
            dir.mkpath(home + CReporter::UserSettingsLocation);
            QFile::copy(CReporter::PrivacySettingsFileSystem, home + CReporter::PrivacySettingsFileUser);
        }

        sm_Instance = new CReporterPrivacySettingsModel();
    }
    return sm_Instance;
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::freeSingleton
// ----------------------------------------------------------------------------
void CReporterPrivacySettingsModel::freeSingleton()
{
    if (sm_Instance != 0) {
        delete sm_Instance;
        sm_Instance = 0;
    }
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::CReporterPrivacySettingsModel
// ----------------------------------------------------------------------------
CReporterPrivacySettingsModel::CReporterPrivacySettingsModel() :
    CReporterSettingsBase("crash-reporter-settings", "crash-reporter-privacy")
{

}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::CReporterPrivacySettingsModel
// ----------------------------------------------------------------------------
CReporterPrivacySettingsModel::~CReporterPrivacySettingsModel()
{

}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::coreDumpingEnabled
// ----------------------------------------------------------------------------
bool CReporterPrivacySettingsModel::coreDumpingEnabled() const
{
    return value(Settings::CoreDumping, QVariant(true)).toBool();
}

QString CReporterPrivacySettingsModel::enduranceCollectMark()
{
    return CReporterCoreRegistry::instance()->getCoreLocationPaths().first() +
           "/endurance-enabled-mark";
}

QString CReporterPrivacySettingsModel::journalSpyMark()
{
    return CReporterCoreRegistry::instance()->getCoreLocationPaths().first() +
           "/journalspy-enabled-mark";
}

bool CReporterPrivacySettingsModel::enduranceEnabled() const
{
    return QFile::exists(enduranceCollectMark());
}

bool CReporterPrivacySettingsModel::journalSpyEnabled() const
{
    return QFile::exists(journalSpyMark());
}

bool CReporterPrivacySettingsModel::notificationsEnabled() const
{
    return value(Settings::Notifications, QVariant(true)).toBool();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::autoDeleteDuplicates
// ----------------------------------------------------------------------------
bool CReporterPrivacySettingsModel::autoDeleteDuplicates() const
{
    return value(Settings::AutoDeleteDuplicates, QVariant(true)).toBool();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::autoDeleteMaxSimilarCores
// ----------------------------------------------------------------------------
int CReporterPrivacySettingsModel::autoDeleteMaxSimilarCores() const
{
    return value(Settings::AutoDeleteMaxSimilarCores, QVariant(5)).toInt();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::automaticSendingEnabled
// ----------------------------------------------------------------------------
bool CReporterPrivacySettingsModel::automaticSendingEnabled() const
{
    return value(Settings::AutomaticSending, QVariant(true)).toBool();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::includeCore
// ----------------------------------------------------------------------------
bool CReporterPrivacySettingsModel::includeCore() const
{
    return value(Privacy::IncludeCore, QVariant(true)).toBool();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::includeSystemLog
// ----------------------------------------------------------------------------
bool CReporterPrivacySettingsModel::includeSystemLog() const
{
    return value(Privacy::IncludeSysLog, QVariant(true)).toBool();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::includePackageList
// ----------------------------------------------------------------------------
bool CReporterPrivacySettingsModel::includePackageList() const
{
    return value(Privacy::IncludePkgList, QVariant(true)).toBool();
}

bool CReporterPrivacySettingsModel::includeStackTrace() const
{
    return value(Privacy::IncludeStackTrace, QVariant(false)).toBool();
}

bool CReporterPrivacySettingsModel::downloadDebuginfo() const
{
    return value(Privacy::DownloadDebuginfo, QVariant(false)).toBool();
}

bool CReporterPrivacySettingsModel::privacyNoticeAccepted() const
{
    return value(Settings::NoticeAccepted, QVariant(false)).toBool();
}

bool CReporterPrivacySettingsModel::allowMobileData() const
{
    return value(Settings::AllowMobileData, QVariant(false)).toBool();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::reduceCore
// ----------------------------------------------------------------------------
bool CReporterPrivacySettingsModel::reduceCore() const
{
    return value(Privacy::ReduceCore, QVariant(true)).toBool();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::setCoreDumpingEnabled
// ----------------------------------------------------------------------------
void CReporterPrivacySettingsModel::setCoreDumpingEnabled(bool value)
{
    if (setValue(Settings::CoreDumping, QVariant(value)))
        emit coreDumpingEnabledChanged();
}

void CReporterPrivacySettingsModel::setEnduranceEnabled(bool value)
{
    if (value == enduranceEnabled()) {
        return;
    }

    if (value) {
        QFile(enduranceCollectMark()).open(QFile::WriteOnly);
    } else {
        QFile::remove(enduranceCollectMark());
    }

    emit enduranceEnabledChanged();
}

void CReporterPrivacySettingsModel::setJournalSpyEnabled(bool value)
{
    if (value == journalSpyEnabled()) {
        return;
    }

    if (value) {
        QFile(journalSpyMark()).open(QFile::WriteOnly);
    } else {
        QFile::remove(journalSpyMark());
    }

    emit journalSpyEnabledChanged();
}

void CReporterPrivacySettingsModel::setNotificationsEnabled(bool value)
{
    if (setValue(Settings::Notifications, QVariant(value)))
        emit notificationsEnabledChanged();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::setAutoDeleteDuplicates
// ----------------------------------------------------------------------------
void CReporterPrivacySettingsModel::setAutoDeleteDuplicates(bool value)
{
    if (setValue(Settings::AutoDeleteDuplicates, QVariant(value)))
        emit autoDeleteDuplicatesChanged();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::setAutoDeleteMaxSimilarCores
// ----------------------------------------------------------------------------
void CReporterPrivacySettingsModel::setAutoDeleteMaxSimilarCores(int value)
{
    setValue(Settings::AutoDeleteMaxSimilarCores, QVariant(value));
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::setAutomaticSendingEnabled
// ----------------------------------------------------------------------------
void CReporterPrivacySettingsModel::setAutomaticSendingEnabled(bool value)
{
    if (setValue(Settings::AutomaticSending, QVariant(value)))
        emit automaticSendingEnabledChanged();
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::setIncludeCore
// ----------------------------------------------------------------------------
void CReporterPrivacySettingsModel::setIncludeCore(bool value)
{
    setValue(Privacy::IncludeCore, QVariant(value));
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::setIncludeSystemLog
// ----------------------------------------------------------------------------
void CReporterPrivacySettingsModel::setIncludeSystemLog(bool value)
{
    setValue(Privacy::IncludeSysLog, QVariant(value));
}

// ----------------------------------------------------------------------------
// CReporterPrivacySettingsModel::setIncludePackageList
// ----------------------------------------------------------------------------
void CReporterPrivacySettingsModel::setIncludePackageList(bool value)
{
    setValue(Privacy::IncludePkgList, QVariant(value));
}

void CReporterPrivacySettingsModel::setIncludeStackTrace(bool value)
{
    if (setValue(Privacy::IncludeStackTrace, QVariant(value)))
        emit includeStackTraceChanged();
}

void CReporterPrivacySettingsModel::setDownloadDebuginfo(bool value)
{
    if (setValue(Privacy::DownloadDebuginfo, QVariant(value)))
        emit downloadDebuginfoChanged();
}

void CReporterPrivacySettingsModel::setPrivacyNoticeAccepted(bool value)
{
    if (setValue(Settings::NoticeAccepted, QVariant(value)))
        emit privacyNoticeAcceptedChanged();
}

void CReporterPrivacySettingsModel::setAllowMobileData(bool value)
{
    if (setValue(Settings::AllowMobileData, QVariant(value)))
        emit allowMobileDataChanged();
}

void CReporterPrivacySettingsModel::setReduceCore(bool value)
{
    setValue(Privacy::ReduceCore, QVariant(value));
}

// End of file.

