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

#ifndef CREPORTERSENDALLDIALOG_H
#define CREPORTERSENDALLDIALOG_H

// System includes.

#include <MDialog>

// Forward declarations.

class CReporterSendAllDialogPrivate;

/*!
  * @class CReporterSendAllDialog
  * @brief This class implements dialog for sending all crash reports found in the system.
  *
  * This dialog pops up right after device boot up, if there are unsent crash reports.
  * User option to ignore the dialog, when nothing is done or to send or delete all reports.
  *
  */
class CReporterSendAllDialog : public MDialog
{
    Q_OBJECT

    public:
        /*!
          * @brief Class constructor
          *
          * @param files List of rich-core files.
          * @param server Server address.
          * @param filesize Total file size.
          */
        CReporterSendAllDialog(const QStringList& files, const QString& server,
                                     const QString& filesize);

        /*!
          * @brief Class destructor.
          *
          */
        virtual ~CReporterSendAllDialog();

     Q_SIGNALS:
        /*!
          * @brief Sent, when user interacts with the dialog.
          *
          * @param buttonId Unique identifier of the button.
          */
        void actionPerformed(int buttonId);

    protected:
        //! @reimp
        virtual void createcontent();

    private: // data
        Q_DECLARE_PRIVATE( CReporterSendAllDialog )
        //! @arg Pointer to private data class.
        CReporterSendAllDialogPrivate *d_ptr;
};

#endif // CREPORTERSENDALLDIALOG_H
