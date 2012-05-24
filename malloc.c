/*  an - Anagram generator
    Copyright (C) 2012  Paul Martin <pm@debian.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/* Safe allocation */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "malloc.h"

void *
safe_calloc(size_t nmemb, size_t size)
{
    void *ret = calloc(nmemb, size);
    if (ret == NULL) {
        fprintf(stderr, "Out of memory in calloc()\n");
        exit(99);
    }
    return ret;
}

char *
safe_strdup(const char *s)
{
    char *r = strdup(s);
    if (r == NULL) {
        fprintf(stderr, "Out of memory in strdup()\n");
        exit(99);
    }
    return r;
}
