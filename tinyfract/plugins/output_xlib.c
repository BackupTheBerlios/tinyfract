 // Written by Ch. Tronche (http://tronche.lri.fr:8000/)
// Copyright by the author. This is unmaintained, no-warranty free software. 
// Please use freely. It is appreciated (but by no means mandatory) to
// acknowledge the author's contribution. Thank you.
// Started on Thu Jun 26 23:29:03 1997

//
// Xlib tutorial: 2nd program
// Make a window appear on the screen and draw a line inside.
// If you don't understand this program, go to
// http://tronche.lri.fr:8000/gui/x/xlib-tutorial/2nd-program-anatomy.html
//

#include <X11/Xlib.h> // Every Xlib program must include this
#include <assert.h>   // I include this to test return values the lazy way
#include <unistd.h>   // So we got the profile for 10 seconds

int main(void)
{
	int      blackColor,whiteColor;
	Display *dpy;
	XEvent   event;
	Window   win;
 	Pixmap   pxmap;

	/* Open the display */
	assert(dpy=XOpenDisplay(NULL));
	
	/* Get some colors */

	XColor mycolor;	
	mycolor.red=65535;
	mycolor.green=0;
	mycolor.blue=0;
	mycolor.flags=DoRed|DoGreen|DoBlue;

	XAllocColor(dpy,DefaultColormap(dpy,DefaultScreen(dpy)),&mycolor);
	blackColor=mycolor.pixel;
		
	//blackColor=BlackPixel(dpy,DefaultScreen(dpy));
	whiteColor=WhitePixel(dpy,DefaultScreen(dpy));

	/* Create the window */
	win=XCreateSimpleWindow(dpy,DefaultRootWindow(dpy),0,0,200,100,0,whiteColor,whiteColor);
	/* Pixmap */
	pxmap=XCreatePixmap(dpy,DefaultRootWindow(dpy),200,100,16);

	
	/* We want to get MapNotify events */
	XSelectInput(dpy,win,StructureNotifyMask);

	/* "Map" the window (that is, make it appear on the screen) */
	XMapWindow(dpy,win);

	/* Create a "Graphics Context" */
	GC gc=XCreateGC(dpy,win,0,NULL);
	
	XGCValues gcpxval;
	gcpxval.function=GXset;
	GC gcpx=XCreateGC(dpy,pxmap,GCFunction,&gcpxval);
	

	/* Tell the GC we draw using the white color */
	XSetForeground(dpy,gc,blackColor);
		/* Fill a rectangle */
		XFillRectangle(dpy,pxmap,gcpx,0,0,200,100);
		/* Draw the line */
		XDrawLine(dpy,pxmap,gc,10,60,180,20);
		/* Draw a Rectangle */
		XDrawRectangle (dpy,pxmap,gc,10,10,20,20);
		/* Put Pixmap onto the window */

	do
	{
		/* Wait for an event */
		XNextEvent(dpy,&event);

		switch (event.type)
		{
			case MapNotify:
			case Expose:
			case ButtonPress:
			case ConfigureNotify:
			case MotionNotify:	
			case ButtonRelease:
			
			default:
			
				/* Put fractal pixmap onto the window */
				XCopyArea(dpy,pxmap,win,gc,0,0,200,100,0,0);
				/* Send request to the server */
			        XFlush(dpy);
				break;	
		}
		sleep(1);
	}
	while (1);

	return 0;
}

