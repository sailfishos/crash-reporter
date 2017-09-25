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

#ifndef CREPORTERDAEMONADAPTOR_H
#define CREPORTERDAEMONADAPTOR_H

// System includes

#include <QtDBus/QtDBus>

// Forward declarations

class QStringList;
class CReporterDaemonAdaptorPrivate;

/*!
 * @class CReporterDaemonAdaptor
 * @brief This class provides external interface for the daemon process over D-Bus.
 *
 * CReporterDaemonAdaptor class is a light-weight wrapper relaying calls into
 * the daemon object and signals from it. This class needs to be allocated from the stack
 * and will be destroyed automatically, when the parent class is destroyed.
 */
class CReporterDaemonAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.nokia.CrashReporter.Daemon")

public:
    /*!
     * @brief Constructor for the CReporterDaemonAdaptor class
     *
     * @param parent pointer to the parent class instance.
     */
    CReporterDaemonAdaptor(QObject *parent);

    /*!
     * @brief Class destructor.
     *
     */
    ~CReporterDaemonAdaptor();

public Q_SLOTS:

    /*!
     * @brief By calling this method, client can request daemon process to start
     * monitoring rich core dumps.
     *
     * @note This method call is asyncronous.
     */
    Q_NOREPLY void startCoreMonitoring();

    /*!
     * @brief By calling this method, client can request daemon process to stop
     * monitoring rich core dumps.
     *
     * @note This method call is asyncronous.
     */
    Q_NOREPLY void stopCoreMonitoring();

    /*!
     * @brief By calling this method, client This function returns list of
     * core files in the file system.
     *
     * @return List of absolute core file paths.
     */
    QStringList getAllCoreFiles();

private: // data

    Q_DECLARE_PRIVATE(CReporterDaemonAdaptor)
    //! Pointer to private data class.
    CReporterDaemonAdaptorPrivate *d_ptr;
};

#endif // CREPORTERDAEMONADAPTOR_H
