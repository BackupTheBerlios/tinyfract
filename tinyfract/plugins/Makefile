.PHONY: all clean

CC=gcc-3.0
CFLAGS=-O3 -fPIC
LD=gcc-3.0
LDFLAGS=
RM=/bin/rm


all: output_xlib.so 
clean:
	$(RM) -f *.o *.so

mandelbrot.o: mandelbrot.c
	$(CC) $(CFLAGS) -c -o $@ $<
mandelbrot.so: mandelbrot.o
	$(LD) $(LDFLAGS) -o $@ -shared $^

output_xlib.o: output_xlib.c
	$(CC) $(CFLAGS) -c -o $@ $<

output_xlib.so: output_xlib.o
	$(LD) $(LDFLAGS) -o $@ -shared -L/usr/X11R6/lib -lXt -lXaw $^ 
