Feature: Display privacy disclaimer
    As an end user 
    I want to see what kind of private information might be
    included in the crash reports.

Scenario: Display privacy disclaimer.
    Given The "Crash Reporter Settings" applet is active
    When I tap "ShowPrivacyButton" button
    Then A dialog with title "Crash Reporter Privacy Disclaimer" is displayed
    When I reject the dialog
    Then The dialog is closed