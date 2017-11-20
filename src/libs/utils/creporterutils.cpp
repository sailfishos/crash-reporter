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

#include <sys/types.h> // for stat()
#include <sys/stat.h>

#ifndef CREPORTER_UNIT_TEST
#include <ssudeviceinfo.h>
#endif

#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QProcess>

#include "creporterutils.h"

#include "creporternamespace.h"
#include "../autouploader_interface.h" // generated
#include "../ssu_interface.h" // generated

namespace CReporter {
namespace LoggingCategory {
Q_LOGGING_CATEGORY(cr, "creporter", QtInfoMsg)
}
}

using CReporter::LoggingCategory::cr;

const QString richCoreTmpNoteFile = "/tmp/rich-core-note.txt";
const QString coreSuffixRcore = "rcore";
const QString coreSuffixRcoreLzo = "rcore.lzo";

CReporterUtils::CReporterUtils() {}

bool CReporterUtils::validateCore(const QString &path)
{
    return (path.endsWith(coreSuffixRcoreLzo, Qt::CaseInsensitive) ||
            path.endsWith(coreSuffixRcore, Qt::CaseInsensitive));
}

char *CReporterUtils::qstringToChar(const QString &str)
{
    return str.toLatin1().data();
}

bool CReporterUtils::isMounted(const QString &path)
{
#if defined(CREPORTER_SDK_HOST) || defined(CREPORTER_UNIT_TEST)
    Q_UNUSED(path);
    // Skip, if running in scratchbox.
    qCDebug(cr) << "Scratchbox target -> skip check and return true";
    return true;
#else
    struct stat st;
    memset(&st, 0, sizeof(st));

    if (stat(CReporterUtils::qstringToChar(path), &st) == 0) {
        qCDebug(cr) << "Path:" << path << "is mounted. Device ID:" << st.st_dev;
        return true;
    }

    qCDebug(cr) << "Path:" << path << "not mounted.";
    return false;
#endif // defined(CREPORTER_SDK_HOST) || defined(CREPORTER_UNIT_TEST)
}

bool CReporterUtils::removeFile(const QString &path)
{
    QFileInfo fi(path);
    qCDebug(cr) << "Removing file:" << fi.absoluteFilePath();
    return QFile::remove(fi.absoluteFilePath());
}

QStringList CReporterUtils::parseCrashInfoFromFilename(const QString &filePath)
{
    qCDebug(cr) << "Parse:" << filePath;

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
    qCDebug(cr) << "PID:" << pid;

    // Remove dash and PID from the original basename.
    baseName = baseName.remove(searchIndex, pid.size() + 1);

    // Repeat above for SIGNUM, HWID and IMEI. Finally, string what is left in the basename,
    // is the application name.

    // Extract SIGNUM.
    searchIndex = baseName.lastIndexOf("-");
    QString signum = baseName.right(baseName.size() - (searchIndex + 1));
    qCDebug(cr) << "SIGNUM:" << signum;

    baseName = baseName.remove(searchIndex, signum.size() + 1);

    // Extract HWID.
    searchIndex = baseName.lastIndexOf("-");
    QString hwid = baseName.right(baseName.size() - (searchIndex + 1));
    qCDebug(cr) << "HWID:" << hwid;

    baseName = baseName.remove(searchIndex, hwid.size() + 1);

    qCDebug(cr) << "Application name:" << baseName;

    // Append results to list. Index 0 = Application name ....
    QStringList result;
    result << baseName << hwid << signum << pid;

    return result;
}

bool CReporterUtils::appendToLzo(const QString &text, const QString &filePath)
{
    QFile tmpFile(richCoreTmpNoteFile);
    // Create local temp file, where comments are written.
    if (!tmpFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qCWarning(cr) << "Unable to open file:" << filePath;
        return false;
    }

    QTextStream out(&tmpFile);
    out << text;
    tmpFile.close();

    // Append to *.lzo.
    QString cmd = QString("/usr/bin/lzop -c %1 >> %2").arg(richCoreTmpNoteFile).arg(filePath) ;
    int res = system(qstringToChar(cmd));
    qCDebug(cr) << "System returned:"  << res;

    // Remove temp file.
    if (!tmpFile.remove()) {
        qCWarning(cr) << "Unable to remove file:" << richCoreTmpNoteFile;
    }
    return true;
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
    if (reply.isError()) {
        qCWarning(cr) << "DBus unavailable, UUID might be incorrect.";
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
    return !(fileName.contains(CReporter::QuickFeedbackPrefix) ||
             fileName.contains(CReporter::EndurancePackagePrefix) ||
             fileName.contains(CReporter::PowerExcessPrefix) ||
             fileName.contains(CReporter::OneshotFailurePrefix) ||
             fileName.contains(CReporter::HWrebootPrefix) ||
             fileName.contains(CReporter::HWSMPLPrefix) ||
             fileName.contains(CReporter::OverheatShutdownPrefix) ||
             fileName.contains(CReporter::JournalSpyPrefix));
}

bool CReporterUtils::notifyAutoUploader(const QStringList &filesToUpload,
                                        bool obeyNetworkRestrictions)
{
    qCDebug(cr) << "Requesting crash-reporter-autouploader to upload"
                << filesToUpload.size() << "files.";

    ComNokiaCrashReporterAutoUploaderInterface proxy(CReporter::AutoUploaderServiceName,
            CReporter::AutoUploaderObjectPath, QDBusConnection::sessionBus());

    QDBusPendingReply <bool>reply =
        proxy.uploadFiles(filesToUpload, obeyNetworkRestrictions);
    // This blocks.
    reply.waitForFinished();

    if (reply.isError()) {
        qCWarning(cr) << "D-Bus error occurred:" << reply.error().name() << reply.error().message();

        return false;
    }
    return true;
}

QProcess *CReporterUtils::invokeLogCollection(const QString &label)
{
    QScopedPointer<QProcess> richCoreHelper(new QProcess(qApp));

    richCoreHelper->start("/usr/libexec/rich-core-helper",
                          QStringList() << label);
    if (!richCoreHelper->waitForStarted()) {
        qCWarning(cr) << "Problem invoking rich-core-dumper.";
        return 0;
    }

    typedef void (QProcess::*FinishedSignal)(int, QProcess::ExitStatus);
    connect(richCoreHelper.data(), (FinishedSignal) &QProcess::finished,
            richCoreHelper.data(), &QProcess::deleteLater);

    return richCoreHelper.take();
}

static void runServiceHelper(const QString &service, bool run)
{
    QProcess::execute("/usr/libexec/crashreporter-servicehelper", QStringList() << service << (run ? "start" : "stop"));
}

void CReporterUtils::setEnduranceServiceState(bool run)
{
    runServiceHelper(QStringLiteral("endurance"), run);
}

void CReporterUtils::setJournalSpyServiceState(bool run)
{
    runServiceHelper(QStringLiteral("journalspy"), run);
}

CReporterUtils *CReporterUtils::instance()
{
    /* Will be set to zero when someone outside calls delete on the object. This
     * happens when qmlRegisterSingletonType() is used with the instance. */
    static QPointer<CReporterUtils> instance;
    if (!instance) {
        instance = new CReporterUtils;
    }

    return instance.data();
}
