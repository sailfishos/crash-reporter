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

#ifndef CREPORTERHTTPCLIENT_H
#define CREPORTERHTTPCLIENT_H

#include <QObject>

#include "creporterexport.h"

class CReporterHttpClientPrivate;
class CReporterHttpCntx;

/*!
  * @class CReporterHttpcClient
  * @brief Public class for uploading files to the remote server.
  *
  */
class CREPORTER_EXPORT CReporterHttpClient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(State state READ state)
    Q_ENUMS(State)

public:

    /*!
    * @enum Represent client's internal state.
    *
    * Updated by the clientStateChanged() signal.
    *
    * @sa clientStateChanged()
    */
    enum State {
        //! Initial state.
        None = 0,
        //! Session has been initiated.
        Init,
        //! Client is creating network session.
        Connecting,
        //! Client is connected.
        Sending,
        //! Client is aborting all pending requests.
        Aborting,
    };

    CReporterHttpClient(QObject *parent = 0);

    ~CReporterHttpClient();

    /*!
     * @brief Initiates client session. Call this after instantiating object
     * from this class.
     *
     * @param deleteAfterSending If set to true, file is deleted from the system
     *  after it has been sent.
     */
    void initSession(bool deleteAfterSending = true);

    /*!
     * @brief Returns Http client state.
     *
     * @return State.
     */
    State state() const;

    /*! @brief Returns state in string format.
     *
     * @return State as string.
     */
    QString stateToString(CReporterHttpClient::State state) const;

Q_SIGNALS:
    /*!
     * @brief Sent, when all pending network replies have finished.
     *
     * If client wants to use the same instance of this class for the new requests,
     * it needs to call initSession first.
     */
    void finished();

    /*!
     * @brief Sent, when error is detected during upload.
     *
     * @param file Path to the file, which upload request returned error.
     * @param errorString Textual presentation of the error happend.
     */
    void uploadError(const QString &file, const QString &errorString);

    /*!
     * @brief Sent when upload progresses.
     *
     * @param done Sent data in percentage value.
     */
    void updateProgress(int done);

    /*!
     * @brief Emitted, when client's internal state changes.
     *
     * @param state New state.
     */
    void clientStateChanged(CReporterHttpClient::State state);

public Q_SLOTS:
    /*!
     * @brief Uploads file(s) to the remote server.
     *
     * This function returns immediately,
     * after upload has been initiated. Upload progess and status can be monitored by
     * connecting to uploadProgress, uploadDone and uploadError signals.
     *
     */
    bool upload(const QString &file);

    /*!
     * @brief Cancels ongoing request.
     *
     */
    void cancel();

private:
    Q_DECLARE_PRIVATE( CReporterHttpClient )

    CReporterHttpClientPrivate *d_ptr;
#ifdef CREPORTER_UNIT_TEST
    friend class Ut_CReporterHttpClient;
#endif
};

#endif // CREPORTERHTTPCLIENT_H

