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

// User includes.

#include "creporterincludesettingscontainer.h"
#include "creporterprivacysettingsmodel.h"

/*!
  * @class CReporterIncludeSettingsContainerPrivate
  * @brief Private CReporterIncludeSettingsContainer class.
  *
  * @sa CReporterIncludeSettingsContainer
  */
class CReporterIncludeSettingsContainerPrivate
{
    public:
        //! @arg To group buttons modifying settings related to each other.
        MButtonGroup *m_buttons;
};

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterIncludeSettingsContainer::CReporterIncludeSettingsContainer
// ----------------------------------------------------------------------------
CReporterIncludeSettingsContainer::CReporterIncludeSettingsContainer(QGraphicsItem *parent) :
        MStylableWidget(parent),
         d_ptr(new CReporterIncludeSettingsContainerPrivate())
{
    setObjectName("SettingsContainer");
    initWidget();
}

// ----------------------------------------------------------------------------
// CReporterIncludeSettingsContainer::~CReporterIncludeSettingsContainer
// ----------------------------------------------------------------------------
CReporterIncludeSettingsContainer::~CReporterIncludeSettingsContainer()
{
    delete d_ptr;
    d_ptr = 0;
}

// ----------------------------------------------------------------------------
// CReporterIncludeSettingsContainer::initWidget
// ----------------------------------------------------------------------------
void CReporterIncludeSettingsContainer::initWidget()
{
    Q_D(CReporterIncludeSettingsContainer);

    bool dumpingEnabled = CReporterPrivacySettingsModel::instance()->coreDumpingEnabled();

    MLayout* mainLayout = new MLayout(this);
    MLinearLayoutPolicy* mainPolicy = new MLinearLayoutPolicy(mainLayout, Qt::Vertical);
    mainPolicy->setVerticalSpacing(0);
    mainLayout->setPolicy(mainPolicy);
    mainLayout->setContentsMargins(0,20,0,20);

    //% "Include into future crash reports"
    MLabel* header = new MLabel(qtTrId("qtn_dcp_include_container_title_text"));
    header->setStyleName("CommonHeaderInverted");
    header->setWordWrap(true);
    mainPolicy->addItem(header);

    MStylableWidget* separator = new MStylableWidget(this);
    separator->setStyleName("CommonHeaderDividerInverted");
    mainPolicy->addItem(separator);

    // Create main layout and policy.
    MLayout *layout = new MLayout(mainLayout);
    MGridLayoutPolicy *policy = new MGridLayoutPolicy(layout);
    policy->setVerticalSpacing(20.0);
    policy->setObjectName("ContainerLayout");
    layout->setPolicy(policy);
    layout->setContentsMargins(0,0,0,0);

    // Group for buttons.
    d->m_buttons = new MButtonGroup(this);
    d->m_buttons->setExclusive(false);

    // Label for including Core dump text.
    //% "Core dump"
    MLabel *label = new MLabel(qtTrId("qtn_dcp_include_core_dump_text"), this);
    label->setObjectName("IncludeCoreDumpLabel");
    label->setStyleName("CommonSingleTitleInverted");
    label->setWordWrap(true);

    // Toggle button for include core option.
    MButton *button = new MButton(this);
    button->setObjectName("IncludeCoreDumpSwitch");
    button->setCheckable(true);
    button->setViewType(MButton::switchType);
    button->setStyleName("CommonSwitchInverted");
    d->m_buttons->addButton(button, IncludeCoreBtn);

    button->setChecked(CReporterPrivacySettingsModel::instance()->includeCore());

    policy->addItem(label, 0, 0, Qt::AlignLeft | Qt::AlignVCenter);
    policy->addItem(button, 0, 1, Qt::AlignRight | Qt::AlignVCenter );

    // Label for including syslog text.
    //% "System log (if exists)"
    label = new MLabel(qtTrId("qtn_dcp_include_syslog_text"), this);
    label->setObjectName("IncludeSyslogLabel");
    label->setStyleName("CommonSingleTitleInverted");
    label->setWordWrap(true);

    // Toggle button for include system log option.
    button = new MButton(this);
    button->setObjectName("IncludeSyslogSwitch");
    button->setCheckable(true );
    button->setViewType(MButton::switchType);
    button->setStyleName("CommonSwitchInverted");
    d->m_buttons->addButton(button, IncludeSyslogBtn);
    button->setChecked(CReporterPrivacySettingsModel::instance()->includeSystemLog());

    policy->addItem(label, 1, 0, Qt::AlignLeft | Qt::AlignVCenter);
    policy->addItem(button, 1, 1, Qt::AlignRight | Qt::AlignVCenter );

    // Label for including installed packages text.
    //% "List of installed packages"
    label = new MLabel(qtTrId("qtn_dcp_include_installed_packages_text"), this);
    label->setObjectName("IncludePackagesLabel");
    label->setStyleName("CommonSingleTitleInverted");
    label->setWordWrap(true);

    // Toggle button for include package list option.
    button = new MButton(this);
    button->setObjectName("IncludePackagesSwitch");
    button->setCheckable(true);
    button->setViewType(MButton::switchType);
    button->setStyleName("CommonSwitchInverted");
    d->m_buttons->addButton(button, IncludePackagesBtn);
    button->setChecked(CReporterPrivacySettingsModel::instance()->includePackageList());

    policy->addItem(label, 2, 0, Qt::AlignLeft | Qt::AlignVCenter);
    policy->addItem(button, 2, 1, Qt::AlignRight | Qt::AlignVCenter );

    // Label for reducing core size text.
    //% "Reduce core dump size"
    label = new MLabel(qtTrId("qtn_dcp_reduce_core_dump_size"), this);
    label->setObjectName("ReduceCoreDumpLabel");
    label->setStyleName("CommonSingleTitleInverted");
    label->setWordWrap(true);

    // Toggle button for reduce core option.
    button = new MButton(this);
    button->setObjectName("ReduceCoreDumpSwitch");
    button->setCheckable(true);
    button->setViewType(MButton::switchType);
    button->setStyleName("CommonSwitchInverted");
    d->m_buttons->addButton(button, ReduceCoreSizeBtn);

    button->setChecked(CReporterPrivacySettingsModel::instance()->reduceCore());

    policy->addItem(label, 3, 0, Qt::AlignLeft | Qt::AlignVCenter);
    policy->addItem(button, 3, 1, Qt::AlignRight | Qt::AlignVCenter );

    mainPolicy->addItem(layout);

    // Connect to receive notifications, when settings changes.
    connect(CReporterPrivacySettingsModel::instance(), SIGNAL(valueChanged(QString,QVariant)),
            this, SLOT(settingsChanged(QString,QVariant)));
    QList<MButton*> btns = d->m_buttons->buttons();
    for(int i = 0; i < btns.size(); i++) {
        connect(btns.at(i), SIGNAL(toggled(bool)),
                this, SLOT(groupButtonToggled(bool)), Qt::DirectConnection);

    }

    setEnabled(dumpingEnabled);
}

// ----------------------------------------------------------------------------
// CReporterIncludeSettingsContainer::groupButtonClicked
// ----------------------------------------------------------------------------
void CReporterIncludeSettingsContainer::groupButtonToggled(bool checked)
{
    Q_D(CReporterIncludeSettingsContainer);

    // get id of signal sender MButton object
    int id = d->m_buttons->id(qobject_cast<MButton*>(sender()));

    switch (id) {
        case IncludeCoreBtn:
            CReporterPrivacySettingsModel::instance()->setIncludeCore(checked);
            break;
        case IncludeSyslogBtn:
            CReporterPrivacySettingsModel::instance()->setIncludeSystemLog(checked);
            break;
        case IncludePackagesBtn:
            CReporterPrivacySettingsModel::instance()->setIncludePackageList(checked);
            break;
        case ReduceCoreSizeBtn:
            CReporterPrivacySettingsModel::instance()->setReduceCore(checked);
            break;
        default:
            // Unknown button.
            break;
    };
}

// ----------------------------------------------------------------------------
// CReporterIncludeSettingsContainer::settingsChanged
// ----------------------------------------------------------------------------
void CReporterIncludeSettingsContainer::settingsChanged(const QString& key,
                                                        const QVariant& value )
{
    Q_D(CReporterIncludeSettingsContainer);
    QList<MButton*> btns = d->m_buttons->buttons();
    for(int i = 0; i < btns.size(); i++) {
        disconnect(btns.at(i), SIGNAL(toggled(bool)),
                   this, SLOT(groupButtonToggled(bool)));

    }

    if (key == Settings::ValueCoreDumping) {
        // If core dumping setting was changed.
        MWidget *panel = qobject_cast<MWidget *>(d->m_buttons->parent());

        if (true == value.toBool()) {

            panel->setEnabled(true);

            QList<MButton*> buttons = d->m_buttons->buttons();
            foreach (MButton *button, buttons) {
                button->click();
            }
            d->m_buttons->button(IncludeCoreBtn)->
                    setChecked(CReporterPrivacySettingsModel::instance()->includeCore());
            d->m_buttons->button(IncludeSyslogBtn)->
                    setChecked(CReporterPrivacySettingsModel::instance()->includeSystemLog());
            d->m_buttons->button(IncludePackagesBtn)->
                    setChecked(CReporterPrivacySettingsModel::instance()->includePackageList());
            d->m_buttons->button(ReduceCoreSizeBtn)->
                    setChecked(CReporterPrivacySettingsModel::instance()->reduceCore());        
         }
         else {
             // Deactivate parent widget.
            panel->setEnabled(false);
         }
    }
    for(int i = 0; i < btns.size(); i++) {
        connect(btns.at(i), SIGNAL(toggled(bool)),
                this, SLOT(groupButtonToggled(bool)), Qt::DirectConnection);

    }
}


// End of file.
