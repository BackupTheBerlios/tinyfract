.PHONY: all clean

CC=gcc-3.0
CFLAGS=-O3 -fPIC -DDEBUG
LD=gcc-3.0
LDFLAGS=
RM=/bin/rm


#all: aa.so lambda.so mandelbrot.so x11.so render_dumb.so render_recurse.so
all: mandelbrot.so x11.so render_recurse.so test_tinyfract render_dumb.so lambda.so

clean:
	$(RM) -f *.o *.so

test_tinyfract: test_main.c ../plugin_loader.c
	$(CC) $(CFLAGS) -ldl -lgmp -o $@ $^

aa.o: aa.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<

aa.so: aa.o
	$(LD) $(LDFLAGS) -o $@ -shared -laa $^ 

lambda.o: lambda.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
lambda.so: lambda.o
	$(LD) $(LDFLAGS) -o $@ -lgmp -shared $^ 

mandelbrot.o: mandelbrot.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
mandelbrot.so: mandelbrot.o
	$(LD) $(LDFLAGS) -o $@ -lgmp -shared $^ 

x11.o: x11.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
x11.so: x11.o
	$(LD) $(LDFLAGS) -o $@ -shared -L/usr/X11R6/lib -lXt -lXaw $^ 

render_dumb.o: render_dumb.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
render_dumb.so: render_dumb.o
	$(LD) $(LDFLAGS) -o $@ -lgmp -shared $^ 

render_recurse.o: render_recurse.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
render_recurse.so: render_recurse.o
	$(LD) $(LDFALGS) -o $@ -lgmp -shared $^

