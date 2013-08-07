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

#include "stdlib.h"

#include <QDir>
#include <QSettings>

#include "ut_creporterapplicationsettings.h"
#include "creporterapplicationsettings.h"
#include "creportersettingsinit_p.h"

const QString systemSettingsPath("/tmp/crash-reporter-settings/system");
const QString userSettingsPath("/tmp/crash-reporter-settings/user");

void Ut_CReporterApplicationSettings::initTestCase()
{
    // Create test directories.
    QDir dir;
    dir.mkpath(systemSettingsPath);
    dir.mkpath(userSettingsPath);
}

void Ut_CReporterApplicationSettings::init()
{
    creporterSettingsInit(systemSettingsPath, userSettingsPath);

    QSettings systemSettings(QSettings::NativeFormat, QSettings::SystemScope, "crash-reporter-settings",
                             "crash-reporter");
    systemSettings.setValue(Server::ValueServerAddress, "127.0.0.1");
    systemSettings.setValue(Server::ValueServerPort, 8080);
    systemSettings.sync();

    QSettings userSettings(QSettings::NativeFormat, QSettings::UserScope, "crash-reporter-settings",
                             "crash-reporter");
    userSettings.setValue(Proxy::ValueProxyAddress, "127.0.0.2");
    userSettings.setValue(Proxy::ValueProxyPort, 1234);
    userSettings.sync(); 
}

void Ut_CReporterApplicationSettings::testReadSettings()
{
    // Test reading settings.
    const CReporterApplicationSettings *const settings = CReporterApplicationSettings::instance();

    // Read from system scope location.
    QVERIFY(settings->serverUrl() == "127.0.0.1");
    QVERIFY(settings->serverPort() == 8080);

    // Read from user scope location.
    QVERIFY(settings->proxyUrl() == "127.0.0.2");
    QVERIFY(settings->proxyPort() == 1234);
}

void Ut_CReporterApplicationSettings::testReadNotFoundSettings()
{
    // Test reading not found settings.
    const CReporterApplicationSettings *const settings = CReporterApplicationSettings::instance();

    // Value is not found from settings; default value is returned.
    QVERIFY(settings->serverPath() == DefaultApplicationSettings::ValueServerPathDefault);
}

void Ut_CReporterApplicationSettings::cleanupTestCase()
{
    CReporterApplicationSettings::instance()->freeSingleton();
}

void Ut_CReporterApplicationSettings::cleanup()
{
    system("rm -rf /tmp/crash-reporter-settings");
}

QTEST_MAIN(Ut_CReporterApplicationSettings)
