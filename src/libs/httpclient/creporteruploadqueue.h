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

#ifndef CREPORTERUPLOADQUEUE_H
#define CREPORTERUPLOADQUEUE_H

// System includes.

#include <QObject>

// User includes.

#include "creporterexport.h"

// Forward declarations.

class CReporterUploadQueuePrivate;
class CReporterUploadItem;

/*!
  * @class CReporterUploadQueue
  * @brief Maintains a linked list of the files to be uploaded.
  *
  */
class CREPORTER_EXPORT CReporterUploadQueue : public QObject
{

    Q_OBJECT

    public:
        /*!
         * @brief Class constructor.
         *
         * @param parent Owner of this class instance.
         */
        CReporterUploadQueue(QObject *parent=0);

        /*!
         * @brief Class destructor.
         *
         */
        ~CReporterUploadQueue();

        void enqueue(CReporterUploadItem *item);
        CReporterUploadItem* dequeue() const;

        /*!
         * @brief Returns number of items to be uploaded.
         *
         * @note Return value may not be the same as number of item currently
         *  in queue. This function return number of items queued count from the first call
         *  to enqueue() until done() signal.
         */
        int totalNumberOfItems() const;

        /*!
         * @brief Clears upload queue for items.
         *
         */
        void clear();

     Q_SIGNALS:

        /*!
         * @brief Sent, when all items in the queue are handled.
         *
         */
        void done();

        /*!
         * @brief Sent, when @a item is taken from queue.
         * 
         * @param item Pointer to next item to handle.
         */
        void nextItem(CReporterUploadItem *item);

        /*!
         * @brief Sent, when @a item is added to queue queue.
         * 
         * @param item Pointer to next item to handle.
         */
        void itemAdded(CReporterUploadItem *item);

     protected Q_SLOTS:
        /*!
         * @brief Called, when processing item is finished.
         *
         */
        void itemFinished();

     protected:
         /*!
          * @brief Emits nextItem(CReporterUploadItem *item).
          *
          */
         void emitNextItem();

     private:
        Q_DECLARE_PRIVATE(CReporterUploadQueue)
        //! @arg Pointer to private class.
        CReporterUploadQueuePrivate *d_ptr;
#ifdef CREPORTER_UNIT_TEST
        friend class Ut_CReporterUploadQueue;
#endif // CREPORTER_UNIT_TEST
};

#endif // CREPORTERUPLOADQUEUE_H

// End of file.
