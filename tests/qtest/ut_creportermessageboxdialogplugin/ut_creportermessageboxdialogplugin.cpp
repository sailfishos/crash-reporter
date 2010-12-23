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

#include <QSignalSpy>

#include <MApplication>
#include <MDialog>

#include "ut_creportermessageboxdialogplugin.h"
#include "creportermessageboxdialogplugin.h"
#include "creporterdialogserver_stub.h"
#include "creporternamespace.h"

// Unit test.
void Ut_CReporterMessageBoxDialogPlugin::init()
{
    static MApplication *app = 0;

    if (app == 0) {
            int argc = 2;
            const char *argv[] = {"./ut_creportermessageboxdialogplugin", "-software", 0};
            app = new MApplication(argc, (char **)argv);
        }

    m_Server = new TestDialogServer();
    m_Subject = new CReporterMessageBoxDialogPlugin();
}

void Ut_CReporterMessageBoxDialogPlugin::initTestCase()
{
}

void Ut_CReporterMessageBoxDialogPlugin::cleanup()
{
    if (m_Server != 0) {
        delete m_Server;
        m_Server = 0;
    }

    if (m_Subject != 0) {
        if (m_Subject->isInitialized()) {
           m_Subject->destroy();
        }
        delete m_Subject;
        m_Subject = 0;
    }
}

void Ut_CReporterMessageBoxDialogPlugin::cleanupTestCase()
{
}

void Ut_CReporterMessageBoxDialogPlugin::testInitializePlugin()
{
   // Test plugin initialized successfully.
   m_Subject->initialize(m_Server);
   QVERIFY(m_Subject->name() == CReporter::MessageBoxDialogType);
   QVERIFY(m_Subject->isInitialized() == true);
}

void Ut_CReporterMessageBoxDialogPlugin::testRequestPluginNotInitialized()
{
    // Test request rejected, if plugin is not initialized.
    QVariantList args;
    args << QString("Test message.") << true;

    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == false);
    QVERIFY(m_Subject->isInitialized() == false);
}

void Ut_CReporterMessageBoxDialogPlugin::testDestroyPlugin()
{
    // Test destroying plugin.
    m_Subject->initialize(m_Server);
    m_Subject->destroy();
    QVERIFY(m_Subject->isInitialized() == false);
}

void Ut_CReporterMessageBoxDialogPlugin::testRequestPluginInvalidArgumentCount()
{
    // Test request rejected, when arguments are not valid.
    m_Subject->initialize(m_Server);
    QVariantList args;
    args << QString("Message1")
            << QString("Message2");

    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == false);
}

void Ut_CReporterMessageBoxDialogPlugin::testRequestPluginInvalidArgumentType()
{
    // Test request rejected, when arguments are not valid.
    m_Subject->initialize(m_Server);
    QVariantList args;
    args << 555;

    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == false);
}

void Ut_CReporterMessageBoxDialogPlugin::testShowMessageBox()
{
    // Test showing message box.
    m_Subject->initialize(m_Server);
    QVariantList args;
    args << QString("message");

    bool retVal = m_Subject->requestDialog(args);
    QVERIFY(retVal == true);

    QSignalSpy requestCompletedSpy(m_Subject, SIGNAL(requestCompleted()));

    QVERIFY(m_Server->showDialogCalled == true);
    QVERIFY(m_Subject->isActive() == true);

    // Disappear dialog.
    m_Server->dialog->accept();
    // Dialog closed.
    m_Subject->dialogFinished();
    QVERIFY(requestCompletedSpy.count() == 1);
}


QTEST_APPLESS_MAIN(Ut_CReporterMessageBoxDialogPlugin)
