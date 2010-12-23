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

// System includes.

#include <QDebug>
#include <QDir>
#include <QPluginLoader>
#include <QTimer>
#include <QDBusConnection>

#include <MSceneManager>
#include <MApplication>
#include <MDialog>
#include <MApplicationService>

// User includes.

#include "creporterdialogserver.h"
#include "creporterdialogplugininterface.h"
#include "creporternotification.h"
#include "creporterdialogserverdbusadaptor.h"
#include "creporterdialogserver_p.h"
#include "creporternamespace.h"
#include "creporterinfobanner.h"

/*!
  * @class CReporterDialogRequest
  * @brief This class represents request passed to dialog plugins.
  *
  */
class CReporterDialogRequest
{
    public:
        CReporterLoadedPlugin *plugin;
        QVariantList arguments;
        QDBusMessage reply;
};

/*!
  * @class CReporterLoadedPlugin
  * @brief A class representing loaded dialog plugin.
  *
  */
class CReporterLoadedPlugin
{
    public:
        CReporterDialogPluginInterface *iface;
};

// ******** Class CReporterDialogServerPrivate ********

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterDialogServerPrivate::CReporterDialogServerPrivate
// ----------------------------------------------------------------------------
CReporterDialogServerPrivate::CReporterDialogServerPrivate() :
       m_scheduleTimer(0)
{

}

// ----------------------------------------------------------------------------
// CReporterDialogServerPrivate::~CReporterDialogServerPrivate
// ----------------------------------------------------------------------------
CReporterDialogServerPrivate::~CReporterDialogServerPrivate()
{

}

// ----------------------------------------------------------------------------
// CReporterDialogServerPrivate::processRequests
// ----------------------------------------------------------------------------
void CReporterDialogServerPrivate::processRequests()
{
    qDebug() << __PRETTY_FUNCTION__ << "Prosessing request queue...";

    m_requestQueueMutex.lock();
    // If a plugin is active, simply show the previously opened dialog
    if (isPluginActive()) {
        MWindow* win = MApplication::activeWindow();
        if (m_appService && win != 0 && !win->isHidden())
        {
            qDebug() << __PRETTY_FUNCTION__ << "UI was already open. Previously opened dialog is shown.";
            CReporterInfoBanner::show(qtTrId("This Crash Reporter dialog was already open."));
            m_appService->launch();
            // Remove the request from the queue
            m_requestQueue.takeFirst();
            m_requestQueueMutex.unlock();
            return;
        }
    }

    if (m_requestQueue.isEmpty()) {
        // Return, if no requests to process.
        qDebug() << __PRETTY_FUNCTION__ << "No requests to process.";
        m_appService = 0;
        // Exit from main loop.
        MApplication::instance()->quit();
        m_requestQueueMutex.unlock();
        return;
    }

    // Take the first request from the queue and dispatch it.
    CReporterDialogRequest *request = m_requestQueue.takeFirst();

    // Request dialog from the plugin.
    if (!request->plugin->iface->requestDialog(request->arguments)) {
        qDebug() << __PRETTY_FUNCTION__ << "Plugin:"
                << request->plugin->iface->name() << "rejected the request.";
        if (request->reply.isDelayedReply()) {
            // If message reply was delayed reply to sender.
            QDBusConnection::sessionBus().send(
                    request->reply.createErrorReply(QDBusError::InvalidArgs, "Invalid arguments"));
        }
    }
    else {
        if (request->reply.isDelayedReply()) {
            // If message reply was delayed reply to sender.
            QDBusMessage reply = request->reply.createReply();
            reply << true;
            QDBusConnection::sessionBus().send(reply);
        }
    }
    m_requestQueueMutex.unlock();
    delete request;
}

// ----------------------------------------------------------------------------
// CReporterDialogServerPrivate::requestCompleted
// ----------------------------------------------------------------------------
void CReporterDialogServerPrivate::requestCompleted()
{
    CReporterDialogPluginInterface *plugin =
            qobject_cast<CReporterDialogPluginInterface *>(sender());

    qDebug() << __PRETTY_FUNCTION__ << "Plugin:" << plugin->name()
            << "completed request.";

    // Process request queue.
    processRequests();
}

// ----------------------------------------------------------------------------
// CReporterDialogServerPrivate::dialogDestroyed
// ----------------------------------------------------------------------------
void CReporterDialogServerPrivate::dialogDestroyed(QObject *dialog)
{
    qDebug() << __PRETTY_FUNCTION__ << "Dialog destroyed.";

    m_dialogs.removeAll(dialog);

    if (m_dialogs.isEmpty()) {
        qDebug() << __PRETTY_FUNCTION__ << "No more dialogs -> hide window.";
        MWindow *appWindow = MApplication::activeWindow();
        if (appWindow != 0) {
            appWindow->hide();
        }
    }
}
// ----------------------------------------------------------------------------
// CReporterDialogServerPrivate::loadPlugins
// ----------------------------------------------------------------------------
void CReporterDialogServerPrivate::loadPlugins()
{
    QDir pluginsDir(m_pluginPath);
    qDebug() << __PRETTY_FUNCTION__ << "Load plugins from:" <<  pluginsDir.absolutePath();

    // Go through all plugins in the directory.
    foreach (QString fileName, pluginsDir.entryList(QDir::Files)) {
        QPluginLoader pluginLoader(pluginsDir.absoluteFilePath(fileName));
        QObject *pluginObject = pluginLoader.instance();

        if (pluginObject != 0) {
            qDebug() << __PRETTY_FUNCTION__ << "Initialize plugin:" << fileName;
            loadPlugin(pluginObject);
        } else {
            qWarning() << __PRETTY_FUNCTION__ << "Plugin loading failed:"
                    << pluginLoader.errorString();
        }
    }
}

// ----------------------------------------------------------------------------
// CReporterDialogServerPrivate::loadPlugin
// ----------------------------------------------------------------------------
void CReporterDialogServerPrivate::loadPlugin(QObject *pluginObject)
{
    Q_ASSERT(pluginObject != 0);
    CReporterDialogPluginInterface *dialogPlugin =
            qobject_cast<CReporterDialogPluginInterface *>(pluginObject);

    if (dialogPlugin) {
        // Try to Initialize plugin.
        dialogPlugin->initialize(q_ptr);
        if (dialogPlugin->isInitialized()) {
            qDebug() << __PRETTY_FUNCTION__ << "Plugin:" << dialogPlugin->name() << "loaded.";

            // Append to list, if initialization was successfull.
            CReporterLoadedPlugin *loadedPlugin = new CReporterLoadedPlugin();
            loadedPlugin->iface = dialogPlugin;

            m_plugins.append(loadedPlugin);
            connect(loadedPlugin->iface, SIGNAL(requestCompleted()), SLOT(requestCompleted()));
        }
        else {
            qWarning() << __PRETTY_FUNCTION__ << "Initialization failed.";
        }
    }
}

// ----------------------------------------------------------------------------
// CReporterDialogServerPrivate::destroyPlugins
// ----------------------------------------------------------------------------
void CReporterDialogServerPrivate::destroyPlugins()
{
    qDebug() << __PRETTY_FUNCTION__ << "Destroy all plugins.";

    // Go through all plugins in the list and call destroy.
    foreach (CReporterLoadedPlugin *plugin, m_plugins) {
        plugin->iface->destroy();
        delete plugin;
    }
}

// ----------------------------------------------------------------------------
// CReporterDialogServerPrivate::isPluginActive
// ----------------------------------------------------------------------------
bool CReporterDialogServerPrivate::isPluginActive()
{
    // Loop through plugins and check, if there active one.
    foreach(CReporterLoadedPlugin *loadedPlugin, m_plugins) {
        if (loadedPlugin->iface->isActive()) {
            qDebug() << __PRETTY_FUNCTION__ << "Plugin:" << loadedPlugin->iface->name()
                    << "is active.";
            return true;
        }
    }
    return false;
}


// ******** Class CReporterDialogServer ********

// ======== MEMBER FUNCTIONS ========

// ----------------------------------------------------------------------------
// CReporterDialogServer::CReporterDialogServer
// ----------------------------------------------------------------------------
CReporterDialogServer::CReporterDialogServer(const QString &pluginPath, MApplicationService *service) :
        d_ptr(new CReporterDialogServerPrivate())
{
    d_ptr->m_appService = service;
    d_ptr->q_ptr = this;
    d_ptr->m_pluginPath = pluginPath;

    // Load plugins.
    d_ptr->loadPlugins();

    // Create adaptor class. Needs to be taken from the stack.
    new CReporterDialogServerDBusAdaptor(this);
    // Register service name and object.
    QDBusConnection::sessionBus().registerService(CReporter::DialogServerServiceName);
    QDBusConnection::sessionBus().registerObject(CReporter::DialogServerObjectPath, this);
}

// ----------------------------------------------------------------------------
// CReporterDialogServer::~CReporterDialogServer
// ----------------------------------------------------------------------------
CReporterDialogServer::~CReporterDialogServer()
{
    qDebug() << __PRETTY_FUNCTION__ << "Entered destructor.";

    d_ptr->m_requestQueueMutex.lock();
    // Delete requests.
    QList<CReporterDialogRequest *> reqs =  d_ptr->m_requestQueue;
    // Clear list.
    d_ptr->m_requestQueue.clear();
    // Delete entries.
    qDeleteAll(reqs);
    d_ptr->m_requestQueueMutex.unlock();

    // Delete plugins.
    d_ptr->destroyPlugins();

    MWindow *appWindow = MApplication::activeWindow();
    if (appWindow != 0) {
        appWindow->close();
    }

    delete d_ptr;
    d_ptr = 0;

    qDebug() << __PRETTY_FUNCTION__ << "Server destroyed.";
}

// ----------------------------------------------------------------------------
// CReporterDialogServer::createRequest
// ----------------------------------------------------------------------------
bool CReporterDialogServer::createRequest(const QString &dialogName,
                                          const QVariantList &arguments)
{
    CReporterLoadedPlugin *handlerPlugin = 0;

    // Find handler plugin for the request.
    foreach(CReporterLoadedPlugin *plugin, d_ptr->m_plugins) {
        if (plugin->iface->name() == dialogName) {
                qDebug() << __PRETTY_FUNCTION__ << "Request handler found:"
                        << plugin->iface->name();
                handlerPlugin = plugin;
                break;
        }
    }

    // No handler found.
    if (handlerPlugin == 0) return false;

    CReporterDialogRequest *request = new CReporterDialogRequest();
    request->plugin = handlerPlugin;
    request->arguments = arguments;
    request->reply = QDBusMessage();

    qDebug() << __PRETTY_FUNCTION__ << "Queue new request...";
    d_ptr->m_requestQueueMutex.lock();
    d_ptr->m_requestQueue.append(request);
    d_ptr->m_requestQueueMutex.unlock();

    return true;
}

// ----------------------------------------------------------------------------
// CReporterDialogServer::showDialog
// ----------------------------------------------------------------------------
void CReporterDialogServer::showDialog(MDialog *dialog)
{
    qDebug() << __PRETTY_FUNCTION__ << "Dialog visibility requested.";

    if (dialog == 0) return;

    // Bring window on top.
    MWindow *appWindow = MApplication::activeWindow();
    if (appWindow != 0) {
            appWindow->show();
            appWindow->activateWindow();
            appWindow->raise();
        }

    d_ptr->m_dialogs.append(dialog);

    // Show dialog without animation.
    MSceneManager *sceneMgr = appWindow->sceneManager();
    sceneMgr->appearSceneWindowNow(dialog, MSceneWindow::DestroyWhenDone);

    connect(dialog, SIGNAL(destroyed(QObject*)),
            d_ptr, SLOT(dialogDestroyed(QObject*)));
}

// ----------------------------------------------------------------------------
// CReporterDialogServer::hideDialog
// ----------------------------------------------------------------------------
void CReporterDialogServer::hideDialog(MDialog *dialog)
{
    if (dialog->isVisible()) {
        qDebug() << __PRETTY_FUNCTION__ << "Hide Dialog.";
        // Hide dialog without animation.
        MWindow *appWindow = MApplication::activeWindow();
        MSceneManager *sceneMgr = appWindow->sceneManager();
        sceneMgr->disappearSceneWindowNow(dialog);
    }
}

// ----------------------------------------------------------------------------
// CReporterDialogServer::callReceived
// ----------------------------------------------------------------------------
QDBusError::ErrorType CReporterDialogServer::callReceived(const QString &dialogName,
                                                 const QVariantList &arguments, const QDBusMessage &message)
{
    qDebug() << __PRETTY_FUNCTION__ << "Dialog:" << dialogName <<  "requested.";

    CReporterLoadedPlugin *handlerPlugin = 0;

    // Here we previously checked whether a dialog is already active and gave a dbus error if yes

    // Find handler plugin for the request.
    foreach(CReporterLoadedPlugin *plugin, d_ptr->m_plugins) {
        if (plugin->iface->name() == dialogName) {
                qDebug() << __PRETTY_FUNCTION__ << "Request handler found:"
                        << plugin->iface->name();
                handlerPlugin = plugin;
                break;
        }
    }

    // Handler plugin not found.
    if (handlerPlugin == 0) {
        qWarning() << __PRETTY_FUNCTION__ << "Handler plugin not found.";
        return QDBusError::NotSupported;
    }

    // This has been disabled because we will now show the currently open dialog when one exists
    // and there's a new dbus call
    // If plugin is active, reject request.
    /*if (handlerPlugin->iface->isActive()) {
        qWarning() << __PRETTY_FUNCTION__ << "Handler plugin active. Request ignored.";
        return QDBusError::Failed;
    }*/

    // Create new scheduled request.
    CReporterDialogRequest *request = new CReporterDialogRequest();
    request->plugin = handlerPlugin;
    request->arguments = arguments;
    request->reply = message;

    // Queue and schedule request.
    d_ptr->m_requestQueueMutex.lock();
    d_ptr->m_requestQueue.append(request);

    if (d_ptr->m_scheduleTimer == 0) {
        d_ptr->m_scheduleTimer = new QTimer(this);
        connect(d_ptr->m_scheduleTimer, SIGNAL(timeout()), d_ptr, SLOT(processRequests()));
    }

    if (!d_ptr->m_scheduleTimer->isActive() && !d_ptr->m_requestQueue.isEmpty()) {
            d_ptr->m_scheduleTimer->setSingleShot(true);
            d_ptr->m_scheduleTimer->start(10);
    }
    d_ptr->m_requestQueueMutex.unlock();

    // Request queued successfully.
    return QDBusError::NoError;
}

// ----------------------------------------------------------------------------
// CReporterDialogServer::quit()
// ----------------------------------------------------------------------------
void CReporterDialogServer::quit()
{
    qDebug() << __PRETTY_FUNCTION__ << "Quit dialog server.";
    d_ptr->m_appService = 0;
    MApplication::instance()->quit();
}

// End of file.
