
DESTDIR =
prefix = /usr/local
bindir = $(prefix)/bin

VERSION=0.9.61

CC=gcc
CPP=g++

OPTIMIZE=-O3 -mtune=native
#OPTIMIZE=-O0 -g

COMPILEFLAGS=$(OPTIMIZE) -DVERSION=\"$(VERSION)\" -Wall -Wextra -Wno-unused
LINKFLAGS=-ljack -lsndfile -lm -lpthread

targets = jack_capture

# TODO: configure target after check_dependencies:
# #pkg-config --exists liblo
COMPILEFLAGS+=-DHAVE_LIBLO `pkg-config --cflags liblo`
LINKFLAGS+=`pkg-config --libs liblo`

# TODO: configuration option
COMPILEFLAGS+=-DEXEC_HOOKS
# TODO: configuration option
COMPILEFLAGS+=-DSTORE_SYNC

all: check_dependencies jack_capture

install: $(targets)
	mkdir -p $(DESTDIR)$(bindir)
	install -m755 $(targets) $(DESTDIR)$(bindir)

uninstall:
	rm $(DESTDIR)$(bindir)/jack_capture

check_dependencies:
	@echo
	@echo "Checking dependencies: "
	which bash
	which tr
	which install
	which pkg-config
	which $(CC)
	which $(CPP)
	$(CC) -E testsndfile.c >/dev/null
	@echo "All seems good "
	@echo

dist: clean
	rm -fr /tmp/jack_capture-$(VERSION)
	rm -fr jack_capture-$(VERSION)
	mkdir /tmp/jack_capture-$(VERSION)
	cp -a * /tmp/jack_capture-$(VERSION)/
	mv /tmp/jack_capture-$(VERSION) .
	tar cvf jack_capture-$(VERSION).tar jack_capture-$(VERSION)
	gzip jack_capture-$(VERSION).tar
	marcel_upload jack_capture-$(VERSION).tar.gz
	ls -la jack_capture-$(VERSION)
	rm -fr jack_capture-$(VERSION)


jack_capture: setformat.c jack_capture.c vringbuffer.c osc.c Makefile das_config.h config_flags
	$(CC) $(COMPILEFLAGS) jack_capture.c vringbuffer.c osc.c -o jack_capture $(LINKFLAGS) `cat config_flags`


jack_capture_gui2: jack_capture_gui2.cpp
	$(CPP) $(OPTIMIZE) jack_capture_gui2.cpp `pkg-config --libs --cflags gtk+-2.0` -o jack_capture_gui2

config_flags: Makefile das_config.h
	cat das_config.h |grep COMPILEFLAGS|sed s/\\/\\/COMPILEFLAGS// >config_flags

das_config.h: gen_das_config_h.sh
	bash gen_das_config_h.sh >das_config.h
	@echo
	@echo "jack_capture was configured with the following options:"
	@echo "------------------------------"
	@cat das_config.h
	@echo "------------------------------"
	@echo

setformat.c: gen_setformat_c.sh
	bash gen_setformat_c.sh >setformat.c


clean:
	rm -f *~ jack_capture jack_capture_gui2 config_flags *.wav *.flac *.ogg *.mp3 *.au *.aiff *.wavex temp.c* setformat.c* das_config.h* a.out *.gz  *.orig "#*"
