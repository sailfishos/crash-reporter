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

// System includes.

#include <QObject>
#include <QQueue>
#include <QDebug>

// User includes.

#include "creporteruploadqueue.h"
#include "creporteruploaditem.h"

/*!
   * \class CReporterUploadQueuePrivate
   * \brief Private data class for CReporterUploadQueue.
   *
   */
class CReporterUploadQueuePrivate
{
    public:
        QQueue<CReporterUploadItem *> uploadQueue;
        bool notified;
        int nbrOfItems;
};

// *** Class CReporterUploadQueue ****

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterUploadQueue::CReporterUploadQueue
// ----------------------------------------------------------------------------
CReporterUploadQueue::CReporterUploadQueue(QObject *parent) :
        QObject(parent),
        d_ptr(new CReporterUploadQueuePrivate())
{
    qDebug() << __PRETTY_FUNCTION__ << "Upload queue initialized.";
    d_ptr->uploadQueue.clear();
    d_ptr->notified = false;
    d_ptr->nbrOfItems = 0;
}

// ----------------------------------------------------------------------------
// CReporterUploadQueue::~CReporterUploadQueue
// ----------------------------------------------------------------------------
CReporterUploadQueue::~CReporterUploadQueue()
{
    qDebug() << __PRETTY_FUNCTION__ << "Upload queue destroyed.";
    // Empty queue.
    clear();

    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterUploadQueue::enqueue
// ----------------------------------------------------------------------------
void CReporterUploadQueue::enqueue(CReporterUploadItem *item)
{
    Q_ASSERT(item != 0);
    qDebug() << __PRETTY_FUNCTION__ << "Append new item to queue...";

    item->setParent(this);
    d_ptr->uploadQueue.append(item);

    emit itemAdded(item);

    connect(item, SIGNAL(done()), this, SLOT(itemFinished()));

    if (!d_ptr->notified) {
        d_ptr->nbrOfItems = 0;
        qDebug() << __PRETTY_FUNCTION__ << "Added to empty queue => notify engine.";
        d_ptr->notified = true;
        // If list was empty, notify engine to start uploading.
        emit emitNextItem();
    }

    d_ptr->nbrOfItems++;
}

// ----------------------------------------------------------------------------
// CReporterUploadQueue::itemFinished
// ----------------------------------------------------------------------------
void CReporterUploadQueue::itemFinished()
{
    qDebug() << __PRETTY_FUNCTION__ << "Item finished.";

    CReporterUploadItem *item = qobject_cast<CReporterUploadItem *>(sender());
    item->deleteLater();

    if (d_ptr->uploadQueue.isEmpty()) {
        qDebug() << __PRETTY_FUNCTION__ << "Queue is empty => emit done()";
        d_ptr->notified = false;
        emit done();
    }
    else {
        qDebug() << __PRETTY_FUNCTION__ << "Queue size:" << d_ptr->uploadQueue.size();
        emit emitNextItem();
    }
}

// ----------------------------------------------------------------------------
// CReporterUploadQueue::totalNumberOfItems()
// ----------------------------------------------------------------------------
int CReporterUploadQueue::totalNumberOfItems() const
{
    return d_ptr->nbrOfItems;
}

// ----------------------------------------------------------------------------
// CReporterUploadQueue::clear
// ----------------------------------------------------------------------------
void CReporterUploadQueue::clear()
{
    if (d_ptr->uploadQueue.size() != 0) {
         QQueue<CReporterUploadItem *> items = d_ptr->uploadQueue;
        // Clear list.
        d_ptr->uploadQueue.clear();
        // Delete entries.
        qDeleteAll(items);
    }
}

// ----------------------------------------------------------------------------
// CReporterUploadQueue::emitNextItem
// ----------------------------------------------------------------------------
void CReporterUploadQueue::emitNextItem()
{
    qDebug() << __PRETTY_FUNCTION__ << "Emit nextItem().";
    CReporterUploadItem* item = d_ptr->uploadQueue.dequeue();

    emit nextItem(item);
}

// End of file.
