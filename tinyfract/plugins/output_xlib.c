









/*-------------------------------------------------------------------*/
/* xtgrafic.c: Xlib-Graphik im Xt-Core-Widget    Ress.File XTgraphic */
/*             J.Schitko  August 92           (geht auch mit ANSI-C) */
/*-------------------------------------------------------------------*/

#include <stdio.h>
#include <X11/Intrinsic.h>     /* damit auch Header X11/Core.h aktiv */

#include <X11/StringDefs.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>

#define  BILDGROESSE_X     150            /* Groesse des xlib-Bildes */
#define  BILDGROESSE_Y     150

static Widget w_parent,        /* viele Widgets global schadet nicht */
                w_topform,             /* Form-Widget um alles herum */
                  w_commandbox,   /* Box um einzelne Command-Buttons */
                  w_xtgraphic;      /* Core-Widget fuer xlib-Graphik */

/*-------------- Globale Deklaration der xlib-Strukturen -----------*/
Display         *mydisplay;  /* Basisstruktur fuer Serververbindung */
int             myscreen;                          /* Screen-Nummer */
Window          mywindow;                              /* Window-ID */
XEvent          myevent;                        /* Struktur "Event" */
GC              mygc;                       /* neu: Graphik-Kontext */
int             i;                                /* gut fuer alles */

/*---------------------- Funktion Init xlib ------------------------*/
void init_xlib()
{
  unsigned long black, white;        /* Nummern der Farben          */

  mydisplay = XtDisplay(w_parent);   /* Display des PARENT-Widgets! */
  mywindow  = XtWindow(w_xtgraphic); /* Window des CORE-Widgets !   */

  myscreen = DefaultScreen(mydisplay);

  black = BlackPixel(mydisplay, DefaultScreen(mydisplay));
  white = WhitePixel(mydisplay, DefaultScreen(mydisplay));

  /* Init graphical context; letzter Parameter nur fuer ANSI-C, = 0 */
  mygc = XCreateGC(mydisplay,mywindow,0,(XGCValues*)NULL);

  XSetForeground(mydisplay, mygc, black);          /* Farben setzen */
  XSetBackground(mydisplay, mygc, white);
  XDrawLine (mydisplay, mywindow, mygc, 0,0,150,150);
}
/*---------------------- Callback-Funktion Quit --------------------*/
static void Quit(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  exit(0);
}

/*----------------- Callback-Funktion cleargraphic -----------------*/
static void cleargraphic(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  XClearWindow(mydisplay, mywindow);       /* Clear-Befehl der xlib */
}

/*---------------- Callback-Funktion male_rechtecke ----------------*/
static void male_rechtecke(w, client_data, call_data)
Widget w;
XtPointer client_data, call_data;
{
  XDrawRectangle(mydisplay, mywindow, mygc,        /* 10*10 Groesse */
                 rand()%140,rand()%140,10,10);
                 /* Das sind ganz MIESE Zufallszahlen!*/
XDrawLine (mydisplay, mywindow, mygc, 0,0,150,150);
}

/*---------------- Definiere die 3 Command-Widgets -----------------*/
static void fill_commandbox(w)      /* w = Parent dieser 3 Commands */
Widget w;
{
  Widget w_rechtecke, w_clear_graphic, w_quit;     /* lokal reicht */

  w_rechtecke = XtVaCreateManagedWidget(
                   "rechtecke", commandWidgetClass, w, NULL);
  XtAddCallback(w_rechtecke, XtNcallback, male_rechtecke, 0);

  w_clear_graphic = XtVaCreateManagedWidget(
                   "cleargraphic", commandWidgetClass, w,
                   XtNfromVert, (XtArgVal) w_rechtecke,
                   NULL);              /* Position per Hardcoding! */
  XtAddCallback( w_clear_graphic, XtNcallback, cleargraphic, 0);

  w_quit = XtVaCreateManagedWidget(
                   "quit",commandWidgetClass, w,
                   XtNvertDistance, (XtArgVal) 40,
                   XtNfromVert,     (XtArgVal) w_clear_graphic,
                   NULL);              /* Position per Hardcoding! */
  XtAddCallback( w_quit, XtNcallback, Quit, 0);
}

/*========================= Hauptprogramm =========================*/
void main(argc,argv)
int argc;
char *argv[];
{
  XtAppContext appcontext;                     /* To make Xt happy */

  w_parent = XtVaAppInitialize(&appcontext,
                 "XTgrafik",                     /* Ressource-File */
                 NULL, 0, &argc, argv, NULL, NULL);

  w_topform = XtVaCreateManagedWidget(        /* Form-Box um alles */
                 "topform",
                 formWidgetClass, w_parent, NULL);


  w_commandbox = XtVaCreateManagedWidget( /* Box um die 3 Commands */
                 "commandbox",
                 formWidgetClass,
                 w_topform,
                 NULL);


  fill_commandbox(w_commandbox);     /* Command Buttons definieren */


  w_xtgraphic = XtVaCreateManagedWidget(  /* Core-Widget fuer xlib */
                  "xtgraphic",
                  coreWidgetClass,                /* widget class  */
                  w_topform,                      /* parent widget */
                 /* Hardcoding; die Box soll neben der Grafik sein */
                  XtNheight,     (XtArgVal) BILDGROESSE_Y,
                  XtNwidth,      (XtArgVal) BILDGROESSE_X,
                  XtNfromHoriz,  (XtArgVal) w_commandbox,
                  NULL);

 
  XtRealizeWidget(w_parent);                   /* alles darstellen */

  init_xlib();           /* Initialisiere Xlib darf danach kommen! */

  XtAppMainLoop(appcontext);/* Event-Bearbeitung nur fuer Toolkit! */
}	
