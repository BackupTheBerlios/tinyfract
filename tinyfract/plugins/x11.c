#include <aalib.h>
#include <stdlib.h>
#include <stdio.h>
#include "../common.h"
#include "../plugin.h"


typedef struct
{
	XGCValues gcpxval;
	GC        gcpx;
	GC 	  gc;
	Display   *dpy;
	XEvent    event;
	Window    win;
 	Pixmap    pxmap;
} x11_t;	

/* Constructor and destructor for asciiart output. */
static x11_t* constructor_x11(const view_dimension_t dimension)
{
	x11_t* context;
	
	/* Get memory for the output context. */
	if (!(context=malloc(sizeof(x11_t)))) return NULL;

	/* Initialize the viewport. */


	/* Open the display */
	dpy=XOpenDisplay(NULL);
	
	/* Get some colors */

	XColor mycolor;	
	mycolor.red=65535;
	mycolor.green=0;
	mycolor.blue=0;
	mycolor.flags=DoRed|DoGreen|DoBlue;

	XAllocColor(dpy,DefaultColormap(context->dpy,DefaultScreen(dpy)),&mycolor);
	blackColor=mycolor.pixel;
		
	//blackColor=BlackPixel(dpy,DefaultScreen(dpy));
	whiteColor=WhitePixel(context->dpy,DefaultScreen(dpy));

	/* Create the window */
	win=XCreateSimpleWindow(context->dpy,DefaultRootWindow(dpy),0,0,200,100,0,whiteColor,whiteColor);
	/* Pixmap */
	pxmap=XCreatePixmap(context->dpy,DefaultRootWindow(dpy),200,100,16);

	
	/* We want to get MapNotify events */
	XSelectInput(context->dpy,win,StructureNotifyMask);

	/* "Map" the window (that is, make it appear on the screen) */
	XMapWindow(context->dpy,win);

	/* Create a "Graphics Context" */
	gc=XCreateGC(context->dpy,win,0,NULL);
	
	gcpxval.function=GXset;
	gcpx=XCreateGC(context->dpy,pxmap,GCFunction,&gcpxval);
	

	/* Tell the GC we draw using the white color */
	XSetForeground(context->dpy,gc,blackColor);
	
	
	/* Return the handle. */
	return context;
}

void destructor_x11(x11_t* handle)
{
	x11_close(handle->context_x11);
	free(handle);
}

/* Blit rectangle from pixelbuffer to asciiart viewport. */
void blit_rect_x11(x11_t* handle, const view_position_t position, const view_dimension_t dimension, pixel_value values[])
{
}


/* Fill rectangle in asciiart viewport with color. */
void fill_rect_x11(x11_t* handle, const view_position_t position, const view_dimension_t dimension, const pixel_value value)
{
}

/* Flush asciiart viewport */
void flush_viewport_x11(x11_t* handle)
{
		/* Fill a rectangle */
	XFillRectangle(handle->dpy,pxmap,gcpx,0,0,200,100);
		/* Draw the line */
	XDrawLine(handle->dpy,pxmap,gc,10,60,180,20);
		/* Draw a Rectangle */
	XDrawRectangle (handle->dpy,pxmap,gc,10,10,20,20);
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

