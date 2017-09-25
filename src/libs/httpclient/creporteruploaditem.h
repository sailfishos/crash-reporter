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

#ifndef CREPORTERUPLOADITEM_H
#define CREPORTERUPLOADITEM_H

// System includes.

#include <QObject>

// System includes.

#include "creporterexport.h"

// Forward declarations.

class CReporterUploadItemPrivate;

/*!
  * @class CReporterUploadItem
  * @brief The class representing one file in the upload queue.
  *
  */
class CREPORTER_EXPORT CReporterUploadItem : public QObject
{
    Q_OBJECT

public:

    /*!
     * @enum ItemStatus
     * @brief Values for upload item status.
     */
    typedef enum ItemStatus {
        //! Item is waiting.
        Waiting = 0,
        //! Item is being uploaded.
        Sending,
        //! Sending was attempted, but failed due to error.
        Error,
        //! Item was processed successfully.
        Finished,
        //! Item was cancelled.
        Cancelled,
    } ItemStatus;

    /*!
     * @brief Class constructor.
     *
     * @param file Path to file.
     */
    CReporterUploadItem(const QString &file);

    /*!
     * @brief Class destructor.
     *
     */
    virtual ~CReporterUploadItem();

    /*!
     * @brief Returns size of the file to upload.
     *
     * @return File size in bytes.
     */
    qint64 filesize() const;

    /*!
     * @brief Returns path to file.
     *
     * @return File path.
     */
    QString filename() const;

    /*!
     * @brief Marks item as done. Causes to emit done().
     *
     * @sa done()
     */
    void markDone();

    /*!
     * @brief Returns item status.
     *
     * @return Item status.
     */
    CReporterUploadItem::ItemStatus status() const;

    /*!
     * @brief Returns status in string.
     *
     * @return Status in human readable format.
     */
    QString statusString() const;

    /*!
     * @brief Returns error string, if item status is @a error.
     *
     * @return Error string.
     */
    QString errorString() const;

public Q_SLOTS:
    /*!
     * @brief Starts uploading to remote server.
     *
     * @return True, if HTTP request was sent successfully; otherwise false.
     */
    bool startUpload();

    /*!
     * @brief Cancels upload.
     *
     */
    void cancel();

Q_SIGNALS:
    /*!
     * @brief Sent after markDone() is called for item.
     *
     */
    void done();

    /*!
     * @brief Sent to indicate upload progress.
     *
     * @param done Percentage value marking progress.
     */
    void updateProgress(int done);

    /*!
     * @brief Sent, when upload has finished.
     *
     */
    void uploadFinished();

private Q_SLOTS:
    /*!
     * @brief Emits uploadFinished().
     *
     */
    void emitUploadFinished();

    /*!
     * @brief Called, when uploading fails due to an error.
     *
     * @param file Path to file of which uploading failed.
     * @param errorString HTTP error string.
     */
    void uploadError(const QString &file, const QString &errorString);

protected:
    /*!
     * @brief Sets item status.
     *
     * @param status Next status.
     */
    void setStatus(ItemStatus status);

    /*!
     * @brief Sets error string for item.
     *
     * @param errorString Item error string.
     */
    void setErrorString(const QString &errorString);

private:
    Q_DECLARE_PRIVATE(CReporterUploadItem);
    //! @arg Pointer to private class.
    CReporterUploadItemPrivate *d_ptr;
#ifdef CREPORTER_UNIT_TEST
    friend class Ut_CReporterUploadItem;
#endif // CREPORTER_UNIT_TEST
};

#endif // CREPORTERUPLOADITEM_H

// End of file.
