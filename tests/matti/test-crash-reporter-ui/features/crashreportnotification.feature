Feature: Crash Report Notification Dialog
	As an end user
	I want to be notified when application crashes. I have an option to
	send or delete the crash report or I can just ignore the dialog.

Scenario: Pressing Send button from the dialog will upload the crash report.
	Given there are no crash reports in the system 
	When I execute application which crashes
    Then a notification with text "<p><b>Application crashapplication crashed.</b></p><p>Tap to send crash report.</p>" is displayed
    When application "crash-reporter-ui" starts 
    Then dialog with title "The application crashapplication crashed." is displayed
	When I press "Send" button
	Then progress bar is displayed to represent upload status
    When upload finishes
    Then A message box with title "Crash Reporter" is displayed
        And crash reports are deleted
    When I tap OK button from the dialog
    Then Crash Reporter UI is closed

Scenario: Pressing Delete button from the dialog will remove the crash report from the system.
    Given there are no crash reports in the system 
    When I execute application which crashes
    Then a notification with text "<p><b>Application crashapplication crashed.</b></p><p>Tap to send crash report.</p>" is displayed
    When application "crash-reporter-ui" starts 
    Then dialog with title "The application crashapplication crashed." is displayed
	When I press "Delete" button
    Then crash reports are deleted
        And Crash Reporter UI is closed
	
Scenario: Pressing dialog close button will ignore the dialog
    Given there are no crash reports in the system 
    When I execute application which crashes
    Then a notification with text "<p><b>Application crashapplication crashed.</b></p><p>Tap to send crash report.</p>" is displayed
    When application "crash-reporter-ui" starts 
    Then dialog with title "The application crashapplication crashed." is displayed
	When I press "Close" button
    Then Crash Reporter UI is closed