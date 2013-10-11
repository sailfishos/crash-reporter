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

#include <csignal>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QTimer>

// User includes.

#include "creporterdaemon.h"
#include "creporterutils.h"
#include "creporternamespace.h"
#include "creporterapplicationsettings.h"

#ifndef QT_NO_DEBUG_OUTPUT
#include "creporterlogger.h"
#endif // QT_NO_DEBUG_OUTPUT

// Local macros and definitions.

#define CREPORTER_PID_FILE      "/tmp/crash-reporter-daemon.pid"
#define CREPORTER_STARTUP_DELAY 30000 // 30 sec timer to delay start up

#ifndef QT_NO_DEBUG_OUTPUT
#define LOG_FILE    "/tmp/crash-reporter-daemon.log"
#endif // QT_NO_DEBUG_OUTPUT

extern char *g_progname;

/*!
 * @brief Gets crash-reporter-daemon pid and saves it to file.
 *
 * @param app Reference to application.
 * @return True, if first startup; otherwise false.
 */
bool getPid(QCoreApplication &app)
{
    QFile pidFile(CREPORTER_PID_FILE);
    qint64 pid = 0;
    bool firstStartup = true;

	// Get new PID.
	pid = app.applicationPid();
    qDebug() << __PRETTY_FUNCTION__  <<  CReporter::DaemonBinaryName
            << "[" << pid << "] starting...";

    if (pidFile.exists()) {
        firstStartup = false;
		qDebug() << __PRETTY_FUNCTION__ << "Removing stale PID file.";
		pidFile.remove();
	}
	
    if (pidFile.open(QIODevice::WriteOnly)) {
        QTextStream out(&pidFile);
		out << pid;
		pidFile.close();
	}

    qDebug() <<  __PRETTY_FUNCTION__ << "Startup delayed =" << firstStartup;
    return firstStartup;
}

void signalHandler(int signal) {
    Q_UNUSED(signal)

    QCoreApplication::exit(0);
}

/*!
  * @brief Crash Reporter daemon main function.
  *
  * @param argc Argument count.
  * @param argv Arguments.
  */
int main(int argc, char **argv)
{
    signal(SIGINT, &signalHandler);
    signal(SIGTERM, &signalHandler);

#ifndef QT_NO_DEBUG_OUTPUT
    g_progname = (char *) "crash-reporter-daemon";
    Logger logger(CReporterApplicationSettings::instance()->loggerType());
#endif // QT_NO_DEBUG_OUTPUT

    QCoreApplication app(argc, argv);

    bool firstStartup = getPid(app);
	
    CReporterDaemon daemon;

    if (firstStartup) {
        // If no PID file was found, delay startup.
        daemon.setDelayedStartup(CREPORTER_STARTUP_DELAY);
    }
    else {
        if (!daemon.initiateDaemon()) {
            // Connecting to D-BUS user session most propably failed.
            // Remove PID file (to delay next startup) and quit application.
            QFile::remove(CREPORTER_PID_FILE);
            return EXIT_FAILURE;
        }
    }

    qDebug() << __PRETTY_FUNCTION__ << "Crash Reporter version is " << QString(CREPORTERVERSION);

    // Enter Qt main loop.
    int retVal = app.exec();

    CReporterApplicationSettings::instance()->freeSingleton();
    return retVal;
}

// End of file.
