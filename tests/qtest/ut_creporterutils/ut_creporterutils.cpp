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

#include <QFileInfo>
#include <QDir>

#include "creporterutils.h"
#include "ut_creporterutils.h"

void Ut_CReporterUtils::init()
{
}

void Ut_CReporterUtils::initTestCase()
{
}

void Ut_CReporterUtils::cleanup()
{
}

void Ut_CReporterUtils::cleanupTestCase()
{
}

void Ut_CReporterUtils::testValidateCore()
{
    // Valid core.
    bool retVal = CReporterUtils::validateCore(
            QFileInfo("/media/mmc1/core-dumps/test-1234-11-4321.rcore.lzo"));
    QVERIFY(retVal == true);

    // Valid core with a '.' in the file name.
    retVal = CReporterUtils::validateCore(
            QFileInfo("/media/mmc1/core-dumps/test.launch-1234-11-4321.rcore.lzo"));
    QVERIFY(retVal == true);

    // Partial core.
    retVal = CReporterUtils::validateCore(
            QFileInfo("/media/mmc1/core-dumps/.test-1234-11-4321.rcore.lzo"));
    QVERIFY(retVal == false);

    /*// Crash Reporter core.
    retVal = CReporterUtils::validateCore(
            QFileInfo("/media/mmc1/core-dumps/crash-reporter-daemon-1234-11-4321.rcore.lzo"));
    QVERIFY(retVal == false);

    // Crash Reporter core.
    retVal = CReporterUtils::validateCore(
            QFileInfo("/media/mmc1/core-dumps/crash-reporter-ui-1234-11-4321.rcore.lzo"));
    QVERIFY(retVal == false);

    // Crash Reporter core.
    retVal = CReporterUtils::validateCore(
            QFileInfo("/media/mmc1/core-dumps/crash-reporter-autouploader-1234-11-4321.rcore.lzo"));
    QVERIFY(retVal == false);*/

    // Invalid file.
    retVal = CReporterUtils::validateCore(
            QFileInfo("/media/mmc1/core-dumps/test-1234-11-4321.tar"));
    QVERIFY(retVal == false);
}

void Ut_CReporterUtils::testRemoveFile()
{
    QDir::setCurrent(QDir::homePath());
    QFile file;
    file.setFileName("test-1234-11-4321.rcore.lzo");
    file.open(QIODevice::ReadWrite);
    file.close();

    QString path = QDir::homePath() +"/" + file.fileName();
    bool retVal = CReporterUtils::removeFile(path);
    QVERIFY(retVal == true);
}

void Ut_CReporterUtils::testParseCrashInfoFromFilename()
{
    QString path = "/media/mmc1/core-dumps/application-1234-11-4321.rcore.lzo";
    QStringList info = CReporterUtils::parseCrashInfoFromFilename(path);

    QVERIFY(info.at(0) == "application");
    QVERIFY(info.at(1) == "1234");
    QVERIFY(info.at(2) == "11");
    QVERIFY(info.at(3) == "4321");
}

void Ut_CReporterUtils::testFileSizeToString()
{
    QString sizeToStr = CReporterUtils::fileSizeToString(0);
    QVERIFY(sizeToStr == "0 kB");

    sizeToStr = CReporterUtils::fileSizeToString(2279196);
    QVERIFY(sizeToStr == "2.2 MB" || sizeToStr == "2,2 MB");

    sizeToStr = CReporterUtils::fileSizeToString(1024);
    QVERIFY(sizeToStr == "1 kB");
}

QTEST_MAIN(Ut_CReporterUtils)
