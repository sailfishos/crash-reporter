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

#ifndef CREPORTEDIALOGSERVER_P_H
#define CREPORTEDIALOGSERVER_P_H

// System includes.

#include <QObject>
#include <QMutex>

// Forward declarations.

class QTimer;
class CReporterLoadedPlugin;
class CReporterDialogServer;
class CReporterDialogRequest;
class MApplicationService;

/*!
  * @class CReporterDialogServerPrivate
  * @brief Private CReporterDialogServerPrivate
  *
  */
class CReporterDialogServerPrivate : public QObject
{
    Q_OBJECT

    public:
        /*!
          * @brief Class constructor.
          *
          */
        CReporterDialogServerPrivate();

        /*!
          * @brief Class destructor.
          *
          */
        virtual ~CReporterDialogServerPrivate();

    public Q_SLOTS:
        /*!
          * @brief When called, dequeues the next dialog request and handles it.
          *
          */
        void processRequests();

        /*!
          * @brief Slot is called, when a dialog plugin completes any request.
          *
          * This slot is connected to requestCompleted() signal.
          *
          */
        void requestCompleted();

        /*!
          * @brief Called, when a dialog is destroyed.
          *
          * @param dialog Destroyed object.
          */
        void dialogDestroyed(QObject *dialog);

    public:
        /*!
         * @brief Loads all dialog plugins from path supplied in constructor.
         *
         */
        void loadPlugins();

        /*!
          * @brief Loads the @a pluginObject supplied as parameter.
          *
          * @param pluginObject Plugin to load.
          */
        void loadPlugin(QObject *pluginObject);

        /*!
          * @brief Destroys all dialog loaded plugins.
          *
          */
        void destroyPlugins();

        /*!
          * @brief Checks, if there already active pluging.
          *
          * @return True, if there's active plugin; otherwise false.
          */
        bool isPluginActive();

    public:
        //! @arg Path, where plugins to be loaded are searched.
        QString m_pluginPath;
        //! @arg List of loaded plugins.
        QList<CReporterLoadedPlugin *> m_plugins;
        //! @arg Linked list maintaining requests.
        QList<CReporterDialogRequest *> m_requestQueue;
        //! @arg Mutex for request queue access
        QMutex m_requestQueueMutex;
        //! @arg List of active dialogs.
        QList<QObject *> m_dialogs;
        //! @arg Timer to schedule dialog request.
        QTimer *m_scheduleTimer;
        //! @arg Pointer to the crash-reporter-ui's MApplicationService
        MApplicationService *m_appService;

        Q_DECLARE_PUBLIC(CReporterDialogServer)

    protected:
        //! @arg Pointer to public class instance.
        CReporterDialogServer *q_ptr;
#ifdef CREPORTER_UNIT_TEST
    friend class ut_creporterdialogserver;
#endif
};

#endif // CREPORTEDIALOGSERVER_P_H
