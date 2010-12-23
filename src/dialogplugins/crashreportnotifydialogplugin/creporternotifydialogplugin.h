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

#ifndef CREPORTERNOTIFYDIALOGPLUGIN_H
#define CREPORTERNOTIFYDIALOGPLUGIN_H

// User includes.

#include "creporterdialogplugininterface.h"
#include "creporterdialogserverinterface.h"

// Forward declarations.

class CReporterNotifyDialogPluginPrivate;

/*!
  * @class CReporterNotifyDialogPlugin
  * @brief This class implements dialog plugin interface.
  *
  * @sa CReporterDialogPluginInterface
  */
class CReporterNotifyDialogPlugin : public CReporterDialogPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CReporterDialogPluginInterface)

    public:
        //! @reimp
        virtual void initialize(CReporterDialogServerInterface *server);
        //! @reimp
        virtual void destroy();
        //! @reimp
        virtual bool isInitialized() const;
        //! @reimp
        virtual QString name() const;
        //! @reimp
        virtual bool requestDialog(const QVariantList &arguments);
        //! @reimp
        virtual bool isActive() const;

    private Q_SLOTS:
        /*!
          * @brief Called, when dialog button is pressed.
          *
          * @param buttonId Unique identifier of the button.
          */
        void actionPerformed(int buttonId);

        /*!
          * @brief Called, when dialog is closed.
          *
          */
        void dialogFinished();

        /*!
          * @brief Called, when notification timeouts.
          *
          */
        void notificationTimeout();

        /*!
          * @brief Called, when user interacts with the notification.
          *
          */
        void notificationActivated();

    private: // data
       Q_DECLARE_PRIVATE( CReporterNotifyDialogPlugin )
       //! @arg Pointer to private data class.
       CReporterNotifyDialogPluginPrivate *d_ptr;
#ifdef CREPORTER_UNIT_TEST
       friend class Ut_CReporterNotifyDialogPlugin;
#endif // CREPORTER_UNIT_TEST
};

#endif // CREPORTERNOTIFYDIALOGPLUGIN_H

// End of file.
