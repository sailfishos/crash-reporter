/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2014 Jolla Ltd.
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

import QtQuick 2.0
import Sailfish.Silica 1.0
import com.jolla.settings.crashreporter 1.0

TextSwitch {
    id: serviceSwitch

    property alias serviceName: service.serviceName
    property alias managerType: service.managerType
    property alias serviceEnabled: service.enabled

    signal beforeStateChange(bool newState)
    signal afterStateChange(bool newState)

    automaticCheck: false

    SystemdService {
        id: service
    }

    onClicked: {
        var newState = !checked

        beforeStateChange(newState)

        if (newState) {
            service.start()
        } else {
            service.stop()
        }

        afterStateChange(newState)
    }

    states: [
        State {
            name: "inactive"
            when: (service.state == SystemdService.Inactive)
            PropertyChanges { target: serviceSwitch; checked: false }
        },
        State {
            name: "activating"
            when: (service.state == SystemdService.Activating)
            PropertyChanges { target: serviceSwitch; checked: true; busy: true }
        },
        State {
            name: "active"
            when: (service.state == SystemdService.Active)
            PropertyChanges { target: serviceSwitch; checked: true }
        }
    ]
}
