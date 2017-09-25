/*
 * This file is a part of crash-reporter.
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

#ifndef POWEREXCESSWATCH_H
#define POWEREXCESSWATCH_H

#include <QObject>

class PowerExcessHandlerPrivate;

class PowerExcessHandler: public QObject
{
    Q_OBJECT

public:
    PowerExcessHandler(QObject *parent = 0);
    ~PowerExcessHandler();

private:
    Q_DISABLE_COPY(PowerExcessHandler)
    Q_DECLARE_PRIVATE(PowerExcessHandler)
    QScopedPointer<PowerExcessHandlerPrivate> d_ptr;

    Q_PRIVATE_SLOT(d_func(), void handleUdevNotification())
};

#endif // POWEREXCESSWATCH_H
