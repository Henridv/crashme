
# unix makefile for crashme program.
MAKEFLAGS	+= -rR --no-print-directory

SRCDIR		= src
BUILDDIR	= build

CFLAGS		= -DPRNG_MT -Wall -std=c99
LDFLAGS		= --cref --error-unresolved-symbols

CC			= $(CROSS_COMPILE)gcc
LD			= $(CROSS_COMPILE)ld

ifneq ($(origin CROSS_COMPILE), undefined)
	CFLAGS	+=	-marm -mabi=aapcs-linux -mno-thumb-interwork -msoft-float \
				-ggdb3 \
				-ffreestanding -fno-common -fno-stack-protector \
				-Wextra -Wcast-qual -Wformat=2 -Winit-self -Winline -Wlogical-op \
				-Wmissing-declarations -Wmissing-prototypes -Wnested-externs -Wpacked \
				-Wredundant-decls -Wshadow -Wstrict-prototypes -Wundef -Wvla -Wwrite-strings \
				-Wno-empty-body -Wno-unused-label -Wno-unused-parameter \
				-Werror=format-extra-args \
				-Werror=implicit-function-declaration -Werror=implicit-int \
				-Werror=init-self -Werror=parentheses -Werror=return-type -Werror=uninitialized
endif

OBJECTS := crashme.new.o pddet.o mt19937ar.o
OBJECTS := $(addprefix $(BUILDDIR)/, $(notdir $(OBJECTS)))

TARGETS := $(BUILDDIR)/crashme.new

all: $(TARGETS)

$(BUILDDIR)/crashme.new: $(OBJECTS)
	@echo "CC $@"
	@$(CC) -o $(BUILDDIR)/crashme.new $(BUILDDIR)/crashme.new.o

$(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
	@echo "CC $@"
	@$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR):
	@mkdir -p $(BUILDDIR)

showdefs:
	$(CC) -dM -E - < /dev/null

clean:
	@rm -f crashme pddet *.o core crashme.txt crashme.zip \
            crashme.tgz crashme_i386.zip *.plg *.ncb *.opt 2> /dev/null
	@rm -f crashme.new 2> /dev/null
	@rm -rf release debug $(BUILDDIR) 2> /dev/null

# create for dist for people without nroff

crashme.txt: crashme.man
	nroff -man crashme.man | col -bx > crashme.txt


DIST_FILES = crashme.man crashme.c crashme.html \
             crashme.vms-opt descrip.mms makefile \
             pddet.c crashme.txt

# These files were in the distribution.
# but they got lost and it isn't clear

# crashme.dsw crashme.dsp pddet.dsp

crashme.zip: $(DIST_FILES)
	zip -D -j crashme.zip $(DIST_FILES)

crashme.tgz: $(DIST_FILES)
	tar cvf - $(DIST_FILES) | gzip -v > crashme.tgz

check:
	nsgmls -s crashme.html

dist: crashme.zip crashme.tgz

#crashme_i386.zip

CRASHME_I386=release/crashme.exe release/pddet.exe

crashme_i386.zip: $(DIST_FILES) $(CRASHME_I386)
	zip -D -j crashme_i386.zip $(DIST_FILES) $(CRASHME_I386)


####

LOG_DIR=logs
# This directory is in Apple Mac Darwin.
# It will fill up with junk and probably really
# slow down things unless you turn off crash reporting
# for crashme.
LOG_DIR2=$$HOME/Library/Logs/DiagnosticReports

showcrashlogs:
	ls $(LOG_DIR)
	if [ -d $(LOG_DIR2) ]; then ls $(LOG_DIR2); fi




ptest1:
	./pddet

ptest2:
	./crashme 100 666 100

ptest3:
	./crashme 100 667 100

ptest4:
	mkdir -p $(LOG_DIR)
	sh -c "CRASHPRNG=RAND;export CRASHPRNG;CRASHLOG=$(LOG_DIR)/crashme-ptest4.log;export CRASHLOG;./crashme 100 666 100"


ptest5:
	./crashme -100 666 100

ptest6:
	@echo This tests alarm clock
	@echo you can also use control-C
	./crashme 0 666 3

ptest7:
	./crashme +256 666 10 3

ptest8:
	./crashme +256 666 10 3 4

ptest9:
	./crashme +256 666 100 3 4

ptest10:
	./crashme +4096 666 100 00:00:05 4

ptest11:
	./crashme +4096 666 250 00:03:00 4


# here are some tests taken from the BAT files done for windows.

test1: test1a test1b test1c

test1a:
	mkdir -p $(LOG_DIR)
	./pddet -examine 16
	./pddet -examine 16 > $(LOG_DIR)/pddet.log

test1b:
	sh -c "CRASHPRNG=MT;export CRASHPRNG;./crashme -64 666 5 -15 3" > $(LOG_DIR)/crashme-prng_mt.log
	sh -c "CRASHPRNG=RAND;export CRASHPRNG;./crashme -64 666 5 -15 3" > $(LOG_DIR)/crashme-prng_rand.log
	sh -c "CRASHPRNG=VNSQ;export CRASHPRNG;./crashme -64 666 5 -15 3" > $(LOG_DIR)/crashme-prng_vnsq.log

test1c: test1ca test1cb test1cc

test1ca:
	sh -c "CRASHPRNG=MT;export CRASHPRNG;CRASHLOG=$(LOG_DIR)/crashme-test1-mt.log;export CRASHLOG;./crashme 8192 666 100 00:00:30 3"

test1cb:
	sh -c "CRASHPRNG=RAND;export CRASHPRNG;CRASHLOG=$(LOG_DIR)/crashme-test1-rand.log;export CRASHLOG;./crashme 8192 666 100 00:00:30 3"


test1cc:
	sh -c "CRASHPRNG=VNSQ;export CRASHPRNG;CRASHLOG=$(LOG_DIR)/crashme-test1-vnsq.log;export CRASHLOG;./crashme 8192 666 100 00:00:30 3"


# Use this to disable all crash report generation for the current user.


report-crash-stop:
	-launchctl unload -w /System/Library/LaunchAgents/com.apple.ReportCrash.plist
	-sudo launchctl unload -w /System/Library/LaunchDaemons/com.apple.ReportCrash.Root.plist

report-crash-start:
	-launchctl load -w /System/Library/LaunchAgents/com.apple.ReportCrash.plist
	-sudo launchctl load -w /System/Library/LaunchDaemons/com.apple.ReportCrash.Root.plist

