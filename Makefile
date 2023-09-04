# $Id: Makefile,v 1.10 2023/02/12 18:02:46 leavens Exp leavens $
# Makefile for lexer in COP 3402

TARGET = lexer
CC = gcc
CFLAGS = -g -std=c17 -Wall
RM = rm -f
SUBMISSIONZIPFILE = submission.zip
TESTSZIPFILE = ~/WWW/COP3402/homeworks/hw2-tests.zip
PROVIDEDFILES = token.[ch] lexer_output.[ch] utilities.[ch] lexer.h
ZIP = zip -9

$(TARGET): *.c *.h
	$(CC) $(CFLAGS) -o $(TARGET) `cat sources.txt`

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<

sources.txt:
	echo *.c >sources.txt

clean:
	$(RM) *~ *.o *.myo '#'*
	$(RM) $(TARGET).exe
	$(RM) *.stackdump
	$(RM) $(SUBMISSIONZIPFILE)

# make file.out from file.txt by running the $(TARGET)
%.out: %.pl0 $(TARGET)
	./$(TARGET) $< > $@ 2>&1

%.myo: %.pl0 $(TARGET)
	./$(TARGET) $< > $@ 2>&1

check-outputs: $(TARGET) hw2-*test*.pl0
	DIFFS=0; \
	for f in `echo hw2-test*.pl0 hw2-errtest*.pl0 | sed -e 's/\\.pl0//g'`; \
	do \
		echo running "$$f.pl0"; \
		./$(TARGET) "$$f.pl0" >"$$f.myo" 2>&1; \
		diff -w -B "$$f.out" "$$f.myo" && echo 'passed!' || DIFFS=1; \
	done; \
	if test 0 = $$DIFFS; \
	then \
		echo 'All tests passed!'; \
	else \
		echo 'Test(s) failed!'; \
	fi

create-outputs: $(TARGET) hw2-*test*.pl0
	for f in `echo hw2-*test*.pl0 | sed -e 's/\\.pl0//g'`; \
	do \
		echo running "$$f.pl0"; \
		./$(TARGET) "$$f.pl0" >"$$f.out" 2>&1; \
	done

$(SUBMISSIONZIPFILE): sources.txt *.c *.h *.myo
	$(ZIP) $(SUBMISSIONZIPFILE) sources.txt *.c *.h *.myo

hw2-tests.zip: create-outputs $(TESTSZIPFILE)

$(TESTSZIPFILE): hw2-*test*.pl0 Makefile $(PROVIDEDFILES)
	$(RM) $(TESTSZIPFILE)
	chmod 755 hw2-*test*.pl0 hw2-*test*.out Makefile $(PROVIDEDFILES)
	$(ZIP) $(TESTSZIPFILE) hw2-*test*.pl0 hw2-*test*.out Makefile $(PROVIDEDFILES)
