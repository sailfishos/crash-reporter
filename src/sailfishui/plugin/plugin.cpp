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


class CrashReporterQmlPlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.jolla.settings.crashreporter")

public:
    void initializeEngine(QQmlEngine *engine, const char *uri)
    {
        Q_UNUSED(uri)

        AppTranslator *engineeringEnglish = new AppTranslator(engine);
        engineeringEnglish->load("crash-reporter_eng_en", "/usr/share/translations");

        AppTranslator *translator = new AppTranslator(engine);
        translator->load(QLocale(), "crash-reporter", "-", "/usr/share/translations");
    }

    void registerTypes(const char *uri)
    {
        Q_UNUSED(uri)

        // QtQuick requires to register something. Otherwise import fails.
        qmlRegisterUncreatableType<AppTranslator>("com.jolla.settings.crashreporter.translations", 1, 0,
                                                  "CrashReporterTranslations", "Crash Reporter translations loaded by import");
    }
};

#include "plugin.moc"
