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

#include <QTimer>

// User includes.

#include "creporterinfobanner.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterInfoBanner::show
// ----------------------------------------------------------------------------
CReporterInfoBanner* CReporterInfoBanner::show(const QString &message, const QString &bannerType,
                                               const QString &iconID)
{
    CReporterInfoBanner *b = new CReporterInfoBanner(bannerType);
    b->setObjectName("InformationBanner");

    b->setTitle(message);
    b->setIconID(iconID);

    #ifndef CREPORTER_UNIT_TEST
    b->appear(MSceneWindow::DestroyWhenDone);
    #endif // CREPORTER_UNIT_TEST

    return b;
}

// ----------------------------------------------------------------------------
// CReporterInfoBanner::CReporterInfoBanner
// ----------------------------------------------------------------------------
CReporterInfoBanner::CReporterInfoBanner(QString bannerType) :
        MBanner()
{
    setStyleName(bannerType);
}

// ----------------------------------------------------------------------------
// CReporterInfoBanner::~CReporterInfoBanner
// ----------------------------------------------------------------------------
CReporterInfoBanner::~CReporterInfoBanner()
{
}

// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterInfoBanner::notificationTimeout
// ----------------------------------------------------------------------------
void CReporterInfoBanner::notificationTimeout()
{
    QTimer *t = qobject_cast<QTimer *>(sender());

    if (t != NULL) {
        CReporterInfoBanner *b = qobject_cast<CReporterInfoBanner *>(t->parent());
        if (b != NULL) {
#ifndef CREPORTER_UNIT_TEST
            b->disappear();
#else
            delete b;
            b = 0;
#endif // CREPORTER_UNIT_TEST
        }
    }
}

// End of file.
