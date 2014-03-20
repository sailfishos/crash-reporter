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

class PendingUploadsModelPrivate {
public:
    QMap<QString, int> contents;
};

PendingUploadsModel::PendingUploadsModel(QObject *parent):
  QAbstractListModel(parent), d_ptr(new PendingUploadsModelPrivate) {}

int PendingUploadsModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const PendingUploadsModel);

    Q_UNUSED(parent);

    return d->contents.size();
}

QVariant PendingUploadsModel::data(const QModelIndex &index, int role) const
{
    Q_D(const PendingUploadsModel);

    Q_ASSERT(index.row() < d->contents.size());

    switch (role) {
        case Application:
            return d->contents.keys()[index.row()];
        case ReportCount:
            return d->contents.values()[index.row()];
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
    Q_D(PendingUploadsModel);

    beginResetModel();

    d->contents.clear();

    foreach (const QString &filePath, data) {
        QString filename =
                CReporterUtils::parseCrashInfoFromFilename(filePath)[0];
        if (d->contents.contains(filename)) {
            int count = d->contents[filename];
            d->contents[filename] = ++count;
        } else {
            d->contents[filename] = 1;
        }
    }

    endResetModel();
}

PendingUploadsModel::~PendingUploadsModel() {}
