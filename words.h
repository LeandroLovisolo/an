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

#ifndef WORDS_H
#define WORDS_H

#include "bitfield.h"

#define LONGEST_WORD 128

struct word {
    char *utf8_form;
    int length;
    struct bitfield *bits;
};

struct wordlist {
    struct word *word;
    struct wordlist *next;
};

void free_word(struct word *word);

void add_word(struct wordlist **words, UChar *alphabet,
              struct bitfield *masterbits, int maxlen, int minlen,
              const char *word);

void load_words(struct wordlist **words, UChar *alphabet,
                struct bitfield *masterbits, int maxlen, int minlen,
                const char *filename);

struct wordlist *push_wordstack(struct wordlist *stack, struct word* word);

struct wordlist *pop_wordstack(struct wordlist *stack);

void print_wordstack(struct wordlist *stack);

#endif /* WORDS_H */
