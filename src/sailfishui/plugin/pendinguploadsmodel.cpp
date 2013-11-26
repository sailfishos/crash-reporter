/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Jakub Adam <jakub.adam@jollamobile.com>
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
 */

#include "pendinguploadsmodel.h"

#include <QStringList>

#include "creporterutils.h"

PendingUploadsModel::PendingUploadsModel(QObject *parent):
  QAbstractListModel(parent) {}

int PendingUploadsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return contents.size();
}

QVariant PendingUploadsModel::data(const QModelIndex &index, int role) const
{
    Q_ASSERT(index.row() < contents.size());

    switch (role) {
        case Application:
            return contents.keys()[index.row()];
        case ReportCount:
            return contents.values()[index.row()];
        default:
            return QVariant();
    }
}

QHash<int,QByteArray> PendingUploadsModel::roleNames() const
{
    QHash<int,QByteArray> result;
    result.insert(Application, "application");
    result.insert(ReportCount, "reportCount");

    return result;
}

void PendingUploadsModel::setData(const QStringList &data)
{
    beginResetModel();

    contents.clear();

    foreach (const QString &filePath, data) {
        QString filename =
                CReporterUtils::parseCrashInfoFromFilename(filePath)[0];
        if (contents.contains(filename)) {
            int count = contents[filename];
            contents[filename] = ++count;
        } else {
            contents[filename] = 1;
        }
    }

    endResetModel();
}
