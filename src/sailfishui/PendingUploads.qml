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

Page {
    RemorsePopup {
        id: deleteReportsRemorse
        function run() {
            //% "Deleting %n crash report(s)"
            execute(qsTrId("quick-feedback_deleting", Adapter.reportsToUpload),
                function() {
                    Adapter.deleteAllCrashReports()
                })
        }
    }

    SilicaListView {
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                enabled: Adapter.reportsToUpload > 0
                //% "Delete unsent reports"
                text: qsTrId("quick-feedback_delete_reports")
                onClicked: {
                    deleteReportsRemorse.run()
                }
            }
            MenuItem {
                enabled: Adapter.reportsToUpload > 0
                //% "Upload crash reports now"
                text: qsTrId("quick-feedback_upload_now")
                onClicked: {
                    Adapter.uploadAllCrashReports();
                }
            }
        }

        header: PageHeader {
            //% "Pending uploads"
            title: qsTrId("crash-reporter_pending_uploads")
        }

        model: Adapter.pendingUploads

        VerticalScrollDecorator {}

        delegate: ListItem {
            x: Theme.paddingLarge
            width: parent.width - (2 * Theme.paddingLarge)

            Label {
                id: appLabel

                text: model.application
            }
            Label {
                anchors.right: parent.right

                text: Qt.formatDateTime(model.dateCreated)
                font.pixelSize: Theme.fontSizeExtraSmall
                color: Theme.primaryColor
            }
            Row {
                visible: Utils.reportIncludesCrash(model.application)

                anchors.top: appLabel.bottom
                anchors.left: parent.left

                Label {
                    text: "PID "
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.secondaryColor
                }
                Label {
                    text: model.pid
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: Theme.primaryColor
                }
            }
        }

        onCountChanged: {
            if (count == 0) {
                pageStack.pop()
            }
        }
    }
}
