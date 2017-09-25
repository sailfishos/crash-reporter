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

#ifndef UT_CREPORTERUPLOADITEM_H
#define UT_CREPORTERUPLOADITEM_H

#include <QTest>

class CReporterUploadItem;

class CReporterHttpClient : public QObject
{
    Q_OBJECT

public:
    CReporterHttpClient(QObject *parent = 0);

    ~CReporterHttpClient();

    void initSession(bool deleteAfterSending = true);

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

class Ut_CReporterUploadItem : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();
    void init();

    void testSendingItem();
    void testSendingItemFailed();
    void testSendingItemCancelled();
    void testFailingUploadStarting();
    void testCancellingWaitingItem();

    void cleanupTestCase();
    void cleanup();

private:
    CReporterUploadItem *m_Subject;
};

#endif // UT_CREPORTERUPLOADITEM_H
