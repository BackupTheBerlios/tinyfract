.PHONY: all clean

CC=gcc
CFLAGS=-O3 -fPIC
LD=gcc
LDFLAGS=
RM=/bin/rm


all: mandelbrot.so 
clean:
	$(RM) -f *.o *.so


mandelbrot.o: mandelbrot.c
	$(CC) $(CFLAGS) -c -o $@ $>
mandelbrot.so: mandelbrot.o 
	$(LD) $(LDFLAGS) -o $@ $^ 


