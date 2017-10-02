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

#ifndef CREPORTERDAEMON_H
#define CREPORTERDAEMON_H

#include <QObject>
#include <QStringList>

class CReporterDaemonPrivate;

/*!
 * @class CReporterDaemon
 * @brief This class implements the Crash Reporter daemon process.
 *
 */
class CReporterDaemon : public QObject
{
    Q_OBJECT

public:
    CReporterDaemon();
    ~CReporterDaemon();

    /*!
      * @brief Sets delayed start up for daemon.
      *
      * @param timeout Millisecond value to delay startup.
      */
    void setDelayedStartup(int timeout);

public Q_SLOTS:
    /*!
     * @brief Initiates daemon process.
     *
     * @return true, if success otherwise false.
     */
    bool initiateDaemon();

    /*!
     * @brief This function emits monitoringActivated signal, if rich core monitoring is not
     * initiated yet.
     *
     * @param fromDBus True, if requested through DBus IF; otherwise false.
     * @sa CReporterDaemonMonitor
     * @sa CReporterDaemonMonitor::monitoringActivated
     */
    void startCoreMonitoring(const bool fromDBus = false);

    /*!
     * @brief This function emits monitoringDeactivated signal, if monitoring has been initiated.
     *
     * @param fromDBus True, if requested through DBus If; otherwise false.
     * @sa CReporterDaemonMonitor
     * @sa CReporterDaemonMonitor::monitoringDeactivated
     */
    void stopCoreMonitoring(const bool fromDBus = false);

    /*!
     *@brief  Returns a list of core file paths in the core-dumps directories.
     *
     * @return List of absolute core file paths. Empty, if no files found.
     */
    QStringList collectAllCoreFiles();

private slots:
    /*!
      * @brief Called, when timer elapses.
      *
      * @param event Pointer to timer event.
      */
    virtual void timerEvent(QTimerEvent *event);

private:
    /*!
      * @brief Starts D-Bus service and registers object.
      *
      * @return True, if registering was successfull; otherwise false.
      */
    bool startService();

    /*!
      * @brief Stops D-Bus service and unregisters object.
      *
      */
    void stopService();

private:
    Q_DECLARE_PRIVATE(CReporterDaemon)
    QScopedPointer<CReporterDaemonPrivate> d_ptr;

    Q_PRIVATE_SLOT(d_func(), void onNotificationsSettingChanged())

#ifdef CREPORTER_UNIT_TEST
    friend class Ut_CReporterDaemon;
    friend class Ut_CReporterDaemonProxy;
#endif
};

#endif // CREPORTERDAEMON_H
