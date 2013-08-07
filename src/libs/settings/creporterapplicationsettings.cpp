/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Riku Halonen <riku.halonen@nokia.com>
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

// System includes.

#include <QSettings>

// User includes.

#include "creporterapplicationsettings.h"
#include "creportersettingsinit_p.h"
#include "creporternamespace.h"

// Pointer to this class.
CReporterApplicationSettings* CReporterApplicationSettings::sm_Instance = 0;

/*!
  * @class CReporterApplicationSettingsPrivate
  * @brief Private CReporterApplicationSettings class.
  *
  */
class CReporterApplicationSettingsPrivate
{
    public:
        CReporterApplicationSettingsPrivate(CReporterApplicationSettings *q);

        int intValue(const QString &key, const QVariant &defaultValue) const;

    private:
        Q_DECLARE_PUBLIC(CReporterApplicationSettings)
        CReporterApplicationSettings *q_ptr;
};

CReporterApplicationSettingsPrivate::CReporterApplicationSettingsPrivate(CReporterApplicationSettings *q):
    q_ptr(q)
{
}

int CReporterApplicationSettingsPrivate::intValue(const QString &key,
                                                  const QVariant &defaultValue) const
{
    const Q_Q(CReporterApplicationSettings);

    bool ok;
    int result = q->value(key, defaultValue).toInt(&ok);

    return ok ? result : defaultValue.toInt();
}

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterApplicationSettings::instance
// ----------------------------------------------------------------------------
CReporterApplicationSettings* CReporterApplicationSettings::instance()
{
    if (sm_Instance == 0) {
        sm_Instance = new CReporterApplicationSettings();
    }
    return sm_Instance;
}

// ----------------------------------------------------------------------------
// CReporterApplicationSettings::freeSingleton
// ----------------------------------------------------------------------------
void CReporterApplicationSettings::freeSingleton()
{
    if (sm_Instance != 0) {
        delete sm_Instance;
        sm_Instance = 0;
    }
}

// ----------------------------------------------------------------------------
// CReporterApplicationSettings::~CReporterApplicationSettings
// ----------------------------------------------------------------------------
CReporterApplicationSettings::~CReporterApplicationSettings()
{
    delete d_ptr;
    d_ptr = 0;
}

QString CReporterApplicationSettings::serverUrl() const
{
    return value(Server::ValueServerAddress,
            DefaultApplicationSettings::ValueServerAddressDefault).toString();
}

void CReporterApplicationSettings::setServerUrl(const QString &url)
{
    setValue(Server::ValueServerAddress, url);
}

int CReporterApplicationSettings::serverPort() const
{
    const Q_D(CReporterApplicationSettings);

    return d->intValue(Server::ValueServerPort,
            DefaultApplicationSettings::ValueServerPortDefault);
}

void CReporterApplicationSettings::setServerPort(int port)
{
    setValue(Server::ValueServerPort, port);
}

QString CReporterApplicationSettings::serverPath() const
{
    return value(Server::ValueServerPath,
            DefaultApplicationSettings::ValueServerPathDefault).toString();
}

void CReporterApplicationSettings::setServerPath(const QString &path)
{
    setValue(Server::ValueServerPath, path);
}

bool CReporterApplicationSettings::useSsl() const
{
    return value(Server::ValueUseSsl,
            DefaultApplicationSettings::ValueUseSslDefault).toBool();
}

void CReporterApplicationSettings::setUseSsl(bool state)
{
    setValue(Server::ValueUseSsl, state);
}

QString CReporterApplicationSettings::username() const
{
    return value(Server::ValueUsername,
            DefaultApplicationSettings::ValueUsernameDefault).toString();
}

void CReporterApplicationSettings::setUsername(const QString &username)
{
    setValue(Server::ValueUsername, username);
}

QString CReporterApplicationSettings::password() const
{
    return value(Server::ValuePassword,
            DefaultApplicationSettings::ValuePasswordDefault).toString();
}

void CReporterApplicationSettings::setPassword(const QString &password)
{
    setValue(Server::ValuePassword, password);
}

bool CReporterApplicationSettings::useProxy() const
{
    return value(Server::ValueUseProxy,
            DefaultApplicationSettings::ValueUseProxyDefault).toBool();
}

void CReporterApplicationSettings::setUseProxy(bool state)
{
    setValue(Server::ValueUseProxy, state);
}

QString CReporterApplicationSettings::proxyUrl() const
{
    return value(Proxy::ValueProxyAddress,
            DefaultApplicationSettings::ValueProxyAddressDefault).toString();
}

void CReporterApplicationSettings::setProxyUrl(const QString &url)
{
    setValue(Proxy::ValueProxyAddress, url);
}

int CReporterApplicationSettings::proxyPort() const
{
    const Q_D(CReporterApplicationSettings);

    return d->intValue(Proxy::ValueProxyPort,
            DefaultApplicationSettings::ValueProxyPortDefault);
}

void CReporterApplicationSettings::setProxyPort(int port)
{
    setValue(Proxy::ValueProxyPort, port);
}

bool CReporterApplicationSettings::usbNetworking() const
{
    return value(Connectivity::ValueUsbNetworking,
            DefaultApplicationSettings::ValueUsbNetworkingDefault).toBool();
}

void CReporterApplicationSettings::setUsbNetworking(bool state)
{
    setValue(Connectivity::ValueUsbNetworking, state);
}

QString CReporterApplicationSettings::loggerType() const
{
    return value(Logging::ValueLoggerType,
            DefaultApplicationSettings::ValueLoggerTypeDefault).toString();
}

void CReporterApplicationSettings::setLoggerType(const QString &type)
{
    setValue(Logging::ValueLoggerType, type);
}

// ----------------------------------------------------------------------------
// CReporterApplicationSettings::CReporterApplicationSettings
// ----------------------------------------------------------------------------
CReporterApplicationSettings::CReporterApplicationSettings() :
    CReporterSettingsBase("crash-reporter-settings", "crash-reporter"),
    d_ptr(new CReporterApplicationSettingsPrivate(this))
{
}

// End of file.
