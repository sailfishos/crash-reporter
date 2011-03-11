/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Riku Halonen <riku.halonen@nokia.com>
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
 *
 */

// System includes.

#include <MLayout>
#include <MLabel>
#include <MLinearLayoutPolicy>
#include <MGridLayoutPolicy>
#include <MButton>
#include <MContainer>
#include <MButtonGroup>
#include <MMessageBox>
#include <QDebug>

// User includes.

#include "creportermonitorsettingscontainer.h"
#include "creporterprivacysettingsmodel.h"

/*!
  * @class CReporterMonitorSettingsContainerPrivate
  * @brief Private CReporterMonitorSettings class
  *
  */
class CReporterMonitorSettingsContainerPrivate
{
    public:
        //! @arg Button group to group related buttons.
        MButtonGroup *m_buttons;
};
// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterMonitorSettingsContainer::CReporterMonitorSettingsContainer
// ----------------------------------------------------------------------------
CReporterMonitorSettingsContainer::CReporterMonitorSettingsContainer(QGraphicsItem *parent) :
        MStylableWidget(parent),
        d_ptr(new CReporterMonitorSettingsContainerPrivate())
{
    setObjectName("SettingsContainer");
    initWidget();
}

// ----------------------------------------------------------------------------
// CReporterMonitorSettingsContainer::~CReporterMonitorSettingsContainer
// ----------------------------------------------------------------------------
CReporterMonitorSettingsContainer::~CReporterMonitorSettingsContainer()
{
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterMonitorSettingsContainer::updateButtons
// ----------------------------------------------------------------------------
void CReporterMonitorSettingsContainer::updateButtons()
{
    Q_D(CReporterMonitorSettingsContainer);
    // Set att buttons statuses based on current settings.
    if (d->m_buttons->button(SaveCrashReportsBtn)->isChecked()
        != CReporterPrivacySettingsModel::instance()->coreDumpingEnabled())
    {
        d->m_buttons->button(SaveCrashReportsBtn)->
                setChecked(CReporterPrivacySettingsModel::instance()->coreDumpingEnabled());
    }
    if (d->m_buttons->button(NotifyCrashReportsBtn)->isChecked()
        != CReporterPrivacySettingsModel::instance()->sendingEnabled())
    {
        d->m_buttons->button(NotifyCrashReportsBtn)->
                setChecked(CReporterPrivacySettingsModel::instance()->sendingEnabled());
    }
    if (d->m_buttons->button(AutomaticSendingBtn)->isChecked()
        != CReporterPrivacySettingsModel::instance()->automaticSendingEnabled())
    {
        d->m_buttons->button(AutomaticSendingBtn)->
                setChecked(CReporterPrivacySettingsModel::instance()->automaticSendingEnabled());
    }
    if (d->m_buttons->button(AutoDeleteCrashReportsBtn)->isChecked()
        != CReporterPrivacySettingsModel::instance()->autoDeleteDuplicates())
    {
        d->m_buttons->button(AutoDeleteCrashReportsBtn)->
                setChecked(CReporterPrivacySettingsModel::instance()->autoDeleteDuplicates());
    }
}

// ----------------------------------------------------------------------------
// CReporterMonitorSettingsContainer::initWidget
// ----------------------------------------------------------------------------
void CReporterMonitorSettingsContainer::initWidget()
{
    Q_D(CReporterMonitorSettingsContainer);

    bool dumpingEnabled = CReporterPrivacySettingsModel::instance()->coreDumpingEnabled();

    MLayout* mainLayout = new MLayout(this);
    MLinearLayoutPolicy* mainPolicy = new MLinearLayoutPolicy(mainLayout, Qt::Vertical);
    mainPolicy->setVerticalSpacing(0);
    mainLayout->setPolicy(mainPolicy);
    mainLayout->setContentsMargins(0,20,0,20);

    //% "Monitoring settings"
    MLabel* header = new MLabel(qtTrId("qtn_dcp_monitoring_container_title"));
    header->setStyleName("CommonHeaderInverted");
    header->setWordWrap(true);
    mainPolicy->addItem(header);

    MStylableWidget* separator = new MStylableWidget(this);
    separator->setStyleName("CommonHeaderDividerInverted");
    mainPolicy->addItem(separator);

    // Create layout and policy.
    MLayout *layout = new MLayout(mainLayout);
    MGridLayoutPolicy *policy = new MGridLayoutPolicy(layout);
    policy->setVerticalSpacing(20.0);
    policy->setObjectName("Containerpolicy");
    layout->setPolicy(policy);
    layout->setContentsMargins(0,0,0,0);

    // Group for buttons.
    d->m_buttons = new MButtonGroup(this);
    d->m_buttons->setExclusive(false);

    // Label for save crash reports text.
    //% "Save crash reports"
    MLabel *label = new MLabel(qtTrId("qtn_dcp_save_crash_reports_text"), this);
    label->setObjectName("SaveCrashReportsLabel");
    label->setStyleName("CommonSingleTitleInverted");
    label->setWordWrap(true);

    // Toggle button for save crash reports option.
    MButton *button = new MButton(this);
    button->setObjectName("SaveCrashReportsSwitch");
    button->setViewType(MButton::switchType);
    button->setCheckable(true);
    button->setChecked(dumpingEnabled);
    button->setStyleName("CommonSwitchInverted");
    d->m_buttons->addButton(button, SaveCrashReportsBtn);

    policy->addItem(label, 0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    policy->addItem(button, 0, 1, Qt::AlignRight | Qt::AlignVCenter);

    // Label for monitor and send crash reports text.
    //% "Show notifications"
    label = new MLabel(qtTrId("qtn_dcp_show_notifications_text"), this);
    label->setObjectName("ShowNotificationsLabel");
    label->setStyleName("CommonSingleTitleInverted");
    label->setWordWrap(true);

    // Toggle button for monitor and send crash reports option.
    button = new MButton(this);
    button->setObjectName("ShowNotificationsSwitch");
    button->setViewType(MButton::switchType);
    button->setCheckable(true);
    button->setStyleName("CommonSwitchInverted");

    d->m_buttons->addButton(button, NotifyCrashReportsBtn);

    policy->addItem(label, 1, 0, Qt::AlignLeft | Qt::AlignVCenter);
    policy->addItem(button, 1, 1, Qt::AlignRight | Qt::AlignVCenter );

    // Label for automatic sending text.
    //% "Send crash reports automatically"
    label = new MLabel(qtTrId("qtn_dcp_send_automatically_text"), this);
    label->setObjectName("SendAutomaticallyLabel");
    label->setStyleName("CommonSingleTitleInverted");
    label->setWordWrap(true);

    // Toggle button for monitor and send crash reports option.
    button = new MButton(this);
    button->setObjectName("SendAutomaticallySwitch");
    button->setViewType(MButton::switchType);
    button->setCheckable(true);
    button->setStyleName("CommonSwitchInverted");

    d->m_buttons->addButton(button, AutomaticSendingBtn);

    policy->addItem(label, 2, 0, Qt::AlignLeft | Qt::AlignVCenter);
    policy->addItem(button, 2, 1, Qt::AlignRight | Qt::AlignVCenter );

    // Label for auto-delete option.
    //% "Auto-delete repeating duplicates"
    label = new MLabel(qtTrId("qtn_dcp_auto_delete_dups_text"), this);
    label->setObjectName("AutoDeleteDupsLabel");
    label->setStyleName("CommonSingleTitleInverted");
    label->setWordWrap(true);

    // Toggle button for auto-delete option.
    button = new MButton(this);
    button->setObjectName("AutoDeleteDupsSwitch");
    button->setViewType(MButton::switchType);
    button->setCheckable(true);
    button->setStyleName("CommonSwitchInverted");

    if (!dumpingEnabled) {
        // If dumping is not enabled, we cannot toggle auto-delete option.
        button->setEnabled(false);
    }
    d->m_buttons->addButton(button, AutoDeleteCrashReportsBtn);

    policy->addItem(label, 3, 0, Qt::AlignLeft | Qt::AlignVCenter);
    policy->addItem(button, 3, 1, Qt::AlignRight | Qt::AlignVCenter );

    mainPolicy->addItem(layout);

    updateButtons();

    QList<MButton*> btns = d->m_buttons->buttons();
    for(int i = 0; i < btns.size(); i++) {
        connect(btns.at(i), SIGNAL(toggled(bool)),
                this, SLOT(groupButtonToggled(bool)), Qt::DirectConnection);

    }
}

// ----------------------------------------------------------------------------
// CReporterMonitorSettingsContainer::groupButtonClicked
// ----------------------------------------------------------------------------
void CReporterMonitorSettingsContainer::groupButtonToggled(bool checked)
{
    Q_D(CReporterMonitorSettingsContainer);

    // get id of signal sender MButton object
    int id = d->m_buttons->id(qobject_cast<MButton*>(sender()));

    // Disconnect signals to avoid calls to this slot, when changing button state with
    // setChecked function.
    QList<MButton*> btns = d->m_buttons->buttons();
    for(int i = 0; i < btns.size(); i++) {
        disconnect(btns.at(i), SIGNAL(toggled(bool)),
                   this, SLOT(groupButtonToggled(bool)));

    }

    switch (id) {
    // Enable/ disable rich-core dumping. If enabled, possibility toggle sending and auto-delete
    // features on/ off.
    case SaveCrashReportsBtn:
        // Disable auto-delete button, if core dumping is not enabled.
        d->m_buttons->button(AutoDeleteCrashReportsBtn)->setEnabled(checked);
        if (!checked) {
            // If core dumping was disabled, disable notifications and automatic sending.
            CReporterPrivacySettingsModel::instance()->setSendingEnabled(false);
            CReporterPrivacySettingsModel::instance()->setAutomaticSendingEnabled(false);
        }
        // Set new core dumping settings.
        CReporterPrivacySettingsModel::instance()->setCoreDumpingEnabled(checked);
        break;
    // Enable/ disable UI notifications for the new crash reports.
    case NotifyCrashReportsBtn:
        // If notifications were enabled...
        if (checked) {
            // Enable other buttons based on settings.
            d->m_buttons->button(AutoDeleteCrashReportsBtn)->setEnabled(true);
            CReporterPrivacySettingsModel::instance()->setCoreDumpingEnabled(true);
            CReporterPrivacySettingsModel::instance()->setAutomaticSendingEnabled(false);
        }
        // Set new setting.
        CReporterPrivacySettingsModel::instance()->setSendingEnabled(checked);
        break;
    // Enable/disable automatic sending of new crash reports
    case AutomaticSendingBtn:
        if (checked)
        {
            // Enable other buttons based on settings.
            d->m_buttons->button(AutoDeleteCrashReportsBtn)->setEnabled(true);
            CReporterPrivacySettingsModel::instance()->setCoreDumpingEnabled(true);
            CReporterPrivacySettingsModel::instance()->setSendingEnabled(false);
            showAutoUploaderMessage();
        }
        CReporterPrivacySettingsModel::instance()->setAutomaticSendingEnabled(checked);
        break;
    // Toggle auto-deletion on/off.
    case AutoDeleteCrashReportsBtn:
        CReporterPrivacySettingsModel::instance()->setAutoDeleteDuplicates(checked);
        break;
    case NoBtn:
        break;
    default:
        // Unknown button.
        break;

    };
    updateButtons();

    // Reconnect signals/ slots.
    for(int i = 0; i < btns.size(); i++) {
        connect(btns.at(i), SIGNAL(toggled(bool)),
                this, SLOT(groupButtonToggled(bool)), Qt::DirectConnection);

    }
}

// ----------------------------------------------------------------------------
// CReporterMonitorSettingsContainer::showAutoUploaderMessage
// ----------------------------------------------------------------------------
void CReporterMonitorSettingsContainer::showAutoUploaderMessage()
{
    QString msgText;
    msgText += "<p>When automatic uploading is enabled Crash Reporter will try to upload all crash reports to the ";
    msgText += "core server automatically. This requires an automatic Internet access point ";
    msgText += "or an open internet connection. If none is available automatic uploading will ";
    msgText += "simply fail without notifying the user. However, every time automatic uploading is triggered ";
    msgText += "Crash Reporter will try to upload all crash reports that are found. If you have any questions ";
    msgText += "regarding this feature you may contact <b>twux-dev@projects.maemo.org.</b></p>";

    MMessageBox *msgBox = new MMessageBox(msgText);
    msgBox->appear(0, MSceneWindow::DestroyWhenDone);
}

// End of file.
