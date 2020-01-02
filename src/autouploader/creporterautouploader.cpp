/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Raimo Gratseff <ext-raimo.gratseff@nokia.com>
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
#include <QDBusConnection>

#include <notification.h>

#include "creporterautouploader.h"
#include "creporternamespace.h"
#include "creporternwsessionmgr.h"
#include "creportersavedstate.h"
#include "creporteruploadqueue.h"
#include "creporteruploaditem.h"
#include "creporteruploadengine.h"
#include "creporterutils.h"
#include "creporterprivacysettingsmodel.h"

#include "autouploader_adaptor.h" // generated

using CReporter::LoggingCategory::cr;

/*! @class CReporterAutoUploaderPrivate
  * @brief Private CReporterAutoUploaderPrivate class.
  *
  * @sa CReporterAutoUploader
  */
class CReporterAutoUploaderPrivate
{
public:
    //! @arg Upload engine.
    CReporterUploadEngine *engine;
    //! @arg Upload queue.
    CReporterUploadQueue queue;
    //! @arg Is the service active.
    bool activated;
    //! @arg files that have been added to upload queue during this auto uploader session
    QStringList addedFiles;
    /*! Notification object giving user a notice that upload is in progress.*/
    Notification *progressNotification;
    /*! Notification object giving user a notice of successful uploads.*/
    Notification *successNotification;
    /*! Notification object giving user a notice of failed uploads.*/
    Notification *failedNotification;

};

CReporterAutoUploader::CReporterAutoUploader()
    : d_ptr(new CReporterAutoUploaderPrivate)
{
    d_ptr->engine = 0;
    d_ptr->activated = false;
    d_ptr->progressNotification = new Notification(this);
    d_ptr->successNotification = new Notification(this);
    d_ptr->successNotification->setReplacesId(CReporterSavedState::instance()->uploadSuccessNotificationId());
    d_ptr->failedNotification = new Notification(this);
    d_ptr->failedNotification->setReplacesId(CReporterSavedState::instance()->uploadFailedNotificationId());

    CReporterUtils::applyNotificationStyle(d_ptr->progressNotification);
    CReporterUtils::applyNotificationStyle(d_ptr->successNotification);
    CReporterUtils::applyNotificationStyle(d_ptr->failedNotification);

    // Create adaptor class. Needs to be taken from the stack.
    new AutoUploaderAdaptor(this);
    // Register service name and object.
    QDBusConnection::sessionBus().registerObject(CReporter::AutoUploaderObjectPath, this);
    QDBusConnection::sessionBus().registerService(CReporter::AutoUploaderServiceName);
    qCDebug(cr) << "Started Auto Uploader service.";
}

CReporterAutoUploader::~CReporterAutoUploader()
{
    quit();
    delete d_ptr;
    d_ptr = 0;

    CReporterSavedState::freeSingleton();

    qCDebug(cr) << "Service closed.";
}

bool CReporterAutoUploader::uploadFiles(const QStringList &fileList,
                                        bool obeyNetworkRestrictions)
{
    qCDebug(cr) << "Received a list of files to upload.";

    if (fileList.isEmpty())
        return false;

    if (!d_ptr->activated) {
        d_ptr->engine = new CReporterUploadEngine(&d_ptr->queue);
        d_ptr->activated = true;
        connect(d_ptr->engine, SIGNAL(finished(int, int, int)), SLOT(engineFinished(int, int, int)));
    }

    if (obeyNetworkRestrictions &&
            !CReporterNwSessionMgr::canUseNetworkConnection()) {
        qCDebug(cr) << "No unpaid network connection available, aborting crash report upload.";
        QTimer::singleShot(0, this, SLOT(quit()));
        return false;
    }

    foreach (QString filename, fileList) {
        if (!d_ptr->addedFiles.contains(filename)) {
            qCDebug(cr) << "Adding to upload queue: " << filename;
            // CReporterUploadQueue class will own the CReporterUploadItem instance.
            d_ptr->queue.enqueue(new CReporterUploadItem(filename));
            d_ptr->addedFiles << filename;
        } else {
            qCDebug(cr) << filename << "was not added to queue because it had already been added before";
        }
    }

    if (CReporterPrivacySettingsModel::instance()->notificationsEnabled()) {
        //% "Uploading reports"
        QString summary = qtTrId("crash_reporter-notify-uploading_reports");
        //% "%n report(s) to upload"
        QString body = qtTrId("crash_reporter-notify-num_to_upload", fileList.count());
        d_ptr->progressNotification->setSummary(summary);
        d_ptr->progressNotification->setPreviewSummary(summary);
        d_ptr->progressNotification->setBody(body);
        d_ptr->progressNotification->setPreviewBody(body);
        d_ptr->progressNotification->publish();
    }

    return true;
}

void CReporterAutoUploader::quit()
{
    qCDebug(cr) << "Quit auto uploader.";
    if (d_ptr->engine) {
        if (d_ptr->activated) {
            qCDebug(cr) << "Engine active -> cancelling";
            d_ptr->activated = false;
            d_ptr->engine->cancelAll();
        }
        qCDebug(cr) << "Deleting engine.";
        disconnect(d_ptr->engine, SIGNAL(finished(int, int, int)), this, SLOT(engineFinished(int, int, int)));
        d_ptr->engine->deleteLater();
        d_ptr->engine = 0;
    }
    qApp->quit();
}

void CReporterAutoUploader::engineFinished(int error, int sent, int total)
{
    QString message;

    // Construct message.
    switch (error) {
    case CReporterUploadEngine::NoError:
        //% "%n report(s) uploaded successfully."
        message = qtTrId("qtn_crash_reports_uploaded_successfully_text", total);
        break;
    case CReporterUploadEngine::ProtocolError:
    case CReporterUploadEngine::ConnectionNotAvailable:
    case CReporterUploadEngine::ConnectionClosed: {
        //% "Failed to upload report(s)."
        message = qtTrId("qtn_failed_to_send_crash_reports_text");
        //% "%n files attempted"
        QString attemptPart = qtTrId("qtn_crash_reporter-files_attempted", total);
        //% "%n files succeeded"
        QString succeedPart = qtTrId("qtn_crash_reporter-files_succeeded", sent);
        //: %1 replaced with qtn_crash_reporter-files_attempted and %2 with qtn_crash_reporter-files_succeeded
        //: E.g. "<br>Upload results: 3 files attempted 1 file succeeded."
        //% "<br>Upload results: %1 %2."
        message += qtTrId("qtn_crash_reporter-upload_result").arg(attemptPart).arg(succeedPart);
        break;
    }
    default:
        // We should never enter here.
        break;
    }

    if (error != CReporterUploadEngine::NoError) {
        // Add error reason.
        if (error == CReporterUploadEngine::ConnectionNotAvailable) {
            //% "<br>Reason: Failed to create internet connection."
            message += qtTrId("qtn_reason_failed_to_create_internet_connection_text");
        } else {
            QString reason = d_ptr->engine->lastError();
            if (!reason.isEmpty()) {
                // If reason is available, append it.
                //% "<br>Reason: %1."
                message += qtTrId("qtn_reason_%1_text").arg(reason);
            } else {
                // Use default error.
                //% "<br>Reason: Unknown."
                message += qtTrId("qtn_reason_unknown_text");
            }
        }
    }

    if (CReporterPrivacySettingsModel::instance()->notificationsEnabled()) {
        d_ptr->progressNotification->close();

        if (total > sent) {
            int failures = total - sent;
            //% "Failed to send all reports"
            QString summary = qtTrId("crash_reporter-notify-send_failed");
            //% "%n uploads failed"
            QString body = qtTrId("crash_reporter-notify-num_failed", failures);

            d_ptr->failedNotification->setSummary(summary);
            d_ptr->failedNotification->setPreviewSummary(summary);
            d_ptr->failedNotification->setBody(body);
            d_ptr->failedNotification->setPreviewBody(body);
            d_ptr->failedNotification->setItemCount(failures);
            d_ptr->failedNotification->publish();
        } else {
            d_ptr->failedNotification->close();
        }

        CReporterSavedState *state = CReporterSavedState::instance();

        if (sent > 0) {
            sent += state->uploadSuccessCount();

            //% "Report(s) uploaded"
            QString summary = qtTrId("crash_reporter-notify-reports_uploaded", sent);
            d_ptr->successNotification->setSummary(summary);
            d_ptr->successNotification->setPreviewSummary(summary);
            d_ptr->successNotification->setItemCount(sent);
            d_ptr->successNotification->publish();
        }

        state->setUploadSuccessNotificationId(d_ptr->successNotification->replacesId());
        state->setUploadFailedNotificationId(d_ptr->failedNotification->replacesId());
        state->setUploadSuccessCount(sent);
    }

    qCDebug(cr) << "Message: " << message;

    d_ptr->activated = false;
    quit();
}

#include "moc_autouploader_adaptor.cpp"
