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

#ifndef CREPORTERINCLUDESETTINGSCONTAINER_H
#define CREPORTERINCLUDESETTINGSCONTAINER_H

// System includes.

#include <MStylableWidget>

// Forward declarations.

class CReporterIncludeSettingsContainerPrivate;

/*!
  * @class CReporterIncludeSettingsContainer
  * @brief Container class for gouping widgets.
  *
  * This class groups widgets for editing settings, what to include in crash reports
  * in to MContainer.
  */
class CReporterIncludeSettingsContainer : public MStylableWidget
{
    Q_OBJECT

    public:

        /*!
          * @typedef SettingsGroupButton
          * @brief Unique identifiers for toggle buttons.
          *
          */
        enum SettingsGroupButton {
            //! Toggle button for including core dump in crash report.
            IncludeCoreBtn = 0,
            //! Toggle button for syslog core dump in crash report.
            IncludeSyslogBtn,
            //! Toggle button for including list of installed packages in crash report.
            IncludePackagesBtn,
            //! Toggle button for shrinking core size.
            ReduceCoreSizeBtn,
        } typedef SettingsGroupButton;

        /*!
          * @brief Class constructor.
          *
          * @param parent Parent object reference.
          */
        CReporterIncludeSettingsContainer(QGraphicsItem *parent);

        /*!
          * @brief Class destructor.
          *
          */
        virtual ~CReporterIncludeSettingsContainer();

    protected:
        /*!
          * @brief Initiates this widget.
          *
          */
        void initWidget();

    protected Q_SLOTS:
        /*!
          * @brief Called, when button in group is clicked.
          *
          * @param checked Checked status of button.
          */
        void groupButtonToggled(bool checked);

        /*!
          * @brief Called, when setting @a key changes.
          *
          * @param key Changed setting.
          * @param value New setting value.
          */
        void settingsChanged(const QString& key, const QVariant& value);

    private:
        Q_DECLARE_PRIVATE(CReporterIncludeSettingsContainer)
       //! @arg Pointer to private data class.
       CReporterIncludeSettingsContainerPrivate *d_ptr;
};

#endif // CREPORTERINCLUDESETTINGSCONTAINER_H
