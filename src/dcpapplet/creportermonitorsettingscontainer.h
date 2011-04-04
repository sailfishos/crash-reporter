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

#ifndef CREPORTERMONITORSETTINGSCONTAINER_H
#define CREPORTERMONITORSETTINGSCONTAINER_H

// System includes.

#include <MStylableWidget>

// Forward declarations.

class CReporterMonitorSettingsContainerPrivate;

/*!
  * @class CReporterMonitorSettingsContainer
  * @brief Container class for gouping widgets.
  *
  * This class groups widgets for editing settings, whether crash reports are saved,
  * monitored, auto-deleted, automatically sent or not in to MContainer.
  */
class CReporterMonitorSettingsContainer : public MStylableWidget
{
    Q_OBJECT

    public:
        /*!
          * @typedef SettingsGroupButton
          * @brief Unique identifiers for buttons in group.
          */
        enum SettingsGroupButton {
            //! @arg Toggle button for saving crash reports.
            SaveCrashReportsBtn = 0,
            //! @arg Toggle button for notifying about crash reports.
            NotifyCrashReportsBtn,
            //! @arg Toggle button for deleting crash reports automatically.
            AutoDeleteCrashReportsBtn,
            //! @arg Toggle button for automatic sending.
            AutomaticSendingBtn,
            //! @arg Toggle lifelog
            LifelogBtn,
            //! @arg None. Simply update the buttons.
            NoBtn
        } typedef SettingsGroupButton;

        /*!
          * @brief Class constructor.
          *
          * @param parent Parent object reference.
          */
        CReporterMonitorSettingsContainer(QGraphicsItem *parent);

        /*!
          * @brief Class destructor.
          *
          */
        virtual ~CReporterMonitorSettingsContainer();

    public Q_SLOTS:

        /*!
         * @brief Update button states according to settings
         */
        void updateButtons();

    protected:
        /*!
         * @brief Add widgets to the layout.
         */
        void initWidget();

    protected Q_SLOTS:
        /*!
          * @brief Called whenever a button in a group is clicked.
          *
          * @param checked Checked status of button.
          */
        void groupButtonToggled(bool checked);

        /*!
          * @brief Show message box with info on experimental auto uploading feature.
          *
          */
        void showAutoUploaderMessage();

    private:
        Q_DECLARE_PRIVATE(CReporterMonitorSettingsContainer)
       //! @arg Pointer to private data class.
       CReporterMonitorSettingsContainerPrivate *d_ptr;
};

#endif // CREPORTERMONITORSETTINGSCONTAINER_H
