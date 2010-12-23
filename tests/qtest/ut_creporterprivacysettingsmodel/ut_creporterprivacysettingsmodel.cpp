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
#include <QSignalSpy>
#include <QVariant>
#include <QDebug>
#include "ut_creporterprivacysettingsmodel.h"
#include "creporterprivacysettingsmodel.h"
#include "creportersettingsinit_p.h"

const QString systemSettingsPath("/tmp/crash-reporter-settings/system");
const QString userSettingsPath("/tmp/crash-reporter-settings/user");
const QString settingsFile("/tmp/crash-reporter-settings/user/crash-reporter-settings/crash-reporter-privacy.conf");

void Ut_CReporterPrivacySettingsModel::initTestCase()
{
    // Create test directories.
    QDir dir;
    dir.mkpath(systemSettingsPath);
    dir.mkpath(userSettingsPath);
}

void Ut_CReporterPrivacySettingsModel::init()
{
    creporterSettingsInit(systemSettingsPath, userSettingsPath);

    m_settings = new QSettings(settingsFile, QSettings::NativeFormat);

    m_settings->setValue(Settings::ValueSending, false);
    m_settings->setValue(Settings::ValueCoreDumping, true);
    m_settings->setValue(Settings::ValueAutoDeleteDuplicates, true);
    m_settings->setValue(Settings::ValueLifelog, false);
    m_settings->setValue(Settings::ValueAutomaticSending, true);

    m_settings->setValue(Privacy::ValueIncludeCore, true);
    m_settings->setValue(Privacy::ValueIncludeSysLog, false);
    m_settings->setValue(Privacy::ValueIncludePkgList, true);

    m_settings->sync();
}

void Ut_CReporterPrivacySettingsModel::testReadSettings()
{
    bool enabled;
    QVERIFY(CReporterPrivacySettingsModel::instance()->settingsFile() == settingsFile);
    QVERIFY(CReporterPrivacySettingsModel::instance()->isValid()== true);

    enabled = CReporterPrivacySettingsModel::instance()->sendingEnabled();
    QVERIFY(enabled == false);

    enabled = CReporterPrivacySettingsModel::instance()->coreDumpingEnabled();
    QVERIFY(enabled == true);

    enabled = CReporterPrivacySettingsModel::instance()->autoDeleteDuplicates();
    QVERIFY(enabled == true);

    enabled = CReporterPrivacySettingsModel::instance()->lifelogEnabled();
    QVERIFY(enabled == false);

    enabled = CReporterPrivacySettingsModel::instance()->automaticSendingEnabled();
    QVERIFY(enabled == true);

    enabled = CReporterPrivacySettingsModel::instance()->includeCore();
    QVERIFY(enabled == true);

    enabled = CReporterPrivacySettingsModel::instance()->includeSystemLog();
    QVERIFY(enabled == false);

    enabled = CReporterPrivacySettingsModel::instance()->includePackageList();
    QVERIFY(enabled == true);
}

void Ut_CReporterPrivacySettingsModel::testWriteSettings()
{
    QSignalSpy valueChangedSpy(CReporterPrivacySettingsModel::instance(),
                               SIGNAL(valueChanged(QString,QVariant)));

    CReporterPrivacySettingsModel::instance()->setSendingEnabled(true);
    CReporterPrivacySettingsModel::instance()->setCoreDumpingEnabled(false);
    CReporterPrivacySettingsModel::instance()->setAutoDeleteDuplicates(false);
    CReporterPrivacySettingsModel::instance()->setAutomaticSendingEnabled(false);

    CReporterPrivacySettingsModel::instance()->setIncludeCore(false);
    CReporterPrivacySettingsModel::instance()->setIncludeSystemLog(true);
    CReporterPrivacySettingsModel::instance()->setIncludePackageList(false);

    QVERIFY(valueChangedSpy.count() == 7);

    CReporterPrivacySettingsModel::instance()->writeSettings();

    QVERIFY(m_settings->value(Settings::ValueSending).toBool() == true);
    QVERIFY(m_settings->value(Settings::ValueCoreDumping).toBool() == false);
    QVERIFY(m_settings->value(Settings::ValueAutoDeleteDuplicates).toBool() == false);
    QVERIFY(m_settings->value(Settings::ValueAutomaticSending).toBool() == false);

    QVERIFY(m_settings->value(Privacy::ValueIncludeCore).toBool() == false);
    QVERIFY(m_settings->value(Privacy::ValueIncludeSysLog).toBool() == true);
    QVERIFY(m_settings->value(Privacy::ValueIncludePkgList).toBool() == false);
}

void Ut_CReporterPrivacySettingsModel::cleanupTestCase()
{
}

void Ut_CReporterPrivacySettingsModel::cleanup()
{
    if (m_settings != 0) {
        delete m_settings;
        m_settings = 0;
    }

    system("rm -rf /tmp/crash-reporter-settings");
}

QTEST_MAIN(Ut_CReporterPrivacySettingsModel)
