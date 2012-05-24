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

#include <stdlib.h>
#include <stdio.h>
#include <unicode/uchar.h>
#include <unicode/ucnv.h>
#include <unicode/unorm2.h>

#include "unicode.h"
#include "malloc.h"

const UNormalizer2 *norm_decompose = NULL;
UConverter *conv_from_utf8 = NULL;

static void
initializeunicode(void)
{
    UErrorCode uerr = U_ZERO_ERROR;

    /* We're going to be doing a lot of conversions.
       Make sure we're only initializing these once. */
    if (conv_from_utf8 == NULL)
        conv_from_utf8 = ucnv_open("utf8", &uerr);
    if (norm_decompose == NULL)
        /* change UNORM2_DECOMPOSE to UNORM2_COMPOSE if you want accents
           to be significant */
        norm_decompose =
            unorm2_getInstance(NULL, "nfkc_cf", UNORM2_DECOMPOSE, &uerr);
}

UChar *
utf8tointernal(const char *utf8word)
{
    UChar *intermediate=NULL, *decomposed=NULL;
    int len, destlen;
    UErrorCode uerr = U_ZERO_ERROR;
    UChar *p, *q;

    initializeunicode();

    /* Convert from UTF-8 to internal representation */
    destlen = 0;
    do {
        uerr = U_ZERO_ERROR;
        len = ucnv_toUChars(conv_from_utf8,
                            intermediate, destlen,
                            utf8word, -1, &uerr);
        if (uerr == U_BUFFER_OVERFLOW_ERROR) {
            destlen = len + 1;
            intermediate = safe_calloc(destlen,sizeof(UChar));
        }
    } while (uerr == U_BUFFER_OVERFLOW_ERROR);

    if (U_FAILURE(uerr)) {
        fprintf(stderr, "toUChars: %s\n", u_errorName(uerr));
        exit(99);
    }

    /* decompose and case fold */
    destlen = 0;
    do {
        uerr = U_ZERO_ERROR;
        len = unorm2_normalize(norm_decompose,
                               intermediate, -1,
                               decomposed, destlen,
                               &uerr);
        if (uerr == U_BUFFER_OVERFLOW_ERROR) {
            destlen = len + 1;
            decomposed = safe_calloc(destlen,sizeof(UChar));
        }
    } while (uerr == U_BUFFER_OVERFLOW_ERROR);

    if (U_FAILURE(uerr)) {
        fprintf(stderr, "normalize: %s\n", u_errorName(uerr));
        exit(99);
    }

    free(intermediate);

    /* Copy down only alphabetic characters */
    p = q = decomposed;
    while (*p != 0) {
        if (u_isalpha(*p))
            *q++ = *p;
        p++;
    }
    *q = *p;
    return decomposed;
}
