/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Riku Halonen <riku.halonen@nokia.com>
 *
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Jakub Adam <jakub.adam@jollamobile.com>
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

// System includes

#include <stdlib.h> // for getenv()

#include <QCoreApplication>
#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QSignalMapper>

// User includes

#include "creportercoreregistry.h"
#include "creportercoreregistry_p.h"
#include "creportercoredir.h"
#include "creporterutils.h"

// Local macros and definitions.

#define NUM_ENV_MOUNTPOINTS 2
#define NUM_STATIC_MOUNTPOINTS 1
#define MAX_CORE_DIRS (NUM_ENV_MOUNTPOINTS + NUM_STATIC_MOUNTPOINTS + 1)
#define MAX_MOUNTPOINT_NAMELEN (128)

#define MMC_EVENT_TIMEOUT   5000 // Time to wait (ms) after mmc state has changed.

// Local constants.

static const char
mountpoint_env_names[NUM_ENV_MOUNTPOINTS][MAX_MOUNTPOINT_NAMELEN] =
    {
    "MMC_MOUNTPOINT",
    "INTERNAL_MMC_MOUNTPOINT"
    };

#ifndef CREPORTER_UNIT_TEST
static const char
mountpoint_static[NUM_STATIC_MOUNTPOINTS][MAX_MOUNTPOINT_NAMELEN] =
    {
    "/var/cache"
    };
#else
static const char
mountpoint_static[NUM_STATIC_MOUNTPOINTS][MAX_MOUNTPOINT_NAMELEN] =
    {
    "/crash-reporter-tests/home/user/MyDocs"
    };
#endif // CREPORTER_UNIT_TEST

const char core_dumps_suffix[] = "/core-dumps";

// ======== MEMBER FUNCTIONS ========

CReporterCoreRegistryPrivate::CReporterCoreRegistryPrivate()
{
    mapper = new QSignalMapper();
}

CReporterCoreRegistryPrivate::~CReporterCoreRegistryPrivate()
{
    delete mapper;
}

CReporterCoreRegistry::CReporterCoreRegistry(QObject *parent):
  QObject(parent), d_ptr(new CReporterCoreRegistryPrivate())
{
	createCoreLocationRegistry();

	/* TODO: status of MMC card used to be monitored here, and mmcStateChanged()
	 * triggered when card was inserted/removed/mounted via USB. We might need
	 * to re-implement this functionality also for Sailfish. */
    // Cache keys under /system/osso/af and map state change signals to single slot.
}

CReporterCoreRegistry::~CReporterCoreRegistry()
{
    Q_D(CReporterCoreRegistry);

	QList<CReporterCoreDir*> dirs = d->coreDirs;
	// Clear list.
	d->coreDirs.clear();
	// Delete entries.
    qDeleteAll(dirs);
	delete d_ptr;
}

QStringList CReporterCoreRegistry::collectAllCoreFiles() const
{
    Q_D(const CReporterCoreRegistry);

    QStringList out;
	QListIterator<CReporterCoreDir*> iter(d->coreDirs); 

    while (iter.hasNext()) {
		CReporterCoreDir* dir = (CReporterCoreDir*) iter.next();
        dir->collectAllCoreFilesAtLocation(out);
	}
    return out;
}

QStringList CReporterCoreRegistry::getCoreLocationPaths()
{
	Q_D( CReporterCoreRegistry );
    QStringList paths;

	QListIterator<CReporterCoreDir*> iter(d->coreDirs); 

	while ( iter.hasNext() ) {
		// Get paths for core locations.
		CReporterCoreDir* pCoreDir =  (CReporterCoreDir*) iter.next();
		QDir dir( pCoreDir->getDirectory() );

		qDebug() << __PRETTY_FUNCTION__ << "Got directory:" << pCoreDir->getDirectory();

		if ( dir.exists() ) {
			qDebug() << __PRETTY_FUNCTION__ << "Exists. Add to list";
			paths.append(dir.absolutePath());
		}
	}

    qDebug() << __PRETTY_FUNCTION__
             << "Number of mounted locations:" << paths.count();
	return paths;
}

QString CReporterCoreRegistry::checkDirectoryForCores(const QString& path)
{
	Q_D( CReporterCoreRegistry );

	QString coreFilePath;
	QListIterator<CReporterCoreDir*> iter(d->coreDirs); 
	
	while ( iter.hasNext() ) {
		CReporterCoreDir* pCoreDir =  (CReporterCoreDir*) iter.next();
		// Find the correct location.
		if ( pCoreDir->getDirectory() == path ) {
			coreFilePath = pCoreDir->checkDirectoryForCores();
		}
	}	
	return coreFilePath;
}

void CReporterCoreRegistry::refreshRegistry()
{
	qDebug() << __PRETTY_FUNCTION__ << "Emit registryRefreshNeeded().";
	emit registryRefreshNeeded();
}

// ======== LOCAL FUNCTIONS ========

void CReporterCoreRegistry::mmcStateChanged(const QString &key)
{
    qDebug() << __PRETTY_FUNCTION__ << "Key:" << key << "has changed.";
	QTimer::singleShot( MMC_EVENT_TIMEOUT, this, SIGNAL(coreLocationsUpdated()) );
}

void CReporterCoreRegistry::createCoreLocationRegistry()
{
    Q_D(CReporterCoreRegistry);
    const char *name;

#if defined(__arm__) && (!defined(CREPORTER_SDK_HOST) || !defined(CREPORTER_UNIT_TEST))
	qDebug() << __PRETTY_FUNCTION__ << "Get mountpoints from the environment.";
	// Get mount points from environment.
    for (int i = 0; i < NUM_ENV_MOUNTPOINTS; i++) {
		
		name = mountpoint_env_names[i];
        QString mpoint(getenv(name));
	
        if (!mpoint.isEmpty()) {
			CReporterCoreDir* dir = new CReporterCoreDir( mpoint, this );
			d->coreDirs << dir;
		}
    }
#endif // defined(__arm__) && (!defined(CREPORTER_SDK_HOST) || !defined(CREPORTER_UNIT_TEST))
    
    if (d->coreDirs.empty()) {
		qDebug() << __PRETTY_FUNCTION__ << "Nothing in the environment. Using static mountpoints.";
		// Nothing in environment, use static values as fallback.
        for (int i = 0; i < NUM_STATIC_MOUNTPOINTS; i++) {
			
			name = mountpoint_static[i];
			QString mpoint( name );

#if defined(CREPORTER_SDK_HOST) || defined(CREPORTER_UNIT_TEST)
            mpoint.prepend(QDir::homePath());
#endif // defined(CREPORTER_SDK_HOST) || defined(CREPORTER_UNIT_TEST)

            CReporterCoreDir* dir = new CReporterCoreDir(mpoint, this);
			d->coreDirs << dir;
		}
    }

	qDebug() << __PRETTY_FUNCTION__ << "Set core location directories.";

    for (int i = 0; i < d->coreDirs.count(); i++) {
		// Set directory for core locations.
        CReporterCoreDir* dir = (CReporterCoreDir*) d->coreDirs.at(i);
        connect(this, SIGNAL(coreLocationsUpdated()), dir, SLOT(createCoreDirectory()));
        connect(this, SIGNAL(registryRefreshNeeded()), dir, SLOT(updateCoreList()));

        QString tmp(dir->getMountpoint());
        tmp.append(core_dumps_suffix);
        dir->setDirectory(tmp);
    }
	
	// Emit this signal to create directories for core dumps.
	emit coreLocationsUpdated();
}

CReporterCoreRegistry *CReporterCoreRegistry::instance()
{
    static CReporterCoreRegistry *instance = 0;
    if (!instance) {
        instance = new CReporterCoreRegistry(qApp);
    }

    return instance;
}
