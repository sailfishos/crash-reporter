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

#include <QFileInfo>
#include <QDebug>

// User includes.

#include "creporteruploaditem.h"
#include "creporterhttpclient.h"
#include "creporterutils.h"

using CReporter::LoggingCategory::cr;

// Local constants and macros.

const char *status_string[] = {"Waiting", "Sending", "Error", "Finished", "Cancelled", "Unknown"};

// *** Class CReporterUploadItemPrivate ****

/*!
  * @class CReporterUploadItemPrivate
  * @brief Private CReporterUploadItem class.
  *
  */
class CReporterUploadItemPrivate
{
    public:
        QString filepath;
        QString filename;
        QString errorString;
        qint64 filesize;
        CReporterHttpClient *http;
        CReporterUploadItem::ItemStatus status;
};

// *** Class CReporterUploadItem ****

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterUploadItem::CReporterUploadItem
// ----------------------------------------------------------------------------
CReporterUploadItem::CReporterUploadItem(const QString &file) :
        d_ptr(new CReporterUploadItemPrivate())
{
    Q_D(CReporterUploadItem);

    d->filepath = file;
    d->http = 0;

    QFileInfo fi(d->filepath);
    d->filename = fi.fileName();
    d->filesize = fi.size();

    qCDebug(cr) << "New item created:" << d->filename;
    d->status = Waiting;
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::~CReporterUploadItem
// ----------------------------------------------------------------------------
CReporterUploadItem::~CReporterUploadItem()
{
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::filesize
// ----------------------------------------------------------------------------
qint64 CReporterUploadItem::filesize() const
{
    return d_ptr->filesize;
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::filename
// ----------------------------------------------------------------------------
QString CReporterUploadItem::filename() const
{
    return d_ptr->filename;
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::markDone
// ----------------------------------------------------------------------------
void CReporterUploadItem::markDone()
{
    qCDebug(cr) << "Item done.";
    emit done();
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::status
// ----------------------------------------------------------------------------
CReporterUploadItem::ItemStatus CReporterUploadItem::status() const
{
    return d_ptr->status;
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::statusString
// ----------------------------------------------------------------------------
QString CReporterUploadItem::statusString() const
{
    return QString(status_string[d_ptr->status]);
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::errorString
// ----------------------------------------------------------------------------
QString CReporterUploadItem::errorString() const
{
    return d_ptr->errorString;
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::startUpload
// ----------------------------------------------------------------------------
bool CReporterUploadItem::startUpload()
{
    Q_D(CReporterUploadItem);
    qCDebug(cr) << "Starting upload of:" << d->filename;

    d->http = new CReporterHttpClient(this);
    connect(d->http, SIGNAL(finished()), this, SLOT(emitUploadFinished()));
    connect(d->http, SIGNAL(uploadError(QString,QString)),
            this, SLOT(uploadError(QString,QString)));
    connect(d->http, SIGNAL(updateProgress(int)), this, SIGNAL(updateProgress(int)));

    d->http->initSession();
    if (d->http->upload(d->filepath)) {
        setStatus(Sending);
        return true;
    }

    setStatus(Error);
    return false;
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::cancel
// ----------------------------------------------------------------------------
void CReporterUploadItem::cancel()
{
    Q_D( CReporterUploadItem );
    qCDebug(cr) << "Cancel upload;" << d->filename;

    ItemStatus previousStatus = d->status;
    setStatus(Cancelled);

    if (d->http != 0) {
        d->http->cancel();
    }

    if (previousStatus == Waiting) {
        // If this item wasn't sending yet, emit uploadFinished() here,
        // since there's no HTTP yet to trigger it.
        emit uploadFinished();
    }
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::setStatus
// ----------------------------------------------------------------------------
void CReporterUploadItem::setStatus(ItemStatus status)
{
    Q_D(CReporterUploadItem);
    qCDebug(cr) << "Current status:" << statusString();
    d->status = status;
    qCDebug(cr) << "New status:" << statusString();
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::uploadError
// ----------------------------------------------------------------------------
void CReporterUploadItem::uploadError(const QString &file, const QString &errorString)
{
    Q_D(CReporterUploadItem);
    Q_UNUSED(file);

    qCDebug(cr) << "Upload failed;" << d->filename;

    disconnect(d->http, SIGNAL(finished()), this, SLOT(emitUploadFinished()));
    disconnect(d->http, SIGNAL(uploadError(QString,QString)),
            this, SLOT(uploadError(QString,QString)));
    disconnect(d->http, SIGNAL(updateProgress(int)), this, SIGNAL(updateProgress(int)));

    setErrorString(errorString);

    if (d->status != Cancelled) {
        setStatus(Error);
    }
    emit uploadFinished();
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::emitUploadFinished
// ----------------------------------------------------------------------------
void CReporterUploadItem::emitUploadFinished()
{
    setStatus(Finished);
    emit uploadFinished();
}

// ----------------------------------------------------------------------------
// CReporterUploadItem::setErrorString
// ----------------------------------------------------------------------------
void CReporterUploadItem::setErrorString(const QString &errorString)
{
    Q_D( CReporterUploadItem );
    d->errorString = errorString;
}

// End of file.
