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
#include <QDir>
#include <QFile>
#include <QVariant>

#include <MGConfItem>
#include <QMap>

#include "creportercoreregistry.h"
#include "creportercoreregistry_p.h"
#include "creportertestutils.h"
#include "ut_creportercoreregistry.h"

extern QMap<QString, MGConfItem*> gMGConfItems;

void Ut_CReporterCoreRegistry::initTestCase()
{
	CReporterTestUtils::createTestMountpoints();
}

void Ut_CReporterCoreRegistry::init()
{
	paths = CReporterCoreRegistry::instance()->getCoreLocationPaths();
}

void Ut_CReporterCoreRegistry::testRegistryRefreshNeededEmission()
{	

    CReporterCoreRegistry *registry = CReporterCoreRegistry::instance();
	QSignalSpy registryRefreshNeededSpy( registry,
			SIGNAL(registryRefreshNeeded()) );

	registry->refreshRegistry();

	QCOMPARE( registryRefreshNeededSpy.count(), 1 );
}

void Ut_CReporterCoreRegistry::testCoreLocationsUpdatedEmission()
{
    QSignalSpy coreLocationsUpdatedSpy(CReporterCoreRegistry::instance(),
            SIGNAL(coreLocationsUpdated()));

    MGConfItem *conf = gMGConfItems["/system/osso/af/mmc-cover-open"];
    conf->set(QVariant(true));

	QTest::qWait( 5100 );
	QCOMPARE( coreLocationsUpdatedSpy.count(), 1 );
}

void Ut_CReporterCoreRegistry::cleanupTestCase()
{
    CReporterTestUtils::removeTestMountpoints();
}

void Ut_CReporterCoreRegistry::cleanup()
{	
	CReporterTestUtils::removeDirectories( *paths );

	delete paths;
	paths = NULL;
}

QTEST_MAIN(Ut_CReporterCoreRegistry)

