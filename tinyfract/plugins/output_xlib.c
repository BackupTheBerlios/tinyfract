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

	/* Open the display */
	assert(dpy=XOpenDisplay(NULL));

	/* Get some colors */
	blackColor=BlackPixel(dpy,DefaultScreen(dpy));
	whiteColor=WhitePixel(dpy,DefaultScreen(dpy));

	/* Create the window */
	win=XCreateSimpleWindow(dpy,DefaultRootWindow(dpy),0,0,200,100,0,blackColor,blackColor);

	/* We want to get MapNotify events */
	XSelectInput(dpy,win,StructureNotifyMask);

	/* "Map" the window (that is, make it appear on the screen) */
	XMapWindow(dpy,win);

	/* Create a "Graphics Context" */
	GC gc=XCreateGC(dpy,win,0,NULL);

	/* Tell the GC we draw using the white color */
	XSetForeground(dpy,gc,whiteColor);

	/* Wait for the first MapNotify event */
	do
	    XNextEvent(dpy,&event);
	while (event.type!=MapNotify);

	/* Draw the line */
	XDrawLine(dpy,win,gc,10,60,180,20);

	/* Send the "DrawLine" request to the server */
	XFlush(dpy);

	// Wait for 10 seconds
	sleep(10);

	return 0;
}

