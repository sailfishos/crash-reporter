/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
 * Author: Raimo Gratseff <ext-raimo.gratseff@nokia.com>
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

#ifndef UT_CREPORTERSENDSELECTEDDIALOGPLUGIN_H
#define UT_CREPORTERSENDSELECTEDDIALOGPLUGIN_H

#include <QTest>

class CReporterSendSelectedDialogPlugin;
class TestDialogServer;

class Ut_CReporterSendSelectedDialogPlugin : public QObject
{
    Q_OBJECT

    private Q_SLOTS:

        void init();
        void initTestCase();

        void testInitializePlugin();
        void testRequestPluginNotInitialized();
        void testDestroyPlugin();
        void testRequestPluginInvalidArguments();
        void testRequestHandledFilesSent();
        void testRequestHandledFilesDeleted();
        void testRejectDialog();

        void cleanupTestCase();
        void cleanup();

    private:
        CReporterSendSelectedDialogPlugin *m_Subject;
        TestDialogServer *m_Server;
};

#endif // UT_CREPORTERSENDSELECTEDDIALOGPLUGIN_H

