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
#include <QList>

#include "creporteruploadqueue.h"
#include "ut_creporteruploadqueue.h"

static QList<CReporterUploadItem *> items;

// CReporterUploadItem mock object.
CReporterUploadItem::CReporterUploadItem(const QString &file)
{
    Q_UNUSED(file);
    items.append(this);
}

CReporterUploadItem::~CReporterUploadItem()
{
}

void CReporterUploadItem::emitDone()
{
    emit done();
}

// Unit test object.
void Ut_CReporterUploadQueue::init()
{
    m_Subject = new CReporterUploadQueue();
}

void Ut_CReporterUploadQueue::initTestCase()
{

}

void Ut_CReporterUploadQueue::testEnqueueItems()
{
    // Verify that items are enqueued and dequeued correctly.
    QSignalSpy itemAddedSpy(m_Subject, SIGNAL(itemAdded(CReporterUploadItem *)));
    QSignalSpy nextItemSpy(m_Subject, SIGNAL(nextItem(CReporterUploadItem *)));

    QStringList files;
    files << "/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo"
          << "/media/mmc1/core-dumps/application-1234-9-4321.rcore.lzo"
          << "/media/mmc2/core-dumps/application-1234-11-4321.rcore.lzo";

    // Add some items into queue.
    foreach (QString file, files) {
        m_Subject->enqueue(new CReporterUploadItem(file));
    }

    // Check that these are queued.
    QVERIFY(itemAddedSpy.count() == 3);
    QVERIFY(m_Subject->totalNumberOfItems() == 3);

    // Mark items as done.
    foreach (CReporterUploadItem *item, items) {
        item->emitDone();
    }

    QVERIFY(nextItemSpy.count() == 3);
}


void Ut_CReporterUploadQueue::cleanup()
{
    if (m_Subject != 0) {
        delete m_Subject;
        m_Subject = 0;
    }

    items.clear();
}

void Ut_CReporterUploadQueue::cleanupTestCase()
{
}

QTEST_MAIN(Ut_CReporterUploadQueue)
