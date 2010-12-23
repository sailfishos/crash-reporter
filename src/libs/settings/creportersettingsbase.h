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

#ifndef CREPORTERSETTINGSBASE_H
#define CREPORTERSETTINGSBASE_H

// System includes.

#include <QVariant>
#include <QObject>

// User includes.

#include "creporterexport.h"

// Forward declarations.

class CReporterSettingsBasePrivate;

/*!
  * @class CReporterSettingsBase
  * @brief Base class for all classes modifying crash-reporter settings.
  *
  */
class CREPORTER_EXPORT CReporterSettingsBase : public QObject
{
    Q_OBJECT

    public:
        /*!
         * @brief Class constructor.
         *
         * @param organization Organization for QSettings object.
         * @param application Application for QSettings object.
         * @param parent Parent object.
         */
        CReporterSettingsBase(const QString &organization, const QString &application = QString(),
                              QObject *parent=0);

        /*!
         * @brief Class destructor.
         *
         */
        virtual ~CReporterSettingsBase();

        /*!
         * @brief Saves any unsaved settings in the permanent storage.
         *
         */
        void writeSettings();

        /*!
         * @brief Return path to settings file.
         *
         * @return Absolute path to settings file. It's up to the calling code to check, if
         * file exists.
         */
        QString settingsFile() const;

        /*!
         * @brief Checks, if these settings are valid.
         *
         * @return Returns True, if valid otherwise false.
         */
         bool isValid() const;

    Q_SIGNALS:
        /*!
          * @brief Sent, when @a key has changed to @a value.
          *
          * @param key Changed key.
          * @param value New value.
          */
         void valueChanged(const QString &key, const QVariant &value);

    protected:
        /*!
         * @brief Sets setting value.
         *
         * @param key Setting of which value to set.
         * @param value New key value.
         */
        void setValue(const QString &key, const QVariant &value);

        /*!
         * @brief Returns setting value of key.
         *
         * @param key Setting key of which value to find.
         * @param defaultValue Returned, if setting doesn't exist.
         * @return Setting value.
         */
        QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;

    private:
        Q_DECLARE_PRIVATE(CReporterSettingsBase)
        //! Pointer to private data class.
        CReporterSettingsBasePrivate *d_ptr;
};

#endif // CREPORTERSETTINGSBASE_H

// End of file.
