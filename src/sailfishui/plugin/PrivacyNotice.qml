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
import Sailfish.Silica.theme 1.0
import com.jolla.settings.crashreporter 1.0

Item {
    property bool declined: false

    Component.onCompleted: {
        parent.statusChanged.connect(pageStatusChanged)
        parent.enabled = Qt.binding(function() {
            return PrivacySettings.privacyNoticeAccepted
        })
    }

    function pageStatusChanged() {
        if ((status == PageStatus.Active) && !PrivacySettings.privacyNoticeAccepted) {
            if (declined == true) {
                /* Reset the state back so that user is not pushed back
                 * immediately when he enters the page again. */
                declined = false
                pageStack.pop()
            } else {
                pageStack.push(privacyNoticeDialog)
            }
        }
    }

    Component {
        id: privacyNoticeDialog

        Dialog {
            onAccepted: {
                PrivacySettings.privacyNoticeAccepted = true
            }

            onRejected: {
                PrivacySettings.privacyNoticeAccepted = false
                declined = true
            }

            Column {
                anchors.fill: parent

                spacing: Theme.paddingLarge

                DialogHeader {}

                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - (2 * Theme.paddingMedium)

                    font.pixelSize: Theme.fontSizeLarge
                    //% "Privacy notice"
                    text: qsTrId("quick-feedback_privacy_notice")
                    wrapMode: Text.WordWrap
                }
                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - (2 * Theme.paddingMedium)

                    font.pixelSize: Theme.fontSizeSmall
                    //% "Please be warned that Crash Reporter and Quick "
                    //% "Feedback upload statistics of device usage to a remote "
                    //% "server, including pieces of information like IMEI "
                    //% "number that can uniquely identify your device. Crash "
                    //% "reports may also include partial or full snapshots of "
                    //% "program memory, potentially including sensitive data "
                    //% "like your unencrypted passwords, credit card numbers "
                    //% "etc."
                    text: qsTrId("quick-feedback_privacy_notice_text_1")
                    wrapMode: Text.WordWrap
                }
                Label {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - (2 * Theme.paddingMedium)

                    font.pixelSize: Theme.fontSizeSmall
                    //% "By accepting this dialog you declare you are aware of "
                    //% "the potential security risks and give consent to "
                    //% "process the data collected from your device for the "
                    //% "purpose of analyzing bugs in the applications or the "
                    //% "operating system."
                    text: qsTrId("quick-feedback_privacy_notice_text_2")
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
