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

#ifndef CREPORTERDIALOGSERVERDBUSADAPTOR_H
#define CREPORTERDIALOGSERVERDBUSADAPTOR_H

// System includes.

#include <QtDBus/QtDBus>

/*!
  * @class CReporterDialogServerDBusAdaptor
  * @brief Dialog server DBus interface.
 *
 * This class is a light-weight wrapper relaying calls into parent class.
 * This class needs to be allocated from the stack and will be destroyed
 * automatically, when the parent class is destroyed.
 */
class CReporterDialogServerDBusAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.nokia.CrashReporter.DialogServer")

    public:
        /*!
         * @brief Class constructor.
         *
         * @param parent pointer to the parent class instance.
         */
        CReporterDialogServerDBusAdaptor(QObject *parent);

        /*!
         * @brief Class destructor.
         *
         */
        ~CReporterDialogServerDBusAdaptor();

    public Q_SLOTS:
        /*!
          * @brief Requests dialog from the server.
          *
          * @param dialogName Name of the dialog to display.
          * @param arguments Request arguments
          * @param message QDBusMessage for delivering reply message.
          * @return True, if request was queued successfully; otherwise false.
          *
          * @sa CReporter::SendAllDialogType
          * @sa CReporter::SendSelectedDialogType
          * @sa CReporter::NotifyNewDialogType
          * @sa CReporter::UploadDialogType
          */
        bool requestDialog(const QString &dialogName, const QList<QVariant> &arguments,
                           const QDBusMessage &message);

        /*!
          * @brief Request Dialog server to exit from its main loop.
          *
          * @note This method call is asyncronous.
          */
        Q_NOREPLY void quit();
};

#endif // CREPORTERDIALOGSERVERDBUSADAPTOR_H

// End of file.
