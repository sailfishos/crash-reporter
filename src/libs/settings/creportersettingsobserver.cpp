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

// System includes.

#include <QDebug>
#include <QFile>
#include <QStringList>

// User includes.

#include "creportersettingsobserver.h"
#include "creportersettingsobserver_p.h"

// ======== Class CReporterSettingsObserverPrivate ========

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterSettingsObserverPrivate::CReporterSettingsObserverPrivate
// ----------------------------------------------------------------------------
CReporterSettingsObserverPrivate::CReporterSettingsObserverPrivate(const QString &path) :
        m_settings(new QSettings(path, QSettings::NativeFormat, this))
{
    qDebug() << __PRETTY_FUNCTION__ << "Observing file:" << path;
    m_file = path;
    m_watcher.addPath(path);
    connect(&m_watcher, SIGNAL(fileChanged(QString)), this, SLOT(fileChanged(QString)));
}

// ----------------------------------------------------------------------------
// CReporterSettingsObserverPrivate::~CReporterSettingsObserverPrivate
// ----------------------------------------------------------------------------
CReporterSettingsObserverPrivate::~CReporterSettingsObserverPrivate()
{
    qDebug() << __PRETTY_FUNCTION__ << "Destroyed.";
    m_notifications.clear();
}

// ----------------------------------------------------------------------------
// CReporterSettingsObserverPrivate::addWatcher
// ----------------------------------------------------------------------------
void CReporterSettingsObserverPrivate::addWatcher(const QString &key)
{
    // Add to notifications hash, if key doesn't exist yet.
    if (!m_notifications.contains(key)) {
        qDebug() << __PRETTY_FUNCTION__ << "Adding watcher to key:" << key;
        m_notifications.insert(key, m_settings->value(key));
    }
}

// ----------------------------------------------------------------------------
// CReporterSettingsObserverPrivate::removeWatcher
// ----------------------------------------------------------------------------
void CReporterSettingsObserverPrivate::removeWatcher(const QString &key)
{
    if (m_notifications.contains(key)) {
        // Remove from hash.
        qDebug() << __PRETTY_FUNCTION__ << "Remove watcher from key:" << key;
        m_notifications.remove(key);
    }
}

// ----------------------------------------------------------------------------
// CReporterSettingsObserverPrivate::fileChanged
// ----------------------------------------------------------------------------
void CReporterSettingsObserverPrivate::fileChanged(const QString &path)
{
    qDebug() << __PRETTY_FUNCTION__ << "File:" << path << "has changed.";

    if (path != m_file || !QFile::exists(m_file)) {
        return;
    }

    // Some manual file operations such as sed can cause the QFileSystemWatcher to think
    // the file was deleted which stops the file monitoring after the last fileChanged signal.
    m_watcher.addPath(m_file);

    // Synchronization of QSetting is needed, to make changes visible done from another thread.
    m_settings->sync();
    QHash<QString, QVariant>::iterator iter;
        
    for (iter = m_notifications.begin(); iter != m_notifications.end(); ++iter) {
        // Loop through subscribed key/ value pairs.
        QVariant settingValue = m_settings->value(iter.key());

        if (iter.value() != settingValue) {
            qDebug() << __PRETTY_FUNCTION__ << "Key:" << iter.key() << "has changed. Old value:"
                    << iter.value() << "New value:" << settingValue;
            // Update new value and notify.
            m_notifications.insert(iter.key(), settingValue);
            emit q_ptr->valueChanged(iter.key(), settingValue);
         }
     }
}

// ======== Class CReporterSettingsObserver ========

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterSettingsObserver::CReporterSettingsObserver
// ----------------------------------------------------------------------------
CReporterSettingsObserver::CReporterSettingsObserver(const QString &path, QObject *parent) :
        QObject(parent),
        d_ptr(new CReporterSettingsObserverPrivate(path))
{
    Q_D(CReporterSettingsObserver);

    d->q_ptr = this;
}

// ----------------------------------------------------------------------------
// CReporterSettingsObserver::~CReporterSettingsObserver
// ----------------------------------------------------------------------------
CReporterSettingsObserver::~CReporterSettingsObserver()
{
    qDebug() << __PRETTY_FUNCTION__ << "Destroyed.";
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterSettingsObserver::addWatcher
// ----------------------------------------------------------------------------
void CReporterSettingsObserver::addWatcher(const QString &key)
{
    Q_D(CReporterSettingsObserver);

    d->addWatcher(key);
}

// ----------------------------------------------------------------------------
// CReporterSettingsObserver::removeWatcher
// ----------------------------------------------------------------------------
void CReporterSettingsObserver::removeWatcher(const QString &key)
{
    Q_D(CReporterSettingsObserver);

    d->removeWatcher(key);
}

// End of file.
