/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Raimo Gratseff <ext-raimo.gratseff@nokia.com>
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

#ifndef CREPORTERAUTOUPLOADER_H
#define CREPORTERAUTOUPLOADER_H

#include <QObject>
#include <QVariant>
#include <QDBusError>
#include <QStringList>

class CReporterAutoUploaderPrivate;

/*!
  * @class CReporterAutoUploader
  * @brief This class handles automatic uploading of rich-core files
  *
  * Auto Uploader runs in Qt main loop and receives upload requests from D-Bus
  *
  */
class CReporterAutoUploader : public QObject
{
    Q_OBJECT

public:
    CReporterAutoUploader();
    ~CReporterAutoUploader();

public slots:
    /**
     * Queues given rich core files for upload.
     *
     * @param fileList list of files to upload.
     * @param obeyResourcesRestrictions @c false if the files should be uploaded
     *                                  regardless of the network connection type,
     *                                  i.e. also over paid mobile/3G, and battery
     *                                  state.
     * @return @c true if files were successfully queued for upload.
     */
    bool uploadFiles(const QStringList &fileList, bool obeyResourcesRestrictions);

    /**
     * Makes auto-uploader exit its main loop.
     */
    void quit();

private Q_SLOTS:

    /*!
      * @brief Called, when upload engine has finished uploading.
      *
      * @param error Error code.
      * @param sent Number of files uploaded succesfully.
      * @param total Number of files requested to send.
      */
    void engineFinished(int error, int sent, int total);

private:
    Q_DECLARE_PRIVATE(CReporterAutoUploader)

    CReporterAutoUploaderPrivate *d_ptr;

#ifdef CREPORTER_UNIT_TEST
    friend class Ut_CReporterAutoUploader;
#endif
};

#endif // CREPORTERAUTOUPLOADER_H

