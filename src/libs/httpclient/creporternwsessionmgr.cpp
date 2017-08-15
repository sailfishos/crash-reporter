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
#include "creporterutils.h"

#ifndef CREPORTER_UNIT_TEST
#include "creporterprivacysettingsmodel.h"
#endif

using CReporter::LoggingCategory::cr;

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

    //! @return True if default connection is in disconnected state.
    static bool connectionIsActive();
};

QNetworkConfigurationManager& CReporterNwSessionMgrPrivate::networkManager()
{
    static QNetworkConfigurationManager manager;
    return manager;
}

bool CReporterNwSessionMgrPrivate::connectionIsActive()
{
    QNetworkConfiguration config =
            CReporterNwSessionMgrPrivate::networkManager().defaultConfiguration();

    return ((config.state() & QNetworkConfiguration::Active) == QNetworkConfiguration::Active);
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

bool CReporterNwSessionMgr::canUseNetworkConnection()
{
    if (CReporterPrivacySettingsModel::instance()->allowMobileData()) {
        // We're allowed to use any network connection; go on.
        return true;
    }

    // Check that we're not using mobile data.

    QNetworkConfigurationManager &manager =
            CReporterNwSessionMgrPrivate::networkManager();

    manager.updateConfigurations();
    QNetworkConfiguration config(manager.defaultConfiguration());

#ifndef CREPORTER_UNIT_TEST
        qCDebug(cr) << "Network configurations available:";
        foreach(const QNetworkConfiguration& cfg, manager.allConfigurations()) {
            qCDebug(cr) << ' ' << cfg.name() << cfg.bearerTypeName() << cfg.state()
                     << cfg.identifier();
        }
        qCDebug(cr) << "Default configuration:" << config.name();
#endif

    return (config.bearerType() == QNetworkConfiguration::BearerWLAN) ||
           (config.bearerType() == QNetworkConfiguration::BearerEthernet) ||
           !CReporterNwSessionMgrPrivate::connectionIsActive();
}

// ----------------------------------------------------------------------------
// CReporterNwSessionMgr::open
// ----------------------------------------------------------------------------
bool CReporterNwSessionMgr::open()
{
    Q_D(CReporterNwSessionMgr);

    if (d->networkSession == 0) {
        /* Device USB network might not be reported by a configuration manager.
         * If we detect the default connection is inactive, try to bypass
         * the manager and go on without QNetworkSession. If the  cable is
         * plugged in, connection has still a chance to succeed. */
        if (!CReporterNwSessionMgrPrivate::connectionIsActive()) {
            qCDebug(cr) << "No active connection is available. "
                    "Going on, there still might be USB cable connected...";
            return true;
        }

        qCDebug(cr) << "No existing network session.";
        // If there was no network session, create one.
        d->networkSession =
                new QNetworkSession(d->networkManager().defaultConfiguration());

        connect(d->networkSession, SIGNAL(stateChanged(QNetworkSession::State)),
                this, SLOT(networkStateChanged(QNetworkSession::State)));
        connect(d->networkSession, SIGNAL(error(QNetworkSession::SessionError)),
                this, SLOT(networkError(QNetworkSession::SessionError)));
        connect(d->networkSession, SIGNAL(opened()), this, SIGNAL(sessionOpened()));
    }
    else if (d->networkSession->isOpen()) {
        qCDebug(cr) << "Using existing network session.";
        return true;
    }

    qCDebug(cr) << "Opening network session...";
#ifndef CREPORTER_UNIT_TEST 
    if (CReporterPrivacySettingsModel::instance()->automaticSendingEnabled()) {
	qCDebug(cr) << "... with ConnectInBackground set";
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
        qCDebug(cr) << "Close network session.";
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
        qCDebug(cr) << "Stop network session.";
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
    qCDebug(cr) << "Network error occured:" << errorString;

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
        qCDebug(cr) << text << "Invalid";
        break;
    case QNetworkSession::NotAvailable:
        qCDebug(cr) << text << "Not available";
        break;
    case QNetworkSession::Connecting:
        qCDebug(cr) << text << "Connecting";
        break;
    case QNetworkSession::Connected:
        qCDebug(cr) << text << "Connected";
        break;
    case QNetworkSession::Closing:
        qCDebug(cr) << text << "Closing";
        break;
    case QNetworkSession::Disconnected:
        // Existing session disconnected, delete the session
        qCDebug(cr) << text << "Disconnected";
        emit sessionDisconnected();
        d->networkSession->deleteLater();
        d->networkSession = 0;
        break;
    case QNetworkSession::Roaming:
        qCDebug(cr) << text << "Roaming";
        break;
    default:
        break;
    };
}

// End of file.
