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

#include <systemd/sd-journal.h>

#include <QDateTime>
#include <QDebug>
#include <QRegularExpression>
#include <QSocketNotifier>

#include "creporternamespace.h"
#include "creporterutils.h"
#include "journalspy.h"

using CReporter::LoggingCategory::cr;

class JournalSpyPrivate
{
public:
    JournalSpyPrivate(JournalSpy *parent);
    ~JournalSpyPrivate();

    void handleJournalEntries();

private:
    void loadExpressions();
    void parsePattern(const QString &name, QIODevice &io);

    JournalSpy *q_ptr;
    sd_journal *journal;

    struct Expression {
        QString name;
        QHash<QString, QRegularExpression> rexp;
        qint64 lastHit;

        Expression(const QString &name)
            : name(name), lastHit(0)
        {
        }
        bool matches(sd_journal *journal) const;
    };

    QList<Expression> expressions;

    Q_DECLARE_PUBLIC(JournalSpy)
};

JournalSpyPrivate::JournalSpyPrivate(JournalSpy *parent)
    : q_ptr(parent), journal(0)
{
    Q_Q(JournalSpy);

    loadExpressions();
    if (expressions.isEmpty()) {
        qCWarning(cr) << "No defined expressions to watch.";
        return;
    }

    if (sd_journal_open(&journal, SD_JOURNAL_LOCAL_ONLY | SD_JOURNAL_SYSTEM)) {
        qCWarning(cr) << "Failed to open systemd journal.";
        return;
    }

    int fd = sd_journal_get_fd(journal);
    if (fd < 0) {
        qCWarning(cr) << "sd_journal_get_fd() failed.";
        return;
    }

    if (sd_journal_seek_tail(journal)) {
        qCWarning(cr) << "sd_journal_seek_tail() failed.";
        return;
    }
    // Workaround for https://bugzilla.redhat.com/show_bug.cgi?id=979487.
    sd_journal_previous_skip(journal, 1);

    QSocketNotifier *notifier =
        new QSocketNotifier(fd, QSocketNotifier::Read, q);
    QObject::connect(notifier, SIGNAL(activated(int)),
                     q, SLOT(handleJournalEntries()));
}

void JournalSpyPrivate::handleJournalEntries()
{
    sd_journal_process(journal);

    while (sd_journal_next(journal)) {
        QList<Expression>::iterator it;
        for (it = expressions.begin(); it != expressions.end(); ++it) {
            Expression &e = *it;
            if (e.matches(journal)) {
                qint64 previousHit = e.lastHit;
                e.lastHit = QDateTime::currentMSecsSinceEpoch();
                if (e.lastHit - previousHit > JournalSpy::SILENT_PERIOD_MS) {
                    if (CReporterUtils::shouldSavePower()) {
                        qCDebug(cr) << "Battery low, NOT triggering log collection upon found match in the journal:"
                            << e.name;
                    } else {
                        qCDebug(cr) << "Triggering log collection upon found match in the journal:"
                            << e.name;
                        CReporterUtils::invokeLogCollection("JournalSpy-" + e.name);
                    }
                }
                break;
            }
        }
    }
}

bool JournalSpyPrivate::Expression::matches(sd_journal *journal) const
{
    QHash<QString, QRegularExpression>::const_iterator it;
    for (it = rexp.begin(); it != rexp.end(); ++it) {
        const char *val;
        std::size_t len;
        if (sd_journal_get_data(journal, it.key().toUtf8(),
                                reinterpret_cast<const void **>(&val), &len) < 0) {
            return false;
        }

        // Cut off "FIELD=" at the beginning of the message.
        val += it.key().length() + 1;
        len -= it.key().length() + 1;

        QRegularExpressionMatch match = it.value().match(QByteArray(val, len));
        if (!match.hasMatch()) {
            return false;
        }
    }

    return true;
}

void JournalSpyPrivate::loadExpressions()
{
    QFile file(CReporter::SystemSettingsLocation +
               "/crash-reporter-settings/journalspy-expressions.conf");
    file.open(QIODevice::ReadOnly);
    while (!file.atEnd()) {
        QByteArray line = file.readLine().trimmed();

        if (!line.startsWith(';')) {
            continue;
        }

        line.remove(0, 1);

        parsePattern(line, file);
    }
}

void JournalSpyPrivate::parsePattern(const QString &name, QIODevice &io)
{
    Expression expression(name);

    while (!io.atEnd()) {
        char nextChar = '\0';
        io.peek(&nextChar, 1);
        if (nextChar == ';') {
            break;
        }

        QByteArray line = io.readLine().trimmed();
        if (line.isEmpty() || line.startsWith('#')) {
            continue;
        }

        int separator = line.indexOf('=');
        if (separator == -1) {
            continue;
        }

        QString journalField(line.mid(0, separator));
        if (expression.rexp.contains(journalField)) {
            continue;
        }

        QRegularExpression pattern(QRegularExpression(line.mid(separator + 1)));
        if (!pattern.isValid()) {
            qCWarning(cr) << "Invalid regular expression" << pattern.pattern();
            continue;
        }

        qCDebug(cr) << "Watching journal for expression"
                    << qPrintable(journalField) << '='
                    << qPrintable(pattern.pattern());
        expression.rexp.insert(journalField, pattern);
    }

    if (!expression.rexp.isEmpty()) {
        expressions.append(expression);
    }
}

JournalSpyPrivate::~JournalSpyPrivate()
{
    sd_journal_close(journal);
}

JournalSpy::JournalSpy()
    : d_ptr(new JournalSpyPrivate(this))
{
}

JournalSpy::~JournalSpy()
{
}

#include "moc_journalspy.cpp"
