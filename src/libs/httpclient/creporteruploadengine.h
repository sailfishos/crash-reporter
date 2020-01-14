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

#ifndef CREPORTERUPLOADENGINE_H
#define CREPORTERUPLOADENGINE_H

#include <QObject>

#include "creporterexport.h"

class CReporterUploadEnginePrivate;
class CReporterUploadQueue;

/*!
  * @class CReporterUploadEngine
  * @brief This class implements functionality for uploading crash reports
  *         to analysis server.
  *
  */
class CREPORTER_EXPORT CReporterUploadEngine : public QObject
{
    Q_OBJECT

public:
    /*!
      * @enum ErrorType
      * @brief Descripes possible errors occured during upload.
      */
    enum ErrorType {
        //! No error.
        NoError = 0,
        //! Indicates errors occured on HTTP/ SSL protocol levels.
        ProtocolError,
        //! Error occured, when trying to establish internet connection.
        ConnectionNotAvailable,
        //! Internet connection was closed due to network disconnected.
        ConnectionClosed,
    };

    /*!
      * @brief Class constructor.
      *
      * @param queue Upload queue reference.
      * @param parent Parent object reference.
      *
      */
    CReporterUploadEngine(CReporterUploadQueue *queue, QObject *parent = 0);

    ~CReporterUploadEngine();

    /*!
      * @brief Returns last error string.
      *
      * @return Last error string, if available; empty if no error or not valid.
      */
    QString lastError() const;

Q_SIGNALS:
    /*!
      * @brief Sent, when engine has finished uploading files.
      *
      * @param error Error code.
      * @param sent Number of files uploaded successfully.
      * @param total Total number of files requested to send.
      */
    void finished(int error, int sent, int total);

public Q_SLOTS:
    /*!
     * @brief Cancels all pending uploads.
     *
     */
    void cancelAll();

private:
    Q_DECLARE_PRIVATE(CReporterUploadEngine)

    CReporterUploadEnginePrivate *d_ptr;
#ifdef CREPORTER_UNIT_TEST
    friend class Ut_CReporterUploadEngine;
#endif
};

#endif // CREPORTERUPLOADENGINE_H
