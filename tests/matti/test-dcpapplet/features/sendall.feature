Feature: Upload all unsent crash reports
    As an end user 
    I want to be able to upload all unsent crash reports.

Scenario: Upload unsent crash reports.
    Given There are unsent crash reports in the system
    Given The "Crash Reporter Settings" applet is active
    When I tap "SendAllButton" button
    Then A message box with title "Crash Reporter" is displayed
        And crash reports are deleted
    When I tap OK button from the dialog
    Then Crash Reporter UI is closed