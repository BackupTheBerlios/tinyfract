.PHONY: all clean

CC=gcc-3.0
CFLAGS=-O3 -fPIC -DDEBUG
LD=gcc-3.0
LDFLAGS=
RM=/bin/rm


all: aa.so lambda.so mandelbrot.so x11.so
clean:
	$(RM) -f *.o *.so

aa.o: aa.c
	$(CC) $(CFLAGS) -c -o $@ $<
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

x11.o: x11.c
	$(CC) $(CFLAGS) -c -o $@ $<
x11.so: x11.o
	$(LD) $(LDFLAGS) -o $@ -shared -L/usr/X11R6/lib -lXt -lXaw $^ 


