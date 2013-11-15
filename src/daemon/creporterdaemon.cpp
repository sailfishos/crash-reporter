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

#include <QDebug>
#include <QDBusConnection>

// User includes.

#include "creporterdaemon.h"
#include "creporterdaemon_p.h"
#include "creporterdaemonadaptor.h"
#include "creporterdaemonmonitor.h"
#include "creporternwsessionmgr.h"
#include "creportersavedstate.h"
#include "creportercoreregistry.h"
#include "creporterutils.h"
#include "creportersettingsobserver.h"
#include "creporternamespace.h"
#include "creporterprivacysettingsmodel.h"
#include "creporterautouploaderproxy.h"
#include "creporternotification.h"

#ifndef CREPORTER_UNIT_TEST
#include "creportersavedstate.h"
#endif

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
    d->lifelogTimer = 0;
    d->lifelogUpdateCount = 0;

    // Create registry instance preserving core locations.
    d->registry = new CReporterCoreRegistry();
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
    CReporterSavedState::freeSingleton();

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

    if (CReporterPrivacySettingsModel::instance()->notificationsEnabled()
        || CReporterPrivacySettingsModel::instance()->automaticSendingEnabled())
    {
        // Read from settings file, if monitor should be started.
        startCoreMonitoring();
    }

    // Add watcher to monitor changes in settings.
    d->settingsObserver->addWatcher(Settings::ValueNotifications);
    d->settingsObserver->addWatcher(Settings::ValueAutomaticSending);
    d->settingsObserver->addWatcher(Settings::ValueAutoDeleteDuplicates);
    d->settingsObserver->addWatcher(Settings::ValueLifelog);

    connect(d->settingsObserver, SIGNAL(valueChanged(QString,QVariant)),
                this, SLOT(settingValueChanged(QString,QVariant)));

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

    // Start lifelogging if it's enabled in settings
    setLifelogEnabled(CReporterPrivacySettingsModel::instance()->lifelogEnabled());

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

        connect(d->monitor, SIGNAL(richCoreNotify(QString)), SLOT(newCoreDump(QString)));

        qDebug() << __PRETTY_FUNCTION__ << "Core monitoring started.";

        if (fromDBus) {
            CReporterPrivacySettingsModel::instance()->setNotificationsEnabled(true);
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
    if (key == Settings::ValueNotifications || key == Settings::ValueAutomaticSending)
    {
        if (value.toBool())
        {
            startCoreMonitoring();
        }
        else if (!CReporterPrivacySettingsModel::instance()->notificationsEnabled()
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
    else if (key == Settings::ValueLifelog)
    {
        setLifelogEnabled(value.toBool());
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
//  CReporterDaemon::setLifelogEnabled
// ----------------------------------------------------------------------------
void CReporterDaemon::setLifelogEnabled(bool enabled)
{
    Q_D(CReporterDaemon);
    if (enabled && !d->lifelogTimer)
    {
        updateLifelog();
        // Start the timer to update lifelog
        d->lifelogTimer = new QTimer(this);
        connect(d->lifelogTimer, SIGNAL(timeout()), SLOT(updateLifelog()));
        d->lifelogTimer->start(CReporter::LifelogUpdateInterval);
    }
    else if (!enabled && d->lifelogTimer)
    {
        delete d->lifelogTimer;
        d->lifelogTimer = 0;
    }
}

// ----------------------------------------------------------------------------
// constant lifelogging commands used by the update function below
// ----------------------------------------------------------------------------

const char *LL_ID_GEN_CMD = "echo LL_ID_GEN "
    "sn=`/usr/bin/sysinfoclient -g /device/production-sn | /usr/bin/awk 'BEGIN {FS=\" \"} {print $3}'`\\;"
    "hw=`ssu mo | sed 's/^Device model is: //'`\\;"
    "sw=`sed -n 's/PRETTY_NAME=\"\\(.*\\)\"/\\1/p' /etc/os-release`\\;"
    "wlanmac=`ip link show wlan0 | tail -n 1 | awk '{print $2}'`";

const char *LL_ID_CELL_CMD = "echo LL_ID_CELL "
    "imei=`ssu s | grep 'Device UID' | awk '{print $3}'`\\;"
    "imsi=`dbus-send --system --print-reply --dest=org.ofono /ril_0 org.ofono.SimManager.GetProperties | sed -n '/SubscriberIdentity/{n;s/.*\"\\(.*\\)\".*/\\1/p}'`\\;"
    "cellmosw=`/usr/bin/dbus-send --system --print-reply --dest=com.nokia.csd.Info /com/nokia/csd/info com.nokia.csd.Info.GetMCUSWVersion | /usr/bin/tail -n +2`";

const char *LL_TICK_CMD = "echo LL_TICK "
    "date=`/bin/date +%s`\\;"
    "`upower -i /org/freedesktop/UPower/devices/battery_battery | awk '/percentage:/{printf \"batt_perc=%s;\",$2}/state:/{printf \"state=%s;\",$2}'`"
    "uptime=`/bin/cat /proc/uptime`\\;loadavg=`/usr/bin/awk '{print $3,$4,$5}' /proc/loadavg`\\;"
    "memfree=`/usr/bin/awk '/MemFree:/{print $2}' /proc/meminfo`";

const char *LL_CELL_CMD = "echo LL_CELL "
    "date=`/bin/date +%s`\\;"
    "ssc=`/usr/bin/dbus-send --system --print-reply --dest=com.nokia.phone.SSC /com/nokia/phone/SSC com.nokia.phone.SSC.get_modem_state | /usr/bin/tail -n +2 | /usr/bin/awk '{print $2}'`\\;"
    "gprs-status=`/usr/bin/dbus-send --system --print-reply --dest=com.nokia.csd.GPRS /com/nokia/csd/gprs com.nokia.csd.GPRS.GetStatus | /usr/bin/tail -n +2 | /usr/bin/awk '$1~/string|boolean|uint64/ {print $2}'`\\;"
    "gprs-serv-status=`/usr/bin/dbus-send --print-reply --system --dest=com.nokia.csd.GPRS /com/nokia/csd/gprs org.freedesktop.DBus.Properties.GetAll string: | /usr/bin/tail -n +2 | /usr/bin/awk '$2~/boolean|uint64/{print $3}'`";

const char *LL_DF_CMD = "echo -e \"LL_DF\\n\""
    "\"`df`\\n\""
    "LL_DF_END";

const char *LL_MEMINFO_CMD = "echo -e \"LL_MEMINFO\\n\""
    "\"`cat /proc/meminfo`\\n\""
    "LL_MEMINFO_END";

const char *LL_INT_CMD = "echo -e \"LL_INT\\n\""
    "\"`cat /proc/interrupts`\\n\""
    "LL_INT_END";

const char *LL_PERIODIC_COMMANDS[] = {
    "TICK", LL_TICK_CMD,
    "CELL", LL_CELL_CMD,
    "DF", LL_DF_CMD,
    "MEMINFO", LL_MEMINFO_CMD,
    "INT", LL_INT_CMD,
    NULL
};

// ----------------------------------------------------------------------------
//  CReporterDaemon::updateLifelog
// ----------------------------------------------------------------------------
void CReporterDaemon::updateLifelog()
{
    Q_D(CReporterDaemon);
    QStringList* corePathsPtr = d->registry->getCoreLocationPaths();
    QStringList corePaths(*corePathsPtr);
    delete corePathsPtr; corePathsPtr = 0;
    if (corePaths.isEmpty())
    {
        qDebug() << __PRETTY_FUNCTION__ << "No core paths available. Cannot update lifelog";
        return;
    }

    // Abort update if lifelog was updated recently.
    // This happens when e.g. lifelogging is disabled and re-enabled
    if (d->lifelogLastUpdate.isValid() &&
        d->lifelogLastUpdate.addSecs(CReporter::LifelogMinimumUpdateInterval) > QDateTime::currentDateTimeUtc())
        return;

    QFile lifelogFile(corePaths.first() + "/" + "lifelog");

    if (d->lifelogUpdateCount > 23 ||
        d->lifelogUpdateCount < 1 ||
        (d->lifelogLastUpdate.isValid() && d->lifelogLastUpdate.addSecs(12*60*60) < QDateTime::currentDateTimeUtc()))
    {
        d->lifelogUpdateCount = 0;
        d->lifelogLastUpdate = QDateTime();

        //Package old lifelog
        if (lifelogFile.exists() && lifelogFile.open(QFile::ReadOnly))
        {
            QString firstLine = lifelogFile.readLine();
            lifelogFile.close();
            int serialStart = firstLine.indexOf('=')+1;
            QString serialNumber = firstLine.mid(serialStart, firstLine.indexOf(';')-serialStart);
            QString destFilePath = corePaths.first() + "/"
                + QString("%1-%2-%3-%4.rcore.lzo").arg(CReporter::LifelogPackagePrefix,
                                                    QDateTime::currentDateTime().toString("yyyyMMdd-hhmmss"),
                                                    serialNumber,
                                                    QString::number(qrand(),16));
            int ret = system(QString("lzop -U -o\"%1\" \"%2\"").arg(destFilePath, lifelogFile.fileName()).toLocal8Bit());
            qDebug() << __PRETTY_FUNCTION__ << "lifelog lzop exit code:" << ret;
        }
    }

    if (!lifelogFile.exists())
    {
        int ret = system(QString(">\"%1\"").arg(lifelogFile.fileName()).prepend(LL_ID_GEN_CMD).toLocal8Bit());
        qDebug() << __PRETTY_FUNCTION__ << "lifelog ID_GEN command exit code:" << ret;
        ret = system(QString(">>\"%1\"").arg(lifelogFile.fileName()).prepend(LL_ID_CELL_CMD).toLocal8Bit());
        qDebug() << __PRETTY_FUNCTION__ << "lifelog ID_CELL command exit code:" << ret;
    }

    for (int i = 0; LL_PERIODIC_COMMANDS[i]; i += 2) {
        QString command(QString(">>\"%1\"").arg(lifelogFile.fileName()));
        command.prepend(LL_PERIODIC_COMMANDS[i + 1]);
        int ret = system(command.toLocal8Bit());
        qDebug() << __PRETTY_FUNCTION__ << "lifelog" << LL_PERIODIC_COMMANDS[i]
                 << "command exit code:" << ret;
    }

    d->lifelogUpdateCount++;
    d->lifelogLastUpdate = QDateTime::currentDateTimeUtc();
}

// ----------------------------------------------------------------------------
// CReporterDaemon::newCoreDump
// ----------------------------------------------------------------------------

void CReporterDaemon::newCoreDump(const QString& filePath)
{
    Q_D(CReporterDaemon);
    QStringList* corePathsPtr = d->registry->getCoreLocationPaths();
    QStringList corePaths(*corePathsPtr);
    delete corePathsPtr; corePathsPtr = 0;
    if (corePaths.isEmpty())
    {
        qDebug() << __PRETTY_FUNCTION__ << "No core paths available. Cannot update lifelog";
        return;
    }

    QFile lifelogFile(corePaths.first() + "/" + "lifelog");

    if (d->lifelogTimer && lifelogFile.exists() && !filePath.contains(CReporter::LifelogPackagePrefix))
    {
        QString coreDumpCommand = QString("echo LL_COREDUMP date=`date +%s`, %1").arg(filePath);
        int ret = system(QString(">>\"%1\"").arg(lifelogFile.fileName()).prepend(coreDumpCommand).toLocal8Bit());
        qDebug() << __PRETTY_FUNCTION__ << "lifelog COREDUMP command exit code:" << ret;
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
