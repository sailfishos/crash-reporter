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

// System includes.

#include <QDebug>
#include <QNetworkConfiguration>
#include <QNetworkConfigurationManager>

// User includes.

#include "creporternwsessionmgr.h"
#ifndef CREPORTER_UNIT_TEST
#include "creporterprivacysettingsmodel.h"
#endif

/*!
  * \class CReporterNwSessionMgrPrivate
  *
  * \brief Private CReporterNwSessionMgr class.
  */
class CReporterNwSessionMgrPrivate
{
public:
    //! @arg Provides control over the system's access points and enables session management.
    QNetworkSession *networkSession;

    //! @return Manager of the network configurations provided by the system.
    static QNetworkConfigurationManager& networkManager();
};

QNetworkConfigurationManager& CReporterNwSessionMgrPrivate::networkManager()
{
    static QNetworkConfigurationManager manager;
    return manager;
}

// *** Class CReporterNwSessionMgr ****

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterNwSessionMgr::CReporterNwSessionMgr
// ----------------------------------------------------------------------------
CReporterNwSessionMgr::CReporterNwSessionMgr(QObject *parent) :
        QObject(parent),
        d_ptr(new CReporterNwSessionMgrPrivate())
{
    Q_D(CReporterNwSessionMgr);
    d->networkSession = 0;
}

// ----------------------------------------------------------------------------
// CReporterNwSessionMgr::~CReporterNwSessionMgr
// ----------------------------------------------------------------------------
CReporterNwSessionMgr::~CReporterNwSessionMgr()
{
    Q_D(CReporterNwSessionMgr);
    if (d->networkSession) delete d->networkSession; d->networkSession = 0;
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterNwSessionMgr::opened
// ----------------------------------------------------------------------------
bool CReporterNwSessionMgr::opened() const
{
    Q_D(const CReporterNwSessionMgr);

    if (d->networkSession == 0) {
        return false;
    }
    else if (d->networkSession->isOpen()) {
        return true;
    }
    else {
        return false;
    }
}

// ----------------------------------------------------------------------------
// CReporterNwSessionMgr::open
// ----------------------------------------------------------------------------
bool CReporterNwSessionMgr::open()
{
    Q_D(CReporterNwSessionMgr);

    if (d->networkSession == 0) {
        qDebug() << __PRETTY_FUNCTION__ << "No existing network session.";
        // If there was no network session, create one.
        d->networkManager().updateConfigurations();
        d->networkSession =
                new QNetworkSession(d->networkManager().defaultConfiguration());

        connect(d->networkSession, SIGNAL(stateChanged(QNetworkSession::State)),
                this, SLOT(networkStateChanged(QNetworkSession::State)));
        connect(d->networkSession, SIGNAL(error(QNetworkSession::SessionError)),
                this, SLOT(networkError(QNetworkSession::SessionError)));
        connect(d->networkSession, SIGNAL(opened()), this, SIGNAL(sessionOpened()));
    }
    else if (d->networkSession->isOpen()) {
        qDebug() << __PRETTY_FUNCTION__ << "Using existing network session.";
        return true;
    }

    qDebug() << __PRETTY_FUNCTION__ << "Opening network session...";
#ifndef CREPORTER_UNIT_TEST 
    if (CReporterPrivacySettingsModel::instance()->automaticSendingEnabled()) {
	qDebug() << __PRETTY_FUNCTION__ << "... with ConnectInBackground set";
        d->networkSession->setSessionProperty("ConnectInBackground", true);
    }
#endif
    d->networkSession->open();
    return false;
}

// ----------------------------------------------------------------------------
// CReporterNwSessionMgr::close
// ----------------------------------------------------------------------------
void CReporterNwSessionMgr::close()
{
    Q_D(CReporterNwSessionMgr);

    if (d->networkSession != 0) {
        qDebug() << __PRETTY_FUNCTION__ << "Close network session.";
        d->networkSession->close();
    }
}

// ----------------------------------------------------------------------------
// CReporterNwSessionMgr::stop
// ----------------------------------------------------------------------------
void CReporterNwSessionMgr::stop()
{
    Q_D(CReporterNwSessionMgr);

    if (d->networkSession != 0) {
        qDebug() << __PRETTY_FUNCTION__ << "Stop network session.";
        d->networkSession->stop();
    }
}

// ----------------------------------------------------------------------------
// CReporterNwSessionMgr::networkError
// ----------------------------------------------------------------------------
void CReporterNwSessionMgr::networkError(QNetworkSession::SessionError error)
{
    Q_D(CReporterNwSessionMgr);
    Q_UNUSED(error);

    if (d->networkSession == 0) {
        return;
    }

    QString errorString = d->networkSession->errorString();
    qDebug() << __PRETTY_FUNCTION__ << "Network error occured:" << errorString;

    emit networkError(errorString);
}

// ----------------------------------------------------------------------------
// CReporterNwSessionMgr::networkStateChanged
// ----------------------------------------------------------------------------
void CReporterNwSessionMgr::networkStateChanged(QNetworkSession::State state)
{
    Q_D(CReporterNwSessionMgr);

    QString text = "Connection status:";

    switch (state) {
    case QNetworkSession::Invalid:
        qDebug() << __PRETTY_FUNCTION__ << text << "Invalid";
        break;
    case QNetworkSession::NotAvailable:
        qDebug() << __PRETTY_FUNCTION__ << text << "Not available";
        break;
    case QNetworkSession::Connecting:
        qDebug() << __PRETTY_FUNCTION__ << text << "Connecting";
        break;
    case QNetworkSession::Connected:
        qDebug() << __PRETTY_FUNCTION__ << text << "Connected";
        break;
    case QNetworkSession::Closing:
        qDebug() << __PRETTY_FUNCTION__ << text << "Closing";
        break;
    case QNetworkSession::Disconnected:
        // Existing session disconnected, delete the session
        qDebug() << __PRETTY_FUNCTION__ << text << "Disconnected";
        emit sessionDisconnected();
        d->networkSession->deleteLater();
        d->networkSession = 0;
        break;
    case QNetworkSession::Roaming:
        qDebug() << __PRETTY_FUNCTION__ << text << "Roaming";
        break;
    default:
        break;
    };
}

// End of file.
