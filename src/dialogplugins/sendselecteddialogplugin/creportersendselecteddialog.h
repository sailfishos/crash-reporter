/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Raimo Gratseff <ext-raimo.gratseff@nokia.com>
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

#ifndef CREPORTERSENDSELECTEDDIALOG_H
#define CREPORTERSENDSELECTEDDIALOG_H

// System includes

#include <MDialog>
#include <QStringList>

// Forward declarations

class CReporterSendSelectedDialogPrivate;

/*!
  * @class CReporterSendSelectedDialog
  * @brief This class implements dialog for selecting crash reports to send or delete.
  *
  * This dialog pops up right after device boot up, if there are unsent crash reports, or
  * when selecting "Send or Delete Reports" from the settings applet.
  *
  */
class CReporterSendSelectedDialog : public MDialog
{
    Q_OBJECT

public:

    /*!
      * @brief Class constructor
      *
      * @param files List of rich-core files.
      * @param server Server address.
      */
    CReporterSendSelectedDialog(const QStringList& files, const QString& server);

    /*!
      * @brief Class destructor.
      *
      */
    virtual ~CReporterSendSelectedDialog();

    QStringList getSelectedFiles();

Q_SIGNALS:

    /*!
      * @brief Sent, when user interacts with the dialog.
      *
      * @param buttonId Unique identifier of the button.
      */
    void actionPerformed(int buttonId);

    /*!
      * @brief Sent, when user interacts with the dialog.
      *
      * @param selectedFiles List of files that were selected.
      */
    void filesSelected(QStringList selectedFiles);

protected:
    //! @reimp
    virtual void createcontent();

private:

    Q_DECLARE_PRIVATE( CReporterSendSelectedDialog )
    //! @arg Pointer to private data class.
    CReporterSendSelectedDialogPrivate *d_ptr;
};

#endif // CREPORTERSENDSELECTEDDIALOG_H
