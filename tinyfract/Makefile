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

main.o: main.c common.h plugin_loader.h
	$(CC) $(CFLAGS) -c -o $@ -DVERSION_INFO=\"0.1\" -DBUILD_INFO=\"1\" -DBUILD_DATE="\"$(shell date)\"" -DBUILD_HOST="\"$(shell hostname -f)\"" $<
	
plugin_loader.o: plugin_loader.c plugin_loader.h
	$(CC) $(CFLAGS) -c -o $@ $<

tinyfract: main.o plugin_loader.o 
	$(LD) $(LDFLAGS) -ldl -o $@ $^ 

