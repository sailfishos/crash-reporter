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

// System includes.

#include <signal.h>

#include <QDebug>

#include <MApplication>
///#include <MWindow>
#include <MApplicationService>
///#include <MLocale>

// User includes.

#include "creporterautouploader.h"
#include "creporternamespace.h"
///#include "creporterutils.h"
#include "creporterapplicationsettings.h"

#ifndef QT_NO_DEBUG_OUTPUT
#include "creporterlogger.h"
#endif // QT_NO_DEBUG_OUTPUT

extern char *g_progname;

/*! 
 * @class CReporterMApplicationService
 * @brief Overriden M application service class to disable application service.
 *
 */
class CReporterMApplicationService : public MApplicationService
{
    public:
        CReporterMApplicationService(QObject *parent = 0) :
            MApplicationService("com.nokia.crash-reporter-autouploader", parent) {
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
  * @brief Crash Reporter Auto Uploader main function.
  *
  * @param argc Argument count.
  * @param argv Arguments.
  */
int main(int argc, char **argv)
{
#ifndef QT_NO_DEBUG_OUTPUT
        g_progname = (char *) "crash-reporter-autouploader";
        // Determine logging method.
        QString type = CReporterApplicationSettings::instance()->value(Logging::ValueLoggerType,
                                      DefaultApplicationSettings::ValueLoggerTypeDefault).toString();
        Logger logger(type);
#endif // QT_NO_DEBUG_OUTPUT

    // Create new MApplication and service.
    MApplication app(argc, argv, CReporter::AutoUploaderBinaryName,
        new CReporterMApplicationService());

    qDebug() << __PRETTY_FUNCTION__  << CReporter::AutoUploaderBinaryName << "[" << app.applicationPid()
        << "]" << "started.";

    qDebug() << __PRETTY_FUNCTION__ << "Crash Reporter version is " << QString(CREPORTERVERSION);

    CReporterAutoUploader uploader;

    // Enter Qt main loop.
    int retVal = app.exec();

    CReporterApplicationSettings::instance()->freeSingleton();
    qDebug() << __PRETTY_FUNCTION__ << "Shutting down Auto Uploader process.";
    return retVal;
}

// End of file.

