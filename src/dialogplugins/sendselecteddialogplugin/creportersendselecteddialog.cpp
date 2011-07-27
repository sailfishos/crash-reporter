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

// System includes

#include <MLabel>
#include <MLayout>
#include <MLinearLayoutPolicy>
#include <QModelIndex>
#include <MContentItem>
#include <MAbstractCellCreator>
#include <MList>
#include <QAbstractListModel>
#include <QVariant>
#include <QObject>
#include <MWidgetRecycler>
#include <QSignalMapper>
#include <MButton>

// User includes

#include "creporternamespace.h"
#include "creporterutils.h"
#include "creportersendselecteddialog.h"
#include "creportersendfilelistmodel.h"

// ----------------------------------------------------------------------------
// *** Class MContentItemCreator ***
// ----------------------------------------------------------------------------

/*!
  * @class MContentItemCreator
  * @brief A private list item creator class for use in CReporterSendSelectedDialog.
  *
  * Creates MContentItem objects for MList with data from CReporterSendFileListModel
  *
  */
class MContentItemCreator : public MAbstractCellCreator<MContentItem>
{
public:
    /*!
      * @brief Updates contents of an MContentItem with data at given index. Used by MList.
      *
      * @param index Index to a cell in CReporterSendFileListModel.
      * @param cell MContentItem to which the data should be updated
      */
    void updateCell(const QModelIndex& index, MWidget *cell) const
    {
        MContentItem *contentItem = qobject_cast<MContentItem *>(cell);
        QVariant data = index.data(Qt::DisplayRole);
        QStringList rowData = data.value<QStringList>();
        contentItem->setTitle(rowData[0]);
        contentItem->setSubtitle(rowData[1]);
    }

    /*!
     * @brief Gives a new or recycled MContentItem cell for MList.
     *
     * @param index Model index of the new cell
     * @param recycler Widget recycler for cell widgets
     */
    MWidget* createCell(const QModelIndex& index, MWidgetRecycler& recycler) const
    {
        MWidget *newItem = qobject_cast<MWidget *>(recycler.take(MWidget::staticMetaObject.className()));
        if(newItem == NULL)
            newItem = qobject_cast<MWidget*>(new MContentItem(MContentItem::TwoTextLabels));
        updateCell(index, newItem);
        return qobject_cast<MWidget*>(newItem);
    }
};

// ----------------------------------------------------------------------------
// *** Class CReporterSendSelectedDialogPrivate ***
// ----------------------------------------------------------------------------

/*!
  * @class CReporterSendSelectedDialogPrivate
  * @brief Private CReporterSendSelectedDialog class.
  *
  */
class CReporterSendSelectedDialogPrivate
{
    public:
        //! @arg List of files to send.
        QStringList files;
        //! @arg Server address.
        QString server;

        //! @arg List widget for rich-core files
        MList *list;
        //! @arg Cell creator to be used by MList
        MContentItemCreator *cellCreator;
        //! @arg Model for file list data
        CReporterSendFileListModel *model;
};

// *** Class CReporterSendSelectedDialog ***
// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterSendSelectedDialog::CReporterSendSelectedDialog
// ----------------------------------------------------------------------------
CReporterSendSelectedDialog::CReporterSendSelectedDialog(const QStringList& files,
                                                         const QString& server) :
    //% "Crash Reporter"
    MDialog(qtTrId("qtn_crash_reporter_send_selected_dialog_title_text"), M::NoStandardButton),
                d_ptr(new CReporterSendSelectedDialogPrivate())
{
    Q_D( CReporterSendSelectedDialog );

    d->files = files;
    d->server = server;

    // Create widgets.
    createcontent();
}

// ----------------------------------------------------------------------------
// CReporterSendSelectedDialog::~CReporterSendSelectedDialog
// ----------------------------------------------------------------------------
CReporterSendSelectedDialog::~CReporterSendSelectedDialog()
{
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterSendSelectedDialog::getSelectedFiles
// ----------------------------------------------------------------------------
QStringList CReporterSendSelectedDialog::getSelectedFiles()
{
    Q_D(CReporterSendSelectedDialog);

    QStringList selectedFiles;
    QStringList rowData;
    for (int i = 0; i < d->model->rowCount(); i++)
    {
        if (d->list->selectionModel()->isRowSelected(i,QModelIndex()))
        {
            QVariant data = d->model->index(i).data(Qt::DisplayRole);
            rowData = data.value<QStringList>();
            selectedFiles << rowData.at(2);
        }
    }
    return selectedFiles;
}

// ----------------------------------------------------------------------------
// CReporterSendSelectedDialog::createcontent
// ----------------------------------------------------------------------------
void CReporterSendSelectedDialog::createcontent()
{
    Q_D(CReporterSendSelectedDialog);
    setObjectName("CrashReporterSendSelectedDialog");

    int nbrOfFiles = d_ptr->files.count();
    QString message;

    if (nbrOfFiles == 1) {
        //% "<p>This system has 1 stored crash report. Select reports to send to %1 for analysis or to delete.</p>"
        message = qtTrId("qtn_system_has_1_crash_report_send_to_%s").arg(d->server);

    }
    else {
        //% "<p>This system has %1 stored crash reports. Select reports to send to %2 for analysis or to delete.</p>"
        message = qtTrId("qtn_system_has_%1_crash_reports_send_to_%2").arg(nbrOfFiles).arg(d->server);

    }

    // Create content to be placed on central widget.
    QGraphicsWidget *panel = centralWidget();

    // Create layout and policy.
    MLayout *layout = new MLayout(panel);
    layout->setContentsMargins(0,0,0,0);
    panel->setLayout(layout);
    MLinearLayoutPolicy  *policy = new MLinearLayoutPolicy(layout, Qt::Vertical);
    policy->setObjectName("DialogMainLayout");

    // Create message label and hack it to support wordwrapping
    MLabel *messagelabel = new MLabel(message, panel);
    messagelabel->setWordWrap(true);
    messagelabel->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    messagelabel->setObjectName("DialogMessageLabel");
    messagelabel->setStyleName("CommonFieldLabelInverted");

    d->list = new MList(panel);
    d->cellCreator = new MContentItemCreator;

    d->list->setCellCreator(d->cellCreator);
    d->model = new CReporterSendFileListModel(d->files);
    d->list->setItemModel(d->model);
    d->list->setSelectionMode(MList::MultiSelection);

    // Add widgets to the layout
    policy->addItem(messagelabel, Qt::AlignLeft | Qt::AlignTop);
    policy->addItem(d->list, Qt::AlignLeft | Qt::AlignTop);

    // Add buttons to button area.
    QSignalMapper *mapper = new QSignalMapper(this);
    //% "Send Selected"
    MButton* dialogButton = new MButton(qtTrId("qtn_send_selected_button"));
    dialogButton->setStyleName("CommonSingleButtonInverted");
    policy->addItem(dialogButton, Qt::AlignCenter);
//    MButtonModel *dialogButton = addButton(qtTrId("qtn_send_selected_button"), M::ActionRole);
    connect(dialogButton, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(dialogButton, static_cast<int>(CReporter::SendSelectedButton));

    //% "Send All"
    dialogButton = new MButton(qtTrId("qtn_send_all_button"));
    dialogButton->setStyleName("CommonSingleButtonInverted");
    policy->addItem(dialogButton, Qt::AlignCenter);
//    dialogButton = addButton(qtTrId("qtn_send_all_button"), M::ActionRole);
    connect(dialogButton, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(dialogButton, static_cast<int>(CReporter::SendAllButton));

    //% "Delete Selected"
    dialogButton = new MButton(qtTrId("qtn_delete_selected_button"));
    dialogButton->setStyleName("CommonSingleButtonInverted");
    policy->addItem(dialogButton, Qt::AlignCenter);
//    dialogButton = addButton(qtTrId("qtn_delete_selected_button"), M::DestructiveRole);
    connect(dialogButton, SIGNAL(clicked()), mapper, SLOT(map()));
    mapper->setMapping(dialogButton, static_cast<int>(CReporter::DeleteSelectedButton));

    connect(mapper, SIGNAL(mapped(int)), SIGNAL(actionPerformed(int)));
    connect(mapper, SIGNAL(mapped(int)), SLOT(accept()));
}

// End of file.
