/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2021 Jolla Ltd.
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

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>
#include <QTimer>
#include <QTranslator>

#include <notification.h>

#include "creporterapplicationsettings.h"
#include "creportercoreregistry.h"
#include "creporternamespace.h"
#include "creportersavedstate.h"
#include "creporterutils.h"

#include "creporterlogger.h"

const int DEFAULT_SIZE_LIMIT_MB = 100;

QTextStream &qerr()
{
    static QTextStream qerr(stderr);
    return qerr;
}

using CReporter::LoggingCategory::cr;

/*!
  * @brief Crash Reporter storage usage monitor application.
  */
class Application : public QCoreApplication
{
    Q_OBJECT

public:
    Application(int &argc, char **argv)
        : QCoreApplication(argc, argv)
    {
        QTranslator *engineeringEnglish = new QTranslator(this);
        engineeringEnglish->load("crash-reporter_eng_en", "/usr/share/translations");
        installTranslator(engineeringEnglish);

        QTranslator *translator = new QTranslator(this);
        translator->load(QLocale(), "crash-reporter", "-", "/usr/share/translations");
        installTranslator(translator);

        QTimer::singleShot(0, this, &Application::run);
    }

    ~Application()
    {
        CReporterApplicationSettings::freeSingleton();
    }

private:
    void run()
    {
        if (!parseArguments()) {
            exit(1);
            return;
        }

        checkStorageUsage();
        quit();
    }

    bool parseArguments()
    {
        QCommandLineParser parser;
        parser.addHelpOption();
        parser.addPositionalArgument("limit",
                QString("Size limit (MiB) for triggering notification (default: %1)")
                    .arg(DEFAULT_SIZE_LIMIT_MB),
                "[limit]");

        parser.process(*this);

        if (!parser.positionalArguments().isEmpty()) {
            bool ok;
            m_sizeLimitMb = parser.positionalArguments().first().toInt(&ok);
            if (!ok || m_sizeLimitMb <= 0) {
                qerr() << QString("Positive integer expected, got \"%1\"")
                    .arg(parser.positionalArguments().first()) << endl;
                m_sizeLimitMb = DEFAULT_SIZE_LIMIT_MB;
                return false;
            }
        }

        return true;
    }

    void checkStorageUsage()
    {
        qCDebug(cr) << "Size limit:" << m_sizeLimitMb << "MiB";

        qint64 totalSize = 0;
        CReporterCoreRegistry *registry = CReporterCoreRegistry::instance();
        foreach (const QString &coreFile, registry->collectAllCoreFiles()) {
            totalSize += QFileInfo(coreFile).size();
        }

        if (totalSize < (m_sizeLimitMb << 20)) {
            qCDebug(cr) << "Low storage usage - will not be reported";
            return;
        }

        qCDebug(cr) << "High storage usage - will be reported";

        CReporterSavedState *state = CReporterSavedState::instance();

        Notification notification(this);
        notification.setReplacesId(state->storageUsageNotificationId());
        CReporterUtils::applyNotificationStyle(&notification);

        //% "Many reports pending"
        notification.setSummary(qtTrId("crash_reporter-notify-storage_usage"));
        //% "You have collected %n MiB of crash data"
        notification.setBody(qtTrId("crash_reporter-notify-storage_usage_body",
                totalSize >> 20));
        notification.publish();

        state->setStorageUsageNotificationId(notification.replacesId());

        CReporterSavedState::freeSingleton();
    }

private:
    int m_sizeLimitMb{DEFAULT_SIZE_LIMIT_MB};
};

int main(int argc, char **argv)
{
    Logger logger(CReporterApplicationSettings::instance()->loggerType());

    Application app(argc, argv);
    return app.exec();
}

#include "storagemon.moc"
