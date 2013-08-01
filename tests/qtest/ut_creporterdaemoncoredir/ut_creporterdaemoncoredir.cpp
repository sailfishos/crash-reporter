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

#include <stdlib.h>
#include <QStringList>
#include <QFile>
#include <QDir>

#include "ut_creporterdaemoncoredir.h"
#include "creporterdaemoncoredir.h"
#include "creporterdaemoncoredir_p.h"
#include "creportertestutils.h"

QString testMountPoint1("/tmp/crash-reporter-tests/media/mmc1");
QString testMountPoint2("/tmp/crash-reporter-tests/media/mmc2");

void Ut_CReporterDaemonCoreDir::initTestCase()
{

}

void Ut_CReporterDaemonCoreDir::init()
{
    QString mkDir = QString("mkdir -p -m 777 %1 %2").arg(testMountPoint1).arg(testMountPoint2);
    system(mkDir.toLatin1());
}

void Ut_CReporterDaemonCoreDir::testCreationOfDirectoryForCores()
{
    // Set and verify mountpoint.
    dir = new CReporterDaemonCoreDir(testMountPoint1);
    QVERIFY(dir->getMountpoint() == testMountPoint1);

    // Set and verify directory.
    QString compareDirectory = QString(testMountPoint1);
    compareDirectory.append("/core-dumps");
    dir->setDirectory(compareDirectory);
    dir->createCoreDirectory();

    QVERIFY(dir->getDirectory() == compareDirectory);
}

void Ut_CReporterDaemonCoreDir::testCollectingCrashReportsFromDirectory()
{
    dir = new CReporterDaemonCoreDir(testMountPoint2);

    QString coreDirectory = QString(testMountPoint2);
    coreDirectory.append("/core-dumps");
    dir->setDirectory(coreDirectory);
    dir->createCoreDirectory();

    QDir::setCurrent(coreDirectory);

    QFile richCore1;
    richCore1.setFileName("application.rcore.lzo");
    richCore1.open(QIODevice::ReadWrite);
    richCore1.close();

    QFile richCore2;
    richCore2.setFileName("test.rcore.lzo");
    richCore2.open(QIODevice::ReadWrite);
    richCore2.close();

    QFile richCoreInvalid;
    richCoreInvalid.setFileName(".test.rcore.lzo");
    richCoreInvalid.open(QIODevice::ReadWrite);
    richCoreInvalid.close();

    QStringList files;
    dir->collectAllCoreFilesAtLocation(files);

    QCOMPARE(files.count(), 2);
}

void Ut_CReporterDaemonCoreDir::testCheckDirectoryForNewCrashReport()
{
     dir = new CReporterDaemonCoreDir(testMountPoint2);

    QString coreDirectory = QString(testMountPoint2);
    coreDirectory.append("/core-dumps");
    dir->setDirectory(coreDirectory);
    dir->createCoreDirectory();

    QDir::setCurrent(coreDirectory);
    QFile richCore;
    richCore.setFileName("rich-core-application.rcore.lzo");
    richCore.open(QIODevice::ReadWrite);
    richCore.close();

    QString newFile = dir->checkDirectoryForCores();

    QVERIFY(newFile == coreDirectory.append("/rich-core-application.rcore.lzo"));
}

void Ut_CReporterDaemonCoreDir::cleanupTestCase()
{
    QDir::setCurrent(QDir::homePath());
    QString rm = QString("rm -r -f /tmp/crash-reporter-tests");
    system(rm.toLatin1());
}

void Ut_CReporterDaemonCoreDir::cleanup()
{
    QDir::setCurrent(QDir::homePath());
    QString rm = QString("rm -r -f %1 %2").arg(testMountPoint1).arg(testMountPoint2);
    system(rm.toLatin1());

    if (dir) {
        delete dir;
        dir = 0;
    }
}

QTEST_MAIN(Ut_CReporterDaemonCoreDir)
