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

#include <signal.h>

#include <QDebug>

#include <MApplication>
#include <MWindow>
#include <MApplicationWindow>
#include <MApplicationPage>
#include <MApplicationService>
#include <MLocale>

// User includes.

#include "creporternotification.h"
#include "creporterdialogserver.h"
#include "creporternamespace.h"
#include "creporterutils.h"
#include "creporterapplicationsettings.h"
#ifndef QT_NO_DEBUG_OUTPUT
#include "creporterlogger.h"
#endif // QT_NO_DEBUG_OUTPUT

// Local definitions.

#define PLUGINS_DIR     "/usr/lib/crash-reporter/dialogplugins"
#define LOG_FILE        "/tmp/crash-reporter-ui.log"

extern char *g_progname;

/*!
  * @brief Overridden signal handler.
  *
  * Removes notifications, if process was killed unexpectedly.
  *
  * @param sig Received signal.
  */
void signal_handler(int sig)
{
    qDebug() << __PRETTY_FUNCTION__  << "Terminated with signal:" << sig;

    // Cleans up notifications, which have not been dismissed yet by the user.
    CReporterNotification::removeAll();

    // Reset default signal handler to get possible core dump.
    signal(sig, SIG_DFL);
    raise(sig);
}

/*! 
 * @class CReporterMApplicationService
 * @brief Overriden M application service class to disable application service.
 *
 */
class CReporterMApplicationService : public MApplicationService
{
    public:
        CReporterMApplicationService(QObject *parent = 0) :
            MApplicationService("com.nokia.crash-reporter-ui", parent) {
        }

        //! @reimp
        QString registeredName() {
            return QString();
        }
        //! @reimp
        bool isRegistered() {
            return false;
        }
        //! @reimp
        bool registerService() {
            return true;
        }
};

/*!
  * @brief Crash Reporter UI main function.
  *
  * @param argc Argument count.
  * @param argv Arguments.
  */
int main(int argc, char **argv)
{
#ifndef QT_NO_DEBUG_OUTPUT
    g_progname = (char *) "crash-reporter-ui";
    // Determine logging method.
    QString type = CReporterApplicationSettings::instance()->value(Logging::ValueLoggerType,
                                                    DefaultApplicationSettings::ValueLoggerTypeDefault).toString();
    Logger logger(type);
    qDebug() << __PRETTY_FUNCTION__  << "Debug logging started.";
#endif // QT_NO_DEBUG_OUTPUT

    // Setup handlers for signals.
    (void) signal(SIGSEGV, signal_handler);
    (void) signal(SIGTERM, signal_handler);
    (void) signal(SIGINT, signal_handler);
    (void) signal(SIGABRT, signal_handler);

    // Create new MApplication and service.
    MApplicationService *applicationService = new CReporterMApplicationService;
    MApplication app(argc, argv, CReporter::UIBinaryName, applicationService);

    qDebug() << __PRETTY_FUNCTION__  << CReporter::UIBinaryName << "[" << app.applicationPid()
        << "]" << "started.";

    qDebug() << __PRETTY_FUNCTION__ << "Crash Reporter version is " << QString(CREPORTERVERSION);

    // Set-up translation system.
    MLocale locale;
    locale.installTrCatalog("crash-reporter");
    MLocale::setDefault(locale);

    // Create an empty application window
    MApplicationWindow appwin;

    CReporterDialogServer server(PLUGINS_DIR, applicationService);
    QObject::connect(&appwin, SIGNAL(displayEntered()), &server, SLOT(destroyNotifications()));

    // Enter Qt main loop.
    int retVal = app.exec();

    qDebug() << __PRETTY_FUNCTION__ << "Returned from dialog server main loop";

    try
    {
        CReporterNotification::removeAll();
    }
    catch (...)
    {
        // Don't really care if MNotifications bug here
    }

    CReporterApplicationSettings::instance()->freeSingleton();
    return retVal;
}

// End of file.
