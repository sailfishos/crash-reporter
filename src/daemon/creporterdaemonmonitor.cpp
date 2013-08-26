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
#include "creporterutils.h"
#include "creporterdialogserverproxy.h"
#include "creporterautouploaderproxy.h"
#include "creporternamespace.h"
#include "creporternotification.h"
#include "creporterprivacysettingsmodel.h"

// ******** Class CReporterHandledRichCore ********

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterHandledRichCore::CReporterHandledRichCore
// ----------------------------------------------------------------------------
CReporterHandledRichCore::CReporterHandledRichCore(const QString &filePath)
{
    // Parse needed info for file path.
    QStringList rCoreInfo = CReporterUtils::parseCrashInfoFromFilename(filePath);

    binaryName = rCoreInfo[0];
    signalNumber = rCoreInfo[2].toInt();

    QFileInfo fi(filePath);
    size = fi.size();
    lowerLimit = (size * 95) / 100;
    upperLimit = (size * 105) / 100;

    count = 0;

    qDebug() << __PRETTY_FUNCTION__ << "Name:" << binaryName << ", Signal:" << signalNumber
            << ", Size:" << size << ", Lower:" << lowerLimit << ", Upper:" << upperLimit;
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
    if ((binaryName == other.binaryName) && (signalNumber == other.signalNumber) &&
        (other.size >= lowerLimit) && (other.size <= upperLimit)) {
        return true;
    }

    return false;
}

// ******** Class CReporterDaemonMonitorPrivate ********

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterDaemonMonitorPrivate::CReporterDaemonMonitorPrivate
// ----------------------------------------------------------------------------
CReporterDaemonMonitorPrivate::CReporterDaemonMonitorPrivate() :
        autoDelete(false), autoDeleteMaxSimilarCores(0), autoUpload(false)
{
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitorPrivate::~CReporterDaemonMonitorPrivate
// ----------------------------------------------------------------------------
CReporterDaemonMonitorPrivate::~CReporterDaemonMonitorPrivate()
{
    QList<CReporterHandledRichCore *> rCores =  handledRichCores;
    // Clear list.
    handledRichCores.clear();
    // Delete entries.
    qDeleteAll(rCores);
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
    // Sanity check. QFileSystemWatcher will send signal, if monitored directory was removed.
    if (changedDir.exists()) {
        qDebug() << __PRETTY_FUNCTION__ << "Changed directory exists.";
        // Check for new cores in changed directory.
        QString filePath = registry->checkDirectoryForCores(path);

        if (!filePath.isEmpty()) {
            // New core found.
            qDebug() << __PRETTY_FUNCTION__ << "New rich-core file found: " << filePath;

            QStringList details = CReporterUtils::parseCrashInfoFromFilename(filePath);

            emit q_ptr->richCoreNotify(filePath);

            if (autoDelete && checkForDuplicates(filePath)
                && !filePath.contains(CReporter::LifelogPackagePrefix)) {
                // Check for dublicates, if auto-deleting is enabled. If Maximum number of duplicates
                // exeeded, delete file.
                if (CReporterPrivacySettingsModel::instance()->notificationsEnabled())
                {
                    CReporterNotification notification(
                            CReporter::ApplicationNotificationEventType,
                            QString("%1 has crashed once again.").arg(details.at(0)),
                            QString("Duplicate crash report was deleted."));
                }
                CReporterUtils::removeFile(filePath);
                return;
            }

            if (!autoUpload)
            {
                QVariantList arguments;
                arguments << filePath;

                /* TODO: Here multiple-choice notification should be displayed
                 * with options to send or delete the crash report. So far
                 * disabling auto upload is not possible in the UI and we never
                 * get here. This code now at least leaks CReporterNotification
                 * and has to be re-implemented. Standard Sailfish notifications
                 * don't support multiple actions so far.*/

                if (!q_ptr->notifyCrashReporterUI(CReporter::NotifyNewDialogType, arguments)) {
                    // UI failed to launch. Try to show notification instead.
                    // Daemon is not a Meego Touch application, thus translation with MLocale
                    // won't work here.

                    QString notificationSummary;
                    if (filePath.contains(CReporter::LifelogPackagePrefix))
                    {
                        notificationSummary = "New lifelog report is ready.";
                    }
                    else
                    {
                        notificationSummary = "The application %1 crashed.";
                        notificationSummary = notificationSummary.arg(details.at(0));
                    }


                    QString notificationBody("Unable to start Crash Reporter UI");

                    CReporterNotification *notification = new CReporterNotification(
                            CReporter::ApplicationNotificationEventType,
                            notificationSummary, notificationBody);
                    notification->setParent(this);

                    connect(notification, SIGNAL(activated()), this, SLOT(handleNotificationEvent()));
                    connect(notification, SIGNAL(timeouted()), this, SLOT(handleNotificationEvent()));
                }
            }
            else
            {
                QScopedPointer<CReporterNotification> notification;
                QString summary;

                if (CReporterPrivacySettingsModel::instance()->notificationsEnabled() &&
                    !filePath.contains(CReporter::LifelogPackagePrefix)) {

                    summary = QString("%1 has crashed.").arg(details.at(0));

                    notification.reset(new CReporterNotification(
                            CReporter::AutoUploaderNotificationEventType,
                            summary, "Uploading automatically...",
                            this));
                }
                // In AutoUpload-mode try to upload all core files each time a new one has appeared
                QStringList fileList = registry->collectAllCoreFiles();
                if (!q_ptr->notifyAutoUploader(fileList))
                {
                    qWarning() << __PRETTY_FUNCTION__ << "Failed to start Auto Uploader.";
                    if (notification) {
                        notification->update(summary,
                                "Auto uploader failed to start, send the report manually");
                    }
                }
            }
        }
    }
    else // Monitored directory was deleted
    {
        // Re-add core dirs when the parent dir changes so that monitoring is resumed
        // after usb mass storage mode has been disconnected
        QDir changedDir(path);
        if (changedDir.cd("../.."))
        {
            connect(&parentDirWatcher, SIGNAL(directoryChanged(QString)), SLOT(handleParentDirectoryChanged()));
            parentDirWatcher.addPath(changedDir.absolutePath());
            qDebug() << __PRETTY_FUNCTION__ << "Directory was deleted. Started parent dir monitoring.";
        }
        else
        {
            qDebug() << __PRETTY_FUNCTION__ << "Directory was deleted. Parent dir does not exist.";
        }
    }
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitorPrivate::handleParentDirectoryChanged
// ----------------------------------------------------------------------------
void CReporterDaemonMonitorPrivate::handleParentDirectoryChanged()
{
    qDebug() << __PRETTY_FUNCTION__ << "Parent dir has changed. Trying to re-add directory watchers.";
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
    qDebug() << __PRETTY_FUNCTION__ << "Checking, if" << path << "has been handled for"
            << autoDeleteMaxSimilarCores << "times.";

    // Create new entry.
    CReporterHandledRichCore *rCore = new CReporterHandledRichCore(path);

    foreach (CReporterHandledRichCore *handled, handledRichCores) {
        // Loop through list to find duplicates.
        qDebug() << __PRETTY_FUNCTION__  << "Compare to:"
               << "Name:" << handled->binaryName << ", Signal:" << handled->signalNumber
               << ", Lower:" << handled->lowerLimit << ", Upper:" << handled->upperLimit;

        if (*handled == *rCore) {
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

// ******** Class CReporterDaemonMonitor ********

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterDaemonMonitor::CReporterDaemonMonitor
// ----------------------------------------------------------------------------
CReporterDaemonMonitor::CReporterDaemonMonitor(CReporterCoreRegistry *reg) :
    d_ptr(new CReporterDaemonMonitorPrivate())
{
    d_ptr->q_ptr = this;
    d_ptr->registry = reg;
	
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
// CReporterDaemonMonitor::notifyCrashReporterUI
// ----------------------------------------------------------------------------
bool CReporterDaemonMonitor::notifyCrashReporterUI(const QString &dialogName,
                                              const QVariantList &arguments)
{
    qDebug() << __PRETTY_FUNCTION__ << "Sending rich-core(s) to the client.";

    CReporterDialogServerProxy proxy(CReporter::DialogServerServiceName,
                                     CReporter::DialogServerObjectPath, QDBusConnection::sessionBus());

    QDBusPendingReply <bool> pReply =
            proxy.requestDialog(dialogName, arguments);
    // This blocks.
    pReply.waitForFinished();

    if (pReply.isError()) {
        qWarning() << __PRETTY_FUNCTION__ << "D-Bus error occured.";

        // Trace error.
        QDBusError dBusError(pReply.error());
        QString errorName = dBusError.name();

        qDebug() << __PRETTY_FUNCTION__ << "Name:" << errorName;
        qDebug() << __PRETTY_FUNCTION__ << "Message:" << dBusError.message();
        qDebug() << __PRETTY_FUNCTION__ << "Error string:" << dBusError.errorString(dBusError.type());

        if (errorName != "org.freedesktop.DBus.Error.Spawn.ChildSignaled") {
            // If crash-reporter-ui exited by another reason than on a signal.
            // Check, if Crash Reporter UI was started. If, not then system has encountered a
            // serious error preventing UI from starting and DBus message was not delivered.
            QDBusConnection connection = proxy.connection();
            QDBusReply<bool> reply =
                    connection.interface()->isServiceRegistered(CReporter::DialogServerServiceName);

            qDebug() << __PRETTY_FUNCTION__ << "Crash Reporter UI launched:" << reply.value();

            if (!reply.value()) {
                // Launcing UI failed.
                return false;
            }
        }
    }
    return true;
}

// ----------------------------------------------------------------------------
// CReporterDaemonMonitor::notifyAutoUploader
// ----------------------------------------------------------------------------
bool CReporterDaemonMonitor::notifyAutoUploader(const QStringList &filesToUpload)
{
    qDebug() << __PRETTY_FUNCTION__ << "Sending " << filesToUpload.size() << " to be uploaded.";
    CReporterAutoUploaderProxy proxy(CReporter::AutoUploaderServiceName,
                                        CReporter::AutoUploaderObjectPath, QDBusConnection::sessionBus());

    QDBusPendingReply <bool> reply = proxy.uploadFiles(filesToUpload);
    // This blocks.
    reply.waitForFinished();

    if (reply.isError())
    {
        qWarning() << __PRETTY_FUNCTION__ << "D-Bus error occured.";

        // Trace error.
        QDBusError dBusError(reply.error());
        QString errorName = dBusError.name();

        qDebug() << __PRETTY_FUNCTION__ << "Name:" << errorName;
        qDebug() << __PRETTY_FUNCTION__ << "Message:" << dBusError.message();
        qDebug() << __PRETTY_FUNCTION__ << "Error string:" << dBusError.errorString(dBusError.type());

        return false;
    }
    return true;
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
