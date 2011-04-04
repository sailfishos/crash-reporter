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

#ifndef CREPORTERDAEMON_P_H
#define CREPORTERDAEMON_P_H

// System includes.

#include <QString>
#include <QDateTime>

// Forward declarations.

class CReporterDaemonMonitor;
class CReporterDaemonCoreRegistry;
class CReporterSettingsObserver;
class QTimer;

/*!
 * \class CReporterDaemonPrivate
 * \brief Private CReporterDaemon class.
 *
 * \sa CReporterDaemon
 */
class CReporterDaemonPrivate
{
public:
    //! @arg Pointer to monitor object.
    CReporterDaemonMonitor *monitor;
    //! @arg Pointer to registry object.
    CReporterDaemonCoreRegistry *registry;
    //! @arg Pointer to settings observer object.
    CReporterSettingsObserver *settingsObserver;
    //! @arg Startup delay timer Id.
    int timerId;
    //! @arg Lifelog timer pointer. Null if lifelogging is disabled.
    QTimer* lifelogTimer;
    //! @arg Time of last lifelog update
    QDateTime lifelogLastUpdate;
    //! @arg Lifelog update counter
    int lifelogUpdateCount;
};

#endif // CREPORTERDAEMON_P_H
