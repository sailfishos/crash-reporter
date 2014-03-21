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

#ifndef PENDINGUPLOADSMODEL_H
#define PENDINGUPLOADSMODEL_H

#include <QAbstractListModel>

class PendingUploadsModelPrivate;

class PendingUploadsModel: public QAbstractListModel {
    Q_OBJECT
public:
    PendingUploadsModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;

    enum DataRole {
        Application = Qt::UserRole + 1,
        PID,
        DateCreated,
    };

    QVariant data(const QModelIndex &index, int role) const;
    QHash<int,QByteArray> roleNames() const;

    void setData(const QStringList &data);

    ~PendingUploadsModel();
private:
    Q_DECLARE_PRIVATE(PendingUploadsModel);
    QScopedPointer<PendingUploadsModelPrivate> d_ptr;
};

#endif // PENDINGUPLOADSMODEL_H
