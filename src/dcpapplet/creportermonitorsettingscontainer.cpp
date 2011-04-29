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
        != CReporterPrivacySettingsModel::instance()->notificationsEnabled())
    {
        d->m_buttons->button(NotifyCrashReportsBtn)->
                setChecked(CReporterPrivacySettingsModel::instance()->notificationsEnabled());
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
    if (d->m_buttons->button(LifelogBtn)->isChecked()
        != CReporterPrivacySettingsModel::instance()->lifelogEnabled())
    {
        d->m_buttons->button(LifelogBtn)->
                setChecked(CReporterPrivacySettingsModel::instance()->lifelogEnabled());
    }
    if (d->m_buttons->button(InstantDialogsBtn)->isChecked()
        != CReporterPrivacySettingsModel::instance()->instantDialogsEnabled())
    {
        d->m_buttons->button(InstantDialogsBtn)->
                setChecked(CReporterPrivacySettingsModel::instance()->instantDialogsEnabled());
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

    //% "Monitoring"
    MLabel* header = new MLabel(qtTrId("qtn_dcp_monitoring_container_title"));
    header->setStyleName("CommonHeaderInverted");
    header->setWordWrap(true);
    mainPolicy->addItem(header);

    // Create layout and policy.
    MLayout *layout = new MLayout(mainLayout);
    MGridLayoutPolicy *policy = new MGridLayoutPolicy(layout);
    policy->setVerticalSpacing(4.0);
    policy->setObjectName("Containerpolicy");
    layout->setPolicy(policy);
    layout->setContentsMargins(0,0,0,0);
    int layoutLineNumber = 0;

    // Group for buttons.
    d->m_buttons = new MButtonGroup(this);
    d->m_buttons->setExclusive(false);

    // -------------------------------------------------------------------------------- //
    // Label for save crash reports text.
    //% "Collect crash reports"
    MLabel *label = new MLabel(qtTrId("qtn_dcp_save_crash_reports_text"), this);
    label->setObjectName("SaveCrashReportsLabel");
    label->setStyleName("CommonTitleInverted");
    label->setWordWrap(true);

    //% "Generate reports of all crashed applications and services"
    MLabel *desc = new MLabel(qtTrId("qtn_dcp_collect_crash_reports_desc"), this);
    desc->setObjectName("SaveCrashReportsDesc");
    desc->setStyleName("CommonSubTitleInverted");
    desc->setWordWrap(true);

    // Toggle button for save crash reports option.
    MButton *button = new MButton(this);
    button->setObjectName("SaveCrashReportsSwitch");
    button->setViewType(MButton::switchType);
    button->setCheckable(true);
    button->setChecked(dumpingEnabled);
    button->setStyleName("CommonSwitchInverted");
    d->m_buttons->addButton(button, SaveCrashReportsBtn);

    policy->addItem(label, layoutLineNumber, 0, Qt::AlignLeft | Qt::AlignTop);
    policy->addItem(button, layoutLineNumber++, 1, 2, 1, Qt::AlignRight | Qt::AlignVCenter);
    policy->addItem(desc, layoutLineNumber++, 0, Qt::AlignLeft | Qt::AlignTop);

    // -------------------------------------------------------------------------------- //
    // Label for lifelog option.
    //% "Collect lifelog"
    label = new MLabel(qtTrId("qtn_dcp_lifelog_text"), this);
    label->setObjectName("LifelogLabel");
    label->setStyleName("CommonTitleInverted");
    label->setWordWrap(true);

    //% "Save hourly statistics on crashes, memory and battery usage etc."
    desc = new MLabel(qtTrId("qtn_dcp_lifelog_desc"), this);
    desc->setObjectName("LifelogDesc");
    desc->setStyleName("CommonSubTitleInverted");
    desc->setWordWrap(true);

    // Toggle button for lifelog option.
    button = new MButton(this);
    button->setObjectName("LifelogSwitch");
    button->setViewType(MButton::switchType);
    button->setCheckable(true);
    button->setStyleName("CommonSwitchInverted");

    d->m_buttons->addButton(button, LifelogBtn);

    policy->addItem(label, layoutLineNumber, 0, Qt::AlignLeft | Qt::AlignTop);
    policy->addItem(button, layoutLineNumber++, 1, 2, 1, Qt::AlignRight | Qt::AlignVCenter);
    policy->addItem(desc, layoutLineNumber++, 0, Qt::AlignLeft | Qt::AlignTop);

    // -------------------------------------------------------------------------------- //
    // Label for monitor and send crash reports text.
    //% "Show notifications"
    label = new MLabel(qtTrId("qtn_dcp_show_notifications_text"), this);
    label->setObjectName("ShowNotificationsLabel");
    label->setStyleName("CommonTitleInverted");
    label->setWordWrap(true);

    //% "Notify of crashes and Crash Reporter actions"
    desc = new MLabel(qtTrId("qtn_dcp_notifications_desc"), this);
    desc->setObjectName("NotificationsDesc");
    desc->setStyleName("CommonSubTitleInverted");
    desc->setWordWrap(true);

    // Toggle button for monitor and send crash reports option.
    button = new MButton(this);
    button->setObjectName("ShowNotificationsSwitch");
    button->setViewType(MButton::switchType);
    button->setCheckable(true);
    button->setStyleName("CommonSwitchInverted");

    d->m_buttons->addButton(button, NotifyCrashReportsBtn);

    policy->addItem(label, layoutLineNumber, 0, Qt::AlignLeft | Qt::AlignTop);
    policy->addItem(button, layoutLineNumber++, 1, 2, 1, Qt::AlignRight | Qt::AlignVCenter);
    policy->addItem(desc, layoutLineNumber++, 0, Qt::AlignLeft | Qt::AlignTop);

    // -------------------------------------------------------------------------------- //
    // Label for automatic sending text.
    //% "Send reports automatically"
    label = new MLabel(qtTrId("qtn_dcp_send_automatically_text"), this);
    label->setObjectName("SendAutomaticallyLabel");
    label->setStyleName("CommonTitleInverted");
    label->setWordWrap(true);

    //% "Upload all reports in the background using a default internet access point"
    desc = new MLabel(qtTrId("qtn_dcp_sent_automatically_desc"), this);
    desc->setObjectName("SendAutomaticallyDesc");
    desc->setStyleName("CommonSubTitleInverted");
    desc->setWordWrap(true);

    // Toggle button for monitor and send crash reports option.
    button = new MButton(this);
    button->setObjectName("SendAutomaticallySwitch");
    button->setViewType(MButton::switchType);
    button->setCheckable(true);
    button->setStyleName("CommonSwitchInverted");

    d->m_buttons->addButton(button, AutomaticSendingBtn);

    policy->addItem(label, layoutLineNumber, 0, Qt::AlignLeft | Qt::AlignTop);
    policy->addItem(button, layoutLineNumber++, 1, 2, 1, Qt::AlignRight | Qt::AlignVCenter);
    policy->addItem(desc, layoutLineNumber++, 0, Qt::AlignLeft | Qt::AlignTop);

    // -------------------------------------------------------------------------------- //
    // Label for auto-delete option.
    //% "Auto-delete repeating duplicates"
    label = new MLabel(qtTrId("qtn_dcp_auto_delete_dups_text"), this);
    label->setObjectName("AutoDeleteDupsLabel");
    label->setStyleName("CommonTitleInverted");
    label->setWordWrap(true);

    //% "Delete repeating crash reports that are similar to earlier reports"
    desc = new MLabel(qtTrId("qtn_dcp_auto_delete_dups_desc"), this);
    desc->setObjectName("AutoDeleteDupsDesc");
    desc->setStyleName("CommonSubTitleInverted");
    desc->setWordWrap(true);

    // Toggle button for auto-delete option.
    button = new MButton(this);
    button->setObjectName("AutoDeleteDupsSwitch");
    button->setViewType(MButton::switchType);
    button->setCheckable(true);
    button->setStyleName("CommonSwitchInverted");

    d->m_buttons->addButton(button, AutoDeleteCrashReportsBtn);

    policy->addItem(label, layoutLineNumber, 0, Qt::AlignLeft | Qt::AlignTop);
    policy->addItem(button, layoutLineNumber++, 1, 2, 1, Qt::AlignRight | Qt::AlignVCenter);
    policy->addItem(desc, layoutLineNumber++, 0, Qt::AlignLeft | Qt::AlignTop);

    // -------------------------------------------------------------------------------- //
    // Label for instant dialogs option.
    //% "Open crash dialogs instantly"
    label = new MLabel(qtTrId("qtn_dcp_instant_dialogs_text"), this);
    label->setObjectName("InstantDialogsLabel");
    label->setStyleName("CommonTitleInverted");
    label->setWordWrap(true);

    //% "Always open crash dialogs in foreground instead of using notifications"
    desc = new MLabel(qtTrId("qtn_dcp_instant_dialogs_desc"), this);
    desc->setObjectName("InstantDialogsDesc");
    desc->setStyleName("CommonSubTitleInverted");
    desc->setWordWrap(true);

    // Toggle button for instant dialogs option.
    button = new MButton(this);
    button->setObjectName("InstantDialogsSwitch");
    button->setViewType(MButton::switchType);
    button->setCheckable(true);
    button->setStyleName("CommonSwitchInverted");

    d->m_buttons->addButton(button, InstantDialogsBtn);

    policy->addItem(label, layoutLineNumber, 0, Qt::AlignLeft | Qt::AlignTop);
    policy->addItem(button, layoutLineNumber++, 1, 2, 1, Qt::AlignRight | Qt::AlignVCenter);
    policy->addItem(desc, layoutLineNumber++, 0, Qt::AlignLeft | Qt::AlignTop);

    // -------------------------------------------------------------------------------- //

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
        // Set new core dumping settings.
        CReporterPrivacySettingsModel::instance()->setCoreDumpingEnabled(checked);
        break;
    // Enable/ disable UI notifications for the new crash reports.
    case NotifyCrashReportsBtn:
        // Set new setting.
        CReporterPrivacySettingsModel::instance()->setNotificationsEnabled(checked);
        break;
    // Enable/disable automatic sending of new crash reports
    case AutomaticSendingBtn:
        CReporterPrivacySettingsModel::instance()->setAutomaticSendingEnabled(checked);
        break;
    // Toggle auto-deletion on/off.
    case AutoDeleteCrashReportsBtn:
        CReporterPrivacySettingsModel::instance()->setAutoDeleteDuplicates(checked);
        break;
    // Toggle lifelogging
    case LifelogBtn:
        CReporterPrivacySettingsModel::instance()->setLifelogEnabled(checked);
        break;
    // Toggle instant dialogs
    case InstantDialogsBtn:
        CReporterPrivacySettingsModel::instance()->setInstantDialogsEnabled(checked);
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
