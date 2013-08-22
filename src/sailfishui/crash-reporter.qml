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

            TextSwitch {
                id: reporterSwitch
                automaticCheck: false
                checked: CrashReporterService.running
                //% "Report application crashes"
                text: qsTrId("settings_crash-reporter_report_crashes")
                //% "Collects and uploads a report when application crashes"
                description: qsTrId("settings_crash-reporter_report_crashes_description")

                property bool startingUp: false

                onClicked: {
                    if (!checked && CrashReporterService.masked) {
                        /* Unmask the crash reporter service first and wait
                         * for finished. */
                        startingUp = true;
                        CrashReporterService.masked = false
                    } else {
                        CrashReporterService.masked = checked
                        CrashReporterService.running = !checked
                    }
                }
                
                Connections {
                    target: CrashReporterService
                    onMaskedChanged: {
                        if (reporterSwitch.startingUp && !CrashReporterService.masked) {
                            reporterSwitch.startingUp = false
                            CrashReporterService.running = true
                        }
                    }
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
                automaticCheck: false
                checked: PrivacySettings.coreDumping
                //% "Include core dump"
                text: qsTrId("settings_crash-reporter_include_core")
                //% "Includes an application core dump into the crash report."
                description: qsTrId("settings_crash-reporter_include_core_description")
                onClicked: PrivacySettings.coreDumping = !PrivacySettings.coreDumping
            }

            TextSwitch {
                automaticCheck: false
                checked: PrivacySettings.notifications
                //% "Notifications"
                text: qsTrId("settings_crash-reporter_notifications")
                //% "Displays user notifications when core dump is being uploaded"
                description: qsTrId("settings_crash-reporter_notifications_description")
                onClicked: PrivacySettings.notifications = !PrivacySettings.notifications
            }

            TextSwitch {
                automaticCheck: false
                checked: PrivacySettings.includeStackTrace
                //% "Include stack trace"
                text: qsTrId("settings_crash-reporter_include_stack_trace")
                //% "Includes stack trace into crash report. Can trigger installation of a debugger and debug symbols when enabled."
                description: qsTrId("settings_crash-reporter_include_stack_trace_description")
                onClicked: PrivacySettings.includeStackTrace = !PrivacySettings.includeStackTrace
            }

            TextSwitch {
                automaticCheck: false
                checked: PrivacySettings.autoDeleteDuplicates
                //% "Auto-delete duplicates"
                text: qsTrId("settings_crash-reporter_autodelete_duplicates")
                //% "Automatically deletes duplicate rich cores"
                description: qsTrId("settings_crash-reporter_autodelete_duplicates_description")
                onClicked: PrivacySettings.autoDeleteDuplicates = !PrivacySettings.autoDeleteDuplicates
            }

            TextSwitch {
                automaticCheck: false
                checked: PrivacySettings.lifelog
                //% "Life logging"
                text: qsTrId("settings_crash-reporter_lifelog")
                //% "Periodically collects system statistics"
                description: qsTrId("settings_crash-reporter_lifelog_description")
                onClicked: PrivacySettings.lifelog = !PrivacySettings.lifelog
            }

            SectionHeader {
                //% "Logging"
                text: qsTrId("settings_crash-reporter_logging")
            }

            ComboBox {
                //% "Logger type"
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
                        //% "none"
                        text: qsTrId("settings_crash-reporter_logging_type_none")
                    }
                    MenuItem {
                        id: fileItem
                        //% "file"
                        text: qsTrId("settings_crash-reporter_logging_type_file")
                    }
                    MenuItem {
                        id: syslogItem
                        //% "system log"
                        text: qsTrId("settings_crash-reporter_logging_type_syslog")
                    }
                }
            }
        }
    }
}
