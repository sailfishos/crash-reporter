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

#include "ut_creporternwsessionmgr.h"

void Ut_CReporterNwSessionMgr::test_opened() {
	mgr = new CReporterNwSessionMgr();
	QCOMPARE(mgr->opened(), false);
}

void Ut_CReporterNwSessionMgr::test_open() {
	mgr = new CReporterNwSessionMgr();
	mgr->open();
	QCOMPARE(mgr->opened(), true);
}

void Ut_CReporterNwSessionMgr::test_openTwice() {
	mgr = new CReporterNwSessionMgr();
	mgr->open();
	QCOMPARE(mgr->opened(), true);
	mgr->open();
	QCOMPARE(mgr->opened(), true);
}

void Ut_CReporterNwSessionMgr::test_close() {
	mgr = new CReporterNwSessionMgr();
	mgr->open();
	QCOMPARE(mgr->opened(), true);
    mgr->stop();
	QCOMPARE(mgr->opened(), false);
}

QTEST_MAIN(Ut_CReporterNwSessionMgr)
