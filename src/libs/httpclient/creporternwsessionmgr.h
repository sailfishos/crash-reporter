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

#ifndef CREPORTERNWSESSIONMGR_H
#define CREPORTERNWSESSIONMGR_H

#include <QObject>
#include <QNetworkSession>

#include "creporterexport.h"


class CReporterNwSessionMgrPrivate;

/*!
  * @class CReporterNwSessionMgr
  *
  * @brief This class provides and controls network access.
  *
  * Uses Bearer Management API to control connectivity state.
  */
class CREPORTER_EXPORT CReporterNwSessionMgr : public QObject
{
    Q_OBJECT

public:
    CReporterNwSessionMgr(QObject *parent = 0);
    ~CReporterNwSessionMgr();

    /*!
      * @brief Return the state of the network session.
      *
      * @return True, if session is opened; otherwise false.
      */
    bool opened() const;

    /*!
     * @return True if crash reporter is allowed to use the current network
     * connection for its data transmissions. For example uploads through
     * mobile network, which may carry additional charges from the provider,
     * can be disabled in the settings.
     */
    static bool canUseNetworkConnection();

Q_SIGNALS:
    /*!
     * @brief This signal is emitted, when the network session has been opened.
     *
     */
    void sessionOpened();

    /*!
     * @brief This signal is emitted, when the network session has been disconnected.
     *
     */
    void sessionDisconnected();

    /*!
     * @brief This signal is emitted after network error has occurred.
     *
     * @param errorString Human-readable error string.
     */
    void networkError(const QString &errorString);

public Q_SLOTS:
    /*!
     * @brief Opens network session.
     *
     * @return True, if there was already an active session, which can be used for
     * socket operations. False, if there was no proper session, which can be re-used.
     * Opening new session was triggered and status can be monitored by connecting to
     * sessionOpened(), sessionDisconnected() or networkError().
     */
    bool open();

    /*!
     * @brief Closes active session.
     *
     */
    void close();

    /*!
     * @brief Brutely stop underlying network interface
     *
     */
    void stop();

protected Q_SLOTS:
    /*!
     * @brief Called, when error() signal is emitted by the QNetworkSession class.
     *
     * @param error Describes the error occurred.
     */
    void networkError(QNetworkSession::SessionError error);

    /*!
     * @brief Called, when state of the network session changes.
     *
     * @param state Describes the new state.
     */
    void networkStateChanged(QNetworkSession::State state);

private:
    Q_DECLARE_PRIVATE(CReporterNwSessionMgr)

    CReporterNwSessionMgrPrivate *d_ptr;
};

#endif // CREPORTERNWSESSIONMGR_H
