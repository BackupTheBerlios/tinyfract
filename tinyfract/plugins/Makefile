.PHONY: all clean

CC=gcc-3.0
CFLAGS=-O3 -fPIC -DDEBUG
LD=gcc-3.0
LDFLAGS=
RM=/bin/rm


all: fractal_lambda.so fractal_mandelbrot.so output_aa.so output_x11.so render_dumb.so render_recurse.so
clean:
	$(RM) -f *.o *.so

fractal_lambda.o: fractal_lambda.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
fractal_lambda.so: fractal_lambda.o
	$(LD) $(LDFLAGS) -o $@ -shared $^ 

fractal_mandelbrot.o: fractal_mandelbrot.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
fractal_mandelbrot.so: fractal_mandelbrot.o
	$(LD) $(LDFLAGS) -o $@ -shared $^ 

output_aa.o: output_aa.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
output_aa.so: output_aa.o
	$(LD) $(LDFLAGS) -o $@ -shared -laa $^ 

output_x11.o: output_x11.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
output_x11.so: output_x11.o
	$(LD) $(LDFLAGS) -o $@ -shared -L/usr/X11R6/lib -lXt -lXaw $^ 

render_dumb.o: render_dumb.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
render_dumb.so: render_dumb.o
	$(LD) $(LDFLAGS) -o $@ -shared $^ 

render_recurse.o: render_recurse.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
render_recurse.so: render_recurse.o
	$(LD) $(LDFALGS) -o $@ -shared $^

