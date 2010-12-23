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

#include <QGraphicsLinearLayout>

#include <MLayout>
#include <MGridLayoutPolicy>
#include <MContainer>
#include <MProgressIndicator>
#include <MLabel>
#include <MButton>
#include <MLocale>

// User includes.

#include "creporteruploaddialog.h"
#include "creporteruploaditem.h"
#include "creporterutils.h"
#include "creporteruploadqueue.h"
#include "creporternamespace.h"

// Local constants.

const QString DialogButtonCancel = "Cancel";

/*!
  * @class  CReporterUploadDialogPrivate
  * @brief Private CReporterUploadDialog class
  *
  */
class CReporterUploadDialogPrivate
{
    public:
       //! @arg Label for displaying file name.
       MLabel *filenameLabel;
       //! @arg Label for displaying file size.
       MLabel *filesizeLabel;
       //! @arg Upload progress bar.
       MProgressIndicator *progressBar;
       //! @arg Label for displaying percentage value of upload progress.
       MLabel *uploadProgressLabel;
       //! @arg Label for uppload status (sent/ total).
       MLabel *uploadStatusLabel;
       //! @arg Total number of files to send.
       int totalNbrOfFiles;
       //! @arg Number of sent files.
       int sentFiles;
       //! @arg Upload queue pointer.
       CReporterUploadQueue *queue;
};

// *** Class CReporterUploadDialog ****

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterUploadDialog::CReporterUploadDialog
// ----------------------------------------------------------------------------
CReporterUploadDialog::CReporterUploadDialog(CReporterUploadQueue *queue,
                                             QGraphicsItem *parent) :
        //% "Crash Reporter"
        MDialog(qtTrId("qtn_upload_dialog_title_text"), M::NoStandardButton),
        d_ptr(new CReporterUploadDialogPrivate())
{
    Q_UNUSED(parent);

    d_ptr->queue =queue;
    connect(d_ptr->queue, SIGNAL(itemAdded(CReporterUploadItem*)),
            this, SLOT(handleItemAdded(CReporterUploadItem*)));
    connect(d_ptr->queue, SIGNAL(nextItem(CReporterUploadItem*)),
            this, SLOT(updateContent(CReporterUploadItem*)));

    d_ptr->sentFiles = 0;
    d_ptr->totalNbrOfFiles = 0;

    // Create widget content.
    createcontent();
}

// ----------------------------------------------------------------------------
// CReporterUploadDialog::~CReporterUploadDialog
// ----------------------------------------------------------------------------
CReporterUploadDialog::~CReporterUploadDialog()
{
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterUploadDialog::createContent
// ----------------------------------------------------------------------------
void CReporterUploadDialog::createcontent()
{
    Q_D(CReporterUploadDialog);

    setObjectName("CrashReporterUploadDialog");

    setProgressIndicatorVisible(true);
    setCloseButtonVisible(false);

    // Get central widget for layouting components.
    QGraphicsWidget *panel = centralWidget();

    // Create widgets.
    d->filenameLabel = new MLabel("", panel);
    d->filenameLabel->setAlignment(Qt::AlignLeft);
    d->filenameLabel->setObjectName("FileNameLabel");

    d->filesizeLabel = new MLabel("", panel);
    d->filesizeLabel->setAlignment(Qt::AlignRight);
    d->filesizeLabel->setObjectName("FileSizeLabel");

    d->progressBar = new MProgressIndicator(panel, "bar");
    d->progressBar->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum));
    d->progressBar->setObjectName("UploadProgressBar");

    d->uploadStatusLabel = new MLabel("", panel);

    d->uploadStatusLabel->setAlignment(Qt::AlignLeft);
    d->uploadStatusLabel->setObjectName("UploadStatusLabel");

    d->uploadProgressLabel = new MLabel("", panel);
    d->uploadProgressLabel->setAlignment(Qt::AlignRight);
    d->uploadProgressLabel->setObjectName("UploadProgressLabel");

    // Create layout and policy.
    MLayout *layout = new MLayout(panel);
    panel->setLayout(layout);
    MGridLayoutPolicy  *policy = new MGridLayoutPolicy(layout);
    policy->setObjectName("DialogMainLayout");

    // Add items to layout.
    policy->addItem(d->filenameLabel, 0, 0, Qt::AlignLeft);
    policy->addItem(d->filesizeLabel, 0, 1, Qt::AlignRight);
    policy->addItem(d->progressBar, 1, 0, 1, 2, Qt::AlignCenter);
    policy->addItem(d->uploadStatusLabel, 2, 0, Qt::AlignLeft);
    policy->addItem(d->uploadProgressLabel, 2, 1, Qt::AlignRight);

    // Add buttons to button area.
    //% "Cancel"
    MButtonModel *button = addButton(qtTrId("qtn_cr_cancel_button_text"));
    button->setObjectName("DialogButton");
    connect(button, SIGNAL(clicked()), this, SLOT(handleClicked()));
}

// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterUploadDialog::updateProgress
// ----------------------------------------------------------------------------
void CReporterUploadDialog::updateProgress(int done)
{
    Q_D(CReporterUploadDialog);

    if (isProgressIndicatorVisible()) {
        setProgressIndicatorVisible(false);
    }

    // Update progress bar and percentage value of the upload status.
    if (done <= d->progressBar->maximum()) {
        d->progressBar->setValue(done);
        MLocale locale;
        QString formattedDone = locale.formatNumber(done);
        d->uploadProgressLabel->setText(QString("%1%").arg(formattedDone));
    }
}

// ----------------------------------------------------------------------------
// CReporterUploadDialog::uploadFinished
// ----------------------------------------------------------------------------
void CReporterUploadDialog::uploadFinished()
{
    Q_D(CReporterUploadDialog);

    CReporterUploadItem *item = qobject_cast<CReporterUploadItem *>(sender());

    if (item->status() != CReporterUploadItem::Finished) {
        return;
    }

    d->sentFiles++;

    MLocale locale;
    //% "Files sent: %1/%2"
    QString labelText = qtTrId("qtn_files_sent_%1_%2_text").arg(locale.formatNumber(d->sentFiles))
                .arg(locale.formatNumber(d->totalNbrOfFiles));
    d->uploadStatusLabel->setText(labelText);
}

// ----------------------------------------------------------------------------
// CReporterUploadDialog::handleClicked
// ----------------------------------------------------------------------------
void CReporterUploadDialog::handleClicked()
{
    // Determine clicked button.
    MButtonModel *model = qobject_cast<MButtonModel *>(sender());
    QString text = model->text();

    if (text == DialogButtonCancel) {
        int id = static_cast<int>(CReporter::CancelButton);
        emit actionPerformed(id);
    }
}

// ----------------------------------------------------------------------------
// CReporterUploadDialog::updateContent
// ----------------------------------------------------------------------------
void CReporterUploadDialog::updateContent(CReporterUploadItem *item)
{
    Q_D(CReporterUploadDialog);

    d->progressBar->reset();
    d->progressBar->setRange(0, 100);
    d->filenameLabel->setText(item->filename());
    d->filesizeLabel->setText(CReporterUtils::fileSizeToString(item->filesize()));

    MLocale locale;
    d->uploadProgressLabel->setText(QString("%1%").arg(locale.formatNumber(0)));

    connect(item, SIGNAL(updateProgress(int)), SLOT(updateProgress(int)));
    connect(item,SIGNAL(uploadFinished()), this, SLOT(uploadFinished()));
}

// ----------------------------------------------------------------------------
// CReporterUploadDialog::handleItemAdded
// ----------------------------------------------------------------------------
void CReporterUploadDialog::handleItemAdded(CReporterUploadItem *item)
{
    Q_D(CReporterUploadDialog);
    Q_UNUSED(item);

    d->totalNbrOfFiles++;

    if (d->totalNbrOfFiles > 1) {
        MLocale locale;

        //% "Files sent: %1/%2"
        QString labelText = qtTrId("qtn_files_sent_%1_%2_text").arg(locale.formatNumber(d->sentFiles))
                    .arg(locale.formatNumber(d->totalNbrOfFiles));

        d->uploadStatusLabel->setText(labelText);
    }
}

// End of file.
