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

// System includes.

#include <QSignalMapper>

#include <MButton>
#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>

// User includes.

#include "creporternamespace.h"
#include "creportersendalldialog.h"

/*!
  * @class CReporterSendAllDialogPrivate
  * @brief Private CReporterSendAllDialo class.
  *
  */
class CReporterSendAllDialogPrivate
{
    public:
        //! @arg List of files to send.
        QStringList files;
        //! @arg Server address.
        QString server;
        //! @arg Total file size..
        QString filesize;
};

// *** Class CReporterSendAllDialog ****

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterSendAllDialog::CReporterSendAllDialog
// ----------------------------------------------------------------------------
CReporterSendAllDialog::CReporterSendAllDialog(const QStringList& files,
                                                          const QString& server, const QString& filesize) :
        //% "Crash Reporter"
        MDialog(qtTrId("qtn_crash_reporter_send_all_dialog_title_text"), M::NoStandardButton),
        d_ptr(new CReporterSendAllDialogPrivate())
{
    Q_D( CReporterSendAllDialog );

    d->files = files;
    d->server = server;
    d->filesize = filesize;

    // Create widgets.
    createcontent();
}

// ----------------------------------------------------------------------------
// CReporterSendAllDialog::~CReporterSendAllDialog
// ----------------------------------------------------------------------------
CReporterSendAllDialog::~CReporterSendAllDialog()
{
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterSendAllDialog::createContent
// ----------------------------------------------------------------------------
void CReporterSendAllDialog::createcontent()
{
    Q_D(CReporterSendAllDialog);

    setObjectName("CrashReporterSendAllDialog");

    // Create content to be placed on central widget.
    QGraphicsWidget *panel = centralWidget();

    // Central widget content.
    QString message;
    QString details;

    //% "Receiving server: %1"
    QString serverDetails = qtTrId("qtn_receiving_server_%1_text").arg(d_ptr->server);

    int nbrOfFiles = d_ptr->files.count();

    if (nbrOfFiles == 1) {
        //% "This system has 1 stored crash report.<br>Send for analysis?"
        message = qtTrId("qtn_this_system_has_store_1_crash_report_text");
        //% "Filesize: %1<br>%2"
        details = qtTrId("qtn_filesize_%1_%2_text").arg(d->filesize).arg(serverDetails);
    }
    else {
        //% "This system has %n stored crash reports.<br>Send for analysis?"
        message = qtTrId("qtn_this_system_has_store_%n_crash_reports_text", nbrOfFiles);
        //% "Total: %1<br>%2"
        details = qtTrId("qtn_total_%1_%2_text").arg(d->filesize).arg(serverDetails);
    }

    MLabel *upperLabel = new MLabel(message, panel);
    upperLabel->setWordWrap(true);
    upperLabel->setObjectName("DialogUpperLabel");

    MLabel *detailsLabel = new MLabel(details, panel);
    detailsLabel->setObjectName("DetailsLabel");

    // Create layout and policy.
    MLayout *layout = new MLayout(panel);
    panel->setLayout(layout);
    MLinearLayoutPolicy  *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    policy->setObjectName("DialogMainLayout");

    // Place items on central widget.
    policy->addItem(upperLabel, Qt::AlignLeft | Qt::AlignTop);
    policy->addItem(detailsLabel, Qt::AlignLeft | Qt::AlignTop);

    // Add buttons to button area.
    QSignalMapper *mapper = new QSignalMapper(this);
    //% "Send"
    MButtonModel *dialogButton = addButton(qtTrId("qtn_cr_send_button"));
    dialogButton->setObjectName("DialogButton");

    connect(dialogButton, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(dialogButton, static_cast<int>(CReporter::SendAllButton));

    //% "Delete all"
    dialogButton = addButton(qtTrId("qtn_delete_all_button"));
    dialogButton->setObjectName("DialogButton");

    connect(dialogButton, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(dialogButton, static_cast<int>(CReporter::DeleteAllButton));

    connect(mapper, SIGNAL(mapped(int)), SIGNAL(actionPerformed(int)));
}

// End of file.
