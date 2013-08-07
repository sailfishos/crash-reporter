/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Riku Halonen <riku.halonen@nokia.com>
 *
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Jakub Adam <jakub.adam@jollamobile.com>
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

// System includes.

#include <QNetworkAccessManager>
#include <QAuthenticator>
#include <QList>
#include <QNetworkReply>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QSslConfiguration>
#include <QNetworkProxy>
#include <QTime>

#include <ssudeviceinfo.h>

// User includes.

#include "creporterhttpclient.h"
#include "creporterhttpclient_p.h"
#include "creporterapplicationsettings.h"
#include "creporterutils.h"

const char *clientstate_string[] = {"None", "Init", "Connecting", "Sending", "Aborting"};

// ******** Class CReporterHttpClientPrivate ********

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::CReporterHttpClientPrivate
// ----------------------------------------------------------------------------
CReporterHttpClientPrivate::CReporterHttpClientPrivate() :
		m_manager( 0 ),
        m_reply( 0 )
{
		m_clientState = CReporterHttpClient::None;
}

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::~CReporterHttpClientPrivate
// ----------------------------------------------------------------------------
CReporterHttpClientPrivate::~CReporterHttpClientPrivate()
{
    CReporterApplicationSettings::freeSingleton();

    if (m_reply != 0) {
        m_reply->abort();
        m_reply = 0;
    }

    if (m_manager != 0) {
		delete m_manager;
		m_manager = 0;
	}
}

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::init
// ----------------------------------------------------------------------------
void CReporterHttpClientPrivate::init(bool deleteAfterSending)
{
	qDebug() << __PRETTY_FUNCTION__ << "Initiating HTTP session.";
	m_deleteFileFlag = deleteAfterSending;
	m_userAborted = false;
    m_httpError = false;

    if (CReporterApplicationSettings::instance()->value(Server::ValueUseProxy,
                                                        QVariant(DefaultApplicationSettings::ValueUseProxyDefault)).toBool()) {
        qDebug() << __PRETTY_FUNCTION__ << "Network proxy defined.";

        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(CReporterApplicationSettings::instance()->
                          value(Proxy::ValueProxyAddress, QVariant(DefaultApplicationSettings::ValueProxyAddressDefault)).toString());
        proxy.setPort(CReporterApplicationSettings::instance()->
                      value(Proxy::ValueProxyPort, QVariant(DefaultApplicationSettings::ValueProxyPortDefault)).toInt());
        QNetworkProxy::setApplicationProxy(proxy);
    }

    if (m_manager == 0) {
        m_manager = new QNetworkAccessManager(q_ptr);
        Q_CHECK_PTR(m_manager);

        connect(m_manager, SIGNAL(authenticationRequired(QNetworkReply*, QAuthenticator*)),
                this, SLOT(handleAuthenticationRequired(QNetworkReply*, QAuthenticator*)));
	}
   stateChange(CReporterHttpClient::Init);
}

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::createRequest
// ----------------------------------------------------------------------------
bool CReporterHttpClientPrivate::createRequest(const QString &file)
{
    Q_ASSERT(m_manager != NULL);
    qDebug() << __PRETTY_FUNCTION__ << "Create new request.";

    if (m_clientState != CReporterHttpClient::Init) {
        return false;
    }

    QNetworkRequest request;
    QByteArray dataToSend;

    // Set server URL and port.
    QUrl url(CReporterApplicationSettings::instance()->
             value(Server::ValueServerAddress, QVariant(DefaultApplicationSettings::ValueServerAddressDefault)).toString());

    if (CReporterApplicationSettings::instance()->value(Server::ValueUseSsl,
                                                        QVariant(DefaultApplicationSettings::ValueUseSslDefault)).toBool()) {
        qDebug() << __PRETTY_FUNCTION__ << "SSL is enabled.";
        QSslConfiguration ssl(QSslConfiguration::defaultConfiguration());
        ssl.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(ssl);
    }

    // Set file to be the current.
    m_currentFile.setFile(file);
    qDebug() << __PRETTY_FUNCTION__ << "File to upload:" << m_currentFile.absoluteFilePath();
    qDebug() << __PRETTY_FUNCTION__ << "File size:" << m_currentFile.size() / 1024 << "kB's";

    // For PUT, we need to append file name to the path.
    QString serverPath = CReporterApplicationSettings::instance()->
                         value(Server::ValueServerPath,
                               QVariant(DefaultApplicationSettings::ValueServerPathDefault)).toString() +
                         "/" + m_currentFile.fileName();

    url.setPath(serverPath);

    SsuDeviceInfo deviceInfo;
    url.setQuery("uuid=" + deviceInfo.deviceUid() + "&model=" + deviceInfo.deviceModel());

    request.setUrl(url);
    qDebug() << __PRETTY_FUNCTION__ << "Upload URL:" << url.toString();

    if (!createPutRequest(request, dataToSend)) {
        qDebug() << __PRETTY_FUNCTION__ << "Failed to create network request.";
        return false;
    }

    // Send request and connect signal/ slots.
    m_reply = m_manager->put(request, dataToSend);

    if (m_reply == 0) {
        return false;
    }

    // Connect QNetworkReply signals.
    connect(m_reply, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(handleSslErrors(QList<QSslError>)));
    connect(m_reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(handleError(QNetworkReply::NetworkError)));
    connect(m_reply, SIGNAL(finished()), this, SLOT(handleFinished()));

    stateChange(CReporterHttpClient::Connecting);
    return true;
}

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::cancel
// ----------------------------------------------------------------------------
void CReporterHttpClientPrivate::cancel()
{
    qDebug() <<  __PRETTY_FUNCTION__ << "User aborted transaction.";
	m_userAborted = true;
	stateChange( CReporterHttpClient::Aborting );

    if (m_reply != 0) {
        qDebug() << __PRETTY_FUNCTION__ << "Canceling transaction.";
		// Abort ongoing transactions.
		m_reply->abort();
	}
	// Clean up.
	handleFinished();
}

// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::handleAuthenticationRequired
// ----------------------------------------------------------------------------
void CReporterHttpClientPrivate::handleAuthenticationRequired(QNetworkReply *reply,
                                                        QAuthenticator *authenticator)
{
	qDebug() << __PRETTY_FUNCTION__ << "Fill in the credentials.";

    connect(reply, SIGNAL(uploadProgress(qint64,qint64)),
            this, SLOT(handleUploadProgress(qint64,qint64)));

	// Fill in the credentials.
    authenticator->setUser(CReporterApplicationSettings::instance()->
                           value(Server::ValueUsername,
                                 QVariant(DefaultApplicationSettings::ValueUsernameDefault)).toString());
    authenticator->setPassword(CReporterApplicationSettings::instance()->
                               value(Server::ValuePassword,
                                     QVariant(DefaultApplicationSettings::ValuePasswordDefault)).toString());
}

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::handleSslErrors
// ----------------------------------------------------------------------------
void CReporterHttpClientPrivate::handleSslErrors(const QList<QSslError> &errors )
{
	QString errorString;
	foreach (const QSslError &error, errors) {
		if ( !errorString.isEmpty() ) {
			errorString += ", ";
		 }
		 errorString += error.errorString();
	 }
	qDebug() << "One or more SSL errors occured:" << errorString;

	// Ignore and continue connection.
	m_reply->ignoreSslErrors();
}

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::handleError
// ----------------------------------------------------------------------------
void CReporterHttpClientPrivate::handleError(QNetworkReply::NetworkError error)
{
    if (m_reply && m_reply->error() != QNetworkReply::NoError) {
		// Finished is emitted by QNetworkReply after this, inidicating that
		// the connection is over.
		qCritical() << __PRETTY_FUNCTION__ << "Upload failed.";
        m_httpError = true;
		QString errorString =m_reply->errorString();
        qDebug() << __PRETTY_FUNCTION__ << "Error code:" << error << "," << errorString;
        emit uploadError(m_currentFile.fileName(), errorString);
	}
}

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::handleFinished
// ----------------------------------------------------------------------------
void CReporterHttpClientPrivate::handleFinished()
{
    qDebug() << __PRETTY_FUNCTION__ << "Uploading file:" << m_currentFile.fileName() << "finished.";

    if (!m_httpError && m_deleteFileFlag && !m_userAborted) {
		// Remove file, if upload was successfull and delete was requested.
        CReporterUtils::removeFile(m_currentFile.absoluteFilePath());
	}

    // QNetworkreply objects are owned by QNetworkAccessManager and deleted along with it.
	m_reply = 0;

    stateChange(CReporterHttpClient::Init);
    emit finished();
}

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::handleUploadProgress
// ----------------------------------------------------------------------------
void CReporterHttpClientPrivate::handleUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    qDebug() << __PRETTY_FUNCTION__ << "Sent:" << bytesSent << "Total:" << bytesTotal;

    if (m_userAborted || m_httpError) {
        // Do not update, if aborted.
        return;
    }

    if (m_clientState != CReporterHttpClient::Sending) {
        stateChange(CReporterHttpClient::Sending);
    }

    int done = (int)((bytesSent * 100) / bytesTotal);
    qDebug() << __PRETTY_FUNCTION__ << "Done:" << done << "%";
    emit q_ptr->updateProgress(done);
}

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::stateChange
// ----------------------------------------------------------------------------
void CReporterHttpClientPrivate::stateChange(CReporterHttpClient::State nextState)
{
    qDebug() << __PRETTY_FUNCTION__ << "Current state:" << q_ptr->stateToString( m_clientState );
    qDebug() << __PRETTY_FUNCTION__ << "New state:" << q_ptr->stateToString( nextState );
    m_clientState = nextState;
    emit stateChanged( m_clientState );
}

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::createPutRequest
// ----------------------------------------------------------------------------
bool CReporterHttpClientPrivate::createPutRequest(QNetworkRequest &request,
                                                   QByteArray &dataToSend)
{
    QFile file(m_currentFile.absoluteFilePath());
    // Abort, if file doesn't exist or IO error.
    if (!file.exists() || !file.open(QIODevice::ReadOnly)) {
        return false;
    }

    // Append file.
    dataToSend += file.readAll();
    file.close();

    // Construct HTTP Headers.
    request.setRawHeader("User-Agent", "crash-reporter");
    request.setRawHeader("Accept", "*/*");
    request.setHeader(QNetworkRequest::ContentLengthHeader, dataToSend.size());

    return true;
}

// ******** Class CReporterHttpClient ********

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterHttpClient::CReporterHttpClient
// ----------------------------------------------------------------------------
CReporterHttpClient::CReporterHttpClient(QObject *parent) :
        QObject(parent) ,
        d_ptr(new CReporterHttpClientPrivate())
{
    Q_D(CReporterHttpClient);
	d->q_ptr = this;

	connect( d, SIGNAL(stateChanged(CReporterHttpClient::State)),
			 this, SIGNAL(clientStateChanged(CReporterHttpClient::State)) );
	connect( d, SIGNAL(uploadError(QString,QString)), this, SIGNAL(uploadError(QString,QString)) );
    connect( d, SIGNAL(finished()), this, SIGNAL(finished()) );
}

// ----------------------------------------------------------------------------
// CReporterHttpClient::~CReporterHttpClient
// ----------------------------------------------------------------------------
CReporterHttpClient::~CReporterHttpClient()
{
    qDebug() << __PRETTY_FUNCTION__ << "Client destroyed.";

	delete d_ptr;
	d_ptr = NULL;
}

// ----------------------------------------------------------------------------
// CReporterHttpClient::initSession
// ----------------------------------------------------------------------------
void CReporterHttpClient::initSession(bool deleteAfterSending)
{
    Q_D(CReporterHttpClient);
    d->init(deleteAfterSending);
}

// ----------------------------------------------------------------------------
// CReporterHttpClient::state
// ----------------------------------------------------------------------------
CReporterHttpClient::State CReporterHttpClient::state() const
{
	return d_ptr->m_clientState;
}

// ----------------------------------------------------------------------------
// CReporterHttpClient::stateToString
// ----------------------------------------------------------------------------
QString CReporterHttpClient::stateToString(CReporterHttpClient::State state) const
{
    return  QString(clientstate_string[state]);
}

// ----------------------------------------------------------------------------
// CReporterHttpClient::upload
// ----------------------------------------------------------------------------
bool CReporterHttpClient::upload(const QString &file)
{
	Q_D( CReporterHttpClient );
	qDebug() << __PRETTY_FUNCTION__ << "Upload requested.";

    return d->createRequest(file);
}

// ----------------------------------------------------------------------------
// CReporterHttpClient::cancel
// ----------------------------------------------------------------------------
void CReporterHttpClient::cancel()
{
    Q_D(CReporterHttpClient);
	qDebug() << __PRETTY_FUNCTION__ << "Cancel requested.";

    d->cancel();
}

// End of file
