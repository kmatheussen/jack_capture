PREFIX ?= /usr/local
bindir = $(PREFIX)/bin

VERSION=0.9.73

CC=gcc
CPP=g++

# Needed on OSX
CFLAGS += -I/opt/local/include

OPTIMIZE=-O3
#OPTIMIZE=-O0 -g

COMPILEFLAGS=$(CFLAGS) $(OPTIMIZE) -DVERSION=\"$(VERSION)\" -Wall -Wextra -Wno-unused
LINKFLAGS=$(LDFLAGS) -ljack -lsndfile -lm -lpthread -latomic

OS := $(shell uname)
ifneq ($(OS),Darwin)
	LINKFLAGS += -lrt
endif

targets = jack_capture

all: check_dependencies jack_capture

install: $(targets)
	install -d $(DESTDIR)$(bindir)
	install -m755 $(targets) $(DESTDIR)$(bindir)

uninstall:
	rm -f $(DESTDIR)$(bindir)/jack_capture
	-rmdir $(DESTDIR)$(bindir)

check_dependencies:
	@echo
	@echo "Checking dependencies: "
	which bash
	which tr
	which sed
	which install
	which pkg-config
	which $(CC)
	which $(CPP)
	$(CC) $(CFLAGS) -E testsndfile.c >/dev/null
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


jack_capture: setformat.c jack_capture.c vringbuffer.c upwaker.c osc.c Makefile das_config.h config_flags
	$(CC) $(COMPILEFLAGS) jack_capture.c vringbuffer.c upwaker.c osc.c -o jack_capture $(LINKFLAGS) `cat config_flags`


jack_capture_gui2: jack_capture_gui2.cpp
	$(CPP) $(CPPFLAGS) $(OPTIMIZE) jack_capture_gui2.cpp $(LDFLAGS) `pkg-config --libs --cflags gtk+-2.0` -o jack_capture_gui2

config_flags: Makefile das_config.h
	cat das_config.h |grep COMPILEFLAGS|sed s/\\/\\/COMPILEFLAGS//|tr '\n' ' ' >config_flags

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
