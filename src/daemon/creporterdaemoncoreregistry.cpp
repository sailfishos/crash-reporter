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

#include <QTimer>
#include <QDebug>
#include <QDir>
#include <QSignalMapper>

#include <MGConfItem> // g_conf wrapper

// User includes

#include "creporterdaemoncoreregistry.h"
#include "creporterdaemoncoreregistry_p.h"
#include "creporterdaemoncoredir.h"
#include "creporterutils.h"

// Local macros and definitions.

#define NUM_ENV_MOUNTPOINTS 2
#define NUM_STATIC_MOUNTPOINTS 1
#define MAX_CORE_DIRS (NUM_ENV_MOUNTPOINTS + NUM_STATIC_MOUNTPOINTS + 1)
#define MAX_MOUNTPOINT_NAMELEN (128)

#define MMC_EVENT_TIMEOUT   5000 // Time to wait (ms) after mmc state has changed.

#define MAX_GCONF_KEYLEN    128
#define NUM_GCONF_KEYS      6

static const char
gconf_keys[NUM_GCONF_KEYS][MAX_GCONF_KEYLEN] =
    {
     "/system/osso/af/mmc-device-present",
     "/system/osso/af/internal-mmc-device-present",
     "/system/osso/af/mmc-cover-open",
     "/system/osso/af/mmc/mmc-corrupted",
     "/system/osso/af/mmc-used-over-usb",
     "/system/osso/af/internal-mmc-used-over-usb"
    };

#define EXTERNAL_MMC_PRESENT    "/system/osso/af/mmc-device-present"
#define INTERNAL_MMC_PRESENT 	"/system/osso/af/internal-mmc-device-present"
#define EXTERNAL_COVER_OPEN 	"/system/osso/af/mmc-cover-open"
#define MMC_CORRUPTED 		    "/system/osso/af/mmc/mmc-corrupted"
#define EXTERNAL_MMC_USED_USB	"/system/osso/af/mmc-used-over-usb"
#define INTERNAL_MMC_USED_USB	"/system/osso/af/internal-mmc-used-over-usb"

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

// ********* Class CReporterDaemonCoreDirPrivate ********
// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterDaemonCoreRegistryPrivate::CReporterDaemonCoreRegistryPrivate
// ----------------------------------------------------------------------------
CReporterDaemonCoreRegistryPrivate::CReporterDaemonCoreRegistryPrivate()
{
    mapper = new QSignalMapper();
}

// ----------------------------------------------------------------------------
// CReporterDaemonCoreRegistryPrivate::~CReporterDaemonCoreRegistryPrivate
// ----------------------------------------------------------------------------
CReporterDaemonCoreRegistryPrivate::~CReporterDaemonCoreRegistryPrivate()
{
    delete mapper;
}

// ********* Class CReporterDaemonCoreDir ********
// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterDaemonCoreRegistry::CReporterDaemonCoreRegistry
// ----------------------------------------------------------------------------
CReporterDaemonCoreRegistry::CReporterDaemonCoreRegistry() :
    d_ptr(new CReporterDaemonCoreRegistryPrivate())
{
    Q_D(CReporterDaemonCoreRegistry);

	createCoreLocationRegistry();

    // Cache keys under /system/osso/af and map state change signals to single slot.
    for (int i=0; i< NUM_GCONF_KEYS; i++) {
        MGConfItem *gConf = new MGConfItem(gconf_keys[i], this);
        connect(gConf, SIGNAL(valueChanged()), d->mapper, SLOT(map()));
        d->mapper->setMapping(gConf, gconf_keys[i]);
    }

    connect(d->mapper, SIGNAL(mapped(QString)), this, SLOT(mmcStateChanged(QString)));
}

// ----------------------------------------------------------------------------
// CReporterDaemonCoreRegistry::~CReporterDaemonCoreRegistry
// ----------------------------------------------------------------------------
CReporterDaemonCoreRegistry::~CReporterDaemonCoreRegistry()
{
    Q_D(CReporterDaemonCoreRegistry);

	QList<CReporterDaemonCoreDir*> dirs = d->coreDirs;
	// Clear list.
	d->coreDirs.clear();
	// Delete entries.
    qDeleteAll(dirs);
	delete d_ptr;
}

// ----------------------------------------------------------------------------
// CReporterDaemonCoreRegistry::collectAllCoreFiles
// ----------------------------------------------------------------------------
QStringList CReporterDaemonCoreRegistry::collectAllCoreFiles()
{
    Q_D(CReporterDaemonCoreRegistry);

    QStringList out;
	QListIterator<CReporterDaemonCoreDir*> iter(d->coreDirs); 

    while (iter.hasNext()) {
		CReporterDaemonCoreDir* dir = (CReporterDaemonCoreDir*) iter.next();
        dir->collectAllCoreFilesAtLocation(out);
	}
    return out;
}

// ----------------------------------------------------------------------------
// CReporterDaemonCoreRegistry::getCoreLocationPaths
// ----------------------------------------------------------------------------
QStringList* CReporterDaemonCoreRegistry::getCoreLocationPaths()
{
	Q_D( CReporterDaemonCoreRegistry );
	QStringList* paths = new QStringList();
	
	QListIterator<CReporterDaemonCoreDir*> iter(d->coreDirs); 

	while ( iter.hasNext() ) {
		// Get paths for core locations.
		CReporterDaemonCoreDir* pCoreDir =  (CReporterDaemonCoreDir*) iter.next();
		QDir dir( pCoreDir->getDirectory() );

		qDebug() << __PRETTY_FUNCTION__ << "Got directory:" << pCoreDir->getDirectory();

		if ( dir.exists() ) {
			qDebug() << __PRETTY_FUNCTION__ << "Exists. Add to list";
			paths->append( dir.absolutePath() );
		}
	}

	qDebug() << __PRETTY_FUNCTION__ << "Number of mounted locations:" << paths->count();
	return paths;
}

// ----------------------------------------------------------------------------
// CReporterDaemonCoreRegistry::checkDirectoryForCores
// ----------------------------------------------------------------------------
QString CReporterDaemonCoreRegistry::checkDirectoryForCores(const QString& path)
{
	Q_D( CReporterDaemonCoreRegistry );

	QString coreFilePath;
	QListIterator<CReporterDaemonCoreDir*> iter(d->coreDirs); 
	
	while ( iter.hasNext() ) {
		CReporterDaemonCoreDir* pCoreDir =  (CReporterDaemonCoreDir*) iter.next();
		// Find the correct location.
		if ( pCoreDir->getDirectory() == path ) {
			coreFilePath = pCoreDir->checkDirectoryForCores();
		}
	}	
	return coreFilePath;
}

// ----------------------------------------------------------------------------
// CReporterDaemonCoreRegistry::refreshRegistry
// ----------------------------------------------------------------------------
void CReporterDaemonCoreRegistry::refreshRegistry()
{
	qDebug() << __PRETTY_FUNCTION__ << "Emit registryRefreshNeeded().";
	emit registryRefreshNeeded();
}

// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterDaemonCoreRegistry::mmcStateChanged
// ----------------------------------------------------------------------------
void CReporterDaemonCoreRegistry::mmcStateChanged(const QString &key)
{
    qDebug() << __PRETTY_FUNCTION__ << "Key:" << key << "has changed.";
	QTimer::singleShot( MMC_EVENT_TIMEOUT, this, SIGNAL(coreLocationsUpdated()) );
}

// ----------------------------------------------------------------------------
// CReporterDaemonCoreRegistry::createCoreLocationRegistry
// ----------------------------------------------------------------------------
void CReporterDaemonCoreRegistry::createCoreLocationRegistry()
{
    Q_D(CReporterDaemonCoreRegistry);
    const char *name;

#if defined(__arm__) && (!defined(CREPORTER_SDK_HOST) || !defined(CREPORTER_UNIT_TEST))
	qDebug() << __PRETTY_FUNCTION__ << "Get mountpoints from the environment.";
	// Get mount points from environment.
    for (int i = 0; i < NUM_ENV_MOUNTPOINTS; i++) {
		
		name = mountpoint_env_names[i];
        QString mpoint(getenv(name));
	
        if (!mpoint.isEmpty()) {
			CReporterDaemonCoreDir* dir = new CReporterDaemonCoreDir( mpoint, this );
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

            CReporterDaemonCoreDir* dir = new CReporterDaemonCoreDir(mpoint, this);
			d->coreDirs << dir;
		}
    }

	qDebug() << __PRETTY_FUNCTION__ << "Set core location directories.";

    for (int i = 0; i < d->coreDirs.count(); i++) {
		// Set directory for core locations.
        CReporterDaemonCoreDir* dir = (CReporterDaemonCoreDir*) d->coreDirs.at(i);
        connect(this, SIGNAL(coreLocationsUpdated()), dir, SLOT(createCoreDirectory()));
        connect(this, SIGNAL(registryRefreshNeeded()), dir, SLOT(updateCoreList()));

        QString tmp(dir->getMountpoint());
        tmp.append(core_dumps_suffix);
        dir->setDirectory(tmp);
    }
	
	// Emit this signal to create directories for core dumps.
	emit coreLocationsUpdated();
}
// End of file
