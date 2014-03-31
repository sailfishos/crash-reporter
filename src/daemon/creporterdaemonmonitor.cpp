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
 
#include <csignal>

#include <QListIterator>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QDBusReply>

// User includes.

#include "creporterdaemonmonitor.h"
#include "creporterdaemonmonitor_p.h"
#include "creportercoreregistry.h"
#include "creporternwsessionmgr.h"
#include "creportersavedstate.h"
#include "creporterutils.h"
#include "creporterautouploaderproxy.h"
#include "creporternamespace.h"
#include "creporternotification.h"
#include "creporterprivacysettingsmodel.h"

// ******** Class CReporterHandledRichCore ********

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterHandledRichCore::CReporterHandledRichCore
// ----------------------------------------------------------------------------
CReporterHandledRichCore::CReporterHandledRichCore(const QString &filePath):
  lastCountReset(QDateTime::currentDateTimeUtc())
{
    // Parse needed info for file path.
    QStringList rCoreInfo = CReporterUtils::parseCrashInfoFromFilename(filePath);

    binaryName = rCoreInfo[0];
    signalNumber = rCoreInfo[2].toInt();

    QFileInfo fi(filePath);

    count = 0;

    qDebug() << __PRETTY_FUNCTION__ << "Name:" << binaryName << ", Signal:" << signalNumber;
}

// ----------------------------------------------------------------------------
// CReporterHandledRichCore::~CReporterHandledRichCore
// ----------------------------------------------------------------------------
CReporterHandledRichCore::~CReporterHandledRichCore()
{
}

// ----------------------------------------------------------------------------
// CReporterHandledRichCore::operator==
// ----------------------------------------------------------------------------
bool CReporterHandledRichCore::operator==(const CReporterHandledRichCore &other) const
{
    return (binaryName == other.binaryName) &&
           (signalNumber == other.signalNumber);
}

// ******** Class CReporterDaemonMonitorPrivate ********

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterDaemonMonitorPrivate::CReporterDaemonMonitorPrivate
// ----------------------------------------------------------------------------
CReporterDaemonMonitorPrivate::CReporterDaemonMonitorPrivate() :
  autoDelete(false), autoDeleteMaxSimilarCores(0), autoUpload(false),
  crashNotification(new CReporterNotification(
          CReporter::AutoUploaderNotificationEventType,
          CReporterSavedState::instance()->crashNotificationId(), this)),
  crashCount(0)
{
    connect(crashNotification, &CReporterNotification::timeouted,
            this, &CReporterDaemonMonitorPrivate::resetCrashCount);
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitorPrivate::~CReporterDaemonMonitorPrivate
// ----------------------------------------------------------------------------
CReporterDaemonMonitorPrivate::~CReporterDaemonMonitorPrivate()
{
    CReporterSavedState *state = CReporterSavedState::instance();
    state->setCrashNotificationId(crashNotification->id());

    qDeleteAll(handledRichCores);
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitorPrivate::addDirectoryWatcher
// ----------------------------------------------------------------------------
void CReporterDaemonMonitorPrivate::addDirectoryWatcher()
{
    qDebug() << __PRETTY_FUNCTION__ << "Adding core directory watcher...";

    // Subscribe to receive signals for changed directories.
    connect(&watcher, SIGNAL(directoryChanged(const QString&)),
            this, SLOT(handleDirectoryChanged(const QString&)));

    CReporterCoreRegistry *registry = CReporterCoreRegistry::instance();

    // Subscribe to receive signals for changes in core registry.
    connect(registry, SIGNAL(coreLocationsUpdated()),
                this, SLOT(addDirectoryWatcher()));

    QStringList* corePaths = registry->getCoreLocationPaths();

    if (!corePaths->isEmpty()) {
        registry->refreshRegistry();
        // Add monitored directories to QFileSystemWatcher. Paths are not added
        // if they do not exist, or if they are already being monitored by the file system watcher.
        watcher.addPaths(*corePaths);
    }

    delete corePaths;
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitorPrivate::removeDirectoryWatcher()
// ----------------------------------------------------------------------------
void CReporterDaemonMonitorPrivate::removeDirectoryWatcher()
{
    qDebug() << __PRETTY_FUNCTION__ << "Removing core directory watcher...";

    // Remove watcher from directories.
    watcher.removePaths(watcher.directories());
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitorPrivate::handleDirectoryChanged
// ----------------------------------------------------------------------------
void CReporterDaemonMonitorPrivate::handleDirectoryChanged(const QString &path)
{
    qDebug() << __PRETTY_FUNCTION__ << "Directory:" << path << "has changed.";

    QDir changedDir(path);
    // QFileSystemWatcher will send signal if monitored directory was removed.
    if (!changedDir.exists()) {
        /* Re-add core dirs when the parent dir changes, so that monitoring is
         * resumed after USB mass storage mode has been disconnected */
        if (changedDir.cd("../..")) {
            connect(&parentDirWatcher, SIGNAL(directoryChanged(QString)),
                    SLOT(handleParentDirectoryChanged()));
            parentDirWatcher.addPath(changedDir.absolutePath());
            qDebug() << __PRETTY_FUNCTION__
                     << "Directory was deleted. Started parent dir monitoring.";
        } else {
            qDebug() << __PRETTY_FUNCTION__
                     << "Directory was deleted. Parent dir does not exist.";
        }

        return;
    }

    qDebug() << __PRETTY_FUNCTION__ << "Changed directory exists.";

    CReporterCoreRegistry *registry = CReporterCoreRegistry::instance();

    // Check for new cores in changed directory.
    QString filePath = registry->checkDirectoryForCores(path);

    if (filePath.isEmpty()) {
        return;
    }

    // New core found.
    qDebug() << __PRETTY_FUNCTION__ << "New rich-core file found: " << filePath;

    QStringList details = CReporterUtils::parseCrashInfoFromFilename(filePath);
    bool isUserTerminated = (details[2].toInt() == SIGQUIT);

    emit q_ptr->richCoreNotify(filePath);

    /* Check for duplicates if auto-deleting is enabled. If Maximum number
     * of duplicates is exceeded, delete the file. */
    if (!isUserTerminated && autoDelete && checkForDuplicates(filePath)) {
        if (CReporterPrivacySettingsModel::instance()->notificationsEnabled()) {
            CReporterNotification *notification =
                    new CReporterNotification(
                            CReporter::ApplicationNotificationEventType,
                            0, this);
            notification->setTimeout(5000);
            connect(notification, &CReporterNotification::timeouted,
                    notification, &QObject::deleteLater);
            //% "%1 has crashed again."
            notification->update(
                    qtTrId("crash_reporter-notify-crashed_again").arg(details[0]),
                    //% "Duplicate crash report was deleted."
                    qtTrId("crash_reporter-notify-duplicate_deleted"));
        }
        CReporterUtils::removeFile(filePath);
        return;
    }

    if (!autoUpload) {
        /* TODO: Here multiple-choice notification should be displayed
         * with options to send or delete the crash report. So far
         * disabling auto upload is not possible in the UI and we never
         * get here. Standard Sailfish notifications don't support multiple
         * actions so far. */
    } else {
        if (CReporterPrivacySettingsModel::instance()->notificationsEnabled()) {

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
                summary = qtTrId("crash_reporter-notify-app_terminated");
            } else {
                if (++crashCount > 1) {
                    //% "%1 crashes total"
                    body = qtTrId("crash_reporter-notify-total_crashes").arg(crashCount);
                }
                //% "%1 has crashed."
                summary = qtTrId("crash_reporter-notify-app_crashed");
            }

            crashNotification->update(summary.arg(details.at(0)), body,
                    crashCount);
        }
        if (!CReporterNwSessionMgr::unpaidConnectionAvailable()) {
            qDebug() << __PRETTY_FUNCTION__
                     << "WiFi not available, not uploading now.";
        } else {
            /* In auto-upload mode try to upload all crash reports each
             * time a new one appears. */
            if (!CReporterUtils::notifyAutoUploader(registry->collectAllCoreFiles())) {
                qWarning() << __PRETTY_FUNCTION__
                           << "Failed to start Auto Uploader.";
            }
        }
    }
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitorPrivate::handleParentDirectoryChanged
// ----------------------------------------------------------------------------
void CReporterDaemonMonitorPrivate::handleParentDirectoryChanged()
{
    qDebug() << __PRETTY_FUNCTION__ << "Parent dir has changed. Trying to re-add directory watchers.";

    CReporterCoreRegistry *registry = CReporterCoreRegistry::instance();

    QStringList* corePaths = registry->getCoreLocationPaths();
    int numWatchPaths = watcher.directories().count();

    if (!corePaths->isEmpty()) {
        registry->refreshRegistry();
        // Add monitored directories to QFileSystemWatcher. Paths are not added
        // if they do not exist, or if they are already being monitored by the file system watcher.
        watcher.addPaths(*corePaths);
    }

    delete corePaths;

    if (watcher.directories().count() > numWatchPaths)
    {
        qDebug() << __PRETTY_FUNCTION__ << "Successfully started watching core-dump dir";
        disconnect(this, SLOT(handleParentDirectoryChanged()));
        parentDirWatcher.removePaths(parentDirWatcher.directories());
    }
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitorPrivate::checkForDuplicates
// ----------------------------------------------------------------------------
bool CReporterDaemonMonitorPrivate::checkForDuplicates(const QString &path)
{
    // Ignore reports that don't contain core dumps.
    if (!CReporterUtils::reportIncludesCrash(path)) {
        return false;
    }

    qDebug() << __PRETTY_FUNCTION__ << "Checking, if" << path << "has been handled for"
            << autoDeleteMaxSimilarCores << "times.";

    // Create new entry.
    CReporterHandledRichCore *rCore = new CReporterHandledRichCore(path);

    foreach (CReporterHandledRichCore *handled, handledRichCores) {
        // Loop through list to find duplicates.
        qDebug() << __PRETTY_FUNCTION__  << "Compare to:"
               << "Name:" << handled->binaryName << ", Signal:" << handled->signalNumber;

        if (*handled == *rCore) {
            /* Check if more than a day has passed from last duplicate counter
             * reset. */
            QDateTime now(QDateTime::currentDateTimeUtc());
            if (handled->lastCountReset.addDays(1) < now) {
                qDebug() << __PRETTY_FUNCTION__ << "More than a day has passed,"
                            " resetting duplicate counter";
                handled->count = 0;
                handled->lastCountReset = now;
            }

            handled->count++;
            // Duplicate found. Increment counter.
            qDebug() << __PRETTY_FUNCTION__  << "Matches. Count is now:" << handled->count;

            if (handled->count > autoDeleteMaxSimilarCores) {
                // Maximum exeeded.
                qDebug() << __PRETTY_FUNCTION__  << "Maximum number of duplicates exceeded.";
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

// ----------------------------------------------------------------------------
// CReporterDaemonMonitorPrivate::handleNotificationEvent
// ----------------------------------------------------------------------------
void CReporterDaemonMonitorPrivate::handleNotificationEvent()
{
    // Handle timeouted and activated signals from CReporterNotification
    // and destroy instance.
    CReporterNotification *notification = qobject_cast<CReporterNotification *>(sender());

    if (notification != 0) {
        delete notification;
    }
}

void CReporterDaemonMonitorPrivate::resetCrashCount()
{
    crashCount = 0;
    qDebug() << __PRETTY_FUNCTION__ << "Crash counter was reset.";
}

CReporterDaemonMonitor::CReporterDaemonMonitor(QObject *parent):
  QObject(parent), d_ptr(new CReporterDaemonMonitorPrivate())
{
    d_ptr->q_ptr = this;

	// Add watcher to core directories.
    d_ptr->addDirectoryWatcher();
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitor::~CReporterDaemonMonitor
// ----------------------------------------------------------------------------
CReporterDaemonMonitor::~CReporterDaemonMonitor()
{
    d_ptr->removeDirectoryWatcher();

	delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitor::autoDeleteEnabled
// ----------------------------------------------------------------------------
bool CReporterDaemonMonitor::autoDeleteEnabled()
{
    return d_ptr->autoDelete;
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitor::setAutoDelete
// ----------------------------------------------------------------------------
void CReporterDaemonMonitor::setAutoDelete(bool state)
{
    d_ptr->autoDelete = state;
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitor::autoDeleteMaxSimilarCores
// ----------------------------------------------------------------------------
int CReporterDaemonMonitor::autoDeleteMaxSimilarCores()
{
    return d_ptr->autoDeleteMaxSimilarCores;
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitor::setAutoDeleteMaxSimilarCores
// ----------------------------------------------------------------------------
void CReporterDaemonMonitor::setAutoDeleteMaxSimilarCores(int value)
{
    d_ptr->autoDeleteMaxSimilarCores = value;
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitor::autoUploadEnabled
// ----------------------------------------------------------------------------
bool CReporterDaemonMonitor::autoUploadEnabled()
{
    return d_ptr->autoUpload;
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitor::setAutoUpload
// ----------------------------------------------------------------------------
void CReporterDaemonMonitor::setAutoUpload(bool state)
{
    d_ptr->autoUpload = state;
    if (!state)
    {
        qDebug() << __PRETTY_FUNCTION__ << "Calling quit() on Auto Uploader.";
        CReporterAutoUploaderProxy proxy(CReporter::AutoUploaderServiceName,
                                         CReporter::AutoUploaderObjectPath, QDBusConnection::sessionBus());

        QDBusPendingReply <bool> reply = proxy.quit();
        // This blocks.
        reply.waitForFinished();
    }
}

// End of file.
