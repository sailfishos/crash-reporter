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

#include "creporteruploaditem.h"
#include "ut_creporteruploaditem.h"

static CReporterHttpClient *httpInstance = 0;
static bool initSessionCalled;
static bool uploadCalled;
static bool cancelCalled;
static bool uploadStarted;

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
    uploadCalled = true;
    return uploadStarted;
}

void CReporterHttpClient::cancel()
{
    cancelCalled = true;
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

// Unit test object.
void Ut_CReporterUploadItem::init()
{
    initSessionCalled = false;
    uploadCalled = false;
    cancelCalled =  false;
    uploadStarted = false;

    m_Subject =
        new CReporterUploadItem("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo");
}

void Ut_CReporterUploadItem::initTestCase()
{

}

void Ut_CReporterUploadItem::testSendingItem()
{
    // Test sending item successfully.
    QSignalSpy updateProgressSpy(m_Subject, SIGNAL(updateProgress(int)));
    QSignalSpy uploadFinishedSpy(m_Subject, SIGNAL(uploadFinished()));
    QSignalSpy doneSpy(m_Subject, SIGNAL(done()));

    uploadStarted = true;
    QVERIFY(m_Subject->status() == CReporterUploadItem::Waiting);

    m_Subject->startUpload();
    QVERIFY(uploadCalled == true);
    QVERIFY(m_Subject->status() == CReporterUploadItem::Sending);

    for (int i = 0; i <= 100; i = i + 10) {
        httpInstance->emitUpdateProgress(i);
    }

    httpInstance->emitFinished();
    QVERIFY(updateProgressSpy.count() == 11);
    QVERIFY(uploadFinishedSpy.count() == 1);
    QVERIFY(m_Subject->status() == CReporterUploadItem::Finished);

    m_Subject->done();
    QVERIFY(doneSpy.count() == 1);
}

void Ut_CReporterUploadItem::testSendingItemFailed()
{
    // Test sending item failed.
    QSignalSpy updateProgressSpy(m_Subject, SIGNAL(updateProgress(int)));
    QSignalSpy uploadFinishedSpy(m_Subject, SIGNAL(uploadFinished()));

    uploadStarted = true;
    QVERIFY(m_Subject->status() == CReporterUploadItem::Waiting);

    m_Subject->startUpload();
    QVERIFY(uploadCalled == true);
    QVERIFY(m_Subject->status() == CReporterUploadItem::Sending);

    for (int i = 0; i <= 50; i = i + 10) {
        httpInstance->emitUpdateProgress(i);
    }

    httpInstance->emitUploadError("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo",
                                  "Socket timeout.");
    QVERIFY(updateProgressSpy.count() == 6);
    QVERIFY(uploadFinishedSpy.count() == 1);
    QVERIFY(m_Subject->status() == CReporterUploadItem::Error);
    QVERIFY(m_Subject->errorString() == "Socket timeout.");
}

void Ut_CReporterUploadItem::testSendingItemCancelled()
{
    // Test sending item failed.
    QSignalSpy updateProgressSpy(m_Subject, SIGNAL(updateProgress(int)));
    QSignalSpy uploadFinishedSpy(m_Subject, SIGNAL(uploadFinished()));

    uploadStarted = true;
    QVERIFY(m_Subject->status() == CReporterUploadItem::Waiting);

    m_Subject->startUpload();
    QVERIFY(uploadCalled == true);
    QVERIFY(m_Subject->status() == CReporterUploadItem::Sending);

    for (int i = 0; i <= 50; i = i + 10) {
        httpInstance->emitUpdateProgress(i);
    }

    m_Subject->cancel();
    QVERIFY(cancelCalled == true);

    httpInstance->emitUploadError("/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo",
                                  "User aborted.");

    QVERIFY(updateProgressSpy.count() == 6);
    QVERIFY(uploadFinishedSpy.count() == 1);
    QVERIFY(m_Subject->status() == CReporterUploadItem::Cancelled);
    QVERIFY(m_Subject->errorString() == "User aborted.");
}

void Ut_CReporterUploadItem::testFailingUploadStarting()
{
    uploadStarted = false;
    QVERIFY(m_Subject->status() == CReporterUploadItem::Waiting);

    m_Subject->startUpload();
    QVERIFY(uploadCalled == true);
    QVERIFY(m_Subject->status() == CReporterUploadItem::Error);
}

void Ut_CReporterUploadItem::testCancellingWaitingItem()
{
    QSignalSpy uploadFinishedSpy(m_Subject, SIGNAL(uploadFinished()));
    m_Subject->cancel();
    QVERIFY(cancelCalled == false);
    QVERIFY(uploadFinishedSpy.count() == 1);
    QVERIFY(m_Subject->status() == CReporterUploadItem::Cancelled);
}

void Ut_CReporterUploadItem::cleanup()
{
    if (m_Subject != 0) {
        delete m_Subject;
        m_Subject = 0;
    }
}

void Ut_CReporterUploadItem::cleanupTestCase()
{
}

QTEST_MAIN(Ut_CReporterUploadItem)
