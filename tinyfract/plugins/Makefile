.PHONY: all clean

CC=gcc-3.0
CFLAGS=-O3 -fPIC -DDEBUG
LD=gcc-3.0
LDFLAGS=
RM=/bin/rm


all: aa.so lambda.so mandelbrot.so
#output_xlib.so
clean:
	$(RM) -f *.o *.so

aa.o: aa.c
	$(CC) $(CFLAGS) -I.. -c -o $@ $<
aa.so: aa.o
	$(LD) $(LDFLAGS) -o $@ -shared -laa $^ 

lambda.o: lambda.c
	$(CC) $(CFLAGS) -c -o $@ $<
lambda.so: lambda.o
	$(LD) $(LDFLAGS) -o $@ -shared $^ 

mandelbrot.o: mandelbrot.c
	$(CC) $(CFLAGS) -c -o $@ $<
mandelbrot.so: mandelbrot.o
	$(LD) $(LDFLAGS) -o $@ -shared $^ 

output_xlib.o: output_xlib.c
	$(CC) $(CFLAGS) -c -o $@ $<

output_xlib.so: output_xlib.o
	$(LD) $(LDFLAGS) -o $@ -shared -L/usr/X11R6/lib -lXt -lXaw $^ 
