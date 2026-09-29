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

#include <QFileInfo>
#include <QDebug>

#include "creporteruploaditem.h"
#include "creporterhttpclient.h"
#include "creporterutils.h"

using CReporter::LoggingCategory::cr;

static const char *status_string[] = {"Waiting", "Sending", "Error", "Finished", "Cancelled", "Unknown"};

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

CReporterUploadItem::CReporterUploadItem(const QString &file)
    : d_ptr(new CReporterUploadItemPrivate())
{
    Q_D(CReporterUploadItem);

    d->filepath = file;
    d->http = nullptr;

    QFileInfo fi(d->filepath);
    d->filename = fi.fileName();
    d->filesize = fi.size();

    qCDebug(cr) << "New item created:" << d->filename;
    d->status = Waiting;
}

CReporterUploadItem::~CReporterUploadItem()
{
    delete d_ptr;
    d_ptr = nullptr;
}

qint64 CReporterUploadItem::filesize() const
{
    return d_ptr->filesize;
}

QString CReporterUploadItem::filename() const
{
    return d_ptr->filename;
}

void CReporterUploadItem::markDone()
{
    qCDebug(cr) << "Item done.";
    emit done();
}

CReporterUploadItem::ItemStatus CReporterUploadItem::status() const
{
    return d_ptr->status;
}

QString CReporterUploadItem::statusString() const
{
    return QString(status_string[d_ptr->status]);
}

QString CReporterUploadItem::errorString() const
{
    return d_ptr->errorString;
}

bool CReporterUploadItem::startUpload()
{
    Q_D(CReporterUploadItem);
    qCDebug(cr) << "Starting upload of:" << d->filename;

    d->http = new CReporterHttpClient(this);
    connect(d->http, &CReporterHttpClient::finished,
            this, &CReporterUploadItem::emitUploadFinished);
    connect(d->http, &CReporterHttpClient::uploadError,
            this, &CReporterUploadItem::uploadError);
    connect(d->http, &CReporterHttpClient::updateProgress,
            this, &CReporterUploadItem::updateProgress);

    d->http->initSession();
    if (d->http->upload(d->filepath)) {
        setStatus(Sending);
        return true;
    }

    setStatus(Error);
    return false;
}

void CReporterUploadItem::cancel()
{
    Q_D(CReporterUploadItem);
    qCDebug(cr) << "Cancel upload;" << d->filename;

    ItemStatus previousStatus = d->status;
    setStatus(Cancelled);

    if (d->http) {
        d->http->cancel();
    }

    if (previousStatus == Waiting) {
        // If this item wasn't sending yet, emit uploadFinished() here,
        // since there's no HTTP yet to trigger it.
        emit uploadFinished();
    }
}

void CReporterUploadItem::setStatus(ItemStatus status)
{
    Q_D(CReporterUploadItem);
    qCDebug(cr) << "Current status:" << statusString();
    d->status = status;
    qCDebug(cr) << "New status:" << statusString();
}

void CReporterUploadItem::uploadError(const QString &file, const QString &errorString)
{
    Q_D(CReporterUploadItem);
    Q_UNUSED(file);

    qCWarning(cr) << "Upload failed:" << d->filename << errorString;

    disconnect(d->http, &CReporterHttpClient::finished,
               this, &CReporterUploadItem::emitUploadFinished);
    disconnect(d->http, &CReporterHttpClient::uploadError,
               this, &CReporterUploadItem::uploadError);
    disconnect(d->http, &CReporterHttpClient::updateProgress,
               this, &CReporterUploadItem::updateProgress);

    setErrorString(errorString);

    if (d->status != Cancelled) {
        setStatus(Error);
    }
    emit uploadFinished();
}

void CReporterUploadItem::emitUploadFinished()
{
    setStatus(Finished);
    emit uploadFinished();
}

void CReporterUploadItem::setErrorString(const QString &errorString)
{
    Q_D(CReporterUploadItem);
    d->errorString = errorString;
}
