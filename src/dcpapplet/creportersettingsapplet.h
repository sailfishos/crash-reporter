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

#ifndef CREPORTERSETTINGSAPPLET_H
#define CREPORTERSETTINGSAPPLET_H

// System includes.

#include <QObject>
#include <DcpAppletIf>

/*!
 * @class CReporterSettingsApplet
 * @brief Direct UI Control Panel applet fo Crash Reporter Settings.
 * 
 *  This class implements DcpAppletIf for MControlPanel applets.
 */
class CReporterSettingsApplet : public QObject, public DcpAppletIf
{
	Q_OBJECT
	Q_INTERFACES( DcpAppletIf )

public:
	// Inherited from DcpAppletIf.
    /*!
      * @reimp
      */
    virtual void init();

    /*!
      * @reimp
      */
	virtual DcpWidget* constructWidget( int widgetId );

    /*!
      * @reimp
      */
    virtual QString title() const;

    /*!
      * @reimp
      */
    virtual QVector<MAction*> viewMenuItems();

    /*!
      * @reimp
      */
    virtual DcpBrief* constructBrief( int partId=0 );
	
    /*!
      * @brief Returns page for displaying privacy settings.
      *
      * @return Settings page.
      */
    virtual DcpWidget* privacySettingsPage();

private:
	int m_WidgetIndex;
    int m_PartIndex;
    QString m_Title;
};

#endif // CREPORTERSETTINGSAPPLET_H

// End of file
