.PHONY: all clean

CC=gcc-3.0
CFLAGS=-Wall -O2 -DDEBUG
LD=gcc-3.0
LDFLAGS=
MAKE=make
RM=/bin/rm

all: tinyfract
	$(MAKE) -C plugins

clean:
	$(RM) -f *.o core tinyfract
	$(MAKE) -C plugins clean

main.o: main.c common.h plugin.h plugin_loader.h
	$(CC) $(CFLAGS) -c -o $@ -DVERSION_INFO=\"0.2\" -DBUILD_INFO=\"2\" -DBUILD_USER="\"$(shell whoami)\"" -DBUILD_DATE="\"$(shell date +%Y-%m-%d,%H:%M)\"" -DBUILD_HOST="\"$(shell hostname -f)\"" $<
	
plugin_loader.o: plugin_loader.c common.h plugin.h plugin_loader.h
	$(CC) $(CFLAGS) -c -o $@ $<

param_parser.o: param_parser.c common.h param_parser.h
	$(CC) $(CFLAGS) -c -o $@ $<

tinyfract: main.o plugin_loader.o param_parser.o
	$(LD) $(LDFLAGS) -lgmp -ldl -o $@ $^ 

existenv: existenv.c
	$(LD) $(LDFLAGS) -o $@ $^
