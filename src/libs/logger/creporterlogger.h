/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Riku Halonen <riku.halonen@nokia.com>
 *
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Jakub Adam <jakub.adam@jollamobile.com>
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

#ifndef CREPORTERLOGGER_H
#define CREPORTERLOGGER_H

// System includes.

#include <QFile>
#include <QTextStream>

// User includes.

#include "creporterexport.h"
#include "creporternamespace.h"

// Forward declarations.
class CReporterLoggerPrivate;

/*!
  * @class CReporterLogger
  * @brief This class overrides the default Qt message handler.
  *
  * By default Qt prints debug etc. messages to stderr output. This class takes
  * over the Qt debugging functions and forwards the messages to a file, syslog or
  * suppresses output totally (default).
  */
class CREPORTER_EXPORT CReporterLogger {

    public:
        /*!
         * @brief Class constructor.
         *
         * @param logging Logging type.
         *
         * @sa CReporter::LogType
         */
        CReporterLogger(const QString type = "none");
        
        /*!
         * @brief Class destructor.
         *
         */
        ~CReporterLogger();

    protected:
        /*!
         * @brief Overrides default message handler.
         * 
         * @param type Message type.
         * @param context Additional information about a log message.
         * @param msg Log message.
         */
        static void messageHandler(QtMsgType type,
                                   const QMessageLogContext &context,
                                   const QString &msg);

    private:
        //! @arg Instance pointer to the this class.
        static CReporterLogger *sm_Instance;
        //! @arg The stream write to.
        QTextStream m_stream;
         //! @arg File to write.
        QFile m_file;
        //! @arg Old msg handler.
        QtMessageHandler m_old_msg_handler;
        //! @arg Use syslog for logging.
        static bool sm_Syslog;
        static CReporter::LogType sm_LogType;
};

typedef CReporterLogger Logger;

#endif // CREPORTERLOGGER_H

// end of file.
