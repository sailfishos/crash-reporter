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

class CReporterDaemonMonitor;
class CReporterCoreRegistry;

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
    CReporterCoreRegistry *registry;
    //! @arg Startup delay timer Id.
    int timerId;
};

#endif // CREPORTERDAEMON_P_H
