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

#ifndef CREPORTERDIALOGSERVER_H
#define CREPORTERDIALOGSERVER_H

// System includes.

#include <QObject>
#include <QVariant>
#include <QDBusError>

// User includes.

#include "creporterdialogserverinterface.h"

// Forward declarations.

class CReporterDialogServerPrivate;
class MApplicationService;

/*!
  * @class CReporterDialogServer
  * @brief This class implements server process and interface for dialogs.
  *
  * This server runs in Qt main loop, receives events from D-Bus and dispatches
  * requests to loaded dialog plugins.
  *
  * @sa CReporterDialogServerInterface
  */
class CReporterDialogServer : public QObject,
                                                    public CReporterDialogServerInterface
{
    Q_OBJECT
    Q_INTERFACES(CReporterDialogServerInterface)

    public:
        /*!
          * @brief Class constructor.
          *
          * @param pluginPath Path to dialog plugins to load.
          * @param service Pointer to the application's MApplicationService
          *
          */
        CReporterDialogServer(const QString& pluginPath, MApplicationService* service);

        /*!
          * @brief Class destructor.
          *
          */
        ~CReporterDialogServer();

         //! @reimp
        virtual bool createRequest(const QString &dialogName, const QVariantList &arguments);

        //! @reimp
        virtual void showDialog(MDialog *dialog);

        //! @reimp
        virtual void hideDialog(MDialog *dialog);

    public Q_SLOTS:
        /*!
          * @brief Called, when call signal or response to a remote call is received.
          *
          * @param dialogName Name of the dialog reguested.
          * @param arguments Message arguments.
          * @param message Message received from the caller.
          * @return Error type. QDBusError::NoError, if succeeded.
          */
        QDBusError::ErrorType callReceived(const QString &dialogName,
                                           const QVariantList &arguments,
                                           const QDBusMessage &message);

        /*!
          * @brief Called to request server to quit its main loop.
          *
          */
        void quit();

        //! Destroy all active notifications (e.g. when a window becomes visible)
        void destroyNotifications();

    private: // data

        Q_DECLARE_PRIVATE(CReporterDialogServer)
        //! @arg Pointer to private data.
        CReporterDialogServerPrivate *d_ptr;

#ifdef CREPORTER_UNIT_TEST
    friend class Ut_CReporterDialogServer;
#endif // CREPORTER_UNIT_TEST
};

#endif // CREPORTERDIALOGSERVER_H

// End of file
