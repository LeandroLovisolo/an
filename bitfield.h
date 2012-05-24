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


#ifndef BITFIELD_H
#define BITFIELD_H

#include <unicode/uchar.h>
#include <stdint.h>
#include <stdbool.h>

#define BITFIELD_MAX_LENGTH 64
typedef uint64_t bitpattern;

struct bitfield {
    int depth;
    bitpattern *bits;
};

struct bitfield * make_bitfield(const UChar *word, const UChar *alphabet);

void free_bitfield(struct bitfield *bits);

bool bf_contains(struct bitfield *a, struct bitfield *b);

struct bitfield * bf_subtract(struct bitfield *a, struct bitfield *b);


struct bitfield * bf_normalize(struct bitfield *bf);


UChar *make_alphabet(const UChar *source);



#endif /* BITFIELD_H */
