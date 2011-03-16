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

// System includes.

#include <QDebug>
#include <QDBusConnection>

// User includes.

#include "creporterdaemon.h"
#include "creporterdaemon_p.h"
#include "creporterdaemonadaptor.h"
#include "creporterdaemonmonitor.h"
#include "creporterdaemoncoreregistry.h"
#include "creporterutils.h"
#include "creportersettingsobserver.h"
#include "creporternamespace.h"
#include "creporterprivacysettingsmodel.h"
#include "creporterdialogserverproxy.h"
#include "creporterautouploaderproxy.h"
#include "creporternotification.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterDaemon::CReporterDaemon
// ----------------------------------------------------------------------------
CReporterDaemon::CReporterDaemon() :
    d_ptr(new CReporterDaemonPrivate())
{
    Q_D(CReporterDaemon);

    d->monitor = 0;
    d->timerId = 0;

    // Create registry instance preserving core locations.
    d->registry = new CReporterDaemonCoreRegistry();
    Q_CHECK_PTR(d->registry);

    // Adaptor class is deleted automatically, when the class, it is
    // attached to is deleted.
    new CReporterDaemonAdaptor(this);
}

// ----------------------------------------------------------------------------
// CReporterDaemon::~CReporterDaemon
// ----------------------------------------------------------------------------
CReporterDaemon::~CReporterDaemon()
{	
    Q_D(CReporterDaemon);
    qDebug() << __PRETTY_FUNCTION__ << "Daemon destroyed.";

    if (d->monitor) {
        // Delete monitor instance and stop core monitoring.
        delete d->monitor;
        d->monitor = 0;
    }

	delete d->registry;
    d->registry = 0;

    CReporterPrivacySettingsModel::instance()->freeSingleton();

	delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterDaemon::setDelayedStartup
// ----------------------------------------------------------------------------
void CReporterDaemon::setDelayedStartup(int timeout)
{
    Q_D(CReporterDaemon);

    qDebug() << __PRETTY_FUNCTION__ << "Delaying startup for" << timeout / 1000 << "seconds.";
    if (timeout > 0) {
        d->timerId = startTimer(timeout);
    }
}

// ----------------------------------------------------------------------------
// CReporterDaemon::initiateDaemon
// ----------------------------------------------------------------------------
bool CReporterDaemon::initiateDaemon()
{
    Q_D( CReporterDaemon );
    qDebug() << __PRETTY_FUNCTION__ << "Starting daemon...";

    if (!CReporterPrivacySettingsModel::instance()->isValid())
    {
        qWarning() << __PRETTY_FUNCTION__ << "Invalid settings";
        // Exit, if settings are missing.
        return false;
    }

    QString filename = CReporterPrivacySettingsModel::instance()->settingsFile(); 

    if (!startService())
    {
		return false;
	}

    d->settingsObserver = new CReporterSettingsObserver(filename, this);

    if (CReporterPrivacySettingsModel::instance()->sendingEnabled()
        || CReporterPrivacySettingsModel::instance()->automaticSendingEnabled())
    {
        // Read from settings file, if monitor should be started.
        startCoreMonitoring();
    }

    // Add watcher to monitor changes in settings.
    d->settingsObserver->addWatcher(Settings::ValueSending);
    d->settingsObserver->addWatcher(Settings::ValueAutomaticSending);
    d->settingsObserver->addWatcher(Settings::ValueAutoDeleteDuplicates);

    connect(d->settingsObserver, SIGNAL(valueChanged(QString,QVariant)),
                this, SLOT(settingValueChanged(QString,QVariant)));

    if (CReporterPrivacySettingsModel::instance()->automaticSendingEnabled())
    {
        QStringList files = collectAllCoreFiles();

        if (!files.isEmpty() && !CReporterDaemonMonitor::notifyAutoUploader(files))
        {
            qDebug() << __PRETTY_FUNCTION__ << "Failed to add files to the queue.";
        }
    }
    else if (CReporterPrivacySettingsModel::instance()->sendingEnabled())
    {
        // Collect rich-cores from the system and notify UI, if sending is enabled.
        QStringList files = collectAllCoreFiles();

        if (!files.isEmpty())
        {
            QVariantList arguments;
            arguments << files;

            bool openViaNotification = true; // Show notification
            arguments << openViaNotification;

            // Request dialog from the UI to send or delete crash reports.
            // old way
            //if (!CReporterDaemonMonitor::notifyCrashReporterUI(CReporter::SendAllDialogType,
            //                                                   arguments)) {
            if (!CReporterDaemonMonitor::notifyCrashReporterUI(CReporter::SendSelectedDialogType,
                                                               arguments))
            {
                // UI failed to launch did not succeed. Try to show notification instead.
                // Daemon is not a Meego Touch application, thus translation with MLocale
                // won't work here.
                QString notificationSummary("This system has stored crash reports.");
                QString notificationBody("Unable to start Crash Reporter UI.");
                CReporterNotification *notification = 0;

                try
                {
                    notification = new CReporterNotification("crash-reporter",
                                                             notificationSummary, notificationBody);
                }
                catch (...)
                {
                    // Don't care if MNotifications bug
                    return true;
                }
                notification->setTimeout(10);
                notification->setParent(this);

                connect(notification, SIGNAL(activated()), this, SLOT(handleNotificationEvent()));
                connect(notification, SIGNAL(timeouted()), this, SLOT(handleNotificationEvent()));
            }
        }
    }

    return true;
}

// ----------------------------------------------------------------------------
// CReporterDaemon::startCoreMonitoring
// ----------------------------------------------------------------------------
void CReporterDaemon::startCoreMonitoring(const bool fromDBus)
{
    Q_D(CReporterDaemon);

    qDebug() << __PRETTY_FUNCTION__ << "Core monitoring requested. Called from DBus ="
            << fromDBus;

    if (!d->monitor) {
		// Create monitor instance and start monitoring cores.
        d->monitor = new CReporterDaemonMonitor(d->registry);
        Q_CHECK_PTR(d->monitor);

        qDebug() << __PRETTY_FUNCTION__ << "Core monitoring started.";

        if (fromDBus) {
            CReporterPrivacySettingsModel::instance()->setSendingEnabled(true);
            CReporterPrivacySettingsModel::instance()->writeSettings();
        }
        d->monitor->setAutoDelete(CReporterPrivacySettingsModel::instance()->autoDeleteDuplicates());
        d->monitor->setAutoDeleteMaxSimilarCores(
                CReporterPrivacySettingsModel::instance()->autoDeleteMaxSimilarCores());
        d->monitor->setAutoUpload(CReporterPrivacySettingsModel::instance()->automaticSendingEnabled());
    }
}

// ----------------------------------------------------------------------------
// CReporterDaemon::stopCoreMonitoring
// ----------------------------------------------------------------------------
void CReporterDaemon::stopCoreMonitoring(const bool fromDBus)
{
    Q_D(CReporterDaemon);
	
    if (d->monitor) {
		// Delete monitor instance and stop core monitoring.
		delete d->monitor;
        d->monitor = 0;
        
		qDebug() << __PRETTY_FUNCTION__ << "Core monitoring stopped.";

          if (fromDBus) {
              CReporterPrivacySettingsModel::instance()->setSendingEnabled(false);
              CReporterPrivacySettingsModel::instance()->writeSettings();
        }
	}
}

// ----------------------------------------------------------------------------
// CReporterDaemon::collectAllCoreFiles
// ----------------------------------------------------------------------------
QStringList CReporterDaemon::collectAllCoreFiles()
{
    Q_D(CReporterDaemon);

    return d->registry->collectAllCoreFiles();
}

// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterDaemon::settingValueChanged
// ----------------------------------------------------------------------------
void CReporterDaemon::settingValueChanged(const QString &key, const QVariant &value)
{
    qDebug() << __PRETTY_FUNCTION__ << "Setting:" << key << "has changed; value:" << value;
    if (key == Settings::ValueSending || key == Settings::ValueAutomaticSending)
    {
        if (value.toBool())
        {
            startCoreMonitoring();
        }
        else if (!CReporterPrivacySettingsModel::instance()->sendingEnabled()
               && !CReporterPrivacySettingsModel::instance()->automaticSendingEnabled())
        {
            if (key == Settings::ValueAutomaticSending)
            {
                if (d_ptr->monitor)
                {
                    d_ptr->monitor->setAutoUpload(value.toBool());
                }
            }
            stopCoreMonitoring();
        }
    }
    else if (key == Settings::ValueAutoDeleteDuplicates)
    {
        if (d_ptr->monitor)
        {
            d_ptr->monitor->setAutoDelete(value.toBool());
        }
    }

    if (key == Settings::ValueAutomaticSending)
    {
        if (d_ptr->monitor)
        {
            d_ptr->monitor->setAutoUpload(value.toBool());
        }
    }
}

// ----------------------------------------------------------------------------
//  CReporterDaemon::timerEvent
// ----------------------------------------------------------------------------
void CReporterDaemon::timerEvent(QTimerEvent *event)
{
    Q_D(CReporterDaemon);
    qDebug() << __PRETTY_FUNCTION__ << "Startup timer elapsed -> start now.";

    if (event->timerId() != d->timerId) return;

    // Kill timer and initiate daemon.
    killTimer(d->timerId);
    d->timerId = 0;

    if (!initiateDaemon()) {
        // If D-Bus registration fails, quit application.
        qApp->quit();
    }
}

// ----------------------------------------------------------------------------
//  CReporterDaemon::handleNotificationEvent
// ----------------------------------------------------------------------------
void CReporterDaemon::handleNotificationEvent()
{
    // Handle timeouted and activated signals from CReporterNotification
    // and destroy instance.
    CReporterNotification *notification = qobject_cast<CReporterNotification *>(sender());

    if (notification != 0) {
        delete notification;
    }
}

// ----------------------------------------------------------------------------
// CReporterDaemon::startService
// ----------------------------------------------------------------------------
bool CReporterDaemon::startService()
{
	qDebug() << __PRETTY_FUNCTION__ << "Starting D-Bus service...";

    if (!QDBusConnection::sessionBus().isConnected()) {
	  qWarning() << __PRETTY_FUNCTION__ << "D-Bus not running?";
	  return false;
	  }

    if (!QDBusConnection::sessionBus().registerService(CReporter::DaemonServiceName)) {
      qWarning() << __PRETTY_FUNCTION__
              << "Failed to register service, daemon already running?";
      return false;
	  }

    qDebug() << __PRETTY_FUNCTION__ << "Service:"
            << CReporter::DaemonServiceName << "registered.";

    QDBusConnection::sessionBus().registerObject(CReporter::DaemonObjectPath, this);

    qDebug() << __PRETTY_FUNCTION__ << "Object:"
            << CReporter::DaemonObjectPath << "registered.";

    // Good to go.
	qDebug() << __PRETTY_FUNCTION__ << "D-Bus service started.";
	return true;
}

// ----------------------------------------------------------------------------
// CReporterDaemon::stopService
// ----------------------------------------------------------------------------
void CReporterDaemon::stopService()
{
	qDebug() << __PRETTY_FUNCTION__ << "Stopping D-Bus service...";

    QDBusConnection::sessionBus().unregisterObject(CReporter::DaemonObjectPath);
    QDBusConnection::sessionBus().unregisterService(CReporter::DaemonServiceName);
}

// End of file
