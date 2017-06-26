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

#include <QDebug>
#include <QTime>
#include <stdlib.h>
#include <iostream>
#include <syslog.h>

// User includes.

#include "creporterlogger.h"

// Local constants and definitions.

#define CREPORTER_LOGGER_FILE       "file"
#define CREPORTER_LOGGER_SYSLOG     "syslog"

// Static initializations.

CReporterLogger *CReporterLogger::sm_Instance = 0;
bool CReporterLogger::sm_Syslog = false;
CReporter::LogType CReporterLogger::sm_LogType = CReporter::LogNone;

// External global variables.

char *g_progname;

// ******** Class CReporterLoggerPrivate ********

// ******** Class CReporterLogger ********

// ----------------------------------------------------------------------------
// CReporterLogger::CReporterLogger
// ----------------------------------------------------------------------------
CReporterLogger::CReporterLogger(const QString type)
{
    // save ourself in a static variable
    CReporterLogger::sm_Instance = this;

    if (type == CREPORTER_LOGGER_FILE) {
        CReporterLogger::sm_LogType = CReporter::LogFile;
    }
    else if (type == CREPORTER_LOGGER_SYSLOG) {
        CReporterLogger::sm_LogType = CReporter::LogSyslog;
    }

    switch (CReporterLogger::sm_LogType) {
        // Initialize logging.
        case CReporter::LogSyslog:
            // Init syslog.
            openlog(g_progname, LOG_PID, LOG_USER);
            break;
        case CReporter::LogFile:
            m_file.setFileName(CReporter::DefaultLogFile);
            // Open file for appending.
            if (!m_file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
                std::cerr << __PRETTY_FUNCTION__ << "Failed to open log file: " << qPrintable(CReporter::DefaultLogFile)
                          << " for appending." << std::endl;
                m_old_msg_handler = 0;
                return;
            }
            // Set stream.
            m_stream.setDevice(&m_file);
            break;
        case CReporter::LogNone: // TODO Means rather LogDefault than LogNone
            return;
    };
    // Register ourselves as a debug message handler
    m_old_msg_handler = qInstallMessageHandler(CReporterLogger::messageHandler);
}

// ----------------------------------------------------------------------------
// CReporterLogger::~CReporterLogger
// ----------------------------------------------------------------------------
CReporterLogger::~CReporterLogger ()
{
    if (m_old_msg_handler) {
        qInstallMessageHandler(m_old_msg_handler);
    }
    if (m_file.isOpen()) {
        m_file.close();
    }

    if (CReporterLogger::sm_LogType == CReporter::LogSyslog) {
        closelog();
    }
}

// ----------------------------------------------------------------------------
// CReporterLogger::messageHandler
// ----------------------------------------------------------------------------
void CReporterLogger::messageHandler(QtMsgType type,
                                     const QMessageLogContext &context,
                                     const QString &msg)
{
    Q_UNUSED(context);
    Q_ASSERT(CReporterLogger::sm_LogType != CReporter::LogNone);

    if (CReporterLogger::sm_LogType == CReporter::LogSyslog) {
        int msgLevel = LOG_DEBUG;

        switch (type) {
            case QtDebugMsg:
                msgLevel = LOG_DEBUG;
                break;
            case QtInfoMsg:
                msgLevel = LOG_INFO;
                break;
            case QtWarningMsg:
                msgLevel = LOG_WARNING;
                break;
            case QtCriticalMsg:
                msgLevel = LOG_CRIT;
                break;
            case QtFatalMsg:
                msgLevel = LOG_EMERG;
                break;
            };

            syslog(msgLevel, "%s", msg.toStdString().c_str());
    }
    else if (CReporterLogger::sm_LogType == CReporter::LogFile) {
        QString timeStamp = QTime::currentTime().toString(Qt::ISODate);
        QString msgType;

        switch (type) {
            case QtDebugMsg:
                msgType = ": [DEBUG]: ";
                break;
            case QtInfoMsg:
                msgType = ": [INFO]: ";
                break;
            case QtWarningMsg:
                msgType = ": [WARNING]: ";
                break;
            case QtCriticalMsg:
                msgType = ": [CRITICAL]: ";
                break;
            case QtFatalMsg:
                msgType = ": [FATAL]: ";
                break;
        };
        // Write to stream.
        CReporterLogger::sm_Instance->m_stream << timeStamp << msgType <<
                g_progname << ": " << msg << endl;
    }

    if (type == QtFatalMsg) {
        exit(1);
    }
}

// End of file.
