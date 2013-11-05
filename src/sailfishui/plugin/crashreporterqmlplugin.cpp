/*
 * This file is part of crash-reporter
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
 */

#include <QtQml>

#include "crashreporterqmlplugin.h"

#include "crashreporteradapter.h"
#include "creporterapplicationsettings.h"
#include "creportercoreregistry.h"
#include "creporterprivacysettingsmodel.h"
#include "creporterutils.h"
#include "systemdservice.h"

// using custom translator so it gets properly removed from qApp when engine is deleted
class AppTranslator: public QTranslator
{
public:
    AppTranslator(QObject *parent): QTranslator(parent)
    {
        qApp->installTranslator(this);
    }

    virtual ~AppTranslator()
    {
        qApp->removeTranslator(this);
    }
};

static QObject *adapterSingletontypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    static CrashReporterAdapter *instance = 0;
    if (!instance) {
        instance = new CrashReporterAdapter(qApp);
    }

    return instance;
}

static QObject *privacysettingsSingletontypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return CReporterPrivacySettingsModel::instance();
}

static QObject *applicationsettingsSingletontypeProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return CReporterApplicationSettings::instance();
}

void CrashReporterQmlPlugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(uri)

    AppTranslator *engineeringEnglish = new AppTranslator(engine);
    engineeringEnglish->load("crash-reporter_eng_en", "/usr/share/translations");

    AppTranslator *translator = new AppTranslator(engine);
    translator->load(QLocale(), "crash-reporter", "-", "/usr/share/translations");
}

void CrashReporterQmlPlugin::registerTypes(const char *uri)
{
    Q_UNUSED(uri)

    qmlRegisterSingletonType<CrashReporterAdapter>(
            "com.jolla.settings.crashreporter", 1, 0,
            "Adapter", adapterSingletontypeProvider);

    qmlRegisterSingletonType<CReporterApplicationSettings>(
            "com.jolla.settings.crashreporter", 1, 0,
            "ApplicationSettings", applicationsettingsSingletontypeProvider);

    qmlRegisterSingletonType<CReporterPrivacySettingsModel>(
            "com.jolla.settings.crashreporter", 1, 0,
            "PrivacySettings", privacysettingsSingletontypeProvider);

    qmlRegisterType<SystemdService>(
            "com.jolla.settings.crashreporter", 1, 0, "SystemdService");
}
