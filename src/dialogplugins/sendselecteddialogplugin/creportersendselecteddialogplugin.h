/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Carol Rus <carol.rus@digia.com>
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

#ifndef CREPORTERSENDSELECTEDDIALOGPLUGIN_H
#define CREPORTERSENDSELECTEDDIALOGPLUGIN_H

// System includes.

#include <QStringList>

// User includes.

#include "creporterdialogplugininterface.h"
#include "creporterdialogserverinterface.h"

// Forward declarations.

class CReporterSendSelectedDialogPluginPrivate;

/*!
  * @class CReporterSendSelectedDialogPlugin
  * @brief This class implements dialog plugin interface.
  *
  * @sa CReporterDialogPluginInterface
  */
class CReporterSendSelectedDialogPlugin : public CReporterDialogPluginInterface
{
    Q_OBJECT
    Q_INTERFACES( CReporterDialogPluginInterface )

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

        /*!
          * @brief Updates the list of files to be uploaded/deleted.
          *
          */
        void setFileList(QStringList files);

    private:
       Q_DECLARE_PRIVATE(CReporterSendSelectedDialogPlugin)
       //! @arg Pointer to private data class.
       CReporterSendSelectedDialogPluginPrivate *d_ptr;
#ifdef CREPORTER_UNIT_TEST
       friend class Ut_CReporterSendSelectedDialogPlugin;
#endif // CREPORTER_UNIT_TEST
};

#endif // CREPORTERSENDSELECTEDDIALOGPLUGIN_H

// End of file.
