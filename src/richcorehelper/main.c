/*
 * This file is a part of crash-reporter.
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(int argc, const char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: rich-core-helper report_label\n");
        return EXIT_FAILURE;
    }

    const char *label = argv[1];
    size_t label_len = strlen(label);
    if (label_len == 0) {
        goto invalid_label;
    }

    /* Label is going to be passed to a SETUID script so make sure its content
     * is sane. Only alphanumeric chars, underscore and hyphen are allowed,
     * no spaces. */
    size_t i;
    for (i = 0; i != label_len; ++i) {
        if (!isalnum(label[i]) && !label[i] == '_' && !label[i] == '-') {
            goto invalid_label;
        }
    }

    srand(time(NULL));
    int randomid = rand();

    char command[128];
    snprintf(command, sizeof (command),
            "/usr/sbin/rich-core-dumper --name=%s --signal=%d",
            label, randomid);

    setuid(0);
    char *args[] =
            { "/bin/env",
              "-i",
              "/bin/bash",
              "-c",
              command,
              NULL };
    if (execvp(args[0], args)) {
        fprintf(stderr, "Couldn't invoke rich-core-dumper\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

invalid_label:
    fprintf(stderr, "Invalid crash report label: %s\n", label);
    return EXIT_FAILURE;
}
