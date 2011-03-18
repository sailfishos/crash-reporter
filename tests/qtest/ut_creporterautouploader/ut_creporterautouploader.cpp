/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Raimo Gratseff <ext-raimo.gratseff@nokia.com>
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
#include <QStringList>

#include <MApplication>

#include "ut_creporterautouploader.h"
#include "creporterautouploader.h"
#include "creporterautouploaderdbusadaptor.h"
#include "creporternamespace.h"
#include "creporternotification.h"

// CReporterNotification mock object.
CReporterNotification::CReporterNotification(const QString &eventType,
                                             const QString &summary, const QString &body,
                                             const QString &imageName, QObject *parent)
{
    Q_UNUSED(eventType);
    Q_UNUSED(summary);
    Q_UNUSED(body);
    Q_UNUSED(imageName);
    Q_UNUSED(parent);
}

CReporterNotification::~CReporterNotification()
{}

void CReporterNotification::update(const QString &summary, const QString &body)
{
    Q_UNUSED(summary);
    Q_UNUSED(body);
}

void CReporterNotification::setTimeout(int timeout)
{
    Q_UNUSED(timeout);
}

void CReporterNotification::remove()
{}

int CReporterNotification::timeout() const
{
    return 0;
}

// CReporterUploadItem mock object.
CReporterUploadItem::CReporterUploadItem(const QString &file)
{
    Q_UNUSED(file);
}

CReporterUploadItem::~CReporterUploadItem()
{
}

// CReporterUploadQueue mock object.
static int itemsAddedCount;

CReporterUploadQueue::CReporterUploadQueue(QObject *parent)
{
    Q_UNUSED(parent);
    itemsAddedCount = 0;
}

CReporterUploadQueue::~CReporterUploadQueue()
{
}

void CReporterUploadQueue::enqueue(CReporterUploadItem *item)
{
    delete item;
    itemsAddedCount++;
}

static QString errorString;
static bool cancelAllCalled = false;
static bool lastErrorCalled = false;
static bool engineCreated = false;

// CReporterUploadEngine mock object.
CReporterUploadEngine::CReporterUploadEngine(CReporterUploadQueue *queue, QObject *parent)
{
    Q_UNUSED(queue);
    Q_UNUSED(parent);

    errorString.clear();
    cancelAllCalled = false;
    lastErrorCalled = false;
    engineCreated = true;
}

CReporterUploadEngine::~CReporterUploadEngine()
{
    engineCreated = false;
}

QString CReporterUploadEngine::lastError() const
{
    lastErrorCalled = true;
    return errorString;
}

void CReporterUploadEngine::cancelAll()
{
    cancelAllCalled = true;
}


// Unit test.
void Ut_CReporterAutoUploader::initTestCase()
{
}

void Ut_CReporterAutoUploader::init()
{
    static MApplication *app = 0;

    if (app == 0)
    {
        int argc = 2;
        const char *argv[] = {"./ut_creporterautouploader", "-software", 0};
        app = new MApplication(argc, (char **)argv);
    }

    m_Subject = new CReporterAutoUploader();
    engineCreated = false;
}

void Ut_CReporterAutoUploader::testUploadFiles()
{
    QStringList fileList;
    fileList << "file1" << "file2" << "file3";
    bool uploadFilesRetVal = m_Subject->uploadFiles(fileList);

    QVERIFY(engineCreated);
    QVERIFY(itemsAddedCount == 3);
    QVERIFY(uploadFilesRetVal == true);
}

void Ut_CReporterAutoUploader::testUploadFilesInvalid()
{
    QStringList fileList;
    bool uploadFilesRetVal = m_Subject->uploadFiles(fileList);

    QVERIFY(engineCreated == false);
    QVERIFY(itemsAddedCount == 0);
    QVERIFY(uploadFilesRetVal == false);
}

void Ut_CReporterAutoUploader::testUploadFilesAgain()
{
    QStringList fileList;
    fileList << "file1" << "file2" << "file3";
    bool uploadFilesRetVal = m_Subject->uploadFiles(fileList);
    QVERIFY(uploadFilesRetVal == true);
    QVERIFY(engineCreated);

    fileList.clear();
    fileList << "file4" << "file5";
    uploadFilesRetVal = m_Subject->uploadFiles(fileList);

    QVERIFY(engineCreated);
    QVERIFY(itemsAddedCount == 5);
    QVERIFY(uploadFilesRetVal == true);
}

void Ut_CReporterAutoUploader::testQuit()
{
    QStringList fileList;
    fileList << "file1";
    bool uploadFilesRetVal = m_Subject->uploadFiles(fileList);
    m_Subject->quit();

    QVERIFY(engineCreated);
    QVERIFY(itemsAddedCount == 1);
    QVERIFY(uploadFilesRetVal == true);
    QVERIFY(cancelAllCalled);

}

void Ut_CReporterAutoUploader::cleanup()
{
    if (m_Subject != 0) {
        delete m_Subject;
        m_Subject = 0;
    }
}

void Ut_CReporterAutoUploader::cleanupTestCase()
{
}

QTEST_APPLESS_MAIN(Ut_CReporterAutoUploader)
