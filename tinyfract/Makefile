.PHONY: all clean install distclean

## User settings.
INSTALL_PATH=/usr/local
PLUGINS+=fractal_mandelbrot
PLUGINS+=fractal_lambda
PLUGINS+=output_aa
PLUGINS+=output_png
PLUGINS+=output_x11
PLUGINS+=output_mpeg
PLUGINS+=render_recurse
PLUGINS+=render_pix
PLUGINS+=render_dumb



## Build setup.
TINYFRACT_VERSION=1.2
TINYFRACT_BUILD=jk1

CFLAGS= \
	-std=c99 -Wall -O2 -DDEBUG \
	-DTINYFRACT_PLUGIN_PATH=\"$(INSTALL_PATH)/lib/tinyfract\" \
	-DVERSION_INFO=\"$(TINYFRACT_VERSION)\" \
	-DBUILD_INFO=\"$(TINYFRACT_BUILD)\" \
	-DBUILD_USER="\"$(shell whoami)\"" \
	-DBUILD_DATE="\"$(shell date +%Y-%m-%d,%H:%M)\"" \
	-DBUILD_HOST="\"$(shell hostname -f)\""
LDFLAGS=

CC=gcc
INSTALL=install
LD=gcc
MAKE=make
RM=/bin/rm

export \
	INSTALL_PATH \
	PLUGINS \
	TINYFRACT_VERSION \
	TINYFRACT_BUILD \
	CFLAGS \
	LDFLAGS \
	CC \
	INSTALL \
	LD \
	MAKE \
	RM


## Command line targets.
all: tinyfract
	$(MAKE) -C plugins $@

install:
	$(INSTALL) -d -m 755 $(INSTALL_PATH)/bin
	$(INSTALL) -c -m 755 tinyfract $(INSTALL_PATH)/bin
	$(INSTALL) -c -m 755 gtinyfract.tcl $(INSTALL_PATH)/bin/gtinyfract
	$(MAKE) -C plugins $@

clean:
	$(RM) -f *.o core tinyfract
	$(MAKE) -C plugins $@

distclean:
	$(RM) -f $(INSTALL_PATH)/bin/tinyfract $(INSTALL_PATH)/bin/gtinyfract
	$(MAKE) -C plugins $@
	

## Object files for tinyfract.
main.o: main.c common.h plugin.h plugin_loader.h
	$(CC) $(CFLAGS) -c -o $@ $<
	
plugin_loader.o: plugin_loader.c common.h plugin.h plugin_loader.h
	$(CC) $(CFLAGS) -c -o $@ $<

param_parser.o: param_parser.c common.h param_parser.h
	$(CC) $(CFLAGS) -c -o $@ $<


## tinyfract binary
tinyfract: main.o plugin_loader.o param_parser.o
	$(LD) $(LDFLAGS) -lgmp -ldl -o $@ $^ 

## existenv binary.
existenv: existenv.c
	$(LD) $(LDFLAGS) -o $@ $^

