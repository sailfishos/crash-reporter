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

#ifndef CREPORTERUPLOADENGINE_P_H
#define CREPORTERUPLOADENGINE_P_H

#include <QObject>

#include "creporteruploadengine.h"

class CReporterUploadItem;
class CReporterUploadQueue;
#ifdef CREPORTER_LIBBEARER_ENABLED
class CReporterNwSessionMgr;
#endif

/*!
  * @class CReporterUploadEnginePrivate
  *
  * @brief Private CReporterUploadEngine class.
  */
class CReporterUploadEnginePrivate : public QObject
{
    Q_OBJECT

public:
    /*!
     * @enum State
     *  @brief Descripes engine state.
     *
     */
    typedef enum {
        //! Initial state. No connection.
        NoConnection = 0,
        //! Engine is establishing internet connection.
        Connecting,
        //! Internet connection is created.
        Connected,
        //! Engine is closing internet connection.
        Closing,
        //! Engine is aborting uploads.
        Aborting,
    } State;

    CReporterUploadEnginePrivate();
    ~CReporterUploadEnginePrivate();

    /*!
      * @brief Sets new engine state.
      *
      * @param nextState New state.
      */
    void stateChange(State nextState);

public Q_SLOTS:
    /*!
     * @brief Uploads @a item to remote server.
     *
     * @param item Item to upload.
     */
    void uploadItem(CReporterUploadItem *item);

    /*!
     * @brief Called, when done() signal is received.
     *
     * @sa CReporterUploadQueue::done()
     */
    void queueDone();

    /*!
     * @brief Called, when done() signal is received.
     *
     * @sa CReporterUploadQueue::done()
     */
    void uploadFinished();
#ifdef CREPORTER_LIBBEARER_ENABLED
public Q_SLOTS:
    /*!
     * @brief Called, when network session opens.
     *
     */
    void sessionOpened();

    /*!
     * @brief Called, when network session disconnects.
     *
     */
    void sessionDisconnected();

    /*!
     * @brief Called, when network error occurs.
     *
     * @param errorString Human-readable error string.
     */
    void connectionError(const QString &errorString);
#endif // CREPORTER_LIBBEARER_ENABLED

public:
    /*!
     * @brief Sets human-readable error string.
     *
     * @param message Error message.
     */
    void setErrorString(const QString &message);

    /*!
     * @brief Sets error type.
     *
     * @param type Error type.
     */
    void setErrorType(const CReporterUploadEngine::ErrorType &type);

private:
    /*!
      * @brief Sends CReporterUploadEngine::finished() -signal.
      *
      * @param error Error type.
      * @param sent Number of files uploaded successfully.
      * @param total Total number of files requested to send.
      *
      * @sa CReporterUploadEngine::finished()
      * @sa CReporterUploadEngine::ErrorType
      */
    void emitFinished(CReporterUploadEngine::ErrorType, int sent, int total);

public:
#ifdef CREPORTER_LIBBEARER_ENABLED
    //! @arg Network session manager reference.
    CReporterNwSessionMgr *networkSession;
#endif // CREPORTER_LIBBEARER_ENABLED
    //! @arg Upload queue reference<s.
    CReporterUploadQueue *queue;
    //! @arg Crash report currently handeled.
    CReporterUploadItem *currentItem;
    //! @arg Possible error message, if available.
    QString errorMessage;
    //! @arg Type of error.
    CReporterUploadEngine::ErrorType error;
    //! @arg Total number of files to upload.
    int totalFiles;
    //! @arg Number of files sent.
    int sentFiles;
    //! @arg State of the engine.
    State state;

    Q_DECLARE_PUBLIC(CReporterUploadEngine)
    CReporterUploadEngine *q_ptr;
#ifdef CREPORTER_UNIT_TEST
    friend class Ut_CReporterUploadEngine;
#endif
};

#endif // CREPORTERUPLOADENGINE_P_H
