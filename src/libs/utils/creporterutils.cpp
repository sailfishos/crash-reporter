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

// System includes.

#include <sys/types.h> // for stat()
#include <sys/stat.h>

#ifndef CREPORTER_UNIT_TEST
#include <ssudeviceinfo.h>
#endif

#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QFile>

// User includes.

#include "creporterutils.h"

#include "creporterautouploaderproxy.h"
#include "creporternamespace.h"
#include "../ssu_interface.h" // generated

// Local constants.

const QString richCoreTmpNoteFile = "/tmp/rich-core-note.txt";
const QString coreSuffixRcore = "rcore";
const QString coreSuffixRcoreLzo = "rcore.lzo";

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterUtils::validateCore
// ----------------------------------------------------------------------------
bool CReporterUtils::validateCore(const QString &path)
{
    return (path.endsWith(coreSuffixRcoreLzo, Qt::CaseInsensitive) ||
            path.endsWith(coreSuffixRcore, Qt::CaseInsensitive));
}

// ----------------------------------------------------------------------------
// CReporterUtils::qstringToChar
// ----------------------------------------------------------------------------
char* CReporterUtils::qstringToChar(const QString &str)
{
	return str.toLatin1().data();
}

// ----------------------------------------------------------------------------
// CReporterUtils::isMounted
// ----------------------------------------------------------------------------
bool CReporterUtils::isMounted(const QString &path)
{
#if defined(CREPORTER_SDK_HOST) || defined(CREPORTER_UNIT_TEST)
    Q_UNUSED(path);
	// Skip, if running in scratchbox.
	qDebug() << __PRETTY_FUNCTION__ << "Scratchbox target -> skip check and return true";
	return true;	
#else
	struct stat st;
    memset(&st, 0, sizeof(st));
    
    if (stat(CReporterUtils::qstringToChar(path), &st) == 0) {
		qDebug() << __PRETTY_FUNCTION__ << "Path:" << path << "is mounted. Device ID:" << st.st_dev;	
		return true;
    }

	qDebug() << __PRETTY_FUNCTION__ << "Path:" << path << "not mounted.";
	return false;
#endif // defined(CREPORTER_SDK_HOST) || defined(CREPORTER_UNIT_TEST)
}

// ----------------------------------------------------------------------------
// CReporterUtils::removeFile
// ----------------------------------------------------------------------------
bool CReporterUtils::removeFile(const QString &path)
{
    QFileInfo fi(path);
    qDebug() << __PRETTY_FUNCTION__ << "Removing file:" << fi.absoluteFilePath();
    return QFile::remove(fi.absoluteFilePath());
}

// ----------------------------------------------------------------------------
// CReporterUtils::parseCrashInfoFromFilename
// ----------------------------------------------------------------------------
QStringList CReporterUtils::parseCrashInfoFromFilename(const QString &filePath)
{
    qDebug() << __PRETTY_FUNCTION__ << "Parse:" << filePath;

    QFileInfo fi(filePath);
    // Remove path and file suffix. i.e. /media/mmc1/application-hwid-11-2029.rcore.lzo =>
    // application-hwid-11-2029
    QString baseName = fi.baseName();

    /*
     * The basename format is: application_name-hwid-11-2029
     *
     * Parsing from start until dash '-', because it may appear as part of executable name.
     * Instead, walk from end to beginning.
     */

    // Find the last dash. Start from the last character.
    int searchIndex = baseName.lastIndexOf("-");
    // Extracts PID substring.
    QString pid = baseName.right(baseName.size() - (searchIndex + 1));
    qDebug() << __PRETTY_FUNCTION__ << "PID:" << pid;

    // Remove dash and PID from the original basename.
    baseName = baseName.remove(searchIndex, pid.size() + 1);

    // Repeat above for SIGNUM, HWID and IMEI. Finally, string what is left in the basename,
    // is the application name.

    // Extract SIGNUM.
    searchIndex = baseName.lastIndexOf("-");
    QString signum = baseName.right(baseName.size() - (searchIndex + 1));
    qDebug() << __PRETTY_FUNCTION__ << "SIGNUM:" << signum;

    baseName = baseName.remove(searchIndex, signum.size() + 1);

    // Extract HWID.
    searchIndex = baseName.lastIndexOf("-");
    QString hwid = baseName.right(baseName.size() - (searchIndex + 1));
    qDebug() << __PRETTY_FUNCTION__ << "HWID:" << hwid;

    baseName = baseName.remove(searchIndex, hwid.size() + 1);

    qDebug() << __PRETTY_FUNCTION__ << "Application name:" << baseName;

    // Append results to list. Index 0 = Application name ....
    QStringList result;
    result << baseName << hwid << signum << pid;

    return result;
}

// ----------------------------------------------------------------------------
// CReporterUtils::appendToLzo
// ----------------------------------------------------------------------------
bool CReporterUtils::appendToLzo(const QString &text, const QString &filePath)
{
    QFile tmpFile(richCoreTmpNoteFile);
    // Create local temp file, where comments are written.
    if (!tmpFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
          qDebug() << __PRETTY_FUNCTION__ << "Unable to open file:" << filePath;
          return false;
          }

    QTextStream out(&tmpFile);
    out << text;
    tmpFile.close();

    // Append to *.lzo.
    QString cmd = QString("/usr/bin/lzop -c %1 >> %2").arg(richCoreTmpNoteFile).arg(filePath) ;
    int res = system(qstringToChar(cmd));
    qDebug() << __PRETTY_FUNCTION__ << "System returned:"  << res;

    // Remove temp file.
    if (!tmpFile.remove()) {
        qDebug() << __PRETTY_FUNCTION__ << "Unable to remove file:" << richCoreTmpNoteFile;
    }
    return true;
}

// ----------------------------------------------------------------------------
// CReporterUtils::fileSizeToString
// ----------------------------------------------------------------------------
QString CReporterUtils::fileSizeToString(const quint64 size)
{
    QString text;

    if (size == 0) {
        text = "0 kB";
    }
    else {
        double fSize = ((double)size) / 1024.0;
        int temp = 1;
        // Round smaller than 1 kB to 1 kB
        if (size != 0 && fSize < 1.0) {
            fSize = 1.0;
        }

        while (fSize > 1024.0 && temp < 2) {
            fSize = fSize / 1024.0;
            ++temp;
        }
        // Show one decimal.
        QString formattedNumber(QString("Value: %L1").arg(fSize, 0, 'f', 1));
        if (temp == 1) {
            text = QString("%1 kB").arg(formattedNumber);
        }
        else {
            text = QString("%1 MB").arg(formattedNumber);
        }
    }
    return text;
}

QString CReporterUtils::deviceUid()
{
#ifndef CREPORTER_UNIT_TEST
    static OrgNemoSsuInterface *ssuProxy = 0;
    if (!ssuProxy) {
        ssuProxy = new OrgNemoSsuInterface("org.nemo.ssu", "/org/nemo/ssu",
                QDBusConnection::systemBus(), qApp);
    }

    QDBusPendingReply<QString> reply = ssuProxy->deviceUid();
    reply.waitForFinished();
    if (reply.isError()){
        qDebug() << __PRETTY_FUNCTION__
                 << "DBus unavailable, UUID might be incorrect.";
        return SsuDeviceInfo().deviceUid();
    } else {
        return reply.value();
    }
#else
    return "1234";
#endif
}

QString CReporterUtils::deviceModel()
{
#ifndef CREPORTER_UNIT_TEST
    return SsuDeviceInfo().deviceModel();
#else
    return "Device";
#endif
}

bool CReporterUtils::reportIncludesCrash(const QString &fileName)
{
    return fileName.contains(CReporter::QuickFeedbackPrefix) ||
           fileName.contains(CReporter::EndurancePackagePrefix);
}

bool CReporterUtils::notifyAutoUploader(const QStringList &filesToUpload)
{
    qDebug() << __PRETTY_FUNCTION__
             << "Requesting crash-reporter-autouploader to upload"
             << filesToUpload.size() << "files.";

    CReporterAutoUploaderProxy proxy(CReporter::AutoUploaderServiceName,
            CReporter::AutoUploaderObjectPath, QDBusConnection::sessionBus());

    QDBusPendingReply <bool> reply = proxy.uploadFiles(filesToUpload);
    // This blocks.
    reply.waitForFinished();

    if (reply.isError()) {
        qWarning() << __PRETTY_FUNCTION__ << "D-Bus error occurred.";

        // Trace error.
        QDBusError dBusError(reply.error());

        qDebug() << __PRETTY_FUNCTION__ << "Name:" << dBusError.name();
        qDebug() << __PRETTY_FUNCTION__ << "Message:" << dBusError.message();
        qDebug() << __PRETTY_FUNCTION__
                 << "Error string:" << dBusError.errorString(dBusError.type());

        return false;
    }
    return true;
}
