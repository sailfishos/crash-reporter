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

#include <QTextStream>
#include <QFile>

#include <MLabel>
#include <MLayout>
#include <QGraphicsLinearLayout>

// User includes.

#include "creporterprivacydisclaimerdlg.h"

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterPrivacyDisclaimerDlg::CReporterPrivacyDisclaimerDlg
// ----------------------------------------------------------------------------
CReporterPrivacyDisclaimerDlg::CReporterPrivacyDisclaimerDlg(const QString &filePath) :
        //% "Crash Reporter Privacy Disclaimer"
        MDialog(qtTrId("qtn_dcp_privacy_disclaimer_title_text"), M::CloseButton)
{
    initWidget(filePath);
}

// ----------------------------------------------------------------------------
// CReporterPrivacyDisclaimerDlg::~CReporterPrivacyDisclaimerDlg
// ----------------------------------------------------------------------------
CReporterPrivacyDisclaimerDlg::~CReporterPrivacyDisclaimerDlg()
{
}

// ======== LOCAL FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterPrivacyDisclaimerDlg::initWidget
// ----------------------------------------------------------------------------
void CReporterPrivacyDisclaimerDlg::initWidget(const QString &filePath)
{
    QFile inFile(filePath);
    MLabel *disclaimerLabel = 0;

    if (inFile.open(QIODevice::ReadOnly) )
    {
        QTextStream in(&inFile);
        // Force label to be rich-text type. Needed for auto-wrapping.
        QString statement = "<p>";

        while (!in.atEnd())
        {
            statement.append(in.readLine());
            statement.append(' ');
        }

        statement += "</p>";

        inFile.close();
        disclaimerLabel = new MLabel(statement);
        disclaimerLabel->setWordWrap(true);
    }
    else
    {
        //% "Could not open privacy statement file!"
        disclaimerLabel = new MLabel(qtTrId("qtn_dcp_cannot_open_file_text"));
    }

    QGraphicsLinearLayout *mainLayout = new QGraphicsLinearLayout(Qt::Vertical, centralWidget());
    centralWidget()->setLayout(mainLayout);
    mainLayout->addItem(disclaimerLabel);
}

// End of file.
