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

#include <QAuthenticator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QSslConfiguration>
#include <QNetworkProxy>
#include <QTime>

// User includes.

#include "creportercoreregistry.h"
#include "creporterhttpclient.h"
#include "creporterhttpclient_p.h"
#include "creporterapplicationsettings.h"
#include "creporterutils.h"

const char *clientstate_string[] = {"None", "Init", "Connecting", "Sending", "Aborting"};

CReporterHttpClientPrivate::CReporterHttpClientPrivate(CReporterHttpClient *parent):
  QObject(parent),
  m_manager(0),
  m_reply(0),
  q_ptr(parent)
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

    if (CReporterApplicationSettings::instance()->useProxy()) {
        qDebug() << __PRETTY_FUNCTION__ << "Network proxy defined.";

        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(CReporterApplicationSettings::instance()->proxyUrl());
        proxy.setPort(CReporterApplicationSettings::instance()->proxyPort());
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
    QUrl url(CReporterApplicationSettings::instance()->serverUrl());

    url.setPort(CReporterApplicationSettings::instance()->serverPort());

    if (CReporterApplicationSettings::instance()->useSsl()) {
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
    QString serverPath = CReporterApplicationSettings::instance()->serverPath() +
                         "/" + m_currentFile.fileName();

    url.setPath(serverPath);

    url.setQuery("uuid=" + CReporterUtils::deviceUid() +
            "&model=" + CReporterUtils::deviceModel());

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
    connect(m_reply, &QNetworkReply::uploadProgress,
            this, &CReporterHttpClientPrivate::handleUploadProgress);

    stateChange(CReporterHttpClient::Connecting);
    return true;
}

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::cancel
// ----------------------------------------------------------------------------
void CReporterHttpClientPrivate::cancel()
{
	stateChange( CReporterHttpClient::Aborting );

    if (m_reply != 0) {
        qDebug() << __PRETTY_FUNCTION__ << "Canceling HTTP transaction.";
		// Abort ongoing transactions.
		m_reply->abort();
        m_reply = 0;
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
    Q_UNUSED(reply)

	qDebug() << __PRETTY_FUNCTION__ << "Fill in the credentials.";

	// Fill in the credentials.
    authenticator->setUser(CReporterApplicationSettings::instance()->username());
    authenticator->setPassword(CReporterApplicationSettings::instance()->password());
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
        QString errorString =m_reply->errorString();

        m_reply = 0;

        qDebug() << __PRETTY_FUNCTION__ << "Error code:" << error << "," << errorString;
        emit uploadError(m_currentFile.fileName(), errorString);
	}
}

void CReporterHttpClientPrivate::parseReply()
{
    if (!m_reply) {
        qDebug() << "Server reply is NULL";
        return;
    }

    if (!m_reply->open(QIODevice::ReadOnly)) {
        qDebug() << "Couldn't open server reply for reading.";
        return;
    }

    QJsonDocument reply = QJsonDocument::fromJson(m_reply->readAll());
    if (reply.isNull() || !reply.isObject()) {
        qDebug() << "Error parsing JSON server reply.";
        return;
    }

    QJsonObject json = reply.object();
    int submissionId = static_cast<int>(json.value("submission_id").toDouble(0));
    if (submissionId == 0) {
        qDebug() << "Failed to parse submission id from JSON.";
        return;
    }

    QUrl submissionUrl(CReporterApplicationSettings::instance()->serverUrl());
    submissionUrl.setPort(CReporterApplicationSettings::instance()->serverPort());
    submissionUrl.setPath("/");
    submissionUrl.setFragment(QString("submissions/%1").arg(submissionId));

    QString corePath(CReporterCoreRegistry::instance()->getCoreLocationPaths().first());
    QFile uploadlog(corePath + "/uploadlog");
    if (!uploadlog.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qDebug() << "Couldn't open uploadlog for writing.";
        return;
    }

    QTextStream stream(&uploadlog);
    stream << m_currentFile.fileName() << ' ' << submissionUrl.toString() << '\n';

    uploadlog.close();
}

// ----------------------------------------------------------------------------
// CReporterHttpClientPrivate::handleFinished
// ----------------------------------------------------------------------------
void CReporterHttpClientPrivate::handleFinished()
{
    qDebug() << __PRETTY_FUNCTION__ << "Uploading file:" << m_currentFile.fileName() << "finished.";

    if (m_reply) {
        // Upload was successful.
        parseReply();

        if (m_deleteFileFlag) {
            // Remove file if delete was requested.
            CReporterUtils::removeFile(m_currentFile.absoluteFilePath());
        }
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

    if (!m_reply) {
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

CReporterHttpClient::CReporterHttpClient(QObject *parent):
  QObject(parent) ,
  d_ptr(new CReporterHttpClientPrivate(this))
{
    Q_D(CReporterHttpClient);

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
