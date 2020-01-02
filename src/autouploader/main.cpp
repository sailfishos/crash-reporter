/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Raimo Gratseff <ext-raimo.gratseff@nokia.com>
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

#include <QCoreApplication>
#include <QTranslator>
#include <QLocale>
#include <QDebug>

#include "creporterautouploader.h"
#include "creporternamespace.h"
#include "creporterutils.h"
#include "creporterapplicationsettings.h"

#ifndef QT_NO_DEBUG_OUTPUT
#include "creporterlogger.h"
#endif

using CReporter::LoggingCategory::cr;

int main(int argc, char **argv)
{
#ifndef QT_NO_DEBUG_OUTPUT
    Logger logger(CReporterApplicationSettings::instance()->loggerType());
#endif

    QCoreApplication app(argc, argv);

    QTranslator *engineeringEnglish = new QTranslator(&app);
    engineeringEnglish->load("crash-reporter_eng_en", "/usr/share/translations");
    app.installTranslator(engineeringEnglish);

    QTranslator *translator = new QTranslator(&app);
    translator->load(QLocale(), "crash-reporter", "-", "/usr/share/translations");
    app.installTranslator(translator);

    qCDebug(cr) << CReporter::AutoUploaderBinaryName << "[" << app.applicationPid() << "]" << "started.";
    qCDebug(cr) << "Crash Reporter version is " << QString(CREPORTERVERSION);

    CReporterAutoUploader uploader;

    int retVal = app.exec();

    CReporterApplicationSettings::instance()->freeSingleton();
    qCDebug(cr) << "Shutting down Auto Uploader process.";
    return retVal;
}
