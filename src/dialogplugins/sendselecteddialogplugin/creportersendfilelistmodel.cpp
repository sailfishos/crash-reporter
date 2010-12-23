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

#include <QStringList>
#include <QVariant>
#include <QModelIndex>
#include <QFileInfo>
#include <QDateTime>
#include <MLocale>

// User includes

#include "creporterutils.h"
#include "creportersendfilelistmodel.h"

// ----------------------------------------------------------------------------
// CReporterSendFileListModel::CReporterSendFileListModel
// ----------------------------------------------------------------------------
CReporterSendFileListModel::CReporterSendFileListModel(const QStringList& filenames, QObject *parent)
    : QAbstractListModel(parent)//, contents()
{
    MLocale locale;
    QFileInfo fileinfo;

    for (int i = 0; i < filenames.size(); i++)
    {
        QStringList rowData;
        fileinfo.setFile(filenames.at(i));
        rowData << fileinfo.baseName();
        rowData << CReporterUtils::fileSizeToString(fileinfo.size()) + ' '
                + locale.formatDateTime(fileinfo.created(), MLocale::DateMedium, MLocale::TimeMedium);
        rowData << filenames.at(i);
        contents.push_back(rowData);
    }

}

// ----------------------------------------------------------------------------
// CReporterSendFileListModel::rowCount
// ----------------------------------------------------------------------------
int CReporterSendFileListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return contents.size();
}

// ----------------------------------------------------------------------------
// CReporterSendFileListModel::data
// ----------------------------------------------------------------------------
QVariant CReporterSendFileListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole) {
        return QVariant(contents.at(index.internalId()));
    }
    return QVariant();
}

// ----------------------------------------------------------------------------
// CReporterSendFileListModel::index
// ----------------------------------------------------------------------------
QModelIndex CReporterSendFileListModel::index(int row, int column, const QModelIndex& parent) const
{
    Q_UNUSED(column);
    Q_UNUSED(parent);
    return createIndex(row,0,row);
}

