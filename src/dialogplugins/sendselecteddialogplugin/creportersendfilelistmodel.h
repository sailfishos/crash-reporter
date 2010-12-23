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

#ifndef CREPORTERSENDFILELISTMODEL_H
#define CREPORTERSENDFILELISTMODEL_H

// System includes.

#include <QAbstractListModel>
#include <QStringList>
#include <QList>

/*!
  * @class CReporterSendFileListModel
  * @brief File list model for CReporterSendSelectedDialog
  *
  * Data inside the model is QStringLists with 3 strings in each. The values inside them are
  * [0] the base name of a file, [1] file size + date and time of creation and [2] full filename
  * with path.
  *
  */
class CReporterSendFileListModel : public QAbstractListModel
{
    Q_OBJECT
public:

    /*!
      * @brief Class constructor
      *
      * @param filenames List of rich-core files.
      * @param parent QObject parent
      */
    CReporterSendFileListModel(const QStringList& filenames, QObject *parent = 0);

    /*!
      * @brief Count number of rows in the model
      *
      * @param parent Not used
      * @return Number of rows in the model
      */
    int rowCount(const QModelIndex &parent = QModelIndex()) const;

    /*!
      * @brief Read data from the model at the given index
      *
      * @param index Model index for the data to be read.
      * @param role Only Qt::DisplayRole is supported.
      * @return QStringList with the data from the given index
      */
    QVariant data(const QModelIndex &index, int role) const;

    /*!
      * @brief Creates a model index for the give row in the model
      *
      * @param row Row number
      * @param column Not used
      * @param parent Not used
      * @return Model index for the given row
      */
    QModelIndex index(int row, int column = 0, const QModelIndex &parent = QModelIndex()) const;

private:
    //! @arg Private data structure.
    QList<QStringList> contents;
};

#endif // CREPORTERSENDFILELISTMODEL_H
