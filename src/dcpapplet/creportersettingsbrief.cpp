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

// User includes.

#include "creportersettingsbrief.h"
#include "creporterprivacysettingsmodel.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterSettingsBrief::CReporterSettingsBrief
// ----------------------------------------------------------------------------
CReporterSettingsBrief::CReporterSettingsBrief()
{
    connect(CReporterPrivacySettingsModel::instance(), SIGNAL(valueChanged(QString,QVariant)),
            this, SLOT(settingsChanged(QString,QVariant)));

}

// ----------------------------------------------------------------------------
// CReporterSettingsBrief::~CReporterSettingsBrief
// ----------------------------------------------------------------------------
CReporterSettingsBrief::~CReporterSettingsBrief()
{
    // Singleton class instance can be destroyed now.
    CReporterPrivacySettingsModel::freeSingleton();
}

// ----------------------------------------------------------------------------
// CReporterSettingsBrief::valueText
// ----------------------------------------------------------------------------
QString CReporterSettingsBrief::valueText() const
{
    if (CReporterPrivacySettingsModel::instance()->sendingEnabled())
    {
        //% "Notify of new crash reports"
        return QString(qtTrId("qt_dcp_show_notitications_text"));
    }
    else if (CReporterPrivacySettingsModel::instance()->automaticSendingEnabled())
    {
        //% "Send crash reports automatically"
        return QString(qtTrId("qt_dcp_sending_automatically_text"));
    }
    else
    {
        //% "Notifications and sending disabled"
        return QString(qtTrId("qt_dcp_notifications_and_automatic_uploading_disabled_text"));
    }
}

// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterSettingsBrief::settingsChanged
// ----------------------------------------------------------------------------
void CReporterSettingsBrief::settingsChanged(const QString& key, const QVariant& value)
{
    Q_UNUSED(value);

    if (key == Settings::ValueCoreDumping || key == Settings::ValueAutomaticSending) {
        emit valuesChanged();
    }
}

// End of file
