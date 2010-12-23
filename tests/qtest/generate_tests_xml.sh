#!/bin/sh

#
# This file is part of crash-reporter
#
# Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
#
# Contact: Ville Ilvonen <ville.p.ilvonen@nokia.com>
# Author: Riku Halonen <riku.halonen@nokia.com>
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public License
# version 2.1 as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301 USA
#

PACKAGE="crash-reporter-tests"
SYSTEMTESTSDIR=${1:-.}		# can be set as param $1, defaults to current dir

echo "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>
 <testdefinition version=\"0.1\">
  <suite name=\"$PACKAGE\" domain=\"AutomatedTesting\">
   <set name=\"crash-reporter-unit-tests\" description=\"Crash Reporter Unit Tests\">
"

# Append unit tests.

for TESTCASE in `ls -d ?t_*`; do
    if [ -x $TESTCASE/.out/$TESTCASE ]; then
        if [ -n "`echo $TESTCASE | egrep '^u'`" ]; then
echo "    <case type=\"Functional\" name=\"$TESTCASE\" description=\"$PACKAGE:$TESTCASE\">
     <step>su - user -c \"/usr/lib/$PACKAGE/$TESTCASE\"</step>
    </case>
    "
        fi
    fi
done

echo "    <environments>
     <scratchbox>false</scratchbox>
     <hardware>true</hardware>
    </environments>
   </set>
"

# Append system tests.
cat $SYSTEMTESTSDIR/systemtests.xml

echo "  </suite>
</testdefinition>
"
