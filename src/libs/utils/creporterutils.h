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

#ifndef CREPORTERUTILS_H
#define CREPORTERUTILS_H

// System includes

#include <QFileInfo>

// User includes.

#include "creporterexport.h"

/*! @class CReporterUtils
 *  @brief This class implements utility functions.
 */
class CREPORTER_EXPORT CReporterUtils : public QObject
{
public:

	/*!
     * Checks if the file at given path can be accepted for upload.
     *
     * Valid files are those which have .rcore or .rcore.lzo suffix, the others
     * are rejected by the method.
     *
     * @param path rich core file path
     * @return true if it can be accepted, otherwise false
	 */
    static bool validateCore(const QString &path);

	/*!
     * @brief This function converts QString to char* format.
	 *
     * @param str string to convert.
     * @return string in char* format.
	 */
    static char* qstringToChar(const QString &str);

	/*!
     * @brief Checks the status of the path using stat().
     * 
     * @param path Reference to path of which status is to be checked.
     * @return true, if mounted otherwise false.
     */ 
    static bool isMounted(const QString &path);

	/*!
	 * Removes the given file.
	 *
     * @param file Path to the file to remove.
     * @return true, if operation succeeds, otherwise false.
	 */
    static bool removeFile(const QString &path);

    /*!
     * Parses the components of *rcore.lzo filename.
     *
     * Method takes the path to the rich core file and parses application name,
     * HWID, PID and signal from it. Data can be accessed by referencing
     * indexes of the returned QStringList (0 = Application name, 1 = HWID,
     * 2 = SIGNUM and 3 = PID).
     *
     * @param Absolute file path to rich core file.
     * @return Data extracted to string list.
     */
    static QStringList parseCrashInfoFromFilename(const QString &filePath);

    /*!
      * @brief Appends the user comments to *.lzo -file.
      *
      * @param text File content to be appended.
      * @param filepath Path to *.lzo to be modified.
      * @return True, if operation was successfull otherwise false.
      */
    static bool appendToLzo(const QString &text, const QString &filePath);

    /*!
     * @brief Returns file size in string format.
     *
     * @param size File size to convert.
     * @return Size in string.
     */
    static QString fileSizeToString(const quint64 size);

    /*!
     * @brief Returns the device ID used in SSU requests.
     *
     * @return Device ID.
     */
    static QString deviceUid();

    /*!
     * @brief Returns on what kind of system this application is running.
     *
     * @return Model of the device
     */
    static QString deviceModel();

    /*!
     * Sends a request for auto uploader daemon to add files into upload queue.
     *
     * @param filesToUpload A list of files we want to upload to the server.
     * @return @c true if files were successfully added, otherwise @c true.
     */
    static bool notifyAutoUploader(const QStringList &filesToUpload);
};

#endif // CREPORTERUTILS_H
