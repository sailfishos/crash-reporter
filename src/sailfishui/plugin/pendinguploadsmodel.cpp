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
#include <QDateTime>

#include "creporterutils.h"

class PendingUploadsModelPrivate {
public:
    struct Item {
        QString applicationName;
        pid_t pid;
        QDateTime dateCreated;
    };

    QMap<QString, Item> contents;
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

    PendingUploadsModelPrivate::Item item = d->contents.values()[index.row()];

    switch (role) {
        case Application:
            return item.applicationName;
        case PID:
            return item.pid;
        case DateCreated:
            return item.dateCreated;
        default:
            return QVariant();
    }
}

QHash<int,QByteArray> PendingUploadsModel::roleNames() const
{
    QHash<int,QByteArray> result;
    result.insert(Application, "application");
    result.insert(PID, "pid");
    result.insert(DateCreated, "dateCreated");

    return result;
}

void PendingUploadsModel::setData(const QStringList &data)
{
    Q_D(PendingUploadsModel);

    beginResetModel();

    d->contents.clear();

    foreach (const QString &filePath, data) {
        QStringList info(CReporterUtils::parseCrashInfoFromFilename(filePath));

        PendingUploadsModelPrivate::Item &item = d->contents[filePath];
        item.applicationName = info[0];
        item.pid = info[3].toInt();
        item.dateCreated = QFileInfo(filePath).created();
    }

    endResetModel();
}

PendingUploadsModel::~PendingUploadsModel() {}
