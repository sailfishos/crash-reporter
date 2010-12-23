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

#ifndef CREPORTERINFOBANNER_H
#define CREPORTERINFOBANNER_H

// System includes.

#include <MBanner>

// User includes.

#include "creporterexport.h"

/*!
  * @class CReporterInfoBanner
  * @brief Convenience class to implement custom MBanner.
  *
  */
class CREPORTER_EXPORT CReporterInfoBanner : public MBanner {

    Q_OBJECT

public:
    /*!
      * @brief Displays in-process information banner.
      *
      * @param message Body text of the banner.
      * @param type Type of the banner.
      * @param iconID Icon for the banner.
      * @param timeout Timeout value after banner disappears.
      * @param buttonText Text for the button.
      *
      * @return Pointer to this banner.
      */
    static CReporterInfoBanner* show(const QString &message, const QString &bannerType="InformationBanner",
                                     const QString &iconID="Icon-close");

    /*!
      * @brief Class destructor.
      *
      */
    virtual ~CReporterInfoBanner();

protected:
    /*!
      * @brief Class constructor
      *
      */
    CReporterInfoBanner(QString bannerType);

private Q_SLOTS:
    /*!
      * @brief Called, when timer elapses.
      *
      */
    void notificationTimeout();

};

#endif // CREPORTERINFOBANNER_H

// End of file.
