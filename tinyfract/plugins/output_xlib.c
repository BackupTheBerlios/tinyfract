/* XHelloWorld, (c) 1997 Linux-Magazin, P.Murmann */
 
 #include <stdio.h>
 #include <X11/Xlib.h>
 #include <X11/Xutil.h>
 
 char hello[] = {"."};
 char mystring[] = {":D"};
 
 main(int argc, char *argv[])
 {
   Display *mydpy;
   Window mywin;
   Drawable d;
   GC mygc;
   XEvent myevent;
   KeySym mykey;
   XSizeHints myhint;
   int myscreen;
   unsigned long fg, bg;
   int i;
   char text[10];
   int loop;
 
   if(!(mydpy = XOpenDisplay(NULL))) {
     (void)fprintf(stderr, "Unable to open display: %s\n",
       XDisplayString(NULL));
     exit(1);
   }
   myscreen = DefaultScreen(mydpy);
   bg = WhitePixel(mydpy, myscreen);
   fg = BlackPixel(mydpy, myscreen);
   myhint.x = 200;
   myhint.y = 100;
   myhint.width = 256;
   myhint.height = 256;
   myhint.flags = PPosition | PSize;
   mywin = XCreateSimpleWindow(mydpy,
     DefaultRootWindow(mydpy),
     myhint.x, myhint.y, myhint.width, myhint.height,
    150, fg, bg);
   XSetStandardProperties(mydpy, mywin, hello, hello,
     None, argv, argc, &myhint);
   mygc = XCreateGC(mydpy, mywin, 0, 0);
   XSetForeground(mydpy, mygc, fg);
   XSetBackground(mydpy, mygc, bg);
   XSelectInput(mydpy, mywin,
     ButtonPressMask | KeyPressMask | ExposureMask);
   XMapRaised(mydpy, mywin);
   loop = 1;
 
   while(loop) {
     XNextEvent(mydpy, &myevent);
     switch(myevent.type) {
     case Expose:
       //if(myevent.xexpose.count == 0)
         //XDrawPixel
	XDrawPoint(mydpy, fg, mygc, 10,10);
       
       break;
     case MappingNotify:
       XRefreshKeyboardMapping(&myevent);
       break;
     case ButtonPress:
       XDrawImageString(myevent.xexpose.display,
         myevent.xexpose.window, mygc,
    myevent.xbutton.x, myevent.xbutton.y,
    mystring, strlen(mystring));
       break;
     case KeyPress:
       i = XLookupString(&myevent, text, 10, &mykey, 0);
       if(i == 1 && text[0] == 'q')
         loop = 0;
       break;
     }
     (void)printf("Got event: %i\n", myevent.type);
   }
   XFreeGC(mydpy, mygc);
   XDestroyWindow(mydpy, mywin);
   XCloseDisplay(mydpy);
   exit(0);
 }			
