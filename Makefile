##
## @file Makefile
## Simulating Sender/Receiver message - broadcasting with shared memories
## Beispiel 3
##
## @author Dennis Addo ic16b026@technikum-wien.at
## @author Robert Niedermayer ic16b089@technikum-wien.at
## @date 15/05/2017.
##
## @version 1.0 $
##
##
## URL: $HeadURL: https://cis.technikum-wien.at/documents/bic/2/bes/semesterplan/lu/beispiel3.html $
##
## Last Modified: $Author: Dennis $
##

##
## ------------------------------------------------------------- variables --
##


CC=gcc
CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes \
		-Wwrite-strings -fno-common -g -O3 -std=gnu11
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen
LIBSRC=./sem182/sem182.c ./sem182/sem182.h

OBJECTS1=sender.o sharedlib.o sem182.o
OBJECTS2=receiver.o sharedlib.o sem182.o
IPS=ipcs


EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##


%.o: %.c
	$(CC) $(CFLAGS) -c $<

##
## --------------------------------------------------------------- targets --
##

.PHONY: all sender empfaenger
all: sender empfaenger
#
#sem182.o:
#	$(CC) $(LIBSRC) -o $@

sender: $(OBJECTS1)
	$(CC) $^ -o $@ $(LIBSRC)

empfaenger: $(OBJECTS2)
	$(CC) $^ -o $@ $(LIBSRC)


.SILENT: clean freeshm
.PHONY: clean freeshm testall

##test all with force
testall:
	/usr/local/bin/test_sender_empfaenger.sh -f


clean:
	$(RM) *.o *.d *~ sender empfaenger

## A simple bash loop to search for the user and delete the sharedmemory and semaphores
## created by the specified user. It works on Annuminas, Mac OSx with GNU Make 3.82
freeshm:
	for id in `ipcs -m | egrep "0x[0-9a-f]+ [0-9]+" | grep $$(whoami) | cut -f2 -d" "` ; \
    do \
        ipcrm -m $$id; \
    done

	for id in `ipcs -s | egrep "0x[0-9a-f]+ [0-9]+" | grep $$(whoami) | cut -f2 -d" "` ; \
    do \
       ipcrm -s $$id; \
    done



	echo "After cleaning up ---------->>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>---------------->>>"
	$(IPS)

.PHONY: distclean

distclean: clean
	$(RM) -r doc

doc: html pdf


.PHONY: html

html:
	$(DOXYGEN) doxygen.dcf


pdf: html
	$(CD) doc/pdf && \
	$(MV) refman.tex refman_save.tex && \
	$(GREP) -v $(EXCLUDE_PATTERN) refman_save.tex > refman.tex && \
	$(RM) refman_save.tex && \
	make && \
	$(MV) refman.pdf refman.save && \
	$(RM) *.pdf *.html *.tex *.aux *.sty *.log *.eps *.out *.ind *.idx \
	      *.ilg *.toc *.tps Makefile && \
	$(MV) refman.save refman.pdf

##
## ---------------------------------------------------------- dependencies --
##

## As OBJECTS1 and OBJECTS2

##
## =================================================================== eof ==
##
