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

#ifndef UT_CREPORTERUPLOADENGINE_H
#define UT_CREPORTERUPLOADENGINE_H

#include <QTest>

class CReporterUploadEngine;
class CReporterUploadQueue;

// CReporterHttpClient mock class.
class CReporterHttpClient : public QObject
{
    Q_OBJECT

    CReporterHttpClient(QObject* parent=0);

    ~CReporterHttpClient();

    void initSession(bool deleteAfterSending=true);

    Q_SIGNALS:
        void finished();
        void uploadError(const QString &file, const QString &errorString);
        void updateProgress(int done);

    public Q_SLOTS:
        bool upload(const QString &file);
        void cancel();

    public:
        void emitFinished();
        void emitUploadError(const QString &file, const QString &errorString);
        void emitUpdateProgress(int done);
};

// CReporterNwSessionMgr mock class.
class CReporterNwSessionMgr : public QObject
{
    Q_OBJECT

    public:
        CReporterNwSessionMgr(QObject *parent=0);
        ~CReporterNwSessionMgr();
        bool opened() const;
        void emitSessionOpened();
        void emitSessionDisconnected();
        void emitNetworkError(const QString &errorString);

    Q_SIGNALS:
        void sessionOpened();
        void sessionDisconnected();
        void networkError(const QString &errorString);

    public Q_SLOTS:
        bool open();
        void close();
        void stop();
};

class Ut_CReporterUploadEngine : public QObject
{
    Q_OBJECT

    private Q_SLOTS:
    void initTestCase();
    void init();

    void testUploadItems();
    void testOpeningNetworkSessionFails();
    void testNetworkSessionDisconnectsDuringUpload();
    void testUploadCancelledByTheUser();
    void testUploadFailedProtocolError();

    void cleanupTestCase();
    void cleanup();

private:
    CReporterUploadEngine *m_Subject;
    CReporterUploadQueue *m_Queue;
};

#endif // UT_CREPORTERUPLOADENGINE_H
