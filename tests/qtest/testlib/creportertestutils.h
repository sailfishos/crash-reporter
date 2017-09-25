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

class CReporterTestUtils
{
public:

    static void createTestDataFiles( const QStringList &testFiles, QStringList &compareFiles, const char *testData[] );
    static void cleanupDirectories( const QStringList &paths );
    static void removeDirectories( const QStringList &paths );
    static void removeDirectory( const QString &path );

#ifdef CREPORTER_UNIT_TEST
    static void createTestMountpoints();
    static void removeTestMountpoints();
#endif // CREPORTER_UNIT_TEST

    static QStringList *getCoreDumpPaths();
};

#define UNUSED_RESULT(x) if(x){}
