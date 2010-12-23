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

#ifndef CREPORTERAPPLICATIONSETTINGS_H
#define CREPORTERAPPLICATIONSETTINGS_H

// System includes.

#include <QVariant>

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
  * @namespace Connectivity
  * @brief Key/ value pairs for connectivity related settings.
  *
  */
namespace Connectivity {
    const QString ValueUsbNetworking = "Connectivity/usb_networking";
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
  * @namespace DefaultApplicationSettings
  * @brief Default settings for application. Used as a fallback, if reguested setting is not found.
  *
  */
namespace DefaultApplicationSettings {
    const QString ValueUsernameDefault = "nitrouser";
    const QString ValuePasswordDefault = "uwbJCi4fh";
    const QString ValueServerAddressDefault = "https://crashes.projects.maemo.org";
    const int ValueServerPortDefault = 8080;
    const QString ValueServerPathDefault = "/nitro";
    const bool ValueUseSslDefault = false;
    const bool ValueUseProxyDefault = false;
    const QString ValueProxyAddressDefault = "";
    const QString ValueProxyPortDefault = "";
    const bool ValueUsbNetworkingDefault = false;
    const bool ValueLoggerTypeDefault = "none";
}

/*!
  * @class CReporterApplicationSettings
  * @brief This a singleton class for reading and writing crash-reporter application settings.
  *
  */
class CReporterApplicationSettings : public QObject
{
    Q_OBJECT

    public:
        /*!
         * @brief Creates new a new instance of this class, if not exist and returns it.
         *
         * @return Class refence.
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

        /*!
         * @brief Returns setting value of key.
         *
         * @param key Setting key of which value to find.
         * @param defaultValue Returned, if setting doesn't exist.
         * @return Setting value.
         */
        QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

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
