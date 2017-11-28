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

#ifndef CREPORTERSAVEDSTATE_H
#define CREPORTERSAVEDSTATE_H

#include "creportersettingsbase.h"

class CReporterSavedStatePrivate;

/**
 * @class CReporterApplicationSettings
 * @brief This a singleton class for reading and writing crash-reporter application settings.
 */
class CReporterSavedState: public CReporterSettingsBase
{
    Q_OBJECT

    Q_PROPERTY(quint32 crashNotificationId READ crashNotificationId WRITE setCrashNotificationId NOTIFY crashNotificationIdChanged)
    Q_PROPERTY(quint32 storageUsageNotificationId READ storageUsageNotificationId WRITE setStorageUsageNotificationId NOTIFY storageUsageNotificationIdChanged)
    Q_PROPERTY(quint32 uploadSuccessNotificationId READ uploadSuccessNotificationId WRITE setUploadSuccessNotificationId NOTIFY uploadSuccessNotificationIdChanged)
    Q_PROPERTY(quint32 uploadFailedNotificationId READ uploadFailedNotificationId WRITE setUploadFailedNotificationId NOTIFY uploadFailedNotificationIdChanged)
    Q_PROPERTY(int uploadSuccessCount READ uploadSuccessCount WRITE setUploadSuccessCount NOTIFY uploadSuccessCountChanged)

public:
    /**
     * Maintains an instance of CReporterSavedState.
     *
     * @return class reference.
     */
    static CReporterSavedState *instance();

    /**
     * Frees the class instance.
     */
    static void freeSingleton();

    ~CReporterSavedState();

    quint32 crashNotificationId() const;
    void setCrashNotificationId(quint32 id);

    quint32 storageUsageNotificationId() const;
    void setStorageUsageNotificationId(quint32 id);

    quint32 uploadSuccessNotificationId() const;
    void setUploadSuccessNotificationId(quint32 id);

    quint32 uploadFailedNotificationId() const;
    void setUploadFailedNotificationId(quint32 id);

    int uploadSuccessCount() const;
    void setUploadSuccessCount(int count);

signals:
    void crashNotificationIdChanged();
    void storageUsageNotificationIdChanged();
    void uploadSuccessNotificationIdChanged();
    void uploadFailedNotificationIdChanged();
    void uploadSuccessCountChanged();

private:
    CReporterSavedState();

    Q_DISABLE_COPY(CReporterSavedState)

    Q_DECLARE_PRIVATE(CReporterSavedState)
    QScopedPointer<CReporterSavedStatePrivate> d_ptr;

    static CReporterSavedState *_instance;
};

#endif // CREPORTERSAVEDSTATE_H
