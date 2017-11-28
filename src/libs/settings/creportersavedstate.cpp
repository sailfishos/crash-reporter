/* This file is part of crash-reporter
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
 */

#include "creportersavedstate.h"

/**
 * key/value pairs for persistent application state.
 *
 * @namespace SavedState
 */
namespace SavedState {
const QString CrashNotificationId = "SavedState/crash_notification_id";
const QString StorageUsageNotificationId = "SavedState/storageUsage_notification_id";
const QString UploadSuccessNotificationId = "SavedState/upload_success_notification_id";
const QString UploadFailedNotificationId = "SavedState/upload_failed_notification_id";
const QString UploadSuccessCount = "SavedState/upload_success_count";
}

class CReporterSavedStatePrivate
{
public:
    CReporterSavedStatePrivate(CReporterSavedState *q);

    int intValue(const QString &key, const QVariant &defaultValue) const;

private:
    Q_DECLARE_PUBLIC(CReporterSavedState)
    CReporterSavedState *q_ptr;
};

CReporterSavedStatePrivate::CReporterSavedStatePrivate(CReporterSavedState *q)
    : q_ptr(q)
{
}

int CReporterSavedStatePrivate::intValue(const QString &key, const QVariant &defaultValue) const
{
    const Q_Q(CReporterSavedState);

    bool ok;
    int result = q->value(key, defaultValue).toInt(&ok);

    return ok ? result : defaultValue.toInt();
}

CReporterSavedState *CReporterSavedState::_instance = 0;

CReporterSavedState::CReporterSavedState()
    : CReporterSettingsBase("crash-reporter-settings", "saved-state"),
      d_ptr(new CReporterSavedStatePrivate(this))
{
}

CReporterSavedState *CReporterSavedState::instance()
{
    if (!_instance) {
        _instance = new CReporterSavedState();
    }

    return _instance;
}

void CReporterSavedState::freeSingleton()
{
    if (_instance) {
        delete _instance;
        _instance = 0;
    }
}

CReporterSavedState::~CReporterSavedState()
{
}

quint32 CReporterSavedState::crashNotificationId() const
{
    const Q_D(CReporterSavedState);

    return d->intValue(SavedState::CrashNotificationId, 0);
}

void CReporterSavedState::setCrashNotificationId(quint32 id)
{
    if (setValue(SavedState::CrashNotificationId, id)) {
        emit crashNotificationIdChanged();
    }
}

quint32 CReporterSavedState::storageUsageNotificationId() const
{
    const Q_D(CReporterSavedState);

    return d->intValue(SavedState::StorageUsageNotificationId, 0);
}

void CReporterSavedState::setStorageUsageNotificationId(quint32 id)
{
    if (setValue(SavedState::StorageUsageNotificationId, id)) {
        emit storageUsageNotificationIdChanged();
    }
}

quint32 CReporterSavedState::uploadSuccessNotificationId() const
{
    const Q_D(CReporterSavedState);

    return d->intValue(SavedState::UploadSuccessNotificationId, 0);
}

void CReporterSavedState::setUploadSuccessNotificationId(quint32 id)
{
    if (setValue(SavedState::UploadSuccessNotificationId, id)) {
        emit uploadSuccessNotificationIdChanged();
    }
}

quint32 CReporterSavedState::uploadFailedNotificationId() const
{
    const Q_D(CReporterSavedState);

    return d->intValue(SavedState::UploadFailedNotificationId, 0);
}

void CReporterSavedState::setUploadFailedNotificationId(quint32 id)
{
    if (setValue(SavedState::UploadFailedNotificationId, id)) {
        emit uploadFailedNotificationIdChanged();
    }
}

int CReporterSavedState::uploadSuccessCount() const
{
    const Q_D(CReporterSavedState);

    return d->intValue(SavedState::UploadSuccessCount, 0);
}

void CReporterSavedState::setUploadSuccessCount(int count)
{
    if (setValue(SavedState::UploadSuccessCount, count)) {
        emit uploadSuccessCountChanged();
    }
}
