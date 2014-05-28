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

// System includes.

#include <QDebug>
#include <QDBusConnection>

// User includes.

#include "creporterautouploader.h"
#include "creporternamespace.h"
#include "creporternwsessionmgr.h"
#include "creportersavedstate.h"
#include "creporteruploadqueue.h"
#include "creporteruploaditem.h"
#include "creporteruploadengine.h"
#include "creporternotification.h"
#include "creporterprivacysettingsmodel.h"

#include "autouploader_adaptor.h" // generated

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
        CReporterNotification *progressNotification;
        /*! Notification object giving user a notice of successful uploads.*/
        CReporterNotification *successNotification;
        /*! Notification object giving user a notice of failed uploads.*/
        CReporterNotification *failedNotification;

};

CReporterAutoUploader::CReporterAutoUploader() : d_ptr(new CReporterAutoUploaderPrivate)
{
    d_ptr->engine = 0;
    d_ptr->activated = false;
    d_ptr->progressNotification =
            new CReporterNotification(CReporter::AutoUploaderNotificationEventType,
            0, this);
    d_ptr->successNotification =
            new CReporterNotification(CReporter::AutoUploaderNotificationEventType,
                    CReporterSavedState::instance()->uploadSuccessNotificationId(),
                    this);
    d_ptr->failedNotification =
            new CReporterNotification(CReporter::AutoUploaderNotificationEventType,
                    CReporterSavedState::instance()->uploadFailedNotificationId(),
                    this);

    // Create adaptor class. Needs to be taken from the stack.
    new AutoUploaderAdaptor(this);
    // Register service name and object.
    QDBusConnection::sessionBus().registerService(CReporter::AutoUploaderServiceName);
    QDBusConnection::sessionBus().registerObject(CReporter::AutoUploaderObjectPath, this);
    qDebug() << __PRETTY_FUNCTION__ << "Started Auto Uploader service.";
}

// ----------------------------------------------------------------------------
// CReporterAutoUploader::~CReporterAutoUploader
// ----------------------------------------------------------------------------
CReporterAutoUploader::~CReporterAutoUploader()
{
    quit();
    if (d_ptr)
    {
        delete d_ptr;
        d_ptr = 0;
    }

    CReporterSavedState::freeSingleton();

    qDebug() << __PRETTY_FUNCTION__ << "Service closed.";

}

bool CReporterAutoUploader::uploadFiles(const QStringList &fileList,
        bool obeyNetworkRestrictions)
{
    qDebug() << __PRETTY_FUNCTION__ << "Received a list of files to upload.";

    if (fileList.isEmpty())
        return false;

    if (!d_ptr->activated)
    {
        d_ptr->engine = new CReporterUploadEngine(&d_ptr->queue);
        d_ptr->activated = true;
        connect(d_ptr->engine, SIGNAL(finished(int, int, int)), SLOT(engineFinished(int, int, int)));
    }

    if (obeyNetworkRestrictions &&
        !CReporterNwSessionMgr::canUseNetworkConnection()) {
        qDebug() << __PRETTY_FUNCTION__
                 << "No unpaid network connection available, aborting crash report upload.";
        QTimer::singleShot(0, this, SLOT(quit()));
        return false;
    }

    foreach (QString filename, fileList)
    {
        if (!d_ptr->addedFiles.contains(filename))
        {
            qDebug() << __PRETTY_FUNCTION__ << "Adding to upload queue: " << filename;
            // CReporterUploadQueue class will own the CReporterUploadItem instance.
            d_ptr->queue.enqueue(new CReporterUploadItem(filename));
            d_ptr->addedFiles << filename;
        }
        else
        {
            qDebug() << __PRETTY_FUNCTION__ << filename << "was not added to queue because it had already been added before";
        }
    }

    if (CReporterPrivacySettingsModel::instance()->notificationsEnabled())
    {
        d_ptr->progressNotification->update(
                //% "Uploading reports"
                qtTrId("crash_reporter-notify-uploading_reports"),
                //% "%1 report(s) to upload"
                qtTrId("crash_reporter-notify-num_to_upload").arg(fileList.count()));
    }

    return true;
}

// ----------------------------------------------------------------------------
// CReporterAutoUploader::quit
// ----------------------------------------------------------------------------
void CReporterAutoUploader::quit()
{
    qDebug() << __PRETTY_FUNCTION__ << "Quit auto uploader.";
    if (d_ptr->engine)
    {
        if (d_ptr->activated)
        {
            qDebug() << __PRETTY_FUNCTION__ << "Engine active -> cancelling";
            d_ptr->activated = false;
            d_ptr->engine->cancelAll();
        }
        qDebug() << __PRETTY_FUNCTION__ << "Deleting engine.";
        disconnect(d_ptr->engine, SIGNAL(finished(int, int, int)), this, SLOT(engineFinished(int, int, int)));
        d_ptr->engine->deleteLater();
        d_ptr->engine = 0;
    }
    qApp->quit();
}

// ----------------------------------------------------------------------------
// CReporterAutoUploader::engineFinished
// ----------------------------------------------------------------------------
void CReporterAutoUploader::engineFinished(int error, int sent, int total)
{
    QString message;

    // Construct message.
    switch (error)
    {
        case CReporterUploadEngine::NoError:
            //% "%1 report(s) uploaded successfully."
            message = qtTrId("qtn_%1_crash_reports_uploaded_successfully_text").arg(total);
            break;
        case CReporterUploadEngine::ProtocolError:
        case CReporterUploadEngine::ConnectionNotAvailable:
        case CReporterUploadEngine::ConnectionClosed:
            //% "Failed to upload report(s)."
            message = qtTrId("qtn_failed_to_send_crash_reports_text");
            //% "<br>Upload results: %1 files attempted %2 files succeeded."
            message += qtTrId("qtn_%1_files_attempted_%2_files_succeeded_text").arg(total).arg(sent);
            break;
        default:
        // We should never enter here.
        break;
    };

    if (error != CReporterUploadEngine::NoError)
    {
        // Add error reason.
        if (error == CReporterUploadEngine::ConnectionNotAvailable)
        {
            //% "<br>Reason: Failed to create internet connection."
            message += qtTrId("qtn_reason_failed_to_create_internet_connection_text");
        }
        else
        {
            QString reason = d_ptr->engine->lastError();
            if (!reason.isEmpty())
            {
                // If reason is available, append it.
                //% "<br>Reason: %1."
                message += qtTrId("qtn_reason_%1_text").arg(reason);
            }
            else
            {
                // Use default error.
                //% "<br>Reason: Unknown."
                message += qtTrId("qtn_reason_unknown_text");
            }
        }
    }

    if (CReporterPrivacySettingsModel::instance()->notificationsEnabled()) {
        d_ptr->progressNotification->remove();

        if (total > sent) {
            int failures = total - sent;
            d_ptr->failedNotification->update(
                    //% "Failed to send all reports"
                    qtTrId("crash_reporter-notify-send_failed"),
                    //% "%1 uploads failed"
                    qtTrId("crash_reporter-notify-num_failed").arg(failures),
                    failures);
        } else {
            d_ptr->failedNotification->remove();
        }

        CReporterSavedState *state = CReporterSavedState::instance();

        if (sent > 0) {
            sent += state->uploadSuccessCount();

            QString summary = (sent == 1) ?
                    //% "Report uploaded"
                    qtTrId("crash_reporter-notify-report_uploaded") :
                    //% "Reports uploaded"
                    qtTrId("crash_reporter-notify-reports_uploaded");
            d_ptr->successNotification->update(summary, QString(), sent);
        }

        state->setUploadSuccessNotificationId(d_ptr->successNotification->id());
        state->setUploadFailedNotificationId(d_ptr->failedNotification->id());
        state->setUploadSuccessCount(sent);
    }

    qDebug() << __PRETTY_FUNCTION__ << "Message: " << message;

    d_ptr->activated = false;
    quit();
}

#include "moc_autouploader_adaptor.cpp"
