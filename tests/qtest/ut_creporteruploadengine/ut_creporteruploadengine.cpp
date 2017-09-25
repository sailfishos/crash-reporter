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

#include <QSignalSpy>
#include <QDebug>

#include "creporteruploadengine.h"
#include "creporteruploadengine_p.h"
#include "creporteruploadqueue.h"
#include "creporteruploaditem.h"
#include "ut_creporteruploadengine.h"

static CReporterHttpClient *httpInstance = 0;

// CReporterHttpClient mock object.
CReporterHttpClient::CReporterHttpClient(QObject *parent)
{
    Q_UNUSED(parent);
    httpInstance = this;
}

CReporterHttpClient::~CReporterHttpClient()
{
}

void CReporterHttpClient::initSession(bool deleteAfterSending)
{
    Q_UNUSED(deleteAfterSending);
}

bool CReporterHttpClient::upload(const QString &file)
{
    Q_UNUSED(file);
    return true;
}

void CReporterHttpClient::cancel()
{
}

void CReporterHttpClient::emitFinished()
{
    emit finished();
}

void CReporterHttpClient::emitUploadError(const QString &file, const QString &errorString)
{
    emit uploadError(file, errorString);
}

void CReporterHttpClient::emitUpdateProgress(int done)
{
    emit updateProgress(done);
}

static CReporterNwSessionMgr *sesManager = 0;
static bool openedCalled;
static bool openCalled;
static bool closeCalled;
static bool networkSessionOpened;
static bool stopCalled;

// CReporterNwSessionMgr mock object.
CReporterNwSessionMgr::CReporterNwSessionMgr(QObject *parent)
{
    Q_UNUSED(parent);
    sesManager = this;
    openedCalled = false;
    openCalled = false;
    closeCalled = false;
    networkSessionOpened = false;
    stopCalled = false;
}

CReporterNwSessionMgr::~CReporterNwSessionMgr()
{
}

bool CReporterNwSessionMgr::opened() const
{
    return networkSessionOpened;
}

void CReporterNwSessionMgr::emitSessionOpened()
{
    networkSessionOpened = true;
    emit sessionOpened();
}

void CReporterNwSessionMgr::emitSessionDisconnected()
{
    networkSessionOpened = false;
    emit sessionDisconnected();
}

void CReporterNwSessionMgr::emitNetworkError(const QString &errorString)
{
    emit networkError(errorString);
}

bool CReporterNwSessionMgr::open()
{
    openCalled = true;
    return networkSessionOpened;
}

void CReporterNwSessionMgr::close()
{
    closeCalled = true;
}

void CReporterNwSessionMgr::stop()
{
    stopCalled = true;
}

// Unit test object.
void Ut_CReporterUploadEngine::initTestCase()
{
}

void Ut_CReporterUploadEngine::init()
{
    m_Queue = new CReporterUploadQueue();
    m_Subject = new CReporterUploadEngine(m_Queue);
}

void Ut_CReporterUploadEngine::testUploadItems()
{
    // Test uploading files.
    QSignalSpy finishedSpy(m_Subject, SIGNAL(finished(int, int, int)));
    QSignalSpy nextItemSpy(m_Queue, SIGNAL(nextItem(CReporterUploadItem *)));

    // Queue 2 files.
    m_Queue->enqueue(
        new CReporterUploadItem("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo"));

    m_Queue->enqueue(
        new CReporterUploadItem("/media/mmc2/core-dumps/application-1234-11-4321.rcore.lzo"));

    // Network session is requested to open.
    QVERIFY(openCalled == true);
    // ... session opens.
    sesManager->emitSessionOpened();

    // Uploading first item finishes.
    QVERIFY(nextItemSpy.count() == 1);
    httpInstance->emitFinished();

    // Uploading second item finishes.
    QVERIFY(nextItemSpy.count() == 2);
    httpInstance->emitFinished();

    // Network session is closed.
    QVERIFY(closeCalled == true);

    sesManager->emitSessionDisconnected();
    // Finished signal is received from engine.
    QVERIFY(finishedSpy.count() == 1);
    QList<QVariant> arguments = finishedSpy.takeFirst();
    QVERIFY(arguments.at(0).toInt() == CReporterUploadEngine::NoError);
    QVERIFY(arguments.at(1).toInt() == 2);
    QVERIFY(arguments.at(2).toInt() == 2);

    QVERIFY(m_Subject->lastError().isNull() == true);
}

void Ut_CReporterUploadEngine::testOpeningNetworkSessionFails()
{
    // Test situation when network session doesn't open.
    QSignalSpy finishedSpy(m_Subject, SIGNAL(finished(int, int, int)));

    m_Queue->enqueue(
        new CReporterUploadItem("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo"));

    // Network session is requested to open.
    QVERIFY(openCalled == true);

    // Network error.
    sesManager->emitNetworkError("Aborted.");

    // Disconnected.
    sesManager->emitSessionDisconnected();

    QVERIFY(closeCalled == false);

    QVERIFY(finishedSpy.count() == 1);
    QList<QVariant> arguments = finishedSpy.takeFirst();
    QVERIFY(arguments.at(0).toInt() == CReporterUploadEngine::ConnectionNotAvailable);
    QVERIFY(arguments.at(1).toInt() == 0);
    QVERIFY(arguments.at(2).toInt() == 1);

}

void Ut_CReporterUploadEngine::testNetworkSessionDisconnectsDuringUpload()
{
    // Test situation when network session disconnects during upload.
    QSignalSpy finishedSpy(m_Subject, SIGNAL(finished(int, int, int)));

    m_Queue->enqueue(
        new CReporterUploadItem("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo"));
    m_Queue->enqueue(
        new CReporterUploadItem("/media/mmc2/core-dumps/application-1234-11-4321.rcore.lzo"));

    // Network session is requested to open.
    QVERIFY(openCalled == true);
    // ... session opens.
    sesManager->emitSessionOpened();

    // Network error.
    sesManager->emitNetworkError("Disconnected.");
    // Disconnected.
    sesManager->emitSessionDisconnected();

    // Error is received from HTTP client, since request was aborted.
    httpInstance->emitUploadError("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo",
                                  "User aborted.");

    QVERIFY(closeCalled == false);

    QVERIFY(finishedSpy.count() == 1);
    QList<QVariant> arguments = finishedSpy.takeFirst();
    QVERIFY(arguments.at(0).toInt() == CReporterUploadEngine::ConnectionClosed);
    QVERIFY(arguments.at(1).toInt() == 0);
    QVERIFY(arguments.at(2).toInt() == 2);

    QVERIFY(m_Subject->lastError() == "Disconnected.");
}

void Ut_CReporterUploadEngine::testUploadCancelledByTheUser()
{
    // Test upload cancelled by the user.
    QSignalSpy finishedSpy(m_Subject, SIGNAL(finished(int, int, int)));

    m_Queue->enqueue(
        new CReporterUploadItem("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo"));
    m_Queue->enqueue(
        new CReporterUploadItem("/media/mmc2/core-dumps/application-1234-11-4321.rcore.lzo"));

    // Network session is requested to open.
    QVERIFY(openCalled == true);
    // ... session opens.
    sesManager->emitSessionOpened();
    // User aborts.
    m_Subject->cancelAll();

    // Error is received from HTTP client, since request was aborted.
    httpInstance->emitUploadError("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo",
                                  "User aborted.");

    QVERIFY(closeCalled == true);

    // Disconnected.
    sesManager->emitSessionDisconnected();
    QVERIFY(finishedSpy.count() == 1);
    QList<QVariant> arguments = finishedSpy.takeFirst();
    QVERIFY(arguments.at(0).toInt() == CReporterUploadEngine::ProtocolError);
    QVERIFY(arguments.at(1).toInt() == 0);
    QVERIFY(arguments.at(2).toInt() == 2);

    QVERIFY(m_Subject->lastError() == "User aborted.");
}

void Ut_CReporterUploadEngine::testUploadFailedProtocolError()
{
    // Test situation when uploading fails due to HTTP error.
    QSignalSpy finishedSpy(m_Subject, SIGNAL(finished(int, int, int)));

    m_Queue->enqueue(
        new CReporterUploadItem("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo"));

    // Network session is requested to open.
    QVERIFY(openCalled == true);
    // ... session opens.
    sesManager->emitSessionOpened();

    // Error is received from HTTP client.
    httpInstance->emitUploadError("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo",
                                  "Host not found.");

    // Now protocol error doesn't cancel uploads
    QVERIFY(closeCalled == true);

    // Disconnected.
    sesManager->emitSessionDisconnected();

    QVERIFY(finishedSpy.count() == 1);
    QList<QVariant> arguments = finishedSpy.takeFirst();
    QVERIFY(arguments.at(0).toInt() == CReporterUploadEngine::ProtocolError);
    QVERIFY(arguments.at(1).toInt() == 0);
    QVERIFY(arguments.at(2).toInt() == 1);

    QVERIFY(m_Subject->lastError() == "Host not found.");
}

void Ut_CReporterUploadEngine::cleanup()
{
    if (m_Subject != 0) {
        delete m_Subject;
        m_Subject = 0;
    }

    if (m_Queue != 0) {
        delete m_Queue;
        m_Queue = 0;
    }
}

void Ut_CReporterUploadEngine::cleanupTestCase()
{
}

QTEST_MAIN(Ut_CReporterUploadEngine)

