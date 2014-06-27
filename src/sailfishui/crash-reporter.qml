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

    SilicaFlickable {
        anchors.fill: parent
        contentHeight: content.height

        Column {
            id: content

            width: parent.width

            PageHeader {
                //% "Crash reporter"
                title: qsTrId("settings_crash-reporter")
            }

            PullDownMenu {
                MenuItem {
                    enabled: Adapter.reportsToUpload > 0
                    //% "Show pending uploads"
                    text: qsTrId("quick-feedback_pending_uploads")
                    onClicked: {
                        pageStack.push("PendingUploads.qml")
                    }
                }
                MenuLabel {
                    text: Adapter.reportsToUpload + ((Adapter.reportsToUpload == 1) ?
                        //% " report to upload"
                        qsTrId("quick-feedback_report_to_upload") :
                        //% " reports to upload"
                        qsTrId("quick-feedback_reports_to_upload"));
                }
            }

            SystemdServiceSwitch {
                serviceName: "crash-reporter.service"

                //% "Upload reports automatically"
                text: qsTrId("settings_crash-reporter_upload_automatically")
                //% "Uploads created crash reports to a telemetry server."
                description: qsTrId("settings_crash-reporter_report_crashes_description")

                onAfterStateChange: {
                    serviceEnabled = newState
                }
            }

            ValueButton {
                //% "Server URL"
                label: qsTrId("settings_crash-reporter_server_url")
                value: ApplicationSettings.serverUrl + ":" + ApplicationSettings.serverPort + ApplicationSettings.serverPath

                onClicked: {
                    pageStack.push("ServerSettingsDialog.qml")
                }
            }

            TextSwitch {
                id: createReportsSwitch
                automaticCheck: false
                checked: PrivacySettings.coreDumping
                //% "Create crash reports"
                text: qsTrId("settings_crash-reporter_create reports")
                //% "When an application crashes, a report is created including a core dump and other information to help the developers in tracing the bug."
                description: qsTrId("settings_crash-reporter_create reports_description")
                onClicked: PrivacySettings.coreDumping = !PrivacySettings.coreDumping
            }

            TextSwitch {
                automaticCheck: false
                checked: PrivacySettings.notifications
                //% "Notifications"
                text: qsTrId("settings_crash-reporter_notifications")
                //% "Displays user notifications when crash report is being uploaded."
                description: qsTrId("settings_crash-reporter_notifications_description")
                onClicked: PrivacySettings.notifications = !PrivacySettings.notifications
            }

            TextSwitch {
                automaticCheck: false
                checked: PrivacySettings.autoDeleteDuplicates
                //% "Auto-delete duplicates"
                text: qsTrId("settings_crash-reporter_autodelete_duplicates")
                //% "Each day, uploads only first 5 crash reports of an application. The others, likely duplicate, are deleted without being uploaded to conserve space."
                description: qsTrId("settings_crash-reporter_autodelete_duplicates_description")
                onClicked: PrivacySettings.autoDeleteDuplicates = !PrivacySettings.autoDeleteDuplicates
            }

            SystemdServiceSwitch {
                serviceName: "crash-reporter-endurance.service"
                managerType: SystemdService.SystemManager

                //% "Endurance reports"
                text: qsTrId("settings_crash-reporter_enable_endurance")
                //% "Reports system statistics helping diagnose problems that "
                //% "manifest themselves only after a long-term use of the "
                //% "device like memory leaks, excessive battery drain, or "
                //% "decreasing performance."
                description: qsTrId("settings_crash-reporter_enable_endurance_description")

                onBeforeStateChange: {
                    PrivacySettings.endurance = newState
                }
            }

            SystemdServiceSwitch {
                serviceName: "crash-reporter-journalspy.service"
                managerType: SystemdService.SystemManager

                //% "Journal spy"
                text: qsTrId("settings_crash-reporter_enable_journalspy")
                //% "Watches system logs for predefined regular expressions "
                //% "and creates a telemetry submission upon a found match."
                description: qsTrId("settings_crash-reporter_enable_journalspy_description")

                onBeforeStateChange: {
                    PrivacySettings.journalSpy = newState
                }
            }

            SectionHeader {
                //% "Data transmissions"
                text: qsTrId("settings_crash-reporter_data_transmissions")
            }

            TextSwitch {
                automaticCheck: false
                checked: PrivacySettings.allowMobileData
                //% "Allow using mobile data"
                text: qsTrId("settings_crash-reporter_use_mobile_data")
                //% "Enables crash report transmissions through mobile network; additional charges from the network carrier may apply. When this option is off, WLAN connection must be available in order to send crash reports."
                description: qsTrId("settings_crash-reporter_use_mobile_data_description")
                onClicked: PrivacySettings.allowMobileData = !PrivacySettings.allowMobileData
            }

            SectionHeader {
                //% "Stack trace"
                text: qsTrId("settings_crash-reporter_stack_trace")
            }

            TextSwitch {
                id: includeStackTraceSwitch
                enabled: createReportsSwitch.checked
                automaticCheck: false
                checked: PrivacySettings.includeStackTrace
                //% "Include stack trace"
                text: qsTrId("settings_crash-reporter_include_stack_trace")
                //% "Crash report will include a stack trace generated on the device."
                description: qsTrId("settings_crash-reporter_include_stack_trace_description")
                onClicked: PrivacySettings.includeStackTrace = !PrivacySettings.includeStackTrace
            }

            TextSwitch {
                enabled: includeStackTraceSwitch.checked
                automaticCheck: false
                checked: PrivacySettings.downloadDebuginfo
                //% "Download debug symbols"
                text: qsTrId("settings_crash-reporter_download_debug_symbols")
                //% "Tries to automatically download missing debug symbols before making a stack trace."
                description: qsTrId("settings_crash-reporter_download_debug_symbols_description")
                onClicked: PrivacySettings.downloadDebuginfo = !PrivacySettings.downloadDebuginfo
            }

            SectionHeader {
                //% "Logging"
                text: qsTrId("settings_crash-reporter_logging")
            }

            ComboBox {
                //% "Log reporter activity"
                label: qsTrId("settings_crash-reporter_logger_type")
                currentItem: {
                    switch (ApplicationSettings.loggerType) {
                        case "none":
                            return noneItem
                        case "file":
                            return fileItem
                        case "syslog":
                            return syslogItem
                    }
                    return noneItem
                } 

                onCurrentItemChanged: {
                    var type

                    switch (currentItem) {
                        case noneItem:
                            type = "none"
                            break
                        case fileItem:
                            type = "file"
                            break
                        case syslogItem:
                            type = "syslog"
                            break
                    }

                    ApplicationSettings.loggerType = type
                }

                menu: ContextMenu {
                    MenuItem {
                        id: noneItem
                        //% "no logging"
                        text: qsTrId("settings_crash-reporter_logging_type_none")
                    }
                    MenuItem {
                        id: fileItem
                        //% "into a file in /tmp"
                        text: qsTrId("settings_crash-reporter_logging_type_file")
                    }
                    MenuItem {
                        id: syslogItem
                        //% "into systemd journal"
                        text: qsTrId("settings_crash-reporter_logging_type_syslog")
                    }
                }
            }
            Label {
                x: Theme.paddingLarge
                width: parent.width - (2 * Theme.paddingLarge)
                //% "Debug logging of crash reporter activities to the device doesn't affect the data sent to a server. Change of this setting takes effect after crash reporter restart."
                text: qsTrId("settings_crash-reporter_after_restart")
                font.pixelSize: Theme.fontSizeExtraSmall
                wrapMode: Text.WordWrap
                color: Theme.secondaryColor
            }
        }
    }

    PrivacyNotice {}
}
