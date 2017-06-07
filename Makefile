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


CC=gcc52
CFLAGS=-DDEBUG -Wall -pedantic -Werror -Wextra -Wstrict-prototypes \
		-Wwrite-strings -fno-common -g -O3 -std=gnu11
CP=cp
CD=cd
MV=mv
GREP=grep
DOXYGEN=doxygen

OBJECTS1=sender.o sharedlib.o
OBJECTS2=receiver.o sharedlib.o
SOURCES=receiver.c sender.c sharedlib.c
IPS=ipcs


EXCLUDE_PATTERN=footrulewidth

##
## ----------------------------------------------------------------- rules --
##


%.o: %.c
	$(CC) $(CFLAGS) -c $<

##
##empfaenger.o: receiver.c sharedlib.h
##	$(CC) $(CFLAGS) -c receiver.c
##
##sender.o: sender.c sharedlib.h
##	$(CC) $(CFLAGS) -c sender.c
##
##shared.o: sharedlib.c sharedlib.h
##	$(CC) $(CFLAGS) -c sharedlib.c
##
##empfaenger: empfaenger.o shared.o
##	$(CC) $(CFLAGS) -o empfaenger empfaenger.o shared.o -lsem182
##
##sender: sender.o shared.o
##    	$(CC) $(CFLAGS) -o sender sender.o shared.o -lsem182
##
##
## --------------------------------------------------------------- targets --
##

.PHONY: all
all: sender empfaenger

sender: $(OBJECTS1)
	$(CC) $^ -o $@ -lsem182

empfaenger: $(OBJECTS2)
	$(CC) $^ -o $@ -lsem182


.SILENT: clean freeshm
.PHONY: clean freeshm

##test all with force
testall:
	/usr//local/bin/test_sender_empfaenger.sh -f


clean:
	$(RM) *.o *.d *~ sender empfaenger

## A simple bash loop to search for the user and delete the sharedmemory and semaphores / MQ
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
##
##include $(subst .c,.d,$(SOURCES))
##
##%.d: %.c
##	$(CC) -M $(CFLAGS) $< > $@.$$$$;                  \
##	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
##	rm -f $@.$$$$ ;
##

##
## =================================================================== eof ==
##
