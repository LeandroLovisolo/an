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
#include <stdbool.h>
#include <unicode/uchar.h>
#include <unicode/ustring.h>

#include "bitfield.h"
#include "malloc.h"


struct bitfield *
make_bitfield(const UChar *word, const UChar *alphabet)
{
    int alphabetlength = u_strlen(alphabet);
    int *freqs = safe_calloc(alphabetlength, sizeof(int));
    struct bitfield *bf;
    int maxfreq = 0;
    int index;

    if (freqs == NULL)
        return NULL;

    while (*word) {
        const UChar *p = alphabet;
        for (index = 0; *p && (*p != *word); p++, index++) {
        }
        if (*p) {
            freqs[index]++;
            if (freqs[index] > maxfreq) {
                maxfreq = freqs[index];
            }
        } else {
            /* character not in alphabet */
            free(freqs);
            return NULL;
        }
        word++;
    }

    bf = safe_calloc(1, sizeof(struct bitfield));
    if (bf != NULL) {
        bitpattern thisbit;

        bf->depth = maxfreq;
        bf->bits = safe_calloc(sizeof(bitpattern), maxfreq);
        thisbit = 1;

        for (index = 0; index <alphabetlength; index++) {
            bitpattern *bits = bf->bits;
            while (freqs[index] > 0) {
                *bits |= thisbit;
                bits++;
                freqs[index]--;
            }
            thisbit <<= 1;        
        }
    }
    free(freqs);
    return bf;
}

void
free_bitfield(struct bitfield *bits)
{
    if (bits != NULL)
        free(bits->bits);
    free(bits);
}

bool
bf_contains(struct bitfield *a, struct bitfield *b)
{
    int i;

    if ((a == NULL) || (b == NULL))
        return false;
    if (a->depth < b->depth)
        return false;
    for (i = 0; i < b->depth; i++) {
        if ((a->bits[i] & b->bits[i]) != b->bits[i])
            return false;
    }
    return true;
}

struct bitfield *
bf_subtract(struct bitfield *a, struct bitfield *b)
{
    /* assumption is that both are in normalized form */
    int i;
    int mindepth = a->depth > b->depth ? b->depth : a->depth;
    struct bitfield *nbf = safe_calloc(sizeof(struct bitfield), 1);
    nbf->bits = safe_calloc(sizeof(bitpattern), a->depth);
    nbf->depth = a->depth;
    
    for (i = 0; i < mindepth; i++) {
        nbf->bits[i] = a->bits[i] & ~(a->bits[i] & b->bits[i]);
    }
    for (;i < a->depth ; i++ ) {
        nbf->bits[i] = a->bits[i];
    }
    for (;i < b->depth ; i++ ) {
        nbf->bits[i] = b->bits[i];
    }
    return bf_normalize(nbf);
}

struct bitfield *
bf_normalize(struct bitfield *bf)
{
    /* pack down the bits depthwise*/
    if (bf != NULL) {
        int i,j;
        bitpattern *tbits = safe_calloc(sizeof(bitpattern), bf->depth);
        bitpattern t;
        for (i = 0; i < bf->depth; i++) {
            t = 0;
            for (j = 0; j < bf->depth; j++) {
                t |= bf->bits[j];
            }
            tbits[i] = t;
            if (t != 0) {
                for (j = 0; (t != 0) && (j < bf->depth); j++) {
                    bitpattern p;
                    
                    p = ~(t & bf->bits[j]);
                    t &= p;
                    bf->bits[j] &= p;
                }
            } else {
                /* we have less depth than we started with */
                bf->depth = i;
                break;
            }
        }
        free(bf->bits);
        bf->bits = tbits;
    }
    return bf;
}

UChar *
make_alphabet(const UChar *source)
{
    UChar *dest;
    int sourcelen = u_strlen(source);
    int x, y;

    dest = safe_calloc(sourcelen, sizeof(UChar));

    u_strcpy(dest, source);

    /* Very simple ripple sort as these are very short strings */
    /* No advantage in using qsort() or similar */
    for (x = 0; x < sourcelen - 1; x++) {
        for (y = x + 1; y < sourcelen ; y++) {
            if (dest[x] > dest[y]) {
                UChar temp;

                temp = dest[x];
                dest[x] = dest[y];
                dest[y] = temp;
            }
        }
    }

    x = y = 0;
    while (dest[x]) {
        if (dest[x] != dest[y])
            dest[++y] = dest[x];
        x++;
    }
    dest[++y] = dest[x];
    return dest;
}
