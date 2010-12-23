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

#ifndef CREPORTERNOTIFICATIONDIALOG_H
#define CREPORTERNOTIFICATIONDIALOG_H

// System includes.

#include <MDialog>

// Forward declarations.

class CReporterNotificationDialogPrivate;

/*!
  * @class CReporterNotificationDialog
  * @brief Implements dialog for informing user about new crash report.
  *
  * This dialog pops up, when UI is notified about new crash report. User has an option
  * to send all delete crash report. It's also possible to ignore the dialog totally. In this case
  * crash report is not deleted. The dialog has also an option to modify privacy settings.
  */
class CReporterNotificationDialog : public MDialog
{
    Q_OBJECT

    public:
        /*!
          * @brief Class constructor
          *
          * @param details Contains information parsed from rich-core file name.
          * @param server Address, where crash report is sent.
          * @param filesize File size of the rich-core.
          *
          * @sa CReporterUtils::parseCrashInfoFromFilename
          */
        CReporterNotificationDialog(const QStringList &details, const QString &server,
                                     const QString &filesize);

        /*!
          * @brief Class destructor
          *
          */
        virtual ~CReporterNotificationDialog();

        /*!
          * @brief Returns the text written by the user in the text field.
          *
          * @return Text from the MTextField. Empty, fi no comments.
          */
        QString userComments() const;

     Q_SIGNALS:
        /*!
          * @brief Sent, when dialog button is pressed.
          *
          * @param buttonId Unique ID of the button
          */
        void actionPerformed(int buttonId);

    protected:
        /*!
           * @reimp
           */
        virtual void createcontent();

    private: // data
        Q_DECLARE_PRIVATE(CReporterNotificationDialog)
         //! @arg Pointer to private class.
        CReporterNotificationDialogPrivate *d_ptr;
};

#endif // CREPORTERNOTIFICATIONDIALOG_H
