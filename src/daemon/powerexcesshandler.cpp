/*
 * This file is a part of crash-reporter.
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

#include "powerexcesshandler.h"

#include "creporternamespace.h"
#include "creporterutils.h"

#include <QDebug>
#include <QSocketNotifier>

#include <libudev.h>

using CReporter::LoggingCategory::cr;

class PowerExcessHandlerPrivate
{
public:
    udev *udevHandle;
    udev_monitor *udevMonitor;
    QSocketNotifier *udevSocketNotifier;

    void handleUdevNotification();
};

PowerExcessHandler::PowerExcessHandler(QObject *parent)
    : QObject(parent), d_ptr(new PowerExcessHandlerPrivate)
{
    Q_D(PowerExcessHandler);

    d->udevHandle = udev_new();
    d->udevMonitor = udev_monitor_new_from_netlink(d->udevHandle, "udev");
    udev_monitor_filter_add_match_subsystem_devtype(d->udevMonitor, "misc", 0);
    udev_monitor_enable_receiving(d->udevMonitor);

    d->udevSocketNotifier =
        new QSocketNotifier(udev_monitor_get_fd(d->udevMonitor),
                            QSocketNotifier::Read, this);
    connect(d->udevSocketNotifier, SIGNAL(activated(int)),
            this, SLOT(handleUdevNotification()));
}

void PowerExcessHandlerPrivate::handleUdevNotification()
{
    udev_device *dev = udev_monitor_receive_device(udevMonitor);
    if (!dev) {
        qCWarning(cr) << "udev_monitor_receive_device() failed";
        return;
    }

    static const QString KEVENT_DEVPATH("/devices/virtual/misc/kevent");
    if (udev_device_get_devpath(dev) == KEVENT_DEVPATH) {
        const char *event = udev_device_get_property_value(dev, "EVENT");

        static const QString EVENT_WAKELOCK_DUMP("WakelockDump");
        static const QString EVENT_SUBSYSTEM("Subsystem");
        if (event == EVENT_WAKELOCK_DUMP || event == EVENT_SUBSYSTEM) {
            if (CReporterUtils::shouldSavePower()) {
                qCDebug(cr) << "Power excess detected, NOT requesting dump of system logs on low battery.";
            } else {
                qCDebug(cr) << "Power excess detected, requesting dump of system logs.";

                if (!CReporterUtils::invokeLogCollection(CReporter::PowerExcessPrefix)) {
                    qCWarning(cr) << "Problem invoking rich-core-dumper.";
                }
            }
        }
    }

    udev_device_unref(dev);
}

PowerExcessHandler::~PowerExcessHandler()
{
    Q_D(PowerExcessHandler);

    udev_monitor_unref(d->udevMonitor);
    udev_unref(d->udevHandle);
}

#include "moc_powerexcesshandler.cpp"
