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

#ifndef CREPORTERDAEMONMONITOR_P_H
#define CREPORTERDAEMONMONITOR_P_H

// System includes.

#include <QFileSystemWatcher>
#include <QString>
#include <QList>

// Forward declarations.

class CReporterCoreRegistry;
class CReporterDaemonMonitor;

/*!
  * @class CReporterHandledRichCore
  * @brief This class represent handled rich-core file.
  *
  */
class CReporterHandledRichCore
{
    public:
        /*!
          * @brief Class constructor.
          *
          * @param filePath Rich-core file path.
          */
        CReporterHandledRichCore(const QString &filePath);

        /*!
          * @brief Class destructor.
          *
          */
        ~CReporterHandledRichCore();

        /*!
          * @brief Returns true, if @a other has similar information than this object.
          *
          * @param other CReporterHandledRichCore object.
          */
        bool operator==(const CReporterHandledRichCore &other) const;

    public:
        //! @arg Binary file name of the application.
        QString binaryName;
        //! @arg Signal number (SIGNUM).
        int signalNumber;
        //! @arg File size.
        qint64 size;
        //! @arg Upper file size limit.
        qint64 upperLimit;
        //! @arg Lower file size limit.
        qint64 lowerLimit;
        //! @arg Counter for duplicates.
        int count;
};

/*!
 * @class CReporterDaemonMonitorPrivate
 * @brief Private CReporterDaemonMonitor class.
 *
 * @sa CReporterDaemonMonitor
 */
class CReporterDaemonMonitorPrivate : public QObject
{
    Q_OBJECT

    public:
        /*!
         * @brief Class constructor.
         *
         */
        CReporterDaemonMonitorPrivate();

        /*!
         * @brief Class destructor.
         *
         */
        ~CReporterDaemonMonitorPrivate();

    public Q_SLOTS:
        /*!
         * @brief Adds the "\core-dumps" -directory paths currently present in the file system.
         *     to the QFileSystemWatcher.
         *
         * @sa http://doc.trolltech.com/4.6/qfilesystemwatcher.html.
         */
        void addDirectoryWatcher();

        /*!
         * @brief Removes monitored directories from the QFileSystemWatcher.
         *
         * @sa http://doc.trolltech.com/4.6/qfilesystemwatcher.html.
         */
        void removeDirectoryWatcher();

        /*!
         * @brief Called when directoryChanged signal is received.
         *
         * @param Reference to changed directory.
         */
        void handleDirectoryChanged(const QString &path);

        /*!
         * @brief Re-enables monitoring of core-dump dir when USB mass storage has been disabled and MyDocs is back in use
         *
         */
        void handleParentDirectoryChanged();

        /*!
          * @brief Called, when signal is received from CReporterNotification class.
          *
          * @sa CReporterNotification::activated()
          * @sa CReporterNotification::timeouted()
          */
        void handleNotificationEvent();

    public:
        /*!
         * @brief Checks for duplicate rich-cores, if auto-delete setting is enabled.
         *
         * Deletes rich-core file, if maximum number of similar files have been handled.
         *
         * @param Path to rich-core file to check.
         * @return True, if file was dublicate and can be deleted; otherwise false.
         */
        bool checkForDuplicates(const QString &path);

    public:
        //! @arg Pointer to the registry instance.
        CReporterCoreRegistry *registry;
        //! @arg For monitoring directories.
        QFileSystemWatcher watcher;
        //! @arg Watcher for monitoring the return of an unmounted directory for when core-dumps dir has disappeared because of USB mass storage mode
        QFileSystemWatcher parentDirWatcher;
        //! @arg List of handled rich-cores.
        QList <CReporterHandledRichCore *> handledRichCores;
        //! @arg State of auto-delete feature (enabled/ disabled).
        bool autoDelete;
        //! @arg Number of similar cores to keep when auto-delete is enabled
        int autoDeleteMaxSimilarCores;
        //! @arg State of auto-upload feature (enabled/ disabled).
        bool autoUpload;

        Q_DECLARE_PUBLIC(CReporterDaemonMonitor)
        //! @arg Pointer to public class.
        CReporterDaemonMonitor *q_ptr;
};

#endif // CREPORTERDAEMONMONITOR_P_H

// End of file.
