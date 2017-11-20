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

#include "creportercoreregistry.h"
#include "creporterhttpclient.h"
#include "creporterhttpclient_p.h"
#include "creporterapplicationsettings.h"
#include "creporterutils.h"

using CReporter::LoggingCategory::cr;

const char *clientstate_string[] = {"None", "Init", "Connecting", "Sending", "Aborting"};
const int CONNECTION_TIMEOUT_MS = 2 * 60 * 1000;

CReporterHttpClientPrivate::CReporterHttpClientPrivate(CReporterHttpClient *parent)
    : QObject(parent),
      m_manager(0),
      m_reply(0),
      m_connectionTimeout(this),
      q_ptr(parent)
{
    m_clientState = CReporterHttpClient::None;
    m_connectionTimeout.setInterval(CONNECTION_TIMEOUT_MS);
    connect(&m_connectionTimeout, &QTimer::timeout,
            q_ptr, &CReporterHttpClient::cancel);
}

CReporterHttpClientPrivate::~CReporterHttpClientPrivate()
{
    CReporterApplicationSettings::freeSingleton();

    if (m_reply != 0) {
        m_reply->abort();
        m_reply = 0;
    }

    delete m_manager;
    m_manager = 0;
}

void CReporterHttpClientPrivate::init(bool deleteAfterSending)
{
    qCDebug(cr) << "Initiating HTTP session.";
    m_deleteFileFlag = deleteAfterSending;

    if (CReporterApplicationSettings::instance()->useProxy()) {
        qCDebug(cr) << "Network proxy defined.";

        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(CReporterApplicationSettings::instance()->proxyUrl());
        proxy.setPort(CReporterApplicationSettings::instance()->proxyPort());
        QNetworkProxy::setApplicationProxy(proxy);
    }

    if (m_manager == 0) {
        m_manager = new QNetworkAccessManager(q_ptr);
        Q_CHECK_PTR(m_manager);

        connect(m_manager, SIGNAL(authenticationRequired(QNetworkReply *, QAuthenticator *)),
                this, SLOT(handleAuthenticationRequired(QNetworkReply *, QAuthenticator *)));
    }
    stateChange(CReporterHttpClient::Init);
}

bool CReporterHttpClientPrivate::createRequest(const QString &file)
{
    Q_ASSERT(m_manager != NULL);
    qCDebug(cr) << "Create new request.";

    if (m_clientState != CReporterHttpClient::Init) {
        return false;
    }

    QNetworkRequest request;
    QByteArray dataToSend;

    // Set server URL and port.
    QUrl url(CReporterApplicationSettings::instance()->serverUrl());

    url.setPort(CReporterApplicationSettings::instance()->serverPort());

    if (CReporterApplicationSettings::instance()->useSsl()) {
        qCDebug(cr) << "SSL is enabled.";
        QSslConfiguration ssl(QSslConfiguration::defaultConfiguration());
        ssl.setPeerVerifyMode(QSslSocket::VerifyNone);
        request.setSslConfiguration(ssl);
    }

    // Set file to be the current.
    m_currentFile.setFile(file);
    qCDebug(cr) << "File to upload:" << m_currentFile.absoluteFilePath();
    qCDebug(cr) << "File size:" << m_currentFile.size() / 1024 << "kB's";

    // For PUT, we need to append file name to the path.
    QString serverPath = CReporterApplicationSettings::instance()->serverPath() +
                         "/" + m_currentFile.fileName();

    url.setPath(serverPath);

    url.setQuery("uuid=" + CReporterUtils::deviceUid() +
                 "&model=" + CReporterUtils::deviceModel());

    request.setUrl(url);
    qCDebug(cr) << "Upload URL:" << url.toString();

    if (!createPutRequest(request, dataToSend)) {
        qCWarning(cr) << "Failed to create network request.";
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
    m_connectionTimeout.start();

    stateChange(CReporterHttpClient::Connecting);
    return true;
}

void CReporterHttpClientPrivate::cancel()
{
    stateChange(CReporterHttpClient::Aborting);

    if (m_reply != 0) {
        qCDebug(cr) << "Canceling HTTP transaction.";
        // Abort ongoing transactions.
        m_reply->abort();
        m_reply = 0;
    }
    // Clean up.
    handleFinished();
}

void CReporterHttpClientPrivate::handleAuthenticationRequired(QNetworkReply *reply,
        QAuthenticator *authenticator)
{
    Q_UNUSED(reply)

    qCDebug(cr) << "Fill in the credentials.";

    // Fill in the credentials.
    authenticator->setUser(CReporterApplicationSettings::instance()->username());
    authenticator->setPassword(CReporterApplicationSettings::instance()->password());
}

void CReporterHttpClientPrivate::handleSslErrors(const QList<QSslError> &errors)
{
    QString errorString;
    foreach (const QSslError &error, errors) {
        if (!errorString.isEmpty()) {
            errorString += ", ";
        }
        errorString += error.errorString();
    }
    qCWarning(cr) << "One or more SSL errors occured:" << errorString;

    // Ignore and continue connection.
    m_reply->ignoreSslErrors();
}

void CReporterHttpClientPrivate::handleError(QNetworkReply::NetworkError error)
{
    if (m_reply && m_reply->error() != QNetworkReply::NoError) {
        // Finished is emitted by QNetworkReply after this, inidicating that
        // the connection is over.
        QString errorString = m_reply->errorString();
        m_reply = 0;
        qCWarning(cr) << "Upload failed. Error code:" << error << "," << errorString;
        emit uploadError(m_currentFile.fileName(), errorString);
    }
}

void CReporterHttpClientPrivate::parseReply()
{
    if (!m_reply) {
        qCWarning(cr) << "Server reply is NULL";
        return;
    }

    if (!m_reply->open(QIODevice::ReadOnly)) {
        qCWarning(cr) << "Couldn't open server reply for reading.";
        return;
    }

    QJsonDocument reply = QJsonDocument::fromJson(m_reply->readAll());
    if (reply.isNull() || !reply.isObject()) {
        qCWarning(cr) << "Error parsing JSON server reply.";
        return;
    }

    QJsonObject json = reply.object();
    int submissionId = static_cast<int>(json.value("submission_id").toDouble(0));
    if (submissionId == 0) {
        qCWarning(cr) << "Failed to parse submission id from JSON.";
        return;
    }

    QUrl submissionUrl(CReporterApplicationSettings::instance()->serverUrl());
    submissionUrl.setPort(CReporterApplicationSettings::instance()->serverPort());
    submissionUrl.setPath("/");
    submissionUrl.setFragment(QString("submissions/%1").arg(submissionId));

    QString corePath(CReporterCoreRegistry::instance()->getCoreLocationPaths().first());
    QFile uploadlog(corePath + "/uploadlog");
    if (!uploadlog.open(QIODevice::WriteOnly | QIODevice::Append)) {
        qCWarning(cr) << "Couldn't open uploadlog for writing.";
        return;
    }

    QTextStream stream(&uploadlog);
    stream << m_currentFile.fileName() << ' ' << submissionUrl.toString() << '\n';

    uploadlog.close();
}

void CReporterHttpClientPrivate::handleFinished()
{
    qCDebug(cr) << "Uploading file:" << m_currentFile.fileName() << "finished.";

    m_connectionTimeout.stop();

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

void CReporterHttpClientPrivate::handleUploadProgress(qint64 bytesSent, qint64 bytesTotal)
{
    qCDebug(cr) << "Sent:" << bytesSent << "Total:" << bytesTotal;

    // Upload has started; stop the connection timeout.
    m_connectionTimeout.stop();

    if (!m_reply) {
        // Do not update, if aborted.
        return;
    }

    if (m_clientState != CReporterHttpClient::Sending) {
        stateChange(CReporterHttpClient::Sending);
    }

    if (bytesTotal != 0) {
        int done = (int)((bytesSent * 100) / bytesTotal);
        qCDebug(cr) << "Done:" << done << "%";
        emit q_ptr->updateProgress(done);
    }
}

void CReporterHttpClientPrivate::stateChange(CReporterHttpClient::State nextState)
{
    qCDebug(cr) << "Current state:" << q_ptr->stateToString(m_clientState);
    qCDebug(cr) << "New state:" << q_ptr->stateToString(nextState);
    m_clientState = nextState;
    emit stateChanged(m_clientState);
}

bool CReporterHttpClientPrivate::createPutRequest(QNetworkRequest &request, QByteArray &dataToSend)
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

CReporterHttpClient::CReporterHttpClient(QObject *parent)
    : QObject(parent),
      d_ptr(new CReporterHttpClientPrivate(this))
{
    Q_D(CReporterHttpClient);

    connect(d, SIGNAL(stateChanged(CReporterHttpClient::State)),
            this, SIGNAL(clientStateChanged(CReporterHttpClient::State)));
    connect(d, SIGNAL(uploadError(QString, QString)), this, SIGNAL(uploadError(QString, QString)));
    connect(d, SIGNAL(finished()), this, SIGNAL(finished()));
}

CReporterHttpClient::~CReporterHttpClient()
{
    qCDebug(cr) << "Client destroyed.";
}

void CReporterHttpClient::initSession(bool deleteAfterSending)
{
    Q_D(CReporterHttpClient);
    d->init(deleteAfterSending);
}

CReporterHttpClient::State CReporterHttpClient::state() const
{
    return d_ptr->m_clientState;
}

QString CReporterHttpClient::stateToString(CReporterHttpClient::State state) const
{
    return  QString(clientstate_string[state]);
}

bool CReporterHttpClient::upload(const QString &file)
{
    Q_D(CReporterHttpClient);
    qCDebug(cr) << "Upload requested.";

    return d->createRequest(file);
}

void CReporterHttpClient::cancel()
{
    Q_D(CReporterHttpClient);
    qCDebug(cr) << "Cancel requested.";

    d->cancel();
}
