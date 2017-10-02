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

#include <QSignalSpy>
#include <QFile>

#include "ut_creportersettingsobserver.h"
#include "creportersettingsobserver.h"
#include "creporterprivacysettingsmodel.h"

const QString testSettingsFile("/tmp/crash-reporter-privacy.conf");

void Ut_CReporterSettingsObserver::init()
{
    m_settings = new QSettings(testSettingsFile, QSettings::NativeFormat);

    // Create test data.
    m_settings->setValue(Settings::ValueNotifications, true);
    m_settings->setValue(Settings::ValueCoreDumping, true);
    m_settings->setValue(Settings::ValueAutoDeleteDuplicates, true);

    m_settings->setValue(Privacy::ValueIncludeCore, true);
    m_settings->setValue(Privacy::ValueIncludeSysLog, true);
    m_settings->setValue(Privacy::ValueIncludePkgList, true);

    m_subject = new CReporterSettingsObserver(testSettingsFile);
}

void Ut_CReporterSettingsObserver::initTestCase()
{
    qRegisterMetaType<QVariant>("QVariant");
}

void Ut_CReporterSettingsObserver::testSettingValueChangedSignal()
{
    QSignalSpy valueChangedSpy(m_subject, SIGNAL(valueChanged(QString, QVariant)));
    // Add watcher to key.
    m_subject->addWatcher(Settings::ValueNotifications);
    // Change setting value.
    m_settings->setValue(Settings::ValueNotifications, false);
    m_settings->sync();

    QTest::qWait(50);
    QCOMPARE(valueChangedSpy.count(), 1);

    QVariantList arguments = valueChangedSpy.takeFirst();

    QCOMPARE(arguments.at(0).toString(), Settings::ValueNotifications);
    QCOMPARE(arguments.at(1).toBool(), false);
}

void Ut_CReporterSettingsObserver::testWatcherRemovedAndSettingValueChanged()
{
    QSignalSpy valueChangedSpy(m_subject, SIGNAL(valueChanged(QString, QVariant)));
    // Add watcher to key.
    m_subject->addWatcher(Settings::ValueCoreDumping);

    // Add watcher from key.
    m_subject->removeWatcher(Settings::ValueCoreDumping);

    // Change setting value.
    m_settings->setValue(Settings::ValueCoreDumping, false);
    m_settings->sync();

    QTest::qWait(50);
    QCOMPARE(valueChangedSpy.count(), 0);
}

void Ut_CReporterSettingsObserver::testSettingFileRemoved()
{
    QSignalSpy valueChangedSpy(m_subject, SIGNAL(valueChanged(QString, QVariant)));
    // Add watcher to key.
    m_subject->addWatcher(Settings::ValueCoreDumping);

    QFile file(testSettingsFile);
    file.remove();

    QTest::qWait(50);
    QCOMPARE(valueChangedSpy.count(), 0);
}

void Ut_CReporterSettingsObserver::cleanupTestCase()
{

}

void Ut_CReporterSettingsObserver::cleanup()
{
    QFile file(testSettingsFile);
    file.remove();

    delete m_subject;
    m_subject = 0;

    delete m_settings;
    m_settings = 0;
}

QTEST_MAIN(Ut_CReporterSettingsObserver)
