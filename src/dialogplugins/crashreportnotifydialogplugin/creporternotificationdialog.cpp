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
#include <QGraphicsGridLayout>

#include <MTextEdit>
#include <MButton>
#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>

// User includes.

#include "creporternotificationdialog.h"
#include "creporternamespace.h"

/*!
  * @class CReporterNotificationDialogPrivate
  * @brief Private CReporterNotificationDialog class.
  *
  */
class CReporterNotificationDialogPrivate
{
    public:
        //! @arg Crash report details from rich-core file name.
        QStringList details;
        //! @arg Server address.
        QString server;
        //! @arg Rich-core filesize.
        QString filesize;
        //! @arg Textfield widget.
        MTextEdit *commentField;
        //! @arg Field for contact information
        MTextEdit *contactField;
        //! @arg Check for including contact info
        MButton* contactCheck;
};

// *** Class CReporterNotificationDialog ****

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::CReporterNotificationDialog
// ----------------------------------------------------------------------------
CReporterNotificationDialog::CReporterNotificationDialog(const QStringList &details,
                                                          const QString &server, const QString& filesize) :
        MDialog("", M::NoStandardButton),
        d_ptr(new CReporterNotificationDialogPrivate())
{
    Q_D(CReporterNotificationDialog);

    d->details = details;
    d->server = server;
    d->filesize = filesize;
    d->commentField = 0;
    d->contactField = 0;
    d->contactCheck = 0;

    // Create widgets.
    createcontent();
}

// ----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::~CReporterNotificationDialog
// ----------------------------------------------------------------------------
CReporterNotificationDialog::~CReporterNotificationDialog()
{
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterNotificationDialog::userComments
// ----------------------------------------------------------------------------
QString CReporterNotificationDialog::userComments() const
{
    Q_D(const CReporterNotificationDialog);
    if (d->contactCheck && d->contactCheck->isChecked() && !d->contactField->text().isEmpty())
        return "COMMENT: " + d->commentField->text() + "\nCONTACT_INFO: " + d->contactField->text();
    if (d->commentField)
        return d->commentField->text();
    else
        return QString();
}

// ----------------------------------------------------------------------------
// CReporterNotificationDialog::setContactInfo
// ----------------------------------------------------------------------------
void CReporterNotificationDialog::setContactInfo(QString contactInfo)
{
    Q_D(CReporterNotificationDialog);
    if (d->contactField)
        d->contactField->setText(contactInfo);
}

// ----------------------------------------------------------------------------
// CReporterNotificationDialog::setContactInfo
// ----------------------------------------------------------------------------
QString CReporterNotificationDialog::getContactInfo() const
{
    Q_D(const CReporterNotificationDialog);
    if (d->contactCheck && d->contactCheck->isChecked())
        return d->contactField->text();
    else
        return QString();
}

// ----------------------------------------------------------------------------
// CReporterNotifyDialogPlugin::createContent
// ----------------------------------------------------------------------------
void CReporterNotificationDialog::createcontent()
{
    Q_D(CReporterNotificationDialog);

    setObjectName("CrashReporterNotificationDialog");

    QString title;
    if (d->details.at(0) == CReporter::LifelogPackagePrefix)
    {   //% "New lifelog report"
        title = qtTrId("qtn_new_lifelog_report_text");
    }
    else
    {   //% "%1 has crashed"
        title = qtTrId("qtn_the_application_%1_crashed_text").arg(d->details.at(0));
    }

    // Set title.
    setTitle(title);

    // Create widgets to be placed on central widget.
    QGraphicsWidget *panel = centralWidget();

    // Create layout and policy.
    MLayout *layout = new MLayout(panel);
    panel->setLayout(layout);
    MLinearLayoutPolicy  *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    policy->setObjectName("DialogMainLayout");
    layout->setContentsMargins(0,0,0,0);

    QString message;
    if (d->details.at(0) == CReporter::LifelogPackagePrefix)
    {   //% "New Crash Reporter Lifelog report is ready to be sent. Lifelogs provide information on crashes, "
        //% "battery status, memory usage etc. and are vital for device statistics. Sending them regularly "
        //% "is highly suggested but their generation can be disabled in Crash Reporter settings."
        message = qtTrId("qtn_notify_lifelog_body_text");
    }
    else
    {   //% "Please help improve the software by filling in and sending this report. "
        //% "Please describe what was happening and/or what you were doing when the crash occurred. "
        message = qtTrId("qtn_notify_dialog_header_text");
    }

    MLabel *upperLabel = new MLabel(message, panel);
    upperLabel->setWordWrap(true);
    upperLabel->setObjectName("DialogUpperLabel");
    upperLabel->setStyleName("CommonFieldLabelInverted");
    policy->addItem(upperLabel, Qt::AlignLeft | Qt::AlignTop);

    // Don't add comments or details for lifelogs
    if (d->details.at(0) != CReporter::LifelogPackagePrefix)
    {
        d->commentField = new MTextEdit(MTextEditModel::MultiLine, "", panel);
        d->commentField->setObjectName("DialogCommentField");

        MLayout* contactLayout = new MLayout;
        MLinearLayoutPolicy* contactPolicy = new MLinearLayoutPolicy(contactLayout, Qt::Horizontal);
        d->contactCheck = new MButton;
        d->contactCheck->setViewType(MButton::checkboxType);
        d->contactCheck->setCheckable(true);
        d->contactCheck->setChecked(false);
        MLabel* contactLabel = new MLabel("Contact information:");
        contactLabel->setObjectName("ContactLabel");
        contactLabel->setStyleName("CommonFieldLabelInverted");
        d->contactField = new MTextEdit(MTextEditModel::SingleLine, "");
        contactPolicy->addItem(d->contactCheck);
        contactPolicy->addItem(d->contactField, Qt::AlignVCenter);

        //% "File size: %1, Receiving server: %2. "
        QString details = qtTrId("qtn_receiving_server_and_filesize_text").arg(d->filesize).arg(d->server);

        //% "The following technical information will be included in the report: PID: %1, Signal: %2"
        details += qtTrId("qtn_technical_infomation_included_text")
                            .arg(d->details.at(3)).arg(d->details.at(2));

        MLabel *crashDetailsLabel = new MLabel(details, panel);
        crashDetailsLabel->setObjectName("CrashDetailsLabel");
        crashDetailsLabel->setStyleName("CommonItemInfoInverted");
        crashDetailsLabel->setWordWrap(true);

        connect(d->contactField, SIGNAL(textChanged()), SLOT(setContactChecked()));

        policy->addItem(d->commentField);
        policy->addItem(contactLabel);
        policy->addItem(contactLayout);
        policy->addItem(crashDetailsLabel);
    }

    QSignalMapper *mapper = new QSignalMapper(this);

    //% "Open Settings"
    MButton *optionsButton = new MButton(qtTrId("qtn_cr_options_button"), panel);
    optionsButton->setObjectName("DialogButton");
    optionsButton->setPreferredWidth(140);
    optionsButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    optionsButton->setStyleName("CommonSingleButtonInverted");
    policy->addItem(optionsButton, Qt::AlignCenter);

    connect(optionsButton, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(optionsButton, static_cast<int>(CReporter::OptionsButton));

    // Add buttons to button area.
    //% "Send"
    MButton* dialogButton = new MButton(qtTrId("qtn_cr_button_send"));
    dialogButton->setStyleName("CommonSingleButtonInverted");
    policy->addItem(dialogButton, Qt::AlignCenter);
//    MButtonModel *dialogButton = addButton(qtTrId("qtn_cr_button_send"));
    dialogButton->setObjectName("DialogButton");

    connect(dialogButton, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(dialogButton, SIGNAL(clicked()), this, SLOT(accept()));
    mapper->setMapping(dialogButton, static_cast<int>(CReporter::SendButton));

    //% "Save"
    dialogButton = new MButton(qtTrId("qtn_cr_button_save"));
    dialogButton->setStyleName("CommonSingleButtonInverted");
    policy->addItem(dialogButton, Qt::AlignCenter);
//    dialogButton = addButton(qtTrId("qtn_cr_button_save"));
    dialogButton->setObjectName("DialogButton");

    connect(dialogButton, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(dialogButton, SIGNAL(clicked()), this, SLOT(accept()));
    mapper->setMapping(dialogButton, static_cast<int>(CReporter::SaveButton));

    //% "Delete"
    dialogButton = new MButton(qtTrId("qtn_cr_button_delete"));
    dialogButton->setStyleName("CommonSingleButtonInverted");
    policy->addItem(dialogButton, Qt::AlignCenter);
//    dialogButton = addButton(qtTrId("qtn_cr_button_delete"));
    dialogButton->setObjectName("DialogButton");

    connect(dialogButton, SIGNAL(clicked()), mapper, SLOT(map()));
    connect(dialogButton, SIGNAL(clicked()), this, SLOT(accept()));
    mapper->setMapping(dialogButton, static_cast<int>(CReporter::DeleteButton));

    connect(mapper, SIGNAL(mapped(int)), SIGNAL(actionPerformed(int)));
}

// ----------------------------------------------------------------------------
// CReporterNotificationDialog::setContactChecked
// ----------------------------------------------------------------------------

void CReporterNotificationDialog::setContactChecked()
{
    Q_D(CReporterNotificationDialog);
    d->contactCheck->setChecked(true);
}

// End of file.
