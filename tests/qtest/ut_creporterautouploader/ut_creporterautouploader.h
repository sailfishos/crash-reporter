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

#ifndef UT_CREPORTERAUTOUPLOADER_H
#define UT_CREPORTERAUTOUPLOADER_H

#include <QTest>

class CReporterAutoUploader;

// CReporterUploadItem mock class.
class CReporterUploadItem : public QObject
{
    Q_OBJECT

public:
    CReporterUploadItem(const QString &file);
    ~CReporterUploadItem();

};

// CReporterUploadQueue mock class.
class CReporterUploadQueue : public QObject
{
    Q_OBJECT

    public:
        CReporterUploadQueue(QObject *parent=0);
        ~CReporterUploadQueue();
        void enqueue(CReporterUploadItem *item);
};

// CReporterUploadEngine mock class.
class CReporterUploadEngine : public QObject
{
    Q_OBJECT

    public:

        typedef enum{
            //! No error.
            NoError = 0,
            //! Indicates errors occured on HTTP/ SSL protocol levels.
            ProtocolError,
            //! Error occured, when trying to establish internet connection.
            ConnectionNotAvailable,
            //! Internet connection was closed due to network disconnected.
            ConnectionClosed,
        } ErrorType;

        CReporterUploadEngine(CReporterUploadQueue *queue, QObject *parent=0);

        ~CReporterUploadEngine();

      QString lastError() const;

    Q_SIGNALS:
        void finished(int error, int sent, int total);

    public Q_SLOTS:
        void cancelAll();
};

// Unit test class.
class Ut_CReporterAutoUploader : public QObject
{
    Q_OBJECT

    private Q_SLOTS:

        void initTestCase();
        void init();

        void testUploadFiles();
        void testUploadFilesInvalid();
        void testUploadFilesAgain();
        void testQuit();

        void cleanup();
        void cleanupTestCase();

    private:
        CReporterAutoUploader *m_Subject;
};

#endif // UT_CREPORTERAUTOUPLOADER_H
