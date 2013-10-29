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

#include <sys/types.h> // for chmod()
#include <sys/stat.h>

#include <QDir>
#include <QDebug>
#include <QDirIterator>

// User includes.

#include "creportercoredir.h"
#include "creportercoredir_p.h"
#include "creporterutils.h"

// Local macros and definitions.

#define FILE_PERMISSION 	0777

// Local constants.

const char rcore_file_name_filter[] = "*.rcore";
const char rcore_lzo_file_name_filter[] = "*.rcore.lzo";

// ======== MEMBER FUNCTIONS ========

CReporterCoreDir::CReporterCoreDir(QString& mpoint, QObject* parent)
    : QObject(parent), d_ptr(new CReporterCoreDirPrivate())
{
	d_ptr->mountpoint = mpoint;
	qDebug() << __PRETTY_FUNCTION__ << "Mountpoint set to:" << d_ptr->mountpoint;
}

CReporterCoreDir::~CReporterCoreDir()
{
	delete d_ptr;
}

QString CReporterCoreDir::getDirectory() const
{
    return d_ptr->directory;
}

QString CReporterCoreDir::getMountpoint() const
{
    return d_ptr->mountpoint;
}

void CReporterCoreDir::setDirectory(const QString& dir)
{
    Q_D(CReporterCoreDir);

	d->directory = dir;
	qDebug() << __PRETTY_FUNCTION__ << "Directory set to:" << d->directory;
}
	
void CReporterCoreDir::setMountpoint(const QString& mpoint)
{
    Q_D(CReporterCoreDir);

	d->mountpoint = mpoint;
	qDebug() << __PRETTY_FUNCTION__ << "Mountpoint set to:" << d->mountpoint;
}

void CReporterCoreDir::collectAllCoreFilesAtLocation(QStringList& coreList)
{
    Q_D(CReporterCoreDir);

	qDebug() << __PRETTY_FUNCTION__ << "Collecting cores from:" << d->directory;
	
	QStringList filters;
    filters << QString(rcore_file_name_filter) << QString(rcore_lzo_file_name_filter);

	// Construct iterator for core-dumps directory.
    QDirIterator iter(d->directory, filters, QDir::Files | QDir::NoDotAndDotDot);

    while (iter.hasNext()) {
		QString file = iter.next();
	
		qDebug() << __PRETTY_FUNCTION__ << "File found:" << file << "--> validate.";
		
        if (CReporterUtils::validateCore(iter.fileInfo())) {
			qDebug() << __PRETTY_FUNCTION__ << "File:" << iter.filePath() << "added to list.";
			coreList << iter.filePath();
			}
	}
}

QString CReporterCoreDir::checkDirectoryForCores()
{
    Q_D(CReporterCoreDir);

	QFileInfo fi;
	QString coreFilePath;

	QStringList filters;
    filters << QString(rcore_file_name_filter) << QString(rcore_lzo_file_name_filter);

	// Construct iterator for core-dumps directory.
    QDirIterator iter(d->directory, filters, QDir::Files | QDir::NoDotAndDotDot);

	// Iterate over files in the core-dumps directory.		
    while (iter.hasNext()) {
		
		iter.next();
		fi = iter.fileInfo();

        if (!d->coresAtDirectory.contains(fi.fileName()) &&
                CReporterUtils::validateCore(fi)) {
				// This is valid rich core file, which hasn't been processed before.
				d->coresAtDirectory << fi.fileName();
				coreFilePath = fi.absoluteFilePath();
				qDebug() << __PRETTY_FUNCTION__ << "New core file:" << fi.fileName();
				break;
		}
	}

    if (coreFilePath.isEmpty()) {
		// File was deleted by the user or client from the directory.
		// Refresh directory list.
		updateCoreList();
	}

	return coreFilePath;
}

void CReporterCoreDir::createCoreDirectory()
{
    Q_D(CReporterCoreDir);

    if (CReporterUtils::isMounted(d->mountpoint))
    {
        // Construct new QDir pointing to mount point.
        QDir coreRoot(d->mountpoint);

        if (!coreRoot.exists(d->directory))
        {
            // If core-dumps -directory doesn't exist.
            // Create new sub-directory.
            if (coreRoot.exists(d->directory.left(d->directory.lastIndexOf('/')))
                && coreRoot.mkdir(d->directory))
            {
                qDebug() << __PRETTY_FUNCTION__ << "Created directory:" << d->directory;
                chmod(CReporterUtils::qstringToChar(d->directory), FILE_PERMISSION);
            }
            else
            {
                qWarning() << __PRETTY_FUNCTION__ << "Error while creating directory:" << d->directory;
            }
            // Remove old entries from the list.
            d->coresAtDirectory.clear();
        }
        else
        {
            // There was a "core-dumps" directory already. Fetch possible core files.
            updateCoreList();
        }
    }
}

void CReporterCoreDir::updateCoreList()
{
    Q_D(CReporterCoreDir);

	qDebug() << __PRETTY_FUNCTION__ << "Refreshing core directory list.";

    QDir dir(d->directory);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setNameFilters(QStringList() << rcore_file_name_filter << rcore_lzo_file_name_filter);

	// Remove old entries.
	d->coresAtDirectory.clear();

    QDirIterator it(dir);
    while (it.hasNext()) {
        d->coresAtDirectory.append(it.next());
    }
}

// End of file
