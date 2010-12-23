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

#ifndef CREPORTERSETTINGSOBSERVER_P_H
#define CREPORTERSETTINGSOBSERVER_P_H

// System includes.

#include <QSettings>
#include <QFileSystemWatcher>
#include <QHash>

// User includes.

#include "creportersettingsobserver.h"

/*!
  * @class CReporterSettingsObserverPrivate
  * @brief Private CReporterSettingsObserver class.
  *
  */
class CReporterSettingsObserverPrivate : public QObject
{
    Q_OBJECT

    public:
        /*!
         * @brief Class constructor.
         * 
         * @param path File to monitor.
         */
        CReporterSettingsObserverPrivate(const QString &path);

        /*!
         * @brief Class destructor.
         * 
         */
        ~CReporterSettingsObserverPrivate();

        /*!
         * @brief Adds watcher to @a key.
         * 
         * @param key Key which value changes to monitor.
         */
        void addWatcher(const QString &key);

        /*!
         * @brief Removes watcher from @a key.
         *
         * @param key Key from which watcher should be removed.
         */
        void removeWatcher(const QString &key);

    private Q_SLOTS:
        /*
         * @brief Called, when monitored file changes.
         *
         * @param path File path of the changed file.
         */
        void fileChanged(const QString &path);

    public:
         //! @arg Pointer to public class.
        CReporterSettingsObserver *q_ptr;

    private:
        //! @arg Settings object read from file.
        QSettings *m_settings;
        //! @arg QFileSystemWatcher object.
        QFileSystemWatcher m_watcher;
        //! @arg Hash table maintaining monitored key/ value pairs.
        QHash<QString, QVariant> m_notifications;
        //! @arg File to monitor for changes.
        QString m_file;
};

#endif // CREPORTERSETTINGSOBSERVER_P_H

// End of file.
