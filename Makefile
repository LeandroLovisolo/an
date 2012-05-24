#    an - Anagram generator
#    Copyright (C) 2012 Paul Martin <pm@debian.org>
#
#    This program is free software; you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation; either version 2 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License along
#    with this program; if not, write to the Free Software Foundation, Inc.,
#    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.


INSTALLDIR := $(DESTDIR)/usr/games
MANDIR := $(DESTDIR)/usr/share/man/man6

CC:=gcc
INSTALL:=install

CFLAGS += $(shell icu-config --cflags)
CPPFLAGS += $(shell icu-config --cppflags) -D_BSD_SOURCE=1 -D_GNU_SOURCE=1
LDFLAGS += $(shell icu-config --ldflags)

BIN=an
MAN=an.6
OBJS=an.o bitfield.o malloc.o unicode.o words.o
HEADERS=an.h bitfield.h malloc.h unicode.h words.h


all:	$(BIN)

$(OBJS): $(HEADERS)

$(BIN):	$(OBJS)

$(MAN):

install:	$(BIN)
	$(INSTALL) -m 755 $(BIN) $(INSTALLDIR)/$(BIN)
	$(INSTALL) -m 644 $(MAN) $(MANDIR)/$(MAN)

clean:
	rm -f *.o $(BIN)

distclean:	clean
	rm -f *~
