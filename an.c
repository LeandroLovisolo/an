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

#include <unicode/ustring.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include "bitfield.h"
#include "unicode.h"
#include "words.h"
#include "malloc.h"
#include "an.h"

static void
find_anagrams(struct wordlist *words, struct wordlist *stack,
              struct bitfield *bits, int maxwords, int length, int *maxtotal)
{
    struct bitfield *remaining_bits = NULL;
    struct wordlist *w = words;

    if (maxwords <= 0)
        return;

    if (*maxtotal <= 0)
        return;

    /* printf("word depth = %d, length = %d\n", INT_MAX-maxwords, length); */

    while (w != NULL) {
        if (length >= w->word->length) {
            if (bf_contains(bits, w->word->bits)) {
                int newlength = length - w->word->length;

                /* printf(" considering %s\n", w->word->utf8_form); */
                stack = push_wordstack(stack, w->word);
                if (newlength > 0) {
                    remaining_bits = bf_subtract(bits, w->word->bits);
                    find_anagrams(w, stack, remaining_bits,
                                  maxwords - 1, newlength, maxtotal);
                    free_bitfield(remaining_bits);
                } else {
                    if (*maxtotal > 0) {
                        print_wordstack(stack);
                        fputs("\n", stdout);
                        (*maxtotal)--;
                    }
                }
                stack = pop_wordstack(stack);
            }
        }
        w = w->next;
    }
}


static void
print_help(const char *progname)
{
    const char *helptext[] = {
        "-c, --contain PHRASE  print anagrams containing PHRASE",
        "-d, --dict DICTIONARY search DICTIONARY for words",
        "-l, --length WORDS    find anagrams with up to WORDS number of words",
        "-m, --minimum WORDLEN only use words at least WORDLEN long",
        "-n, --number NUM      print a maximum of NUM anagrams",
        "-w, --words           print words that PHRASE letters make",
        "-t, --test ANAG       test if ANAG can be made with PHRASE",
        "-u, --used PHRASE     flag PHRASE letters as already used",
        "-h, --help            display this help and exit",
        "-v, --version         output version information and exit",
        NULL
    };
    const char **h = helptext;
        
    printf("Usage: %s [options] PHRASE\n\n", progname);
    while (*h) {
        printf("  %s\n", *h);
        h++;
    }
}

int
main(int argc, char **argv)
{
    char *dictionary = NULL;
    char *contains = NULL;
    struct word phrase, contain;
    struct bitfield *temp_bf;
    UChar *internal, *alphabet;
    struct wordlist *words = NULL, *stack = NULL, *w;
    int maxlen, minlen = 0;
    int maxwords = INT_MAX, maxtotal = INT_MAX;
    int opt, longindex;
    bool just_test = false, include_contains = false, just_words = false;
    const struct option long_opts[] = {
        {"contain", required_argument, 0, 'c'},
        {"dict", required_argument, 0, 'd'},
        {"length", required_argument, 0, 'l'},
        {"minimum", required_argument, 0, 'm'},
        {"number", required_argument, 0, 'n'},
        {"words", no_argument, 0, 'w'},
        {"help", no_argument, 0, 'h'},
        {"used", required_argument, 0, 'u'},
        {"test", required_argument, 0, 't'},
        {"version", no_argument, 0, 'v'},
        {0, 0, 0, 0}
    };

    memset(&phrase, 0, sizeof(phrase));
    memset(&contain, 0, sizeof(contain));

    /* parse arguments */

    while ((opt = getopt_long (argc, argv, "c:d:hl:m:n:t:u:vw",
                               long_opts, &longindex)) != -1) {
        switch (opt) {
        case 'c':
            contains = safe_strdup(optarg);
            just_test = false;
            include_contains = true;
            break;
        case 'd':
            dictionary = safe_strdup(optarg);
            break;
        case 'h':
        case '?':
            print_help(argv[0]);
            exit(0);
        case 'l':
            maxwords = atoi(optarg);
            break;
        case 'm':
            minlen = atoi(optarg);
            break;
        case 'n':
            maxtotal = atoi(optarg);
            break;
        case 't':
            contains = safe_strdup(optarg);
            just_test = true;
            break;
        case 'u':
            contains = safe_strdup(optarg);
            include_contains = false;
            break;
        case 'v':
            printf("%s\n", VERSION);
            exit(0);
        case 'w':
            just_words = true;
            break;
        default:
            fprintf(stderr, "Unexpected option %c\n", optopt);
            exit(99);
        }
    }

    if (argc - optind != 1) {
        fprintf(stderr, "%s: incorrect number of arguments\n", argv[0]);
        print_help(argv[0]);
        exit(1);
    }

    phrase.utf8_form = safe_strdup(argv[optind]);

    internal = utf8tointernal(phrase.utf8_form);

    maxlen = phrase.length = u_strlen(internal);

    alphabet = make_alphabet(internal);
    if (u_strlen(alphabet) > BITFIELD_MAX_LENGTH) {
        fprintf(stderr, "The phrase contains too many unique letters.\n");
        exit(1);
    }

    phrase.bits = make_bitfield(internal, alphabet);
    free(internal);

    if (contains) {
        contain.utf8_form = contains;
        internal = utf8tointernal(contains);
        contain.length = u_strlen(internal);
        contain.bits = make_bitfield(internal, alphabet);
        if ((contain.bits == NULL) || !bf_contains(phrase.bits, contain.bits)) {
            printf("Phrase '%s' does not contain '%s'\n",
                   phrase.utf8_form, contain.utf8_form);
            exit(1);
        }
        if (include_contains)
            stack = push_wordstack(stack, &contain);
        temp_bf = bf_subtract(phrase.bits, contain.bits);
        free_bitfield(phrase.bits);
        phrase.bits = temp_bf;
        maxlen -= contain.length;
        if (just_test) {
            if (maxlen == 0) {
                printf("%s can be made from %s\n",
                       contain.utf8_form,
                       phrase.utf8_form);
                exit(0);
            } else {
                printf("%s can not be made from %s\n",
                       contain.utf8_form,
                       phrase.utf8_form);
                exit(1);
            }
        }
    }

    if (dictionary == NULL)
        dictionary = safe_strdup(DEFAULT_DICT);

    load_words(&words, alphabet, phrase.bits, maxlen, minlen, dictionary);

    free(alphabet);

    if (just_words) {
        for (w = words; w != NULL; w = w->next) {
            /* printf(" %3d %s\n", w->word->length, w->word->utf8_form); */
            puts(w->word->utf8_form);
        }
    } else
        find_anagrams(words, stack, phrase.bits, maxwords, maxlen, &maxtotal);

    return 0;
}
