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

// User includes.

#include "creporterdaemon.h"
#include "creporterdaemon_p.h"
#include "creporterdaemonadaptor.h"
#include "creporterdaemonmonitor.h"
#include "creporternwsessionmgr.h"
#include "creportersavedstate.h"
#include "creportercoreregistry.h"
#include "creporterutils.h"
#include "creporternamespace.h"
#include "creporterprivacysettingsmodel.h"
#include "creporternotification.h"
#include "powerexcesshandler.h"

CReporterDaemon::CReporterDaemon() :
  d_ptr(new CReporterDaemonPrivate(this))
{
    // Adaptor class is deleted automatically, when the class, it is
    // attached to is deleted.
    new CReporterDaemonAdaptor(this);

#ifndef CREPORTER_UNIT_TEST
    new PowerExcessHandler(this);
#endif
}

// ----------------------------------------------------------------------------
// CReporterDaemon::~CReporterDaemon
// ----------------------------------------------------------------------------
CReporterDaemon::~CReporterDaemon()
{	
    qDebug() << __PRETTY_FUNCTION__ << "Daemon destroyed.";

    CReporterPrivacySettingsModel::instance()->freeSingleton();
    CReporterSavedState::freeSingleton();
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

    if (CReporterPrivacySettingsModel::instance()->notificationsEnabled()
        || CReporterPrivacySettingsModel::instance()->automaticSendingEnabled())
    {
        // Read from settings file, if monitor should be started.
        startCoreMonitoring();
    }

#ifndef CREPORTER_UNIT_TEST
    // Each time daemon is started, we count successful core uploads from zero.
    CReporterSavedState *state = CReporterSavedState::instance();
    state->setUploadSuccessCount(0);
    state->writeSettings();
#endif

    if (CReporterPrivacySettingsModel::instance()->automaticSendingEnabled())
    {
        QStringList files = collectAllCoreFiles();

        if (!files.isEmpty() &&
            CReporterNwSessionMgr::unpaidConnectionAvailable() &&
            !CReporterUtils::notifyAutoUploader(files))
        {
            qDebug() << __PRETTY_FUNCTION__ << "Failed to add files to the queue.";
        }
    }
    else if (CReporterPrivacySettingsModel::instance()->notificationsEnabled())
    {
        QStringList files = collectAllCoreFiles();

        if (!files.isEmpty()) {
            CReporterNotification *notification = new CReporterNotification(
                    CReporter::ApplicationNotificationEventType,
                    //% "This system has stored crash reports."
                    qtTrId("crash_reporter-notify-has_stored_cores"),
                    QString(), this);

            connect(notification, &CReporterNotification::timeouted,
                    notification, &CReporterNotification::deleteLater);
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
        d->monitor = new CReporterDaemonMonitor(this);
        Q_CHECK_PTR(d->monitor);

        qDebug() << __PRETTY_FUNCTION__ << "Core monitoring started.";

        if (fromDBus) {
            CReporterPrivacySettingsModel::instance()->setNotificationsEnabled(true);
            CReporterPrivacySettingsModel::instance()->writeSettings();
        }
        d->monitor->setAutoDeleteMaxSimilarCores(
                CReporterPrivacySettingsModel::instance()->autoDeleteMaxSimilarCores());
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
              CReporterPrivacySettingsModel::instance()->setNotificationsEnabled(false);
              CReporterPrivacySettingsModel::instance()->writeSettings();
        }
	}
}

// ----------------------------------------------------------------------------
// CReporterDaemon::collectAllCoreFiles
// ----------------------------------------------------------------------------
QStringList CReporterDaemon::collectAllCoreFiles()
{
    return CReporterCoreRegistry::instance()->collectAllCoreFiles();
}

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

CReporterDaemonPrivate::CReporterDaemonPrivate(CReporterDaemon *parent):
  monitor(0), timerId(0), q_ptr(parent)
{
    Q_Q(CReporterDaemon);

    QObject::connect(CReporterPrivacySettingsModel::instance(),
                     SIGNAL(notificationsEnabledChanged()),
                     q, SLOT(onNotificationsSettingChanged()));
}

void CReporterDaemonPrivate::onNotificationsSettingChanged()
{
    Q_Q(CReporterDaemon);

    CReporterPrivacySettingsModel &settings =
            *CReporterPrivacySettingsModel::instance();

    if (settings.notificationsEnabled()) {
        q->startCoreMonitoring();
    } else if (!settings.automaticSendingEnabled()) {
        q->stopCoreMonitoring();
    }
}

#include "moc_creporterdaemon.cpp"
