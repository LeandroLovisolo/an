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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unicode/ustring.h>
#include "words.h"
#include "unicode.h"
#include "bitfield.h"
#include "malloc.h"

/* Wordlist is a simple linked list, ordered in decreasing word length */

void
free_word(struct word *word)
{
    if (word != NULL) {
        free(word->utf8_form);
        free_bitfield(word->bits);
        free(word);
    }
}

static void
insert_word(struct wordlist **words, struct word *newword)
{
    struct wordlist *entry, *p, *oldp;
    if (newword == NULL)
        return;
    entry = safe_calloc(sizeof(struct wordlist), 1);
    entry->word = newword;
    if (*words == NULL) {
        *words = entry;
        return;
    }
    p = *words;
    oldp = NULL;
    while (p) {
        if (p->word->length <= newword->length) {
            if (oldp == NULL) {
                *words = entry;
            } else {
                oldp->next = entry;
            }
            entry->next = p;
            return;
        }
        oldp = p;
        p = p->next;
    }
    oldp->next = entry;
}

void
add_word(struct wordlist **words, UChar *alphabet, struct bitfield *masterbits,
         int maxlen, int minlen, const char *word)
{
    UChar *internal;
    struct word *newword = safe_calloc(sizeof(struct word), 1);
    
    internal = utf8tointernal(word);
    if (internal == NULL) {
        free(newword);
        return;
    }

    newword->length = u_strlen(internal);
    if ((newword->length > maxlen) || (newword->length < minlen)) {
        free(internal);
        free(newword);
        return;
    }

    newword->bits = make_bitfield(internal, alphabet);
    free(internal);

    if (newword->bits == NULL) {
        /* word not contained totally in alphabet */
        free(newword);
        return;
    }
    if (bf_contains(masterbits, newword->bits)) {
        newword->utf8_form = safe_strdup(word);
        insert_word(words, newword);
    } else {
        free_word(newword);
    }

}

void
load_words(struct wordlist **words, UChar *alphabet,
           struct bitfield *masterbits, int maxlen, int minlen,
           const char *filename)
{
    FILE *dict;
    char line[LONGEST_WORD];

    dict = fopen(filename, "r");
    if (dict == NULL) {
        fprintf(stderr, "Cannot open %s for reading.\n", filename);
        exit(99);
    }
    while (fgets(line, LONGEST_WORD, dict) != NULL) {
        char *l = line + strlen(line) - 1;
        while ((l > line) && ((*l == '\r') || (*l == '\n'))) {
            *l = '\0';
            l--;
        }
        if (line[0])
            add_word(words, alphabet, masterbits, maxlen, minlen, line);
    }
    fclose(dict);
}

/* A word stack is wordlist used for storing intermediate results */

struct wordlist *
push_wordstack(struct wordlist *stack, struct word* word)
{
    struct wordlist *entry;
    if (word == NULL)
        return NULL;
    entry = safe_calloc(sizeof(struct wordlist), 1);
    entry->word = word;
    entry->next = stack;
    return entry;
}

struct wordlist *
pop_wordstack(struct wordlist *stack)
{
    struct wordlist *entry;
    entry = stack->next;
    free(stack);
    return entry;
}

void
print_wordstack(struct wordlist *stack)
{
    if (stack == NULL) 
        return;
    if (stack->next != NULL) {
        print_wordstack(stack->next);
        fputs(" ", stdout);
    }
    fputs(stack->word->utf8_form, stdout);
}
    
