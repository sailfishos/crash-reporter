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

#include "qnetworkaccessmanager.h"
#include <QDebug>
#include <QSignalSpy>
#include <QSslConfiguration>


#include "qnetworkreply.h"
#include "ut_creporterhttpclient.h"
#include "creporterhttpclient_p.h"

void Ut_CReporterHttpClient::initTestCase()
{

}

void Ut_CReporterHttpClient::init()
{
    m_Subject = new CReporterHttpClient();
}

void Ut_CReporterHttpClient::cleanup()
{
    if (m_Subject != 0) {
        delete m_Subject;
        m_Subject = 0;
    }
}

void Ut_CReporterHttpClient::cleanupTestCase()
{
}

void Ut_CReporterHttpClient::testInitSession()
{
    // Do init ..
    m_Subject->initSession(false);
    // .. and check the state
    QCOMPARE (m_Subject->state(), CReporterHttpClient::Init);
}

void Ut_CReporterHttpClient::testUpload()
{
    m_Subject->initSession(false);
    // start upload
    QVERIFY (m_Subject->upload("/usr/lib/crash-reporter-tests/testdata/"
			    "crashapplication-0287-11-2260.rcore.lzo"));

    QSignalSpy uploadErrorSpy (m_Subject, SIGNAL(uploadError(const QString&,
							  const QString&)));
    QSignalSpy updateProgressSpy (m_Subject, SIGNAL(updateProgress(int)));
    QSignalSpy finnishedSpy (m_Subject, SIGNAL(finished()));

    m_Subject->d_ptr->m_manager->emitAuthenticationRequired(m_Subject->d_ptr->m_reply);

    // emit signals to simulate upload
    m_Subject->d_ptr->m_reply->emitUploadProgress (0,1000);
    QTest::qWait(100);
    m_Subject->d_ptr->m_reply->emitUploadProgress (500,1000);
    QTest::qWait(100);
    m_Subject->d_ptr->m_reply->emitUploadProgress (1000,1000);
    m_Subject->d_ptr->m_reply->emitFinished ();
    
    // check the signal spies
    QCOMPARE(uploadErrorSpy.count(), 0);
    QCOMPARE(updateProgressSpy.count(), 3);
    QCOMPARE(finnishedSpy.count(), 1);

}

void Ut_CReporterHttpClient::testUploadCancel()
{
    m_Subject->initSession(false);
    // start upload
    QVERIFY (m_Subject->upload("/usr/lib/crash-reporter-tests/testdata/"
			    "crashapplication-0287-11-2260.rcore.lzo"));

    QSignalSpy uploadErrorSpy (m_Subject, SIGNAL(uploadError(const QString&,
							  const QString&)));
    QSignalSpy updateProgressSpy (m_Subject, SIGNAL(updateProgress(int)));
    QSignalSpy finnishedSpy (m_Subject, SIGNAL(finished()));

    m_Subject->d_ptr->m_manager->emitAuthenticationRequired(m_Subject->d_ptr->m_reply);
    m_Subject->d_ptr->m_reply->emitUploadProgress (0,1000);
    QTest::qWait(100);
    // cancel it
    m_Subject->cancel();

    QCOMPARE(uploadErrorSpy.count(), 0);
    QCOMPARE(updateProgressSpy.count(), 1);
    QCOMPARE(finnishedSpy.count(), 1);

}

void Ut_CReporterHttpClient::testNwError()
{
    m_Subject->initSession(false);
    // start upload
    QVERIFY (m_Subject->upload("/usr/lib/crash-reporter-tests/testdata/"
			    "crashapplication-0287-11-2260.rcore.lzo"));

    QSignalSpy uploadErrorSpy (m_Subject, SIGNAL(uploadError(const QString&,
							  const QString&)));
    QSignalSpy updateProgressSpy (m_Subject, SIGNAL(updateProgress(int)));
    QSignalSpy finnishedSpy (m_Subject, SIGNAL(finished()));

    m_Subject->d_ptr->m_manager->emitAuthenticationRequired(m_Subject->d_ptr->m_reply);
    m_Subject->d_ptr->m_reply->emitUploadProgress (100,1000);
    QTest::qWait(100);
    // emit network error
    m_Subject->d_ptr->m_reply->emitError (QNetworkReply::TimeoutError);
    m_Subject->d_ptr->m_reply->emitFinished ();

    QTest::qWait(100);


    QCOMPARE(uploadErrorSpy.count(), 1);
    QCOMPARE(updateProgressSpy.count(), 1);
    QCOMPARE(finnishedSpy.count(), 1);

}

void Ut_CReporterHttpClient::testSslError()
{
    m_Subject->initSession(false);
    // start upload
    QVERIFY (m_Subject->upload("/usr/lib/crash-reporter-tests/testdata/"
			    "crashapplication-0287-11-2260.rcore.lzo"));

    QSignalSpy uploadErrorSpy (m_Subject, SIGNAL(uploadError(const QString&,
							  const QString&)));
    QSignalSpy updateProgressSpy (m_Subject, SIGNAL(updateProgress(int)));
    QSignalSpy finnishedSpy (m_Subject, SIGNAL(finished()));

    m_Subject->d_ptr->m_manager->emitAuthenticationRequired(m_Subject->d_ptr->m_reply);
    m_Subject->d_ptr->m_reply->emitUploadProgress (100,1000);
    QTest::qWait(100);
    // generate some bogus errors
    QList<QSslError> err_list;
    QSslError err (QSslError::NoSslSupport);
    QSslError err2 (QSslError::PathLengthExceeded);

    err_list.append(err);
    err_list.append(err2);
    // emit the sslerrors signal 
    m_Subject->d_ptr->m_reply->emitSslErrors (err_list);
    m_Subject->d_ptr->m_reply->emitFinished ();

    QTest::qWait(100);

    QCOMPARE(uploadErrorSpy.count(), 0);
    QCOMPARE(updateProgressSpy.count(), 1);
    QCOMPARE(finnishedSpy.count(), 1);

}

QTEST_MAIN(Ut_CReporterHttpClient)

