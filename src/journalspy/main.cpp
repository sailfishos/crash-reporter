/*
 * This file is a part of crash-reporter.
 *
 * Copyright (C) 2014 Jolla Ltd.
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
 */

#include <signal.h>

#include <QCoreApplication>
#include <QDebug>
#include <QFileInfo>

#include "journalspy.h"
#include "creporterapplicationsettings.h"
#include "creporterlogger.h"
#include "creporterutils.h"

using CReporter::LoggingCategory::cr;

void signalHandler(int signal)
{
    Q_UNUSED(signal)

    QCoreApplication::exit(0);
}

int main(int argc, char **argv)
{
    signal(SIGINT, &signalHandler);
    signal(SIGTERM, &signalHandler);

    QCoreApplication app(argc, argv);

    qCDebug(cr) << qPrintable(QFileInfo(argv[0]).fileName()) << CREPORTERVERSION
                << "[" << app.applicationPid() << "] started.";

    JournalSpy journalSpy;

    int result = app.exec();

    qCDebug(cr) << "Shutting down the journal spy process.";

    return result;
}
