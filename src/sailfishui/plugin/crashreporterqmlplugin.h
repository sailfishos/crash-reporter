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

#ifndef CRASHREPORTERQMLPLUGIN_H
#define CRASHREPORTERQMLPLUGIN_H

#include <QQmlExtensionPlugin>

class CrashReporterQmlPlugin: public QQmlExtensionPlugin {
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.jolla.settings.crashreporter")

public:
    void initializeEngine(QQmlEngine *engine, const char *uri);

    void registerTypes(const char *uri);

    Q_INVOKABLE void uploadAllCrashReports() const;
};

#endif // CRASHREPORTERQMLPLUGIN_H
