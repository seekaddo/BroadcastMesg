#*******************************************************************************
#                                                                              *
#     Author:      G. Leber                                                    *
#                  Technische Universitaet Wien                                *
#                  Institut fuer Technische Informatik E182/1                  *
#                  Treitlstrasse 3                                             *
#                  1040 Wien                                                   *
#                  Tel.: (0222) 58801 / 8176                                   *
#                  E-Mail: guenther@vmars.tuwien.ac.at                         *
#     File:        Makefile						       *
#     Version:     1.9							       *
#     Date:        6/27/94						       *
#                                                                              *
#******************************************************************************/
# Makefile ported by bernd into the 21st century. and CFLAGS contains the ones
# which must by used by students

PACKAGE := sem182

LIB := /usr/local/lib
INCLUDE	:= /usr/local/include
SRC := /usr/local/src
MAN3 := /usr/local/man/man3
CIS := ../../CIS/lu
ARCHIVES := $(foreach EXT,zip tar.gz,$(PACKAGE).$(EXT))

OBJS := sem182.o msem182.o

ARCHIVE_SOURCES := \
	doxygen.dcf \
	Makefile \
	msem182.3 \
	msem182.c \
	msem182.h \
	sem182.3 \
	sem182.c \
	sem182.h

CC := gcc
CFLAGS := -Wall -Werror -Wextra -Wstrict-prototypes -pedantic -fno-common -std=gnu11 -O3
CPPFLAGS := -I.

INSTALL	:= install
CD := cd
CP := cp
RM := rm -f
TAR := tar
ZIP := zip
AR := ar
RANLIB := ranlib
LN := ln

all: libsem182.a

libsem182.a: $(OBJS) Makefile
	$(AR) cru libsem182.a $?
	$(RANLIB) libsem182.a

.PHONY: archs
archs: $(PACKAGE).zip $(PACKAGE).tar.gz

.PHONY: clean
clean:
	$(RM) libsem182.a $(OBJS)

.PHONY: distclean
distclean: clean
	$(RM) $(PACKAGE).zip $(PACKAGE).tar.gz

.PHONY: install
install: $(INCLUDE)/sem182.h  $(INCLUDE)/msem182.h $(LIB)/libsem182.a \
	$(MAN3)/sem182.3 $(MAN3)/msem182.3 $(SRC)/$(PACKAGE).tar.gz \
	$(SRC)/$(PACKAGE).zip Makefile

.PHONY: cis
cis: $(CIS)/sem182.tar.gz Makefile

$(CIS)/$(PACKAGE).tar.gz: $(PACKAGE).tar.gz Makefile
	$(CP) $(PACKAGE).tar.gz $(CIS)

$(INCLUDE)/sem182.h: sem182.h Makefile
	$(INSTALL) -c -m 644 sem182.h $(INCLUDE)

$(PACKAGE).zip: $(ARCHIVE_SOURCES)
	$(CD) ..  && $(ZIP) -r $(PACKAGE)/$@ $(foreach src,$^,$(PACKAGE)/$(src))

$(PACKAGE).tar.gz: $(ARCHIVE_SOURCES)
	$(CD) .. && $(TAR) --dereference --gzip --create --file $(PACKAGE)/$@ $(foreach src,$^,$(PACKAGE)/$(src))

$(SRC)/$(PACKAGE).tar.gz: $(PACKAGE).tar.gz Makefile
	$(INSTALL) -c -m 644 $(PACKAGE).tar.gz $(SRC)

$(SRC)/$(PACKAGE).zip: $(PACKAGE).zip Makefile
	$(INSTALL) -c -m 644 $(PACKAGE).zip $(SRC)

$(INCLUDE)/msem182.h: msem182.h Makefile
	$(INSTALL) -c -m 644 msem182.h $(INCLUDE)

$(LIB)/libsem182.a: libsem182.a Makefile
	$(INSTALL) -c -m 644 libsem182.a $(LIB)

$(MAN3)/sem182.3: sem182.3 Makefile
	$(INSTALL) -c -m 644 sem182.3 $(MAN3)
	$(RM) $(MAN3)/P.3
	$(RM) $(MAN3)/V.3
	$(RM) $(MAN3)/seminit.3
	$(RM) $(MAN3)/semgrab.3
	$(RM) $(MAN3)/semrm.3
	$(LN) $(MAN3)/sem182.3 $(MAN3)/P.3
	$(LN) $(MAN3)/sem182.3 $(MAN3)/V.3
	$(LN) $(MAN3)/sem182.3 $(MAN3)/seminit.3
	$(LN) $(MAN3)/sem182.3 $(MAN3)/semgrab.3
	$(LN) $(MAN3)/sem182.3 $(MAN3)/semrm.3

$(MAN3)/msem182.3: msem182.3 Makefile
	$(INSTALL) -c -m 644 msem182.3 $(MAN3)
	$(RM) $(MAN3)/mP.3
	$(RM) $(MAN3)/mV.3
	$(RM) $(MAN3)/mseminit.3
	$(RM) $(MAN3)/msemgrab.3
	$(RM) $(MAN3)/msemrm.3
	$(LN) $(MAN3)/msem182.3 $(MAN3)/mP.3
	$(LN) $(MAN3)/msem182.3 $(MAN3)/mV.3
	$(LN) $(MAN3)/msem182.3 $(MAN3)/mseminit.3
	$(LN) $(MAN3)/msem182.3 $(MAN3)/msemgrab.3
	$(LN) $(MAN3)/msem182.3 $(MAN3)/msemrm.3

.PHONY: uninstall
uninstall: Makefile
	$(RM) $(INCLUDE)/sem182.h  $(INCLUDE)/msem182.h $(LIB)/libsem182.a \
		$(MAN3)/sem182.3 $(MAN3)/P.3 $(MAN3)/V.3 $(MAN3)/seminit.3 \
		$(MAN3)/semgrab.3 $(MAN3)/semrm.3 $(MAN3)/msem182.3 \
		$(MAN3)/mP.3 $(MAN3)/mV.3 $(MAN3)/mseminit.3 \
		$(MAN3)/msemgrab.3 $(MAN3)/msemrm.3 $(SRC)/$(PACKAGE).tar.gz \
		$(SRC)/$(PACKAGE).zip
