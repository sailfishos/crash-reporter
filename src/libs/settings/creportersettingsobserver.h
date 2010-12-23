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

#ifndef CREPORTERSETTINGSOBSERVER_H
#define CREPORTERSETTINGSOBSERVER_H

// System includes.

#include <QObject>

// User includes.

#include "creporterexport.h"

// Forward declarations.

class CReporterSettingsObserverPrivate;

/*!
  * @class CReporterSettingsObserver
  * @brief Class for monitoring changes in settings and notifying client.
  *
  */
class CREPORTER_EXPORT CReporterSettingsObserver : public QObject
{
    Q_OBJECT

    public:
        /*!
          * @brief Class constructor.
          *
          * @param path Setting INI-file path to monitor.
          */
        CReporterSettingsObserver(const QString &path, QObject *parent=0);

        /*!
          * @brief Class destructor.
          *
          */
        virtual ~CReporterSettingsObserver();

        /*!
          * @brief Adds watcher to key.
          *
          * @param key Which changes to watch.
          */
        void addWatcher(const QString &key);

        /*!
          * @brief Removes watcher from the key
          *
          * @param key.
          */
        void removeWatcher(const QString &key);

        Q_SIGNALS:

        /*!
         * @brief Sent, when @a key changes to @a value.
         * 
         * @param key Key of which value did change.
         * @param value New value.
         */
        void valueChanged(const QString &key, const QVariant &value);

    private:
        Q_DECLARE_PRIVATE(CReporterSettingsObserver);
        //! @arg Pointer to private data.
        CReporterSettingsObserverPrivate *d_ptr;
#ifdef CREPORTER_UNIT_TEST
        friend class Ut_CReporterSettingsObserver;
#endif // CREPORTER_UNIT_TEST
};

#endif // CREPORTERSETTINGSOBSERVER_H

// End of file.
