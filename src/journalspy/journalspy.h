/*
 * This file is a part of crash-reporter.
 *
 * Copyright (C) 2014 Jolla Ltd.
 * Contact: Jakub Adam <jakub.adam@jollamobile.com>
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
 */

#ifndef JOURNALSPY_H
#define JOURNALSPY_H

#include <QObject>

class JournalSpyPrivate;

class JournalSpy: public QObject
{
    Q_OBJECT

public:
    JournalSpy();

    ~JournalSpy();

private:
    Q_DECLARE_PRIVATE(JournalSpy)
    QScopedPointer<JournalSpyPrivate> d_ptr;

    // No more than 4 matches per expression each day.
    static const qint64 SILENT_PERIOD_MS = 6 * 60 * 60 * 1000;

    Q_PRIVATE_SLOT(d_func(), void handleJournalEntries());
};

#endif // JOURNALSPY_H
