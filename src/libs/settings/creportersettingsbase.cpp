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
#include <QFile>


#include "creportersettingsbase_p.h"
#include "creportersettingsbase.h"
#include "creportersettingsinit_p.h"
#include "creporternamespace.h"

CReporterSettingsBasePrivate::CReporterSettingsBasePrivate()
    : m_settings(0)
{
}

CReporterSettingsBasePrivate::~CReporterSettingsBasePrivate()
{
}

CReporterSettingsBase::CReporterSettingsBase(const QString &organization,
                                             const QString &application, QObject *parent)
    : d_ptr(new CReporterSettingsBasePrivate())
{
    Q_UNUSED(parent);

    creporterSettingsInit(CReporter::SystemSettingsLocation);
    d_ptr->m_settings = new QSettings(QSettings::NativeFormat, QSettings::UserScope,
                                      organization, application, this);

}

CReporterSettingsBase::~CReporterSettingsBase()
{
    delete d_ptr;
    d_ptr = 0;
}

void CReporterSettingsBase::writeSettings()
{
    Q_D(CReporterSettingsBase);
    // Write any unsaved changes to permanent storage.
    d->m_settings->sync();
}

QString CReporterSettingsBase::settingsFile() const
{
    Q_D(const CReporterSettingsBase);

    if (d->m_settings == 0) {
        return QString();
    }
    return d->m_settings->fileName();
}

bool CReporterSettingsBase::isValid() const
{
    Q_D(const CReporterSettingsBase);

    if (d->m_settings == 0) {
        return false;
    } else if (!QFile::exists(d->m_settings->fileName())) {
        return false;
    } else {
        return true;
    }
}

bool CReporterSettingsBase::setValue(const QString &key, const QVariant &value)
{
    Q_D(CReporterSettingsBase);

    QVariant oldValue = d->m_settings->value(key);
    // Set value for the setting.
    d->m_settings->setValue(key, value);

    if (oldValue != value) {
        // Notify change, if setting value was changed.
        emit valueChanged(key, value);
        return true;
    }

    return false;
}

QVariant CReporterSettingsBase::value(const QString &key, const QVariant &defaultValue) const
{
    // Find and return value.
    return d_ptr->m_settings->value(key, defaultValue);
}
