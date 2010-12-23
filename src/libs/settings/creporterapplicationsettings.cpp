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

// System includes.

#include <QSettings>

// User includes.

#include "creporterapplicationsettings.h"
#include "creportersettingsinit_p.h"
#include "creporternamespace.h"

// Pointer to this class.
CReporterApplicationSettings* CReporterApplicationSettings::sm_Instance = 0;

/*!
  * @class CReporterApplicationSettingsPrivate
  * @brief Private CReporterApplicationSettings class.
  *
  */
class CReporterApplicationSettingsPrivate
{
    public:
        //! @arg Settings object.
        QSettings *settings;
};

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterApplicationSettings::instance
// ----------------------------------------------------------------------------
CReporterApplicationSettings* CReporterApplicationSettings::instance()
{
    if (sm_Instance == 0) {
        sm_Instance = new CReporterApplicationSettings();
    }
    return sm_Instance;
}

// ----------------------------------------------------------------------------
// CReporterApplicationSettings::freeSingleton
// ----------------------------------------------------------------------------
void CReporterApplicationSettings::freeSingleton()
{
    if (sm_Instance != 0) {
        delete sm_Instance;
        sm_Instance = 0;
    }
}

// ----------------------------------------------------------------------------
// CReporterApplicationSettings::~CReporterApplicationSettings
// ----------------------------------------------------------------------------
CReporterApplicationSettings::~CReporterApplicationSettings()
{
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterApplicationSettings::value
// ----------------------------------------------------------------------------
QVariant CReporterApplicationSettings::value(const QString &key,
                                             const QVariant &defaultValue) const
{
    return d_ptr->settings->value(key, defaultValue);
}

// ----------------------------------------------------------------------------
// CReporterApplicationSettings::CReporterApplicationSettings
// ----------------------------------------------------------------------------
CReporterApplicationSettings::CReporterApplicationSettings() :
        d_ptr(new CReporterApplicationSettingsPrivate())
{
    // Initiate settings. Settings are first searched from user-spesific location:
    // /home/user/.config/crash-reporter-settings/crash-reporter.conf.  If not found, then search
    // from system-spesific location: /usr/share/crash-reporter-settings/crash-reporter.conf.
    creporterSettingsInit(CReporter::SystemSettingsLocation);
    d_ptr->settings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                                   "crash-reporter-settings", "crash-reporter", this);
}

// End of file.
