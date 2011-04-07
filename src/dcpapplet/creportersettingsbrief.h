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

#ifndef CREPORTERSETTINGSBRIEF_H
#define CREPORTERSETTINGSBRIEF_H

// User includes

#include <DcpBrief>

/*! 
 * @class CReporterSettingsBrief
 * @brief Implements brief widget for launching applet.
 *
 *  Displays applet name and current value.
 */
class CReporterSettingsBrief : public DcpBrief
{
    Q_OBJECT

public:

    /*!
     * @brief Class constructor.
     */
    CReporterSettingsBrief();

    /*!
     * @brief Class destructor.
     */
   ~CReporterSettingsBrief();

   /*!
    * @reimp
    */
   virtual QString titleText() const;

};

#endif // CREPORTERSETTINGSBRIEF_H

// End of file
