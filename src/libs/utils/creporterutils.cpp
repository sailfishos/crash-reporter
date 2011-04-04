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

#include <sys/types.h> // for stat()
#include <sys/stat.h>

#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <MLocale>

// User includes.

#include "creporterutils.h"
#include "creporternamespace.h"

// Local constants.

const QString richCoreTmpNoteFile = "/tmp/rich-core-note.txt";
const QString coreSuffixRcore = "rcore";
const QString coreSuffixRcoreLzo = "rcore.lzo";

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterUtils::validateCore
// ----------------------------------------------------------------------------
bool CReporterUtils::validateCore(const QFileInfo &file)
{
	QString base = file.baseName(); // remove suffix
	qDebug() << __PRETTY_FUNCTION__ << "Base name:" << base;
	
    if (base.length() == 0) {
        // Ignore the partially created cores, these have "." -prefix
        // and thus the base name is empty.
		qDebug() << __PRETTY_FUNCTION__ << "Partial core -> Not valid.";
		return false;
	} 
    /*else if (base.lastIndexOf(CReporter::UIBinaryName) != -1 ||
             base.lastIndexOf(CReporter::DaemonBinaryName) != -1 ||
             base.lastIndexOf(CReporter::AutoUploaderBinaryName) != -1) {
		// Ignore crash_reporter_ui && crash_reporter_daemon cores.
        qDebug() << __PRETTY_FUNCTION__ << "crash-reporter* core -> Not valid.";
		return false;
    }*/
    else if (file.completeSuffix().endsWith(coreSuffixRcore, Qt::CaseInsensitive) ||
             file.completeSuffix().endsWith(coreSuffixRcoreLzo, Qt::CaseInsensitive)) {
		qDebug() << __PRETTY_FUNCTION__ << "Core file is valid.";
		return true;
	}
	else {
		qDebug() << __PRETTY_FUNCTION__ << "Not valid file.";
		return false;
	}
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
    qDebug() << __PRETTY_FUNCTION__ << "Removing file:" << path;
    QFileInfo fi(path);
    QDir::setCurrent(fi.absolutePath());

    return QFile::remove(fi.fileName());
}

// ----------------------------------------------------------------------------
// CReporterUtils::parseCrashInfoFromFilename
// ----------------------------------------------------------------------------
QStringList CReporterUtils::parseCrashInfoFromFilename(const QString &filePath)
{
    qDebug() << __PRETTY_FUNCTION__ << "Parse:" << filePath;

    MLocale locale;
    QFileInfo fi(filePath);
    // Remove path and file suffix. i.e. /media/mmc1/application-xxxx-11-2029.rcore.lzo =>
    // application-xxxx-11-2029
    QString baseName = fi.baseName();

    // Checking if the file is a lifelog report
    if (baseName.startsWith(CReporter::LifelogPackagePrefix))
    {
        qDebug() << __PRETTY_FUNCTION__ << "Lifelog report detected. Will not try to parse crash info.";
        return QStringList() << CReporter::LifelogPackagePrefix << QString() << QString() << QString();
    }

    /*
     * The basename format is: application_name-xxxx-11-2029.rcore.lzo.
     *
     * where xxxx are last four digits of device IMEI.
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

    // Repeat above for SIGNUM and IMEI. Finally, string what is left in the basename,
    // is the application name.

    // Extract SIGNUM.
    searchIndex = baseName.lastIndexOf("-");
    QString signum = baseName.right(baseName.size() - (searchIndex + 1));
    qDebug() << __PRETTY_FUNCTION__ << "SIGNUM:" << signum;

    baseName = baseName.remove(searchIndex, signum.size() + 1);

    // Extract IMEI.
    searchIndex = baseName.lastIndexOf("-");
    QString imei = baseName.right(baseName.size() - (searchIndex + 1));
    qDebug() << __PRETTY_FUNCTION__ << "IMEI:" << imei;

    baseName = baseName.remove(searchIndex, imei.size() + 1);
    qDebug() << __PRETTY_FUNCTION__ << "Application name:" << baseName;

    // Append results to list. Index 0 = Application name ....
    QStringList result;
    result << baseName << imei << signum << pid;

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
    MLocale locale;
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
        QString formattedNumber = locale.formatNumber(fSize, 1);
        if (temp == 1) {
            text = QString("%1 kB").arg(formattedNumber);
        }
        else {
            text = QString("%1 MB").arg(formattedNumber);
        }
    }
    return text;
}

// End of file
