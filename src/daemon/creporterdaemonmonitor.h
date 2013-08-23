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

// System includes.

#include <QObject>
#include <QVariantList>

// Forward declarations.

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
         * @brief Class constructor.
         *
         * @param reg Pointer to CReporterCoreRegistry instance.
         *
         * @sa CReporterCoreRegistry
         */
        CReporterDaemonMonitor(CReporterCoreRegistry *reg);

        /*!
         * @brief Class destructor.
         *
         */
        ~CReporterDaemonMonitor();

        /*!
          * @brief Requests dialog from Crash Reporter UI.
          *
          * @param dialogName Dialog type to reaquest.
          * @param arguments Request arguments.
          *
          * @return True, if request succeeds; otherwise false.
          *
          * @note This function call is synchronous.
          */
        static bool notifyCrashReporterUI(const QString &dialogName,
                                          const QVariantList &arguments);

        /*!
          * @brief Send a list of new files to Auto Uploader.
          *
          * @param filesToUpload List of files to be uploaded.
          *
          * @return True, if files were successfully added to the upload queue.
          */
        static bool notifyAutoUploader(const QStringList &filesToUpload);

        /*!
          * @brief Returns auto-delete property.
          *
          * @return True, if enabled; otherwise false.
          */
        bool autoDeleteEnabled();

        /*!
          * @brief Sets auto-delete property.
          *
          * @param status True or False.
          */
        void setAutoDelete(bool state);

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

        /*!
          * @brief Returns auto-upload property.
          *
          * @return True, if enabled; otherwise false.
          */
        bool autoUploadEnabled();

        /*!
          * @brief Sets auto-upload property.
          *
          * @param status True or False.
          */
        void setAutoUpload(bool state);

Q_SIGNALS:
        /*!
          * @brief Sent, when new rich-core dump is found.
          *
          * @param path Absolute file path to rich-core dump.
          */
        void richCoreNotify(const QString &path);

    private: // data
	
        Q_DECLARE_PRIVATE(CReporterDaemonMonitor)
        //! @arg Pointer to private class.
        CReporterDaemonMonitorPrivate *d_ptr;

#ifdef CREPORTER_UNIT_TEST
	friend class Ut_CReporterDaemonMonitor;
#endif
};

#endif // CREPORTERDAEMONMONITOR_H

// End of file.
