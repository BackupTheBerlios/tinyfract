.PHONY: all clean

CC=gcc
CFLAGS=-Wall -O2 -DDEBUG
LD=gcc
LDFLAGS=
MAKE=make
RM=/bin/rm

all: tinyfract
	$(MAKE) -C plugins

clean:
	$(RM) -f *.o core tinyfract
	$(MAKE) -C plugins clean

main.o: main.c common.h plugin.h plugin_loader.h
	$(CC) $(CFLAGS) -c -o $@ -DVERSION_INFO=\"0.2\" -DBUILD_INFO=\"2\" -DBUILD_DATE="\"$(shell date)\"" -DBUILD_HOST="\"$(shell hostname -f)\"" $<
	
plugin_loader.o: plugin_loader.c common.h plugin.h plugin_loader.h
	$(CC) $(CFLAGS) -c -o $@ $<

tinyfract: main.o plugin_loader.o 
	$(LD) $(LDFLAGS) -ldl -o $@ $^ 

