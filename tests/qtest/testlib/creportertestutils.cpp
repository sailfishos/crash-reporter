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

#include <QDir>
#include <QFile>
#include <QDirIterator>
#include <QDebug>

#include "creportertestutils.h"

#define NUM_ENV_MOUNTPOINTS 2
#define NUM_STATIC_MOUNTPOINTS 3
#define MAX_CORE_DIRS (NUM_ENV_MOUNTPOINTS + NUM_STATIC_MOUNTPOINTS + 1)
#define MAX_MOUNTPOINT_NAMELEN (128)

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
    "/home/user/MyDocs"
    };
#else
static const char
mountpoint_static[NUM_STATIC_MOUNTPOINTS][MAX_MOUNTPOINT_NAMELEN] =
    {
    "/crash-reporter-tests/home/user/MyDocs"
    };
#endif // CREPORTER_UNIT_TEST

const char core_dumps_suffix[] = "/core-dumps";

void CReporterTestUtils::createTestDataFiles( const QStringList& filePaths, 
		QStringList& compareFiles, const char* testData[] )
{	
	for ( int i = 0; i < filePaths.count(); i++ ) {
		
		QDir dir( filePaths.at(i) );
		
		if ( dir.exists() ) {
		
			QString path( filePaths.at(i) );
			path.append( "/" );
			path.append( testData[i] );
						
			QDir::setCurrent( filePaths.at(i) );
			QFile file;
			file.setFileName(testData[i]);

			if ( file.open( QIODevice::ReadWrite ) ) {
				compareFiles << path;
			}
		}
	}	
}

void CReporterTestUtils::cleanupDirectories( const QStringList& paths )
{
	// Remove files from core-dumps directories.
	for ( int i = 0; i < paths.count(); i++ ) {
		QString command("rm -f ");

		command.append(paths.at(i));
		command.append("/*");

		qDebug() << command;
		UNUSED_RESULT(system(command.toLatin1().data()));
	}
}

void CReporterTestUtils::removeDirectories( const QStringList& paths )
{
    QDir::setCurrent(QDir::homePath());

	for ( int i = 0; i < paths.count(); i++ ) {
		QString command("rm -rf ");

		command.append(paths.at(i));

		qDebug() << command;
		UNUSED_RESULT(system(command.toLatin1().data()));
	}
}

void CReporterTestUtils::removeDirectory( const QString& path )
{
	// Remove core-dumps directory.
	QString command("rm -rf ");

	command.append( path );

	qDebug() << command;
	UNUSED_RESULT(system(command.toLatin1().data()));
}

#ifdef CREPORTER_UNIT_TEST
void CReporterTestUtils::createTestMountpoints()
{
    QDir dir(QDir::homePath());
    dir.mkdir("crash-reporter-tests");
    dir.mkdir("crash-reporter-tests/home");
    dir.mkdir("crash-reporter-tests/home/user");
    dir.mkdir("crash-reporter-tests/home/user/MyDocs");
}

void CReporterTestUtils::removeTestMountpoints()
{
    QDir::setCurrent(QDir::homePath());
    UNUSED_RESULT(system("rm -rf crash-reporter-tests"));
}
#endif // CREPORTER_UNIT_TEST

QStringList* CReporterTestUtils::getCoreDumpPaths()
{
    QStringList *paths = new QStringList();

    const char *name;

#if defined(__arm__) && (!defined(CREPORTER_SDK_HOST) || !defined(CREPORTER_UNIT_TEST))
	qDebug() << __PRETTY_FUNCTION__ << "Get mountpoints from the environment.";
	// Get mount points from environment.
    for ( int i = 0; i < NUM_ENV_MOUNTPOINTS; i++ ) {
		
		name = mountpoint_env_names[i];
		QString mpoint( getenv(name) );
	
		if ( !mpoint.isEmpty() ) {
			QString path( mpoint.append( core_dumps_suffix ) );
			QDir dir( path );
			
			if ( dir.exists() ) {
				*paths << path;
				qDebug() << __PRETTY_FUNCTION__ << "Added:" << path;
			} 
		}
    }
#endif // defined(__arm__) && (!defined(CREPORTER_SDK_HOST) || !defined(CREPORTER_UNIT_TEST))
    
	if ( paths->empty() ) {
		qDebug() << __PRETTY_FUNCTION__ << "Nothing in the environment. Using static mountpoints.";
		// Nothing in environment, use static values as fallback.
		for ( int i = 0; i < NUM_STATIC_MOUNTPOINTS; i++ ) {
			
			name = mountpoint_static[i];
			QString mpoint( name );

#if defined(CREPORTER_SDK_HOST) || defined(CREPORTER_UNIT_TEST)
			mpoint.prepend( QDir::homePath() );
#endif // defined(CREPORTER_SDK_HOST) || defined(CREPORTER_UNIT_TEST)

			QString path( mpoint.append( core_dumps_suffix ) );
			QDir dir( path );
			
			if ( dir.exists() ) {
				*paths << path;
				qDebug() << __PRETTY_FUNCTION__ << "Added:" << path;
			}
		}
    }

	return paths;
}
