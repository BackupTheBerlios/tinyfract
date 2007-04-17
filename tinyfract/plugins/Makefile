.PHONY: all install clean distclean


## Setup library/plugin compilation.
CFLAGS+=-fPIC


## Command line targets.
all: $(addsuffix .so,$(PLUGINS))

install:
	$(INSTALL) -d -m 755 $(INSTALL_PATH)/lib/tinyfract
	$(INSTALL) -c -m 755 -t $(INSTALL_PATH)/lib/tinyfract $(addsuffix .so,$(PLUGINS))

clean:
	$(RM) -f *.o *.so

distclean:
	$(RM) -f $(addprefix $(INSTALL_PATH)/share/tinyfract/,$(addsuffix .so,$(PLUGINS)))



## Fractal plugins.
fractal_lambda.o: fractal_lambda.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
fractal_lambda.so: fractal_lambda.o
	$(LD) $(LDFLAGS) -o $@ -lgmp -shared $^ 

fractal_mandelbrot.o: fractal_mandelbrot.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
fractal_mandelbrot.so: fractal_mandelbrot.o
	$(LD) $(LDFLAGS) -o $@ -lgmp -shared $^ 


## Output plugins.
output_aa.o: output_aa.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
output_aa.so: output_aa.o
	$(LD) $(LDFLAGS) -o $@ -shared -laa $^ 

output_x11.o: output_x11.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
output_x11.so: output_x11.o
	$(LD) $(LDFLAGS) -o $@ -shared -L/usr/X11R6/lib -lXt -lXaw $^ 

output_png.o: output_png.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
output_png.so: output_png.o
	$(LD) $(LDFLAGS) -o $@ -shared -lgd $^ 

output_mpeg.o: output_mpeg.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
output_mpeg.so: output_mpeg.o
	$(LD) $(LDFLAGS) -o $@ -shared -lavcodec -lavformat $^ 
#output_mpeg.so: output_mpeg.o
#	$(LD) $(LDFLAGS) -o $@ -shared -lavcodec -lavformat $^ 


## Render plugins.
render_dumb.o: render_dumb.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
render_dumb.so: render_dumb.o
	$(LD) $(LDFLAGS) -o $@ -lgmp -shared $^ 

render_recurse.o: render_recurse.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
render_recurse.so: render_recurse.o
	$(LD) $(LDFALGS) -o $@ -lgmp -shared $^

render_pix.o: render_pix.c ../plugin.h ../common.h
	$(CC) $(CFLAGS) -c -o $@ $<
render_pix.so: render_pix.o
	$(LD) $(LDFALGS) -o $@ -lgmp -shared $^

