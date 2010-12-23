Feature: Send All Crash Reports Dialog
	As an end user
	I want to be notified, if there are unsent crash reports in the device. 
    I have an option to send or delete all I can just ignore the dialog.

Scenario: Pressing Send all button from the dialog will upload the crash reports.
	Given there are unsent crash reports in the system
	When Crash Reporter Daemon starts
    Then a notification with text "<p><b>This system has stored crash reports.</b></p><p>Tap to send or delete.</p>" is displayed
    When application "crash-reporter-ui" starts 
    Then dialog with title "Crash Reporter" is displayed
    When I press "Send" button
    Then progress bar is displayed to represent upload status
    When upload finishes
    Then A message box with title "Crash Reporter" is displayed
        And crash reports are deleted
    When I tap OK button from the dialog
    Then Crash Reporter UI is closed

Scenario: Pressing Delete all button from the dialog will remove all crash reports from the system.
    Given there are unsent crash reports in the system
    When Crash Reporter Daemon starts
    Then a notification with text "<p><b>This system has stored crash reports.</b></p><p>Tap to send or delete.</p>" is displayed
    When application "crash-reporter-ui" starts 
    Then dialog with title "Crash Reporter" is displayed
    When I press "Delete all" button
    Then crash reports are deleted
        And Crash Reporter UI is closed

Scenario: Pressing dialog close button will ignore the dialog
    Given there are unsent crash reports in the system
    When Crash Reporter Daemon starts
    Then a notification with text "<p><b>This system has stored crash reports.</b></p><p>Tap to send or delete.</p>" is displayed
    When application "crash-reporter-ui" starts 
    Then dialog with title "Crash Reporter" is displayed
    When I press "Close" button
    Then Crash Reporter UI is closed