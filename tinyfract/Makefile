.PHONY: all clean

CC=gcc-3.0
CFLAGS=-Wall -O2 -DDEBUG -DVERSION_INFO=\"0.1\" -DBUILD_INFO=\"1\" -DBUILD_DATE="\"$(shell date)\"" -DBUILD_HOST="\"$(shell hostname -f)\""
LD=gcc-3.0
LDFLAGS=-ldl
RM=/bin/rm

all: tinyfract 
clean:
	$(RM) -f *.o core tinyfract

tinyfract: main.o 
	$(LD) $(LDFLAGS) -o $@ $^ 

