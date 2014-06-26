/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2013-2014 Jolla Ltd.
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

#include <errno.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <dbus/dbus.h>
#include <iphbd/libiphb.h>
#include <mce/dbus-names.h>
#include <sys/signalfd.h>
#include <sys/timerfd.h>
#include <sys/wait.h>

static const time_t SNAPSHOT_INTERVAL = IPHB_GS_WAIT_1_HOUR; // seconds
static const time_t KEEPALIVE_TIMER = 30; // seconds
static const time_t AFTER_BOOT_DELAY = 5 * 60; // seconds

DBusConnection *system_bus;
pid_t child_pid = 0;

static void invoke_endurance_collect() {
    if (child_pid != 0) {
        syslog(LOG_WARNING, "Previous snapshot collection is still running, "
                "not spawning a new one.");
        return;
    }

    child_pid = fork();
    if (child_pid == -1) {
        syslog(LOG_ERR, "Error on fork().");
    } else if (child_pid == 0) {
        char *args[] =
                { "/bin/sh", "-c", "/usr/libexec/endurance-collect", NULL };

        if (execvp(args[0], args)) {
            syslog(LOG_CRIT, "Couldn't invoke %s", args[2]);
            exit(EXIT_FAILURE);
        }
    }
}

static void set_timer(int fd, time_t seconds) {
    struct itimerspec its;
    its.it_value.tv_sec = seconds;
    its.it_value.tv_nsec = 0;
    its.it_interval.tv_sec = seconds;
    its.it_interval.tv_nsec = 0;

    timerfd_settime(fd, 0, &its, NULL);
}

static int mce_method(const char *method) {
    if(!system_bus) {
        return FALSE;
    }

    DBusMessage *req = dbus_message_new_method_call(MCE_SERVICE,
            MCE_REQUEST_PATH, MCE_REQUEST_IF, method);
    if(!req) {
        return FALSE;
    }

    dbus_message_set_no_reply(req, TRUE);

    int result = TRUE;
    if(!dbus_connection_send(system_bus, req, 0)) {
        syslog(LOG_ERR, "Failed to send %s.%s.", MCE_REQUEST_IF, method);
        result = FALSE;
    }

    dbus_message_unref(req);

    return result;
}

static int dbus_connect() {
    DBusError err = DBUS_ERROR_INIT;

    system_bus = dbus_bus_get(DBUS_BUS_SYSTEM, &err);
    if(!system_bus) {
        syslog(LOG_CRIT, "Couldn't connect to DBus: %s: %s",
                err.name, err.message);
    }

    dbus_error_free(&err);
    return system_bus != NULL;
}

static void dbus_disconnect() {
    if(system_bus) {
        dbus_connection_unref(system_bus);
        system_bus = NULL;
    }
}

static void send_keepalive() {
    syslog(LOG_NOTICE, "Sending MCE CPU keepalive.");
    mce_method(MCE_CPU_KEEPALIVE_START_REQ);
}

static void discard_input(int fd) {
    // Reading from SIGCHLD signal fd returns EINVAL with smaller buffer
    char buf[sizeof (struct signalfd_siginfo)];
    ssize_t bytes_read;
    do {
        bytes_read = read(fd, buf, sizeof buf);
        if ((bytes_read == -1) && (errno != EAGAIN)) {
            syslog(LOG_ERR, "Error reading from fd %d, errno %d.", fd, errno);
        }
    } while (bytes_read > 0);
}

static void after_boot_delay(iphb_t iphb)
{
    int fd = open("/proc/uptime", O_RDONLY);
    char buf[64];
    ssize_t bytes_read;
    char *uptime_str;
    time_t delay;

    if (fd < 0) {
        syslog(LOG_CRIT, "Couldn't open /proc/uptime.");
        return;
    }

    bytes_read = read(fd, buf, sizeof buf);
    if (bytes_read <= 0) {
        syslog(LOG_CRIT, "Couldn't read from /proc/uptime.");
        return;
    }
    close(fd);

    uptime_str = strtok(buf, ".");
    delay = AFTER_BOOT_DELAY - atoi(uptime_str);
    if (delay <= 0) {
        return;
    }

    syslog(LOG_DEBUG, "Too early after boot; waiting another %ld seconds.",
            delay);

    if (iphb_wait(iphb, delay - 10, delay + 10, 1) < 0) {
        syslog(LOG_CRIT, "Couldn't wait for heartbeat timer.");
    }
}

int main() {
    int result = EXIT_SUCCESS;

    openlog("endurance-collect-daemon", LOG_PID, LOG_USER);
    syslog(LOG_NOTICE, "Starting.");

    if (!dbus_connect()) {
        return EXIT_FAILURE;
    }

    iphb_t iphb = iphb_open(NULL);
    int iphbfd = iphb_get_fd(iphb);
    if (iphbfd == -1) {
        syslog(LOG_CRIT, "Couldn't get iphb file descriptor.");
        return EXIT_FAILURE;
    }
    fcntl(iphbfd, F_SETFL, O_NONBLOCK | O_CLOEXEC);

    after_boot_delay(iphb);

    sigset_t sigset;
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGTERM);
    sigaddset(&sigset, SIGINT);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    int sigfd = signalfd(-1, &sigset, SFD_NONBLOCK | SFD_CLOEXEC);

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGCHLD);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    int childexitfd = signalfd(-1, &sigset, SFD_NONBLOCK);

    int keepalivefd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

    syslog(LOG_DEBUG, "Opened file descriptors: %d %d %d %d",
            iphbfd, sigfd, childexitfd, keepalivefd);

    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(iphbfd, &fdset);

    while (1) {
        if (FD_ISSET(iphbfd, &fdset)) {
            syslog(LOG_DEBUG, "Collecting endurance snapshot...");
            send_keepalive();
            invoke_endurance_collect();
            set_timer(keepalivefd, KEEPALIVE_TIMER);
            discard_input(iphbfd);
        }
        if (FD_ISSET(childexitfd, &fdset)) {
            syslog(LOG_DEBUG, "Snapshot collection finished.");
            wait(NULL);
            child_pid = 0;
            set_timer(keepalivefd, 0);
            discard_input(childexitfd);
            mce_method(MCE_CPU_KEEPALIVE_STOP_REQ);
            if (iphb_wait(iphb, SNAPSHOT_INTERVAL - 10, SNAPSHOT_INTERVAL + 10, 0) < 0) {
                syslog(LOG_CRIT, "Couldn't wait for heartbeat timer.");
                break;
            }
        }
        if (FD_ISSET(sigfd, &fdset)) {
            syslog(LOG_NOTICE, "Exiting.");
            break;
        }
        if (FD_ISSET(keepalivefd, &fdset)) {
            send_keepalive();
            discard_input(keepalivefd);
        }

        FD_ZERO(&fdset);
        FD_SET(iphbfd, &fdset);
        FD_SET(sigfd, &fdset);
        FD_SET(childexitfd, &fdset);
        FD_SET(keepalivefd, &fdset);

        if (select(keepalivefd + 1, &fdset, NULL, NULL, NULL) == -1) {
            syslog(LOG_CRIT, "Error on select().");
            result = EXIT_FAILURE;
            break;
        }
    }

    iphb_close(iphb);
    dbus_disconnect();

    return result;
}
