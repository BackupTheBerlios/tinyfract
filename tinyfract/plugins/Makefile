.PHONY: all clean

CC=gcc-3.0
CFLAGS=-O3 -fPIC -DDEBUG
LD=gcc-3.0
LDFLAGS=
RM=/bin/rm


all: aa.so lambda.so mandelbrot.so x11.so render_dumb.so render_recurse2.so
clean:
	$(RM) -f *.o *.so

aa.o: aa.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
aa.so: aa.o
	$(LD) $(LDFLAGS) -o $@ -shared -laa $^ 

lambda.o: lambda.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
lambda.so: lambda.o
	$(LD) $(LDFLAGS) -o $@ -shared $^ 

mandelbrot.o: mandelbrot.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
mandelbrot.so: mandelbrot.o
	$(LD) $(LDFLAGS) -o $@ -shared $^ 

x11.o: x11.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
x11.so: x11.o
	$(LD) $(LDFLAGS) -o $@ -shared -L/usr/X11R6/lib -lXt -lXaw $^ 

render_dumb.o: render_dumb.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
render_dumb.so: render_dumb.o
	$(LD) $(LDFLAGS) -o $@ -shared $^ 

render_recurse2.o: render_recurse2.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
render_recurse2.so: render_recurse2.o
	$(LD) $(LDFALGS) -o $@ -shared $^
