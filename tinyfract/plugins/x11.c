#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include "../common.h"
#include "../plugin.h"

#define MAX_COLORS 65536
#define COLOR_CEILING 65536
#define iteration_steps 1000

typedef struct
{
	XGCValues gcpxval;
	GC        gcpx;
	GC 	  gc;
	Display*  dpy;
	Window    win;
 	Pixmap    pxmap;
	XColor    colors[MAX_COLORS];
} x11_t;	


/* Color mapping functions. */
float R(int n, int x)
{
        if(x<n/3) return 1-(float)(3*x)/n;
        else if (x>n*2/3) return (float)(3*x)/n-2;
        else return 0;
}

float G(int n, int x)
{
        if(x<n/3) return (float)(3*x)/n;
        else if (x>n*2/3) return 0;
        else return 2-(float)(3*x)/n;
}

float B(int n, int x)
{
        if(x<n/3) return 0;
        else if (x>n*2/3) return 3-(float)(3*x)/n;
        else return (float)(3*x)/n-1;
}

/* Constructor and destructor for X11 output. */
static x11_t* constructor_x11(const view_dimension_t dimension)
{
	x11_t* context;
	int    i;
	
	/* Get memory for the output context. */
	if (!(context=malloc(sizeof(x11_t)))) return NULL;

	/* Initialize the viewport. */
	/* Open the display. */
	context->dpy=XOpenDisplay(NULL);
	

	/* Create the window. */
	int bgColor=WhitePixel(context->dpy,DefaultScreen(context->dpy));
	context->win=XCreateSimpleWindow(context->dpy,DefaultRootWindow(context->dpy),
			0,0,dimension.width,dimension.height,0,bgColor,bgColor);

	/* Create a pixmap as double buffer. */
	context->pxmap=XCreatePixmap(context->dpy,DefaultRootWindow(context->dpy),
			dimension.width,dimension.height,16);

	/* Create a "Graphics Context" for the window and the pixmap. */
	context->gc=XCreateGC(context->dpy,context->win,0,NULL);
	context->gcpx=XCreateGC(context->dpy,context->pxmap,0,NULL);

	/* Allocate colors */
	for(i=0;i<iteration_steps;i++)
	{
		context->colors[i].red=  R(iteration_steps,i)*COLOR_CEILING;
		context->colors[i].green=G(iteration_steps,i)*COLOR_CEILING;
		context->colors[i].blue= B(iteration_steps,i)*COLOR_CEILING;
		context->colors[i].flags=DoRed|DoGreen|DoBlue;
		XAllocColor(context->dpy,DefaultColormap(context->dpy,DefaultScreen(context->dpy)),
			&context->colors[i]);
	}
		
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
	/* Set color for next operation. */
	XSetForeground(handle->dpy,handle->gcpx,handle->colors[value].pixel);
	
	/* Put a pixel into the double buffer. */
	XDrawPoint(handle->dpy,handle->pxmap,handle->gcpx,position.x,position.y);
}

/* Flush X11 viewport */
void flush_viewport_x11(x11_t* handle)
{
	XEvent event;
	XWindowAttributes attributes;
	
	/* We want to get MapNotify events for our window. */
	XSelectInput(handle->dpy,handle->win,StructureNotifyMask);

	/* Now "map" the window (that is, make it appear on the screen). */
	XMapWindow(handle->dpy,handle->win);

	/* Get window size. */
	XGetWindowAttributes(handle->dpy,handle->win,&attributes);
	
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
						0,0,attributes.width,attributes.height,0,0);
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

