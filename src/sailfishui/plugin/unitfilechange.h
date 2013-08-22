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

#ifndef UNITFILECHANGE_H
#define UNITFILECHANGE_H

#include <QDBusArgument>


struct UnitFileChange {
    QString type;
    QString fileName;
    QString destination;
};
Q_DECLARE_METATYPE(UnitFileChange)
Q_DECLARE_METATYPE(QList<UnitFileChange>)

QDBusArgument &operator<<(QDBusArgument &argument, const UnitFileChange &change);

const QDBusArgument &operator>>(const QDBusArgument &argument, UnitFileChange &change);

#endif // UNITFILECHANGE_H
