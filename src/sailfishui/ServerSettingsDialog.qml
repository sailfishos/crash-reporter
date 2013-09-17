/*
 * This file is part of crash-reporter
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

import QtQuick 2.0
import Sailfish.Silica 1.0
import com.jolla.settings.crashreporter 1.0

Dialog {
    property bool acceptable: serverUrlText.acceptableInput
        && serverPortText.acceptableInput
        && serverPathText.acceptableInput
        && usernameText.acceptableInput
        && passwordText.acceptableInput

    canAccept: acceptable

    onAccepted: {
        ApplicationSettings.serverUrl = serverUrlText.text
        ApplicationSettings.serverPort = serverPortText.text
        ApplicationSettings.serverPath = serverPathText.text
        ApplicationSettings.useSsl = (serverUrlText.text.indexOf("https://") == 0)

        ApplicationSettings.username = usernameText.text
        ApplicationSettings.password = passwordText.text
    }

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: column.height

        Column {
            id: column

            width: parent.width
            DialogHeader {}

            TextField {
                id: serverUrlText

                width: parent.width
                inputMethodHints: Qt.ImhUrlCharactersOnly | Qt.ImhNoPredictiveText
                //% "Enter server URL"
                placeholderText: qsTrId("settings_crash-reporter_server_url_placeholder")
                text: ApplicationSettings.serverUrl
                validator: RegExpValidator { regExp: /^http[s]?:\/\/\w([\w-\.]*\w)*$/ }
                label: qsTrId("settings_crash-reporter_server_url")
                EnterKey.enabled: acceptableInput
                EnterKey.onClicked: serverPortText.focus = true
            }

            TextField {
                id: serverPortText

                width: parent.width
                inputMethodHints: Qt.ImhDigitsOnly | Qt.ImhNoPredictiveText
                //% "Enter server port"
                placeholderText: qsTrId("settings_crash-reporter_server_port_placeholder")
                text: ApplicationSettings.serverPort
                validator: IntValidator { bottom: 1; top: 65535 }
                //% "Server port"
                label: qsTrId("settings_crash-reporter_server_port")
                EnterKey.enabled: acceptableInput
                EnterKey.onClicked: serverPathText.focus = true
            }

            TextField {
                id: serverPathText

                width: parent.width
                inputMethodHints: Qt.ImhNoPredictiveText
                //% "Enter server path"
                placeholderText: qsTrId("settings_crash-reporter_server_path_placeholder")
                text: ApplicationSettings.serverPath
                validator: RegExpValidator { regExp: /^\/.*$/ }
                //% "Server path"
                label: qsTrId("settings_crash-reporter_server_path")
                EnterKey.enabled: acceptableInput
                EnterKey.onClicked: usernameText.focus = true
            }

            SectionHeader {
                //% "Security"
                text: qsTrId("settings_crash-reporter_security")
            }

            TextField {
                id: usernameText

                width: parent.width
                inputMethodHints: Qt.ImhNoPredictiveText
                //% "Enter username"
                placeholderText: qsTrId("settings_crash-reporter_username_placeholder")
                text: ApplicationSettings.username
                //% "Username"
                label: qsTrId("settings_crash-reporter_username")
                EnterKey.onClicked: passwordText.focus = true
            }

            TextField {
                id: passwordText

                width: parent.width
                inputMethodHints: Qt.ImhNoPredictiveText
                //% "Enter password"
                placeholderText: qsTrId("settings_crash-reporter_password_placeholder")
                text: ApplicationSettings.password
                //% "Password"
                label: qsTrId("settings_crash-reporter_password")
                echoMode: TextInput.Password
                EnterKey.onClicked: serverUrlText.focus = true
            }
        }
    }
}
