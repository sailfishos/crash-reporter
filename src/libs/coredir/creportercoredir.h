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

#ifndef CREPORTERCOREDIR_H
#define CREPORTERCOREDIR_H

#include <QObject>
#include <QStringList>

class CReporterCoreDirPrivate;

/*!
 * @class CReporterCoreDir
 * @brief This class implements properties and methods for handling
 * core-dumps directory.
 *
 * This class is instantiated by the CReporterCoreRegistry,
 * when daemon process starts. Class provides methods for iterating directory for cores
 * and preserves a list of core files in the directory.
 */
class CReporterCoreDir : public QObject
{
    Q_OBJECT

public:
    /*!
     * @brief Class constructor.
     *
     * @param mpoint Core location mount point.
     * @param parent Owner of this object (default= 0).
     */
    CReporterCoreDir( QString &mpoint, QObject *parent = 0 );

    ~CReporterCoreDir();

    /*!
     * @brief Return core dumps directory path.
     *
     * @return Path in QString format.
     */
    QString getDirectory() const;

    /*!
     * @brief Return mount point of this core location.
     *
     * @return Path in QString format.
     */
    QString getMountpoint() const;

    /*!
     * @brief Set directory path for rich core dumps.
     *
     * @param dir Core dumps directory path.
     */
    void setDirectory( const QString &dir );

    /*!
     * @brief Set mount point for this core location.
     *
     * @param mpoint Mountpoint path.
     */
    void setMountpoint( const QString &mpoint );

    /*!
     * @brief Collects all valid core files from this directory and appends into lists.
     *
     * @param coreList Reference to list populated with absolute core file paths.
     */
    void collectAllCoreFilesAtLocation( QStringList &coreList );

    /*!
     * @brief Checks directory for new core files.
     *
     * @return Absolute path to core file. String is NULL, if new valid
     *  core file was not found.
     */
    QString checkDirectoryForCores();

public Q_SLOTS:
    /*!
      * @brief This function (re-)creates the directory for the rich core dumps.
      *
      */
    void createCoreDirectory();

    /*!
      * @brief This function iterates core-dumps directory for cores and refreshes internal list.
      *
      */
    void updateCoreList();

private:
    Q_DECLARE_PRIVATE(CReporterCoreDir)

    CReporterCoreDirPrivate *d_ptr;
#ifdef CREPORTER_UNIT_TEST
    friend class Ut_CReporterCoreDir;
#endif
};

#endif // CREPORTERCOREDIR_H
