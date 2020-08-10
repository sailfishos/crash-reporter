/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Riku Halonen <riku.halonen@nokia.com>
 *
 * Copyright (C) 2013 - 2019 Jolla Ltd.
 * Copyright (C) 2020 Open Mobile Platform LLC.
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

#include <csignal>

#include <QListIterator>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QDBusReply>

#include <notification.h>

#include "creporterdaemonmonitor.h"
#include "creporterdaemonmonitor_p.h"
#include "creportercoreregistry.h"
#include "creporternwsessionmgr.h"
#include "creportersavedstate.h"
#include "creporterutils.h"
#include "creporternamespace.h"
#include "creporterprivacysettingsmodel.h"
#include "autouploader_interface.h" // generated

using CReporter::LoggingCategory::cr;

CReporterHandledRichCore::CReporterHandledRichCore(const QString &filePath)
    : lastCountReset(QDateTime::currentDateTimeUtc())
{
    // Parse needed info for file path.
    QStringList rCoreInfo = CReporterUtils::parseCrashInfoFromFilename(filePath);

    binaryName = rCoreInfo[0];
    signalNumber = rCoreInfo[2].toInt();

    count = 0;

    qCDebug(cr) << "Name:" << binaryName << ", Signal:" << signalNumber;
}

CReporterHandledRichCore::~CReporterHandledRichCore()
{
}

bool CReporterHandledRichCore::operator==(const CReporterHandledRichCore &other) const
{
    return (binaryName == other.binaryName) &&
           (signalNumber == other.signalNumber);
}


CReporterDaemonMonitorPrivate::CReporterDaemonMonitorPrivate()
    : autoDeleteMaxSimilarCores(0),
      crashNotification(new Notification(this)),
      crashCount(0)
{
    CReporterUtils::applyNotificationStyle(crashNotification);
    crashNotification->setReplacesId(CReporterSavedState::instance()->crashNotificationId());

    connect(crashNotification, &Notification::closed,
            this, &CReporterDaemonMonitorPrivate::resetCrashCount);

    connect(CReporterPrivacySettingsModel::instance(),
            &CReporterPrivacySettingsModel::automaticSendingEnabledChanged,
            this, &CReporterDaemonMonitorPrivate::onSetAutoUploadChanged);
}

CReporterDaemonMonitorPrivate::~CReporterDaemonMonitorPrivate()
{
    CReporterSavedState *state = CReporterSavedState::instance();
    state->setCrashNotificationId(crashNotification->replacesId());

    qDeleteAll(handledRichCores);
}

void CReporterDaemonMonitorPrivate::addDirectoryWatcher()
{
    qCDebug(cr) << "Adding core directory watcher...";

    // Subscribe to receive signals for changed directories.
    connect(&watcher, SIGNAL(directoryChanged(const QString &)),
            this, SLOT(handleDirectoryChanged(const QString &)));

    CReporterCoreRegistry *registry = CReporterCoreRegistry::instance();

    // Subscribe to receive signals for changes in core registry.
    connect(registry, SIGNAL(coreLocationsUpdated()),
            this, SLOT(addDirectoryWatcher()));

    QStringList corePaths(registry->getCoreLocationPaths());

    if (!corePaths.isEmpty()) {
        registry->refreshRegistry();
        // Add monitored directories to QFileSystemWatcher. Paths are not added
        // if they do not exist, or if they are already being monitored by the file system watcher.
        watcher.addPaths(corePaths);
    }
}

void CReporterDaemonMonitorPrivate::removeDirectoryWatcher()
{
    qCDebug(cr) << "Removing core directory watcher...";

    // Remove watcher from directories.
    watcher.removePaths(watcher.directories());
}

void CReporterDaemonMonitorPrivate::handleDirectoryChanged(const QString &path)
{
    qCDebug(cr) << "Directory:" << path << "has changed.";

    QDir changedDir(path);
    // QFileSystemWatcher will send signal if monitored directory was removed.
    if (!changedDir.exists()) {
        /* Re-add core dirs when the parent dir changes, so that monitoring is
         * resumed after USB mass storage mode has been disconnected */
        if (changedDir.cd("../..")) {
            connect(&parentDirWatcher, SIGNAL(directoryChanged(QString)),
                    SLOT(handleParentDirectoryChanged()));
            parentDirWatcher.addPath(changedDir.absolutePath());
            qCDebug(cr) << "Directory was deleted. Started parent dir monitoring.";
        } else {
            qCDebug(cr) << "Directory was deleted. Parent dir does not exist.";
        }

        return;
    }

    qCDebug(cr) << "Changed directory exists.";

    CReporterCoreRegistry *registry = CReporterCoreRegistry::instance();

    // Check for new cores in changed directory.
    QString filePath = registry->checkDirectoryForCores(path);

    if (filePath.isEmpty()) {
        return;
    }

    // New core found.
    qCDebug(cr) << "New rich-core file found: " << filePath;

    QStringList details = CReporterUtils::parseCrashInfoFromFilename(filePath);
    bool isUserTerminated = (details[2].toInt() == SIGQUIT);
    QString appName = details[0];

    emit q_ptr->richCoreNotify(filePath);

    CReporterPrivacySettingsModel &settings =
        *CReporterPrivacySettingsModel::instance();

    /* Check for duplicates if auto-deleting is enabled. If Maximum number
     * of duplicates is exceeded, delete the file. */
    if (!isUserTerminated && settings.autoDeleteDuplicates() &&
            checkForDuplicates(filePath)) {
        if (settings.notificationsEnabled()) {
            Notification notification;
            CReporterUtils::applyNotificationStyle(&notification);
            notification.setIsTransient(true);
            //% "%1 has crashed again."
            notification.setSummary(qtTrId("crash_reporter-notify-crashed_again").arg(appName));
            //% "Duplicate crash report was deleted."
            notification.setBody(qtTrId("crash_reporter-notify-duplicate_deleted"));
            notification.publish();
        }
        CReporterUtils::removeFile(filePath);
        return;
    }

    if (!settings.automaticSendingEnabled()) {
        /* TODO: Here multiple-choice notification should be displayed
         * with options to send or delete the crash report. So far
         * disabling auto upload is not possible in the UI and we never
         * get here. Standard Sailfish notifications don't support multiple
         * actions so far. */
    } else {
        if (settings.notificationsEnabled()) {

            QString body;
            QString summary;

            if (filePath.contains(CReporter::QuickFeedbackPrefix)) {
                //% "New feedback message is ready."
                summary = qtTrId("crash_reporter-notify-quickie_ready");
            } else if (filePath.contains(CReporter::EndurancePackagePrefix)) {
                //% "New endurance report is ready."
                summary = qtTrId("crash_reporter-notify-endurance_ready");
            } else if (filePath.contains(CReporter::PowerExcessPrefix)) {
                //% "Power excess detected."
                summary = qtTrId("crash_reporter-notify-power_excess_detected");
            } else if (isUserTerminated) {
                //% "%1 was terminated."
                summary = qtTrId("crash_reporter-notify-app_terminated").arg(appName);
            } else {
                if (++crashCount > 1) {
                    //% "%n crashes total"
                    body = qtTrId("crash_reporter-notify-total_crashes", crashCount);
                }
                //% "%1 has crashed."
                summary = qtTrId("crash_reporter-notify-app_crashed").arg(appName);
            }

            crashNotification->setSummary(summary);
            crashNotification->setBody(body);
            crashNotification->setItemCount(crashCount);
            crashNotification->publish();
        }
        if (!CReporterNwSessionMgr::canUseNetworkConnection()) {
            qCDebug(cr) << "WiFi not available, not uploading now.";
        } else {
            /* In auto-upload mode try to upload all crash reports each
             * time a new one appears. */
            if (!CReporterUtils::notifyAutoUploader(registry->collectAllCoreFiles())) {
                qCWarning(cr) << "Failed to start Auto Uploader.";
            }
        }
    }
}

void CReporterDaemonMonitorPrivate::handleParentDirectoryChanged()
{
    qCDebug(cr) << "Parent dir has changed. Trying to re-add directory watchers.";

    CReporterCoreRegistry *registry = CReporterCoreRegistry::instance();

    QStringList corePaths(registry->getCoreLocationPaths());
    int numWatchPaths = watcher.directories().count();

    if (!corePaths.isEmpty()) {
        registry->refreshRegistry();
        // Add monitored directories to QFileSystemWatcher. Paths are not added
        // if they do not exist, or if they are already being monitored by the file system watcher.
        watcher.addPaths(corePaths);
    }

    if (watcher.directories().count() > numWatchPaths) {
        qCDebug(cr) << "Successfully started watching core-dump dir";
        disconnect(this, SLOT(handleParentDirectoryChanged()));
        parentDirWatcher.removePaths(parentDirWatcher.directories());
    }
}

bool CReporterDaemonMonitorPrivate::checkForDuplicates(const QString &path)
{
    // Ignore reports that don't contain core dumps.
    if (!CReporterUtils::reportIncludesCrash(path)) {
        return false;
    }

    qCDebug(cr) << "Checking, if" << path << "has been handled for"
                << autoDeleteMaxSimilarCores << "times.";

    // Create new entry.
    CReporterHandledRichCore *rCore = new CReporterHandledRichCore(path);

    foreach (CReporterHandledRichCore *handled, handledRichCores) {
        // Loop through list to find duplicates.
        qCDebug(cr) << "Compare to:"
                    << "Name:" << handled->binaryName << ", Signal:" << handled->signalNumber;

        if (*handled == *rCore) {
            /* Check if more than a day has passed from last duplicate counter
             * reset. */
            QDateTime now(QDateTime::currentDateTimeUtc());
            if (handled->lastCountReset.addDays(1) < now) {
                qCDebug(cr) << "More than a day has passed, resetting duplicate counter";
                handled->count = 0;
                handled->lastCountReset = now;
            }

            handled->count++;
            // Duplicate found. Increment counter.
            qCDebug(cr) << "Matches. Count is now:" << handled->count;

            if (handled->count > autoDeleteMaxSimilarCores) {
                // Maximum exeeded.
                qCDebug(cr) << "Maximum number of duplicates exceeded.";
                delete rCore;
                return true;
            }
            // Maximum not yet exeeded.
            delete rCore;
            return false;
        }
    }
    // Not found. Add to handled list.
    rCore->count++;
    handledRichCores << rCore;

    return false;
}

void CReporterDaemonMonitorPrivate::resetCrashCount()
{
    crashCount = 0;
    qCDebug(cr) << "Crash counter was reset.";
}

void CReporterDaemonMonitorPrivate::onSetAutoUploadChanged()
{
    if (CReporterPrivacySettingsModel::instance()->automaticSendingEnabled()) {
        return;
    }

    qCDebug(cr) << "Calling quit() on crash-reporter-autouploader.";

    ComNokiaCrashReporterAutoUploaderInterface proxy(CReporter::AutoUploaderServiceName,
            CReporter::AutoUploaderObjectPath, QDBusConnection::sessionBus());

    proxy.quit();
}

CReporterDaemonMonitor::CReporterDaemonMonitor(QObject *parent)
    : QObject(parent), d_ptr(new CReporterDaemonMonitorPrivate())
{
    d_ptr->q_ptr = this;

    // Add watcher to core directories.
    d_ptr->addDirectoryWatcher();
}

CReporterDaemonMonitor::~CReporterDaemonMonitor()
{
    d_ptr->removeDirectoryWatcher();

    delete d_ptr;
    d_ptr = 0;
}

int CReporterDaemonMonitor::autoDeleteMaxSimilarCores()
{
    return d_ptr->autoDeleteMaxSimilarCores;
}

void CReporterDaemonMonitor::setAutoDeleteMaxSimilarCores(int value)
{
    d_ptr->autoDeleteMaxSimilarCores = value;
}
