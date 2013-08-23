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

#ifndef CREPORTERCOREREGISTRY_P_H
#define CREPORTERCOREREGISTRY_P_H

// System includes.

#include <QList>

// Forward declarations

class CReporterCoreDir;
class QSignalMapper;

/*!
 * \class CReporterCoreRegistry
 * \brief Private CReporterCoreRegistry class.
 *
 * \sa CReporterCoreRegistry
 */
class CReporterCoreRegistryPrivate
{
    public:
        /*!
          * @brief Class constructor.
          *
          */
        CReporterCoreRegistryPrivate();

        /*!
          * @brief Class destructor.
          *
          */
        virtual ~CReporterCoreRegistryPrivate();

    public:
        //! @arg Signal mapper to map gconf value changes.
        QSignalMapper *mapper;
        //! @arg List of CReporterCoreDir instances.
        QList<CReporterCoreDir*> coreDirs;
};

#endif // CREPORTERCOREREGISTRY_P_H

// End of file.
