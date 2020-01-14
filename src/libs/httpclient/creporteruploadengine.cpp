/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Riku Halonen <riku.halonen@nokia.com>
 *
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Jakub Adam <jakub.adam@jollamobile.com>
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

#include <QDebug>
#include <QVariant>

#include "creporteruploadengine.h"
#include "creporteruploadengine_p.h"
#include "creporteruploadqueue.h"
#include "creporteruploaditem.h"
#include "creporterapplicationsettings.h"
#ifdef CREPORTER_LIBBEARER_ENABLED
#include "creporternwsessionmgr.h"
#endif // CREPORTER_LIBBEARER_ENABLED
#include "creporterutils.h"

using CReporter::LoggingCategory::cr;

static const char *state_string[] = {"NoConnection", "Connecting", "Connected", "Closing", "Aborting"};
static const char *error_string[] = {"NoError", "ProtocolError", "ConnectionNotAvailable", "ConnectionClosed"};


CReporterUploadEnginePrivate::CReporterUploadEnginePrivate()
{
    currentItem = 0;
    errorMessage.clear();
    error = CReporterUploadEngine::NoError;
    sentFiles = 0;
    state = NoConnection;

#ifdef CREPORTER_LIBBEARER_ENABLED
    networkSession = new CReporterNwSessionMgr(this);

    // Network signals.
    connect(networkSession, SIGNAL(networkError(QString)),
            this, SLOT(connectionError(QString)));
    connect(networkSession, SIGNAL(sessionDisconnected()),
            this, SLOT(sessionDisconnected()));
    connect(networkSession, SIGNAL(sessionOpened()),
            this, SLOT(sessionOpened()));
#endif // CREPORTER_LIBBEARER_ENABLED
}

CReporterUploadEnginePrivate::~CReporterUploadEnginePrivate()
{
}

void CReporterUploadEnginePrivate::stateChange(State nextState)
{
    qCDebug(cr) << "State:" << state_string[state] << ", next state:" << state_string[nextState];
    state = nextState;
}

void CReporterUploadEnginePrivate::uploadItem(CReporterUploadItem *item)
{
    qCDebug(cr) << "Got new item to upload:" << item->filename();

    connect(item, SIGNAL(uploadFinished()), this, SLOT(uploadFinished()));

    // Save item.
    currentItem = item;

#ifdef CREPORTER_LIBBEARER_ENABLED
    stateChange(Connecting);
    if (!networkSession->open()) {
        // No network connection. Open new session and wait for sessionOpened() -signal.
        return;
    }
    qCDebug(cr) << "Network connection exists. => start upload.";
#endif // CREPORTER_LIBBEARER_ENABLED
    // We have a network connection. Start upload immediately.
    stateChange(Connected);
    item->startUpload();
}

void CReporterUploadEnginePrivate::queueDone()
{
    qCDebug(cr) << "Queue is empty.";

#ifdef CREPORTER_LIBBEARER_ENABLED
    // Upload queue is empty. Close network session, if exists.
    if (state != NoConnection) {
        stateChange(Closing);
        networkSession->close();
    }
#endif // CREPORTER_LIBBEARER_ENABLED
    emitFinished(error, sentFiles, queue->totalNumberOfItems());
}

void CReporterUploadEnginePrivate::uploadFinished()
{
    CReporterUploadItem *item = qobject_cast<CReporterUploadItem *>(sender());

    qCDebug(cr) << "Upload item:" << item->filename()
                << "finished. Item status was:" << item->statusString();

    if (item->status() == CReporterUploadItem::Error && state != NoConnection) {
        setErrorType(CReporterUploadEngine::ProtocolError);
        setErrorString(item->errorString());
        // Let's try to go on with other files if there was an error but the connection might work
    } else if (item->status() == CReporterUploadItem::Error ||
               item->status() == CReporterUploadItem::Cancelled) {
        // If failure was detected during upload or cancel was requested by the user,
        // cancel also all pending uploads.
        // Failure in upload means, HTTP level error of which we cannot recover.
        // Pending uploads would most propably fail also, thus cancelling all.
        qCDebug(cr) << "Cancel pending uploads.";

        setErrorType(CReporterUploadEngine::ProtocolError);
        setErrorString(item->errorString());

        if (state != NoConnection) {
            stateChange(Aborting);
        }
        // Empty queue.
        queue->clear();
    } else {
        sentFiles++;
    }
    // Mark upload item as done. If there is no more pending uploads, queue will
    // emit done() -signal.
    item->markDone();
}

#ifdef CREPORTER_LIBBEARER_ENABLED
void CReporterUploadEnginePrivate::sessionOpened()
{
    qCDebug(cr) << "Network session opened => start upload.";

    if (state == Connecting) {
        stateChange(Connected);
        currentItem->startUpload();
    }
}

void CReporterUploadEnginePrivate::sessionDisconnected()
{
    // Handle network session closure.
    qCDebug(cr) << "Network session disconnected.";

    // Save current state.
    State oldState = state;
    stateChange(NoConnection);

    switch (oldState) {
    case Closing:
        // Closed upon request.
        // emitFinished(error, sentFiles, queue->totalNumberOfItems()); // workaround for qnetworksession not closing bug
        break;
    case Connecting:
        // Unable to create connection.
        setErrorType(CReporterUploadEngine::ConnectionNotAvailable);
        currentItem->cancel();
        break;
    case Connected:
        // Disconnected by the network.
        setErrorType(CReporterUploadEngine::ConnectionClosed);
        currentItem->cancel();
        break;
    default:
        break;
    }
}

void CReporterUploadEnginePrivate::connectionError(const QString &errorString)
{
    qCDebug(cr) << "Error in network connection. Setting error.";
    setErrorString(errorString);
    sessionDisconnected();
}
#endif // CREPORTER_LIBBEARER_ENABLED

void CReporterUploadEnginePrivate::setErrorString(const QString &message)
{
    if (errorMessage.isNull()) {
        errorMessage = message;
    }
}

void CReporterUploadEnginePrivate::setErrorType(CReporterUploadEngine::ErrorType type)
{
    if (error == CReporterUploadEngine::NoError) {
        error = type;
    }
}

void CReporterUploadEnginePrivate::emitFinished(CReporterUploadEngine::ErrorType error,
        int sent, int total)
{
    qCDebug(cr) << "Signalling finished(). Error:" << error_string[error];

    sentFiles = 0;
    emit q_ptr->finished(static_cast<int>(error), sent, total);
}

CReporterUploadEngine::CReporterUploadEngine(CReporterUploadQueue *queue, QObject *parent)
    : QObject(parent),
      d_ptr(new CReporterUploadEnginePrivate())
{
    Q_D(CReporterUploadEngine);

    d->queue = queue;

    d_ptr->q_ptr = this;

    // Upload queue signals.
    connect(queue, SIGNAL(nextItem(CReporterUploadItem *)),
            d_ptr, SLOT(uploadItem(CReporterUploadItem *)));
    connect(queue, SIGNAL(done()), d_ptr, SLOT(queueDone()));
}

CReporterUploadEngine::~CReporterUploadEngine()
{
    delete d_ptr;
    d_ptr = 0;
}

QString CReporterUploadEngine::lastError() const
{
    return d_ptr->errorMessage;
}

void CReporterUploadEngine::cancelAll()
{
    Q_D(CReporterUploadEngine);
    qCDebug(cr) << "Aborting upload(s).";
    if (d->currentItem) {
        d->currentItem->cancel();
    }
}
