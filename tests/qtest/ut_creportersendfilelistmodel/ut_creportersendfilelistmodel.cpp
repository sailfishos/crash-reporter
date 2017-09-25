/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Raimo Gratseff <ext-raimo.gratseff@nokia.com>
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

#include <cstdlib>
#include <QDir>
#include <QFile>
#include <QString>
#include <QFileInfo>
#include <QModelIndex>
#include <MLocale>

#include "ut_creportersendfilelistmodel.h"
#include "creportersendfilelistmodel.h"

#include "creporterutils.h"

const QString tempFileDir = "/tmp/ut_creportersendfilelistmodel";
const int numFiles = 8;

static bool fileSizeToStringCalled = false;
static QStringList filenames;


// CReporterUtils::fileSizeToString Mock function
QString CReporterUtils::fileSizeToString(quint64 size)
{
    Q_UNUSED(size);
    fileSizeToStringCalled = true;
    return "1 MB";
}

void Ut_CReporterSendFileListModel::init()
{
    QDir dir;
    dir.mkpath(tempFileDir);

    QFile file;
    QString filename;

    for (int i = 0; i < numFiles; i++) {
        filename = tempFileDir + "/testfile" + QString::number(i) + ".extension";
        filenames << filename;
        file.setFileName(filename);
        file.open(QIODevice::WriteOnly);
        file.close();
    }

    testModel = new CReporterSendFileListModel(filenames);
}

void Ut_CReporterSendFileListModel::initTestCase()
{
}

void Ut_CReporterSendFileListModel::testRowCount()
{
    QVERIFY(testModel->rowCount() == numFiles);
}

void Ut_CReporterSendFileListModel::testData()
{
    MLocale locale;
    QString details;
    QStringList rowData;
    QFileInfo fileinfo;

    for (int i = 0; i < numFiles; i++) {
        rowData = testModel->data(testModel->index(i), Qt::DisplayRole).toStringList();
        fileinfo.setFile(filenames.at(i));
        details = CReporterUtils::fileSizeToString(fileinfo.size()) + ' '
                  + locale.formatDateTime(fileinfo.created(), MLocale::DateMedium, MLocale::TimeMedium);
        QVERIFY(rowData.at(0) == "testfile" + QString::number(i));
        QVERIFY(rowData.at(1) == details);
        QVERIFY(rowData.at(2) == filenames.at(i));
    }
}

void Ut_CReporterSendFileListModel::testWrongRole()
{
    QVERIFY(testModel->data(testModel->index(numFiles + 10), Qt::EditRole).isNull());
}

void Ut_CReporterSendFileListModel::cleanupTestCase()
{
}

void Ut_CReporterSendFileListModel::cleanup()
{
    delete testModel;
    QString command = "rm -rf ";
    command += tempFileDir;
    system(command.toLocal8Bit());
}


QTEST_MAIN(Ut_CReporterSendFileListModel)
