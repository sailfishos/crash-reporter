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

#ifndef CREPORTERAPPLICATIONSETTINGS_H
#define CREPORTERAPPLICATIONSETTINGS_H

#include "creportersettingsbase.h"

// Forward declarations.

class CReporterApplicationSettingsPrivate;

// Public namespaces.

/*!
  * @namespace Server
  * @brief Key/ value pairs for server related settings.
  *
  */
namespace Server {
    const QString ValueUsername = "Server/user_name";
    const QString ValuePassword = "Server/password";
    const QString ValueServerAddress = "Server/server_addr";
    const QString ValueServerPort = "Server/server_port";
    const QString ValueServerPath = "Server/server_path";
    const QString ValueUseSsl = "Server/use_ssl";
    const QString ValueUseProxy = "Server/use_proxy";
}

/*!
  * @namespace Proxy
  * @brief Key/ value pairs for proxy related settings.
  *
  */
namespace Proxy {
    const QString ValueProxyAddress = "Proxy/proxy_addr";
    const QString ValueProxyPort = "Proxy/proxy_port";
}

/*!
  * @namespace Logging
  * @brief Key/ value pairs for logging related settings.
  *
  */
namespace Logging {
    const QString ValueLoggerType = "Logging/logger_type";
}

/*!
  * @class CReporterApplicationSettings
  * @brief This a singleton class for reading and writing crash-reporter application settings.
  *
  */
class CReporterApplicationSettings : public CReporterSettingsBase
{
    Q_OBJECT

    Q_PROPERTY(QString serverUrl READ serverUrl WRITE setServerUrl NOTIFY serverUrlChanged)
    Q_PROPERTY(int serverPort READ serverPort WRITE setServerPort NOTIFY serverPortChanged)
    Q_PROPERTY(QString serverPath READ serverPath WRITE setServerPath NOTIFY serverPathChanged)
    Q_PROPERTY(bool useSsl READ useSsl WRITE setUseSsl NOTIFY useSslChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool useProxy READ useProxy WRITE setUseProxy NOTIFY useProxyChanged)
    Q_PROPERTY(QString proxyUrl READ proxyUrl WRITE setProxyUrl NOTIFY proxyUrlChanged)
    Q_PROPERTY(int proxyPort READ proxyPort WRITE setProxyPort NOTIFY proxyPortChanged)
    Q_PROPERTY(QString loggerType READ loggerType WRITE setLoggerType NOTIFY loggerTypeChanged)

    public:
        /*!
         * @brief Creates new a new instance of this class, if not exist and returns it.
         *
         * @return Class reference.
         */
        static CReporterApplicationSettings *instance();

        /*!
         * @brief Frees the class instance.
         *
         */
        static void freeSingleton();

        /*!
         * @brief Class destructor.
         *
         */
        ~CReporterApplicationSettings();

        QString serverUrl() const;
        void setServerUrl(const QString &url);

        int serverPort() const;
        void setServerPort(int port);

        QString serverPath() const;
        void setServerPath(const QString &path);

        bool useSsl() const;
        void setUseSsl(bool state);

        QString username() const;
        void setUsername(const QString &username);

        QString password() const;
        void setPassword(const QString &password);

        bool useProxy() const;
        void setUseProxy(bool state);

        QString proxyUrl() const;
        void setProxyUrl(const QString &url);

        int proxyPort() const;
        void setProxyPort(int port);

        QString loggerType() const;
        void setLoggerType(const QString &type);

    signals:
        void serverUrlChanged();
        void serverPortChanged();
        void serverPathChanged();
        void useSslChanged();
        void usernameChanged();
        void passwordChanged();
        void useProxyChanged();
        void proxyUrlChanged();
        void proxyPortChanged();
        void loggerTypeChanged();

    protected:
        /*!
          * @brief Class constructor.
          *
          */
        CReporterApplicationSettings();

    private:
        Q_DECLARE_PRIVATE(CReporterApplicationSettings)
        //! @arg Private class reference
        CReporterApplicationSettingsPrivate *d_ptr;
        //! @arg Static class reference.
        static CReporterApplicationSettings *sm_Instance;
};

#endif // CREPORTERAPPLICATIONSETTINGS_H

// End of file.
