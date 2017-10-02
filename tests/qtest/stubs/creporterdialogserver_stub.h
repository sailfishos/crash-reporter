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

#include "creporterdialogserverinterface.h"

class TestDialogServer : public QObject,
    public CReporterDialogServerInterface
{
    Q_OBJECT
    Q_INTERFACES(CReporterDialogServerInterface)

public:
    TestDialogServer(QObject *parent = 0) :
        QObject(parent)
    {
        createRequestCalled = false;
        showDialogCalled = false;
        hideDialogCalled = false;
        requestCreated = true;
        dialog = 0;
    }

    virtual bool createRequest(const QString &dialogName,
                               const QVariantList &arguments);

    virtual void processRequests();

    virtual void showDialog(MDialog *dialog);
    virtual void hideDialog(MDialog *dialog);

public:
    bool createRequestCalled;
    bool showDialogCalled;
    bool hideDialogCalled;
    bool requestCreated;
    MDialog *dialog;
};
