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

#ifndef UT_CREPORTERDIALOGSERVER_H
#define UT_CREPORTERDIALOGSERVER_H

#include <QTest>

#include "creporterdialogplugininterface.h"

class CReporterDialogServer;

class TestPlugin : public CReporterDialogPluginInterface
{
    Q_OBJECT
    Q_INTERFACES(CReporterDialogPluginInterface)

    public:
        TestPlugin();
        virtual void initialize(CReporterDialogServerInterface *server);
        virtual void destroy();
        virtual bool isInitialized() const;
        virtual QString name() const;
        virtual bool requestDialog(const QVariantList &arguments);
        virtual bool isActive() const;
        virtual bool isVisible() const;
        void emitRequestCompleted();

    public:
        bool initialized;
        bool initializeCalled;
        bool destroyCalled;
        CReporterDialogServerInterface *server;
        QString pluginName;
        bool active;
        bool requestHandled;
        CReporterDialogRequest *request;
        QVariantList arguments;
        bool accepted;
};

class Ut_CReporterDialogServer : public QObject
{
    Q_OBJECT

    private Q_SLOTS:
        void initTestCase();
        void init();

        void testDialogPluginLoadingSucceeds();
        void testDialogPluginLoadingFails();
        void testDialogPluginDestroyed();
        void testSendAllCrashReportsRequested();
        void testNewCrashReportNotified();
        void testRequestInvalidPlugin();
        void testRequestActivePlugin();
        void testDialogRequestedByAnotherPlugin();
        void testInvalidPluginRequestedByAnotherPlugin();

        void cleanupTestCase();
        void cleanup();

    private:
        CReporterDialogServer *m_Subject;
};

#endif // UT_CREPORTERDIALOGSERVER_H
