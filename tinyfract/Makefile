.PHONY: all clean

CC=gcc-3.0
CFLAGS=-Wall -O2 -DDEBUG -DVERSION_INFO=\"0.1\" -DBUILD_INFO=\"1\" -DBUILD_DATE="\"$(shell date)\"" -DBUILD_HOST="\"$(shell hostname -f)\""
LD=gcc-3.0
LDFLAGS=-ldl
MAKE=make
RM=/bin/rm

all: tinyfract
	$(MAKE) -C plugins

clean:
	$(RM) -f *.o core tinyfract
	$(MAKE) -C plugins clean

	
tinyfract: main.o 
	$(LD) $(LDFLAGS) -o $@ $^ 

