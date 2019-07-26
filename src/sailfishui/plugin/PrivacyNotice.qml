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

Item {
    id: root

    property Page page
    readonly property bool pageActive: page.status === PageStatus.Active
    property bool privacyNoticeShown

    onPageActiveChanged: {
        if (pageActive) {
            if (PrivacySettings.privacyNoticeAccepted) {
                pageStack.pop()
            } else if (!privacyNoticeShown) {
                noticeGiven = true
                pageStack.push(privacyNoticeDialog, null, PageStackAction.Immediate)
            } else {
                pageStack.pop()
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
            }

            SilicaFlickable {
                anchors.fill: parent
                contentHeight: column.height
                Column {
                    id: column

                    width: parent.width
                    spacing: Theme.paddingLarge
                    DialogHeader {}
                    Label {
                        x: Theme.horizontalPageMargin
                        width: parent.width - x*2

                        font.pixelSize: Theme.fontSizeLarge
                        //% "Privacy notice"
                        text: qsTrId("quick-feedback_privacy_notice")
                        wrapMode: Text.WordWrap
                        color: Theme.highlightColor
                    }
                    Label {
                        x: Theme.horizontalPageMargin
                        width: parent.width - x*2

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
                        color: Theme.highlightColor
                    }
                    Label {
                        x: Theme.horizontalPageMargin
                        width: parent.width - x*2

                        font.pixelSize: Theme.fontSizeSmall
                        //% "By accepting this dialog you declare you are aware of "
                        //% "the potential security risks and give consent to "
                        //% "process the data collected from your device for the "
                        //% "purpose of analyzing bugs in the applications or the "
                        //% "operating system."
                        text: qsTrId("quick-feedback_privacy_notice_text_2")
                        wrapMode: Text.WordWrap
                        color: Theme.highlightColor
                    }
                }
                VerticalScrollDecorator {}
            }
        }
    }
}
