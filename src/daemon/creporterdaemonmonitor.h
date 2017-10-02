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

#ifndef CREPORTERDAEMONMONITOR_H
#define CREPORTERDAEMONMONITOR_H

#include <QObject>
#include <QVariantList>

class CReporterCoreRegistry;
class CReporterDaemonMonitorPrivate;

/*!
 * @class CReporterDaemonMonitor
 * @brief This class implements the monitor for the "core-dump" directories.
 *
 *  This class is instantiated by the CReporterDaemon class, when client requests
 *  to start monitoring for core-dumps directories and destroyed when client wants to
 *  stop monitoring. Actual monitoring handled by the QFileSystemWatcher class instance.
 *  When instantiated, class connects directoryChanged signal to private slot.
 */
class CReporterDaemonMonitor : public QObject
{
    Q_OBJECT

public:
    /*!
     * Creates new instance of coredump directory monitor.
     *
     * @param parent the parent QObject.
     *
     * @sa CReporterCoreRegistry
     */
    CReporterDaemonMonitor(QObject *parent);

    ~CReporterDaemonMonitor();

    /*!
      * @brief Returns auto-delete max similar cores property
      *
      * @return Number of similar cores to keep
      */
    int autoDeleteMaxSimilarCores();

    /*!
      * @brief Sets auto-delete max similar count.
      *
      * @param value number of similar cores to keep
      */
    void setAutoDeleteMaxSimilarCores(int value);

signals:
    /*!
      * @brief Sent, when new rich-core dump is found.
      *
      * @param path Absolute file path to rich-core dump.
      */
    void richCoreNotify(const QString &path);

private:
    Q_DECLARE_PRIVATE(CReporterDaemonMonitor)
    CReporterDaemonMonitorPrivate *d_ptr;

#ifdef CREPORTER_UNIT_TEST
    friend class Ut_CReporterDaemonMonitor;
#endif
};

#endif // CREPORTERDAEMONMONITOR_H
