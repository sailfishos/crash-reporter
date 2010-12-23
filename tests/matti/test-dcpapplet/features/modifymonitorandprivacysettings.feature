Feature: Modify and monitor and privacy settings
    As an end user 
    I want to be able to set crash report notifications On/ Off
    and control, what information is included in reports.

Scenario: Enable and disable core dumping option.
    Given The "Crash Reporter Settings" applet is active
    When I tap button "SaveCrashReportsSwitch" next to "SaveCrashReportsLabel" option
    Then Button's "ShowNotificationsSwitch" next to "ShowNotificationsLabel" option, enabled status is "true" and checked status is "false" 
        And Button's "AutoDeleteDupsSwitch" next to "AutoDeleteDupsLabel" option, enabled status is "false" and checked status is "true" 
        And Button's "IncludeCoreDumpSwitch" next to "IncludeCoreDumpLabel" option, enabled status is "false" and checked status is "true" 
        And Button's "IncludeSyslogSwitch" next to "IncludeSyslogLabel" option, enabled status is "false" and checked status is "true"
        And Button's "IncludePackagesSwitch" next to "IncludePackagesLabel" option, enabled status is "false" and checked status is "true"
        And Button's "ReduceCoreDumpSwitch" next to "ReduceCoreDumpLabel" option, enabled status is "false" and checked status is "true"
    When I go back to category view
    Then "Show notifications: OFF" is displayed in brief widget
    When I go back to "Crash Reporter Settings" applet view
    Then Button's "SaveCrashReportsSwitch" next to "SaveCrashReportsLabel" option, enabled status is "true" and checked status is "false"
        And Button's "ShowNotificationsSwitch" next to "ShowNotificationsLabel" option, enabled status is "true" and checked status is "false"
    When I tap button "ShowNotificationsSwitch" next to "ShowNotificationsLabel" option
    Then Button's "SaveCrashReportsSwitch" next to "SaveCrashReportsLabel" option, enabled status is "true" and checked status is "true"
        And Button's "AutoDeleteDupsSwitch" next to "AutoDeleteDupsLabel" option, enabled status is "true" and checked status is "true" 
        And Button's "IncludeCoreDumpSwitch" next to "IncludeCoreDumpLabel" option, enabled status is "true" and checked status is "true" 
        And Button's "IncludeSyslogSwitch" next to "IncludeSyslogLabel" option, enabled status is "true" and checked status is "true"
        And Button's "IncludePackagesSwitch" next to "IncludePackagesLabel" option, enabled status is "true" and checked status is "true"
        And Button's "ReduceCoreDumpSwitch" next to "ReduceCoreDumpLabel" option, enabled status is "true" and checked status is "true"
    When I go back to category view
    Then "Show notifications: ON" is displayed in brief widget
