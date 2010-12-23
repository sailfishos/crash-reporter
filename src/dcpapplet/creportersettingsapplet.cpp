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

#include <MAction>
#include <QtGui>
#include <QDebug>
#include <MTheme>
#include <MLocale>

// User includes.

#include "creportersettingsapplet.h"

#include "creportersettingsbrief.h"
#include "creportersettings.h"
#include "creporterprivacysettingswidget.h"

Q_EXPORT_PLUGIN2(crashreportersettingsapplet, CReporterSettingsApplet)

const QString cssDir  = "/usr/share/duicontrolpanel/themes/style/";
const QString cssFile = "crashreportersettingsapplet.css";

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterSettingsApplet::init
// ----------------------------------------------------------------------------
void CReporterSettingsApplet::init()
{
    MLocale locale;
    locale.installTrCatalog("crash-reporter");
    MLocale::setDefault(locale);

	m_WidgetIndex = DcpCreporterSettings::Main;
}

// ----------------------------------------------------------------------------
// CReporterSettingsApplet::constructWidget
// ----------------------------------------------------------------------------
DcpWidget* CReporterSettingsApplet::constructWidget(int widgetId)
{
    switch (widgetId) {
		case DcpCreporterSettings::Main:
		case DcpCreporterSettings::PrivacySettings:
		default:
            return privacySettingsPage();
			break;
	};
}

// ----------------------------------------------------------------------------
// CReporterSettingsApplet::privacySettingsPage
// ----------------------------------------------------------------------------
DcpWidget* CReporterSettingsApplet::privacySettingsPage()
{
    return new CReporterPrivacySettingsWidget();
}

QString CReporterSettingsApplet::title() const
{
    //% "Crash Reporter"
    return QString(qtTrId("qtn_dcp_applet_title_text"));
}

// ----------------------------------------------------------------------------
// CReporterSettingsApplet::viewMenuItems
// ----------------------------------------------------------------------------
QVector<MAction*> CReporterSettingsApplet::viewMenuItems()
{
    QVector<MAction*> vector;

    return vector;
}

// ----------------------------------------------------------------------------
// CReporterSettingsApplet::constructBrief
// ----------------------------------------------------------------------------
DcpBrief* CReporterSettingsApplet::constructBrief(int partId)
{
    Q_UNUSED(partId);
	return new CReporterSettingsBrief();
}

// End of file
