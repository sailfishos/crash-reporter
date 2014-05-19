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

#ifndef CREPORTERHTTPCLIENT_P_H
#define CREPORTERHTTPCLIENT_P_H

// System includes.

#include  <QList>
#include <QNetworkReply>
#include <QFileInfo>
#include <QHostInfo>

// User includes.

#include "creporterhttpclient.h"

// Forward declarations.

class CReporterCoreRegistry;
class QNetworkAccessManager;
class QAuthenticator;
class QAuthenticator;
class QSslError;

/*!
 * @class CReporterHttpClientPrivate
 * @brief Private CReporterHttpClient class.
 *
 * @sa CReporterHttpClient
 */
class CReporterHttpClientPrivate : public QObject
{
	Q_OBJECT

	public:
        /*!
         * Class constructor.
         *
         */
		CReporterHttpClientPrivate();

        /*!
         * Class destructor.
         *
         */
		~CReporterHttpClientPrivate();

        /*!
         * @brief Initiates client.
         * 
         * @param deleteAfterSending If True file is delete after sent successfully.
         */ 
        void init(bool deleteAfterSending);

        /*!
         * @brief Creates a new request sent over the network.
         * 
         * @param file File to send.
         * @sa <a href="http://doc.trolltech.com/4.6/qnetworkrequest.html">QNetworkRequest</a>
         */
        bool createRequest(const QString &file);

        /*!
         * @brief Cancels ongoing request.
         *
         * @sa <a href="http://doc.trolltech.com/4.6/qnetworkreply.html#abort">QNetworkRequest::abort()</a>
         */
        void cancel();

	Q_SIGNALS:

        /*!
         * @brief Sent when <a href="http://doc.trolltech.com/4.6/qnetworkreply.html">QNetworkReply</a>
         *  finishes.
         */
        void finished();

        /*!
         * @brief Sent when error was detected while uploading @a file.
         *
         * @param file File, which was uploaded when error occured.
         * @param errorString HTTP error string.
         *
         * @sa <a href="http://doc.trolltech.com/4.6/qnetworkreply.html#error-2">QNetworkReply::error()</a>
         * @sa <a href="http://doc.trolltech.com/4.6/qnetworkreply.html#NetworkError-enum">QNetworkReply::NetworkError</a>
         */
        void uploadError(const QString &file, const QString &errorString);

        /*!
         * @brief Sent when client state changes.
         *
         * @param state New state.
         */
        void stateChanged(CReporterHttpClient::State state);

	private Q_SLOTS:

        /*!
         * @brief Called, when server requests authentication.
         *
         * @param reply QNetworkReply object this belongs to.
         * @param authenticator Object to pass authentication information.
         */
        void handleAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);

        /*!
         * @brief Called, when SSL/TLS session encountered errors during the set up.
         *
         * @param errors List of errors. 
         */
        void handleSslErrors(const QList<QSslError> &errors);

        /*!
         * @brief Called, when QNetworkReply detects an error in processing.
         *
         * @param error Network error. 
         */
        void handleError(QNetworkReply::NetworkError error);

        /*!
         * @brief Called, when finished signal is received from QNetworkReply object.
         *
         */
		void handleFinished();

        /*!
         * @brief Called, to update upload progess.
         * 
         * @param bytesSent Sent bytes.
         * @param bytesTotal Total bytes to send.
         */
        void handleUploadProgress(qint64 bytesSent, qint64 bytesTotal);

	private:

        /*!
         * @brief Changes client state to @a nextState.
         * 
         * @param nextState New state.
         */
        void stateChange(CReporterHttpClient::State nextState);

        /*!
         * @brief Creates HTTP PUT request.
         * 
         * @param request New QNetworkRequest.
         * @param dataToSend Data array to send.
         */
        bool createPutRequest(QNetworkRequest &request, QByteArray &dataToSend);

        /*!
         * @brief Reads server reply and save submission URL into a log file.
         */
        void parseReply();

	public:
        //! @arg QNetworkAccessManager object.
		QNetworkAccessManager *m_manager;
        //! @arg QNetworkReply object.
		QNetworkReply *m_reply;
        //! @arg Set to True, if file should be removed after successfull sending.
		bool m_deleteFileFlag;
        //! @arg Current file to process.
        QFileInfo m_currentFile;
        //! @arg Client state.
		CReporterHttpClient::State m_clientState;

        Q_DECLARE_PUBLIC(CReporterHttpClient)

	protected:
        //! @arg Public class reference.
		CReporterHttpClient *q_ptr;
};

#endif // CREPORTERHTTPCLIENT_P_H

// End of file.
