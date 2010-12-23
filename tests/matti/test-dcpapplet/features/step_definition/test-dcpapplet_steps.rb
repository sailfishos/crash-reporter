#############################################################
# test-monitorandprivacysettings_steps.rb
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

def fetch_object(object_types, params = {})
    # Convert parameters to string
    params_str = ""
    if not params.empty?
        params_str += "{"
        params.each {|key, value| params_str += ":#{key} => \"#{value}\", "}
        params_str = params_str[0..-3] + "}"
    end

    # Go through all given object types with parameters and try to find test object.
    for object_type in object_types
        begin
            obj = eval "@dcp_app." + object_type + "(" + params_str + ")"
            return obj
        rescue MobyBase::TestObjectNotFoundError
            next
        end
    end
    return nil
end


def drag_to_object(object_type, param, direction=-1)
    for i in 0..10
        obj = fetch_object([object_type], param)
        visible = obj.attribute("visibleOnScreen").to_s
        
        if visible == "true"
            break
        end

        if direction == -1
            if visible == "false" then
                @dcp_app.MWidget(:name => "glass").drag(:Up, 100)
                sleep 1
            end
        else
            if visible == "false" then
                @dcp_app.MWidget(:name => "glass").drag(direction, 100)
                sleep 1
            end
        end
    end
end

def launch_applet(applet_name)
    
    @dcp_app.MLabel(:name => "ContentItemTitle", :text => applet_name).tap
    verify(5, "Applet failed to open!") do
        @dcp_app.MApplicationMenuButton(:text => applet_name)
    end
    # Wait for applet page to be appeared 
    sleep 3
end

def go_back(times)
    
    for i in 1..times
        @dcp_app.MButton(:name => "BackButton").tap
        sleep 2
    end
end

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
        crash_reporter_ui = @sut.application(:name => "crash-reporter-ui")
        crash_reporter_ui.close
    rescue
        # Do nothing
    end
end

After do
    # Close duicontrolpanel
    @dcp_app.close
   
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
    
    @sut.execute_shell_command("/usr/share/crash-reporter/crash-report-monitoring start")
end

Given ("The \"$applet_name\" applet is active") do |applet_name|
    # Launch duicontrolpanel application.
    @dcp_app = @sut.run(:name => "duicontrolpanel", :arguments => "-nosupervisor")
    
    # Find and open !! Applications category.
    drag_to_object("MLabel", {:text => "!! Applications", :__index => 0})
    @dcp_app.MLabel(:name => "ContentItemTitle", :text => "!! Applications").tap
    
    verify(5, "Applet category failed to open!") do
        @dcp_app.MApplicationMenuButton(:text => "!! Applications")
    end

    launch_applet(applet_name)    
end

Given ("There are unsent crash reports in the system") do 

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

When ("I tap button \"$button_name\" next to \"$label_name\" option") do |button_name, label_name|
    drag_to_object("MLabel", {:objectName => label_name, :__index => 0})
    @dcp_app.MButton(:objectName => button_name).tap
end

Then ("Button's \"$button_name\" next to \"$label_name\" option, enabled status is \"$enabled\" and checked status is \"$checked\"") do |button_name, label_name, enabled, checked|

   drag_to_object("MLabel", {:objectName => label_name, :__index => 0}) 
   verify_true() {@dcp_app.test_object_exists?("MButton", {:objectName => button_name, 
                                                       :enabled => enabled, :checked => checked})}
end

When ("I go back to category view") do
    # Go back to settings main view.
    go_back(2)
    
    # Find and open !! Applications category.
    drag_to_object("MLabel", {:text => "!! Applications", :__index => 0})
    @dcp_app.MLabel(:name => "ContentItemTitle", :text => "!! Applications").tap
    
    verify(5, "Applet category failed to open!") do
        @dcp_app.MApplicationMenuButton(:text => "!! Applications")
    end
end

When ("I go back to \"$applet_name\" applet view") do |applet_name|
    
    launch_applet(applet_name)
end

Then ("\"$brief_text\" is displayed in brief widget") do |brief_text|
    drag_to_object("MLabel", {:text => brief_text, :__index => 0})
end

When ("I tap \"$button_name\" button") do |button_name|
    
    drag_to_object("MButton", {:objectName => button_name, :__index => 0})
    @dcp_app.MButton(:objectName => button_name).tap
end

Then ("A dialog with title \"$dialog_title\" is displayed") do |dialog_title|

    verify(5, "Dialog didn't pop up!") do
        @dcp_app.MLabel(:objectName => "MDialogTitleLabel", :text => dialog_title)
    end 
end

When ("I reject the dialog") do

    @dcp_app.MButton(:objectName => "MDialogCloseButton").tap
end

Then ("The dialog is closed") do

    verify_false() {@dcp_app.test_object_exists?("MLabel", {:objectName => "MDialogTitleLabel"})}
end

Then ("A message box with title \"$title\" is displayed") do |title|

    begin
        @cr_app = @sut.application(:name => "crash-reporter-ui")
    rescue MobyBase::TestObjectNotFoundError
        sleep 5
        @cr_app = @sut.application(:name => "crash-reporter-ui")
    end
    
    verify_true(30, "Message box wasn't displayed.") {
        @cr_app.test_object_exists?("MMessageBox", {:title => title})
    }

end

When ("I tap OK button from the dialog") do

    @cr_app.MButton(:objectName => "MDialogButtonOk").tap
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