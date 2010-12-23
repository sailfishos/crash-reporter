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

#ifndef CREPORTERPRIVACYSETTINGSWIDGET_H
#define CREPORTERPRIVACYSETTINGSWIDGET_H

// System includes.

#include <DcpWidget>

// Forward declarations.

class MButton;
class QDBusPendingCallWatcher;

/*!
  * @class CReporterPrivacySettingsWidget
  * @brief DcpWidget for editing crash-reporter privacy settings, displaying privacy
  * settings dialog and sending crash reports.
  *
  * This view contains widgets for modifying following settings:
  * @li Save crash reports.
  * @li Notify about crash reports.
  * @li Auto delete crash reports
  * @li What information to include in crash reports.
  */
class CReporterPrivacySettingsWidget : public DcpWidget
{
	Q_OBJECT

public:
	/*!
      * @brief Class constructor.
      *
      */
    CReporterPrivacySettingsWidget();

	/*!
      * @brief Class destructor.
	  *
	  */
    ~CReporterPrivacySettingsWidget();

    /*!
      * @reimp
      */
    virtual bool back();

private Q_SLOTS:
    /*!
      * @brief Opens application modal dialog for displaying privacy settings.
      *
      */
	void openPrivacyDisclaimerDialog();

    /*!
      * @brief Called,  when user interacts with the button sending all crash reports in
      *     the system for analysis.
      *
      * @note NOT IMPLEMENTED.
      */
    void handleSendAllButtonClicked();

    /*!
      * @brief Called, when user interacts with the button openning a selection dialog for crash reports in
      *     the system for analysis.
      *
      * @note NOT IMPLEMENTED.
      */
    void handleSendSelectButtonClicked();

    /*!
      * @brief Called when getCoreFiles dbus request has finished.
      *
      */
    void gotCoreFilesReply(QDBusPendingCallWatcher *call);

private:
    /*!
     * @brief Initiates widget, sets layout etc.
     *
     */
	void initWidget();

private:
    //! @arg MButton for displaying privacy settings dialog.
	MButton *m_showPrivacyButton;
    //! @arg MButton for querying and sending crash reports.
    //MButton *m_sendAllButton;
    //! @arg MButton for querying and sending crash reports (show a selection dialog).
    MButton *m_sendSelectButton;
    //! @arg True, when the applet has sent a dbus request for core files and is waiting for reply.
    bool waitingForCoreFilesReply;
};

#endif // CREPORTERPRIVACYSETTINGSWIDGET_H

// End of file.
