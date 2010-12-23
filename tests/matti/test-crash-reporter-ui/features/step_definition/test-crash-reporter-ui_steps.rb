#############################################################
# test-crash-reporter-ui_steps.rb
# step_definition Ruby script
#############################################################

# require MATTI and rSpec spec/story
require 'matti'
include MattiVerify
#require 'spec/story'
#require 'spec/expectations'

PRIVACY_SETTINGS_TEST = "/usr/share/crash-reporter-ui-tests/testdata/crash-reporter-privacy.conf"
PRIVACY_SETTINGS_ORIG = "/home/user/.config/crash-reporter-settings/crash-reporter-privacy.conf"
PRIVACY_SETTINGS_BAK = "/home/user/.config/crash-reporter-settings/crash-reporter-privacy.conf.bak"
APP_SETTINGS_TEST = "/usr/share/crash-reporter-ui-tests/testdata/crash-reporter.conf"
APP_SETTINGS_ORIG = "/home/user/.config/crash-reporter-settings/crash-reporter.conf"
APP_SETTINGS_BAK = "/home/user/.config/crash-reporter-settings/crash-reporter.conf.bak"

TEST_APP = "/usr/lib/crash-reporter-ui-tests/testdata/crashapplication"

CORE_DUMPS_DIR = "/home/user/MyDocs/core-dumps"

Before do
    # Map the controlled sut to be 'sut_qt_maemo', which is specified in matti_parameters.xml
    @sut = MATTI.sut(:Id=>'sut_qt_maemo')
    # Use touch-display
    @sut.input = :touch

    # Save original settings and replace with test settings
    if @sut.execute_shell_command("stat " + PRIVACY_SETTINGS_ORIG) != nil then
        @sut.execute_shell_command("mv " + PRIVACY_SETTINGS_ORIG + " " + PRIVACY_SETTINGS_BAK)
    end
    
    if @sut.execute_shell_command("stat " + APP_SETTINGS_ORIG) != nil then
        @sut.execute_shell_command("mv " + APP_SETTINGS_ORIG + " " + APP_SETTINGS_BAK)
    end
    
    @sut.execute_shell_command("cp -f " + PRIVACY_SETTINGS_TEST + " " + PRIVACY_SETTINGS_ORIG)
    @sut.execute_shell_command("cp -f " + APP_SETTINGS_TEST + " " + APP_SETTINGS_ORIG)
    
    begin
        # Connect to running sysuid.
        @sysuid = @sut.application(:name => "sysuid", :__timeout => 0)
    rescue
        # Not running ... Start sysuid
        @sysuid = @sut.run(:name => "sysuid")
    end
    
    begin
        # Connect to running sysuid.
        @duihome = @sut.application(:name => "duihome", :__timeout=>0)
    rescue
        # Not running ... Start sysuid
        @duihome = @sut.run(:name => "duihome")
    end
    
    begin
        app = @sut.application(:name => "crash-reporter-ui")
        app.close
    rescue
        # Do nothing
    end
end

After do

    # Remove test settings.
    @sut.execute_shell_command("rm -f " + PRIVACY_SETTINGS_ORIG)
    @sut.execute_shell_command("rm -f " + APP_SETTINGS_ORIG)
    
    # Restore original settings.
    if @sut.execute_shell_command("stat " + PRIVACY_SETTINGS_BAK) != nil then       
        @sut.execute_shell_command("mv " + PRIVACY_SETTINGS_BAK + " " + PRIVACY_SETTINGS_ORIG)
    end
    
    if @sut.execute_shell_command("stat " + APP_SETTINGS_BAK) != nil then
        @sut.execute_shell_command("mv " + APP_SETTINGS_BAK + " " + APP_SETTINGS_ORIG)
    end
    
    # Start monitoring...
    @sut.execute_shell_command("/usr/share/crash-reporter/crash-report-monitoring start")
end

Given ("there are unsent crash reports in the system") do

    # Remove and create core-dumps directory.
    @sut.execute_shell_command("rm -rf " + CORE_DUMPS_DIR)
    @sut.execute_shell_command("mkdir " + CORE_DUMPS_DIR)
        
    # Stop monitoring...
    @sut.execute_shell_command("/usr/share/crash-reporter/crash-report-monitoring stop")
    
    # Run crashing test application
    @sut.execute_shell_command(TEST_APP)
    sleep 5
    
    # Start monitoring...
    @sut.execute_shell_command("/usr/share/crash-reporter/crash-report-monitoring start")
end

Given ("there are no crash reports in the system") do

    # Remove and create core-dumps directory.
    @sut.execute_shell_command("rm -rf " + CORE_DUMPS_DIR)
    @sut.execute_shell_command("mkdir " + CORE_DUMPS_DIR)
        
    # Stop monitoring...
    @sut.execute_shell_command("/usr/share/crash-reporter/crash-report-monitoring stop")
    
    # Start monitoring...
    @sut.execute_shell_command("/usr/share/crash-reporter/crash-report-monitoring start")
end

When ("Crash Reporter Daemon starts") do

    # Stop/ start daemon to get notification displayed.
    @sut.execute_shell_command("pkill /usr/bin/crash-reporter-daemon")
end

Then ("a notification with text \"$notification_text\" is displayed") do |notification_text|

    # Give some slack for notification to be displayed
    sleep 5
    
    verify { @duihome.MStatusBar(:objectName => "statusBar").flick(:Down) }
    verify(5, "Notification was not displayed.") do 
        @sysuid.MInfoBanner(:bodyText => notification_text).tap
    end    
end

When ("application \"$appplication_name\" starts") do |application_name|
    # Connect to running crash-reporter-ui.
    verify(10, "Crash Reporter UI did not start.") do
        @crash_reporter_ui = @sut.application(:name => application_name, :__timeout=>0)
    end
end

Then ("dialog with title \"$title\" is displayed") do |title|
    
    # Verify, that dialog is displayed.
    verify(5, "Crash Reporter notification dialog was not displayed.") do
        @crash_reporter_ui.MLabel(:objectName => "MDialogTitleLabel", :text => title)
    end
end

When ("I press \"$caption\" button") do |caption|
    
    if caption == "Close"
        @crash_reporter_ui.MButton(:objectName => "MDialogCloseButton").tap
    else    
        @crash_reporter_ui.MButton(:text => caption).tap
    end
end

Then ("progress bar is displayed to represent upload status") do
    
    # Progress bar is displayed.
    verify_true() {@crash_reporter_ui.test_object_exists?("MProgressIndicator", 
                                                          {:objectName => "UploadProgressBar"})}    
end

When ("upload finishes") do
    verify_false(15) {@crash_reporter_ui.test_object_exists?("MProgressIndicator", 
                                                           {:objectName => "UploadProgressBar"})}    
end

Then ("A message box with title \"$title\" is displayed") do |title|
    
    verify_true(30, "Message box wasn't displayed.") {
        @crash_reporter_ui.test_object_exists?("MMessageBox", {:title => title})
    }

end

When ("I tap OK button from the dialog") do

    @crash_reporter_ui.MButton(:objectName => "MDialogButtonOk").tap
end

Then ("Crash Reporter UI is closed") do
    
    # Verify crash-reporter-ui is closed.
    verify_not(3, "Application crash-reporter-ui was not closed.") {
        @sut.application(:name => "crash-reporter-ui")
    }
    
end

Then ("crash reports are deleted") do

    verify_equal(nil, 5, "Crash reports were not deleted!") { 
        @sut.execute_shell_command("ls -A " + CORE_DUMPS_DIR).index "crashapplication"
    }
end

When ("I execute application which crashes") do
     
    @sut.execute_shell_command(TEST_APP)
    sleep 5
end
