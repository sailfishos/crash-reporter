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

#ifndef CREPORTERUPLOADDIALOG_H
#define CREPORTERUPLOADDIALOG_H

// System includes.

#include <MDialog>

// Forward declarations.

class CReporterUploadDialogPrivate;
class CReporterUploadItem;
class CReporterUploadQueue;

/*!
  * @class CReporterUploadDialog
  * @brief This class implements dialog for displaying upload progress.
  *
  */
class CReporterUploadDialog : public MDialog
{
    Q_OBJECT

    public:
        /*!
          * @brief Class constructor.
          *
          * @param CReporterUploapQueue Queue reference.
          * @param parent Reference to parent object.
          */
        CReporterUploadDialog(CReporterUploadQueue *queue, QGraphicsItem *parent = 0);

        /*!
          * @brief Class destructor
          *
          */
        virtual ~CReporterUploadDialog();

     Q_SIGNALS:
        /*!
          * @brief Sent, when user interacts with the dialog.
          *
          * @param buttonId Unique ID to identify button.
          */
        void actionPerformed(int buttonId);

    protected:        
        //! @reimp
        virtual void createcontent();

    private Q_SLOTS:
        /*!
          * @brief Called to update upload progress.
          *
          * @param done Percentage value of total progress.
          */
        void updateProgress(int done);

        /*!
          * @brief Called when upload is done.
          *
          */
        void uploadFinished();

        /*!
          * @brief Handles button click events.
          *
          */
        void handleClicked();

        /*!
          * @brief Called to update content in dialog.
          *
          * @param item Pointer to new item (crash report) to upload.
          *
          * @sa CReporterUploadItem
          */
        void updateContent(CReporterUploadItem *item);

        void handleItemAdded(CReporterUploadItem*);

    private:
        Q_DECLARE_PRIVATE(CReporterUploadDialog)
        //! @arg Pointer to private class implementation.
        CReporterUploadDialogPrivate *d_ptr;
};

#endif // CREPORTERUPLOADDIALOG_H
