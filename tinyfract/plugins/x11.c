#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include "../common.h"
#include "../plugin.h"


typedef struct
{
	XGCValues gcpxval;
	GC        gcpx;
	GC 	  gc;
	Display*  dpy;
	Window    win;
 	Pixmap    pxmap;
} x11_t;	

/* Constructor and destructor for X11 output. */
static x11_t* constructor_x11(const view_dimension_t dimension)
{
	x11_t* context;
	
	/* Get memory for the output context. */
	if (!(context=malloc(sizeof(x11_t)))) return NULL;

	/* Initialize the viewport. */
	/* Open the display. */
	context->dpy=XOpenDisplay(NULL);
	

	/* Create the window. */
	int bgColor=WhitePixel(context->dpy,DefaultScreen(context->dpy));
	context->win=XCreateSimpleWindow(context->dpy,DefaultRootWindow(context->dpy),
			0,0,200,100,0,bgColor,bgColor);

	/* Create a pixmap as double buffer. */
	context->pxmap=XCreatePixmap(context->dpy,DefaultRootWindow(context->dpy),
			200,100,16);

	/* Create a "Graphics Context" for the window and the pixmap. */
	context->gc=XCreateGC(context->dpy,context->win,0,NULL);
	context->gcpx=XCreateGC(context->dpy,context->pxmap,0,NULL);
	
	/* Return the handle. */
	return context;
}

void destructor_x11(x11_t* handle)
{
	free(handle);
}

/* Blit rectangle from pixelbuffer to X11 viewport. */
void blit_rect_x11(x11_t* handle, const view_position_t position, const view_dimension_t dimension, pixel_value values[])
{
}


/* Fill rectangle in X11 viewport with color. */
void fill_rect_x11(x11_t* handle, const view_position_t position, const view_dimension_t dimension, const pixel_value value)
{
#if 0
	/* Fill a rectangle */
	XFillRectangle(handle->dpy,handle->pxmap,handle->gcpx,0,0,200,100);
		/* Draw the line */
	XDrawLine(handle->dpy,handle->pxmap,handle->gc,10,60,180,20);
		/* Draw a Rectangle */
	XDrawRectangle(handle->dpy,handle->pxmap,handle->gc,10,10,20,20);
#endif

}

/* Put pixel into X11 viewport. */
void put_pixel_x11(x11_t* handle, const view_position_t position, const pixel_value value)
{

	/* Allocate colors */
	XColor mycolor;	
	mycolor.red=value*1000;
	mycolor.green=0;
	mycolor.blue=0;
	mycolor.flags=DoRed|DoGreen|DoBlue;
	XAllocColor(handle->dpy,DefaultColormap(handle->dpy,DefaultScreen(handle->dpy)),&mycolor);
	
	
	/* Set color for next operation. */
	XSetForeground(handle->dpy,handle->gcpx,mycolor.pixel);
	
	/* Put a pixel into the double buffer. */
	XDrawPoint(handle->dpy,handle->pxmap,handle->gcpx,position.x,position.y);
}

/* Flush X11 viewport */
void flush_viewport_x11(x11_t* handle)
{
	XEvent event;

	/* We want to get MapNotify events for our window. */
	XSelectInput(handle->dpy,handle->win,StructureNotifyMask);

	/* Now "map" the window (that is, make it appear on the screen). */
	XMapWindow(handle->dpy,handle->win);

	do
	{
		/* Wait for an event. */
		XNextEvent(handle->dpy,&event);

		switch (event.type)
		{
			case MapNotify:
			case Expose:
			case ButtonPress:
			case ConfigureNotify:
			case MotionNotify:	
			case ButtonRelease:
			default:
				/* Put double buffer onto the window. */
				XCopyArea(handle->dpy,handle->pxmap,handle->win,handle->gc,
						0,0,200,100,0,0);
				/* Send request to the server */
			        XFlush(handle->dpy);
				break;	
		}
		sleep(1);
	}
	while (1);
}


/* Enumerate plugin facilities. */
volatile const plugin_facility_t tinyfract_plugin_facilities[]=
{
	{
		name: "x11",
		type: plugin_facility_output,
		facility:
		{
			output:
			{
				constructor:             (const plugin_output_constructor_t*) &constructor_x11,
				destructor:              (const plugin_output_destructor_t*) &destructor_x11,
				blit_rect_function:      (const plugin_output_blit_rect_function_t*) &blit_rect_x11,
				fill_rect_function:      (const plugin_output_fill_rect_function_t*) &fill_rect_x11,
				flush_viewport_function: (const plugin_output_flush_viewport_function_t*) &flush_viewport_x11,
				put_pixel_function:      (const plugin_output_put_pixel_function_t*) &put_pixel_x11,
			}
		}
	},
	{ plugin_facility_end }
};

