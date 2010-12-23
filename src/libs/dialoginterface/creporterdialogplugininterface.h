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

#ifndef CREPORTERDIALOGPLUGININTERFACE_H
#define CREPORTERDIALOGPLUGININTERFACE_H

// System includes.

#include <QString>
#include <QVariant>

// User includes.

#include "creporterdialogserverinterface.h"

// Forward declarations.

class CReporterDialogRequest;

/*!
  * @class CReporterDialogPluginInterface
  * @brief Pure virtual interface class for dialog plugins. All dialog plugins must implement
  *     this interface.
  *
  */
class CReporterDialogPluginInterface : public QObject
{
    Q_OBJECT

public:
	/*!
     *	@brief Initialize dialog plugin for use.
	 *
	 */
    virtual void initialize(CReporterDialogServerInterface* server) = 0;
	
	/*!
     *	@brief Destroy the plugin.
	 *
	 */
	virtual void destroy() = 0;
	
	/*!
     *	@brief Checks if the plugin has been initialized successfully.
	 *
     *	@return True, if the plugin has been initialized successfully.
	 */
	virtual bool isInitialized() const = 0;

     /*!
     *	@brief Returns the name of the plugin.
     *
     *	@return Plugin name.
     */
    virtual QString name() const = 0;

	/*!
      * @brief Request dialog from the dialog plugin.
	  *
      * @param arguments Request arguments.
      * @return True, if the plugin was able to start handling the request
	  */
    virtual bool requestDialog(const QVariantList &arguments) = 0;

    /*!
      * @brief Checks, if plugin is active (widget can be seen).
      *
      * This function is called, when server receives new request. When this function returns true,
      * requests won't reach this plugin.
      *
      * @return True, if plugin is active; otherwise false.
      */
    virtual bool isActive() const = 0;

	Q_SIGNALS:

        /*!
          * @brief This signal is emitted, when request has been completed.
          *
          */
        void requestCompleted();
};

Q_DECLARE_INTERFACE(CReporterDialogPluginInterface,
                    "com.nokia.CReporterDialogPluginInterface");

#endif // CREPORTERDIALOGPLUGININTERFACE_H

// end of file
