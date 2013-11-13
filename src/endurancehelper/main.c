/*
 * This file is part of crash-reporter
 *
 * Copyright (C) 2013 Jolla Ltd.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2 || (strcmp(argv[1], "start") && strcmp(argv[1], "stop")
        && strcmp(argv[1], "enable") && strcmp(argv[1], "disable"))) {
        fprintf(stderr, "Usage: endurance-helper (start|stop|enable|disable)\n");
        return EXIT_FAILURE;
    }

    char *args[] =
            { "systemctl", argv[1], "crash-reporter-endurance.service", NULL };
    if (execvp(args[0], args)) {
        fprintf(stderr, "Couldn't execute systemctl\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
