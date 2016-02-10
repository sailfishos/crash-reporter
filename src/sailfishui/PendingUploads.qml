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

Page {
    id: root

    property bool _modifyingReportList

    SilicaListView {
        id: uploadsView

        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                enabled: Adapter.reportsToUpload > 0
                //% "Delete unsent reports"
                text: qsTrId("quick-feedback_delete_reports")
                onClicked: uploadsView.headerItem.showDeleteReportsRemorse()
            }
            MenuItem {
                enabled: Adapter.reportsToUpload > 0
                //% "Upload crash reports now"
                text: qsTrId("quick-feedback_upload_now")
                onClicked: {
                    root._modifyingReportList = true
                    Adapter.uploadAllCrashReports();
                }
            }
        }

        header: Item {
            width: parent.width
            height: childrenRect.height

            PageHeader {
                id: header

                //% "Pending uploads"
                title: qsTrId("crash-reporter_pending_uploads")
            }
            Item {
                id: remorseArea

                anchors.top: header.bottom
                height: 0
                width: parent.width
                visible: false

                RemorseItem {
                    id: remorse

                    onCanceled: _collapse()
                    onTriggered: _collapse()

                    function _collapse() {
                        remorseArea.height = 0
                        height = 0
                    }
                }
            }

            function showDeleteReportsRemorse() {
                remorseArea.height = Theme.itemSizeSmall
                //% "Deleting %n crash report(s)"
                remorse.execute(remorseArea, qsTrId("quick-feedback_deleting", Adapter.reportsToUpload),
                    function() {
                        root._modifyingReportList = true
                        Adapter.deleteAllCrashReports()
                    })
            }
        }

        model: Adapter.pendingUploads

        VerticalScrollDecorator {}

        delegate: ListItem {
            id: listDelegate

            function remove() {
                 //% "Deleting "
                 var remorseMessage = qsTrId("settings_crash-reporter_deleting") + model.application

                 remorseAction(remorseMessage, function() {
                     Adapter.deleteCrashReport(model.filePath)
                 })
            }

            contentHeight: crashDetails.visible ? Theme.itemSizeMedium : Theme.itemSizeSmall

            menu: Component {
                ContextMenu {
                    MenuItem {
                        //% "Upload"
                        text: qsTrId("settings_crash-reporter_upload")
                        onClicked: {
                            Utils.notifyAutoUploader([ model.filePath ], false)
                        }
                    }
                    MenuItem {
                        //% "Delete"
                        text: qsTrId("settings_crash-reporter_delete")
                        onClicked: {
                            remove()
                        }
                    }
                }
            }

            Item {
                x: Theme.horizontalPageMargin
                width: parent.width - (2 * Theme.paddingLarge)
                height: parent.height

                Label {
                    id: appLabel
                    anchors {
                        verticalCenter: parent.verticalCenter
                        verticalCenterOffset: crashDetails.visible ? -implicitHeight/2 : 0
                    }
                    width: parent.width - dateLabel.width

                    text: model.application
                    truncationMode: TruncationMode.Fade
                    color: listDelegate.highlighted ? Theme.highlightColor : Theme.primaryColor
                }
                Label {
                    id: dateLabel
                    anchors {
                        right: parent.right
                        verticalCenter: appLabel.verticalCenter
                    }
                    text: Qt.formatDateTime(model.dateCreated)
                    font.pixelSize: Theme.fontSizeExtraSmall
                    color: appLabel.color
                }
                Row {
                    id: crashDetails
                    visible: Utils.reportIncludesCrash(model.application)

                    anchors.top: appLabel.bottom
                    anchors.left: parent.left

                    Label {
                        text: model.signal
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: appLabel.color
                    }
                    Label {
                        text: " PID "
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: listDelegate.highlighted ? Theme.secondaryHighlightColor : Theme.secondaryColor
                    }
                    Label {
                        text: model.pid
                        font.pixelSize: Theme.fontSizeExtraSmall
                        color: appLabel.color
                    }
                }
            }
        }

        onCountChanged: {
            if (count == 0 && root._modifyingReportList) {
                pageStack.pop()
            }
            root._modifyingReportList = false
        }
    }
}
