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

#include <QSettings>

#include "creportersettingsinit_p.h"

void creporterSettingsInit(const QString &systemPath, const QString &userPath)
{
    static bool initialized = false;

    if (!initialized) {
        // Set default paths for storing settings.
        if (!systemPath.isNull()) {
            QSettings::setPath(QSettings::NativeFormat, QSettings::SystemScope, systemPath);
        }
        if (!userPath.isNull()) {
            QSettings::setPath(QSettings::NativeFormat, QSettings::UserScope, userPath);
        }
        initialized = true;
    }
}
