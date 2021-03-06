#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../common.h"
#include "../plugin.h"

#define MAX_COLORS 65536
#define COLOR_CEILING 65535
/* Local data structure. */
typedef struct
{
	XGCValues gcpxval;
	GC               gcpx;
	GC               gc;
	Display*         dpy;
	Window           win;
 	Pixmap           pxmap;
	XColor           colors[MAX_COLORS];
	int*             iterations;
	view_dimension_t geometry;
} x11_t;	


/* Color mapping functions. */
float Rh(float H)
{
	if(H<1.0/3) return 1-(3*H);	
	else if (H>2.0/3) return 3*H-2;
	else return 0;
}

float Gh(float H)
{
	if(H<1.0/3) return (3*H);	
	else if (H>(2.0/3)) return 0;
	else return 2-(3*H);
}

float Bh(float H)
{
	if(H<1.0/3) return 0;	
	else if (H>2.0/3) return 3-(3*H);
	else return (3*H)-1;
}


float R(float H,float S,float Br)
{
	return Br+Rh(H)*S*(1-Br);
}

float G(float H,float S,float Br)
{
	return Br+Gh(H)*S*(1-Br);
}

float B(float H,float S,float Br)
{
	return Br+Bh(H)*S*(1-Br);
}

/* Constructor and destructor for X11 output. */
static x11_t* constructor_x11(const view_dimension_t dimension, const char args[])
{
	x11_t* context;
	int    i;
	float  H=0;
	float  S=0;
	float  Br=0;

	char   des[3];
	int    mod[3];
	float  thres[3];
	
	int    iteration_steps;

	Window       window_id;
	unsigned int window_id_helper;

	char* args_tok;
	char* color_str;
	char* id_str;

	/* Get memory for the output context. */
	if (!(context=malloc(sizeof(x11_t)))) return NULL;

	/* Check if parameters were given. */
	if(args==NULL)
	{
		fprintf(stderr,"Please insert output parameters\n");
		exit(EXIT_FAILURE);
	}

	/* Tok the string. */
	/* First we copy the args in order to let the main args untouched. */
	args_tok=malloc(sizeof(args));
	strcpy(args_tok,args);

	/* Now tok. */
	color_str=strtok(args_tok,"@");
	id_str=strtok(NULL,"@");

	/* Scan the color string. */
	sscanf(args,"%c%d,%f%c%d,%f%c%d,%f",
		&des[0],&mod[0],&thres[0],
		&des[1],&mod[1],&thres[1],
		&des[2],&mod[2],&thres[2]);

	/* Initialize the viewport. */
	/* Open the display. */
	context->dpy=XOpenDisplay(NULL);

	/* Get the window id either from args or get new. */
	if(id_str==NULL)
	{
		window_id=DefaultRootWindow(context->dpy);
	} else
	{
		sscanf(id_str,"0x%x",&window_id_helper);
		window_id=(Window)window_id_helper;
	}

	#ifdef DEBUG
	fprintf(stderr,"Window id is 0x%x\n",(unsigned int)window_id);
	#endif
	
	/* Create the window. */
	int bgColor=WhitePixel(context->dpy,DefaultScreen(context->dpy));
//	context->win=XCreateSimpleWindow(context->dpy,DefaultRootWindow(context->dpy),
//			0,0,dimension.width,dimension.height,0,bgColor,bgColor);
	context->win=XCreateSimpleWindow(context->dpy,window_id,
			0,0,dimension.width,dimension.height,0,bgColor,bgColor);

	/* Create a pixmap as double buffer. */
	context->pxmap=XCreatePixmap(context->dpy,DefaultRootWindow(context->dpy),
			dimension.width,dimension.height,16);

	/* Create a "Graphics Context" for the window and the pixmap. */
	context->gc=XCreateGC(context->dpy,context->win,0,NULL);
	context->gcpx=XCreateGC(context->dpy,context->pxmap,0,NULL);

	
	/* Just for help now. */
	iteration_steps=mod[0]*mod[1]*mod[2];

	
	/* Allocate colors */
	for(i=0;i<iteration_steps;i++)
	{
		int x=i;
		int d;
		
		for (d=0;d<3;d++)
		{
			if (thres[d]<0 || thres[d]>=1)
			{
				fprintf(stderr,"Illegal output format %s.\n",args);
				exit(EXIT_FAILURE);
			}
			switch (des[d])
			{
				case 'h':
				case 'H':
					H=(float)(x%mod[d])/mod[d]+thres[d];
					if (H>1) H=H-1;
					x=x/mod[d];
					break;
				case 's':
				case 'S':
					S=(float)(x%mod[d])/mod[d]+thres[d];
					if (S>1) S=S-1;
					x=x/mod[d];
					break;
				case 'b':
				case 'B':
					Br=(float)(x%mod[d])/mod[d]+thres[d];
					if (Br>1) Br=Br-1;
					x=x/mod[d];
					break;
				default: 
					fprintf(stderr,"Illegal output format %s.\n",args);
					exit(EXIT_FAILURE);
			}	
		}
		
		context->colors[i].red=  R(H,S,Br)*COLOR_CEILING;
		context->colors[i].green=G(H,S,Br)*COLOR_CEILING;
		context->colors[i].blue= B(H,S,Br)*COLOR_CEILING;
		
		context->colors[i].flags=DoRed|DoGreen|DoBlue;
		XAllocColor(context->dpy,DefaultColormap(context->dpy,DefaultScreen(context->dpy)),
			&context->colors[i]);
	}
	
	/* Get memory to safe the iteration steps. */
	context->iterations=malloc(sizeof(ordinal_number_t)*dimension.width*dimension.height);

	/* Copy the geometry */
	context->geometry.width=dimension.width;
	context->geometry.height=dimension.height;

	/* Return the handle. */
	return context;
}

void destructor_x11(x11_t* handle)
{
	free(handle->iterations);
	free(handle);
}

		

/* Blit rectangle from pixelbuffer to X11 viewport. */
void blit_rect_x11(x11_t* handle, const view_position_t position, const view_dimension_t dimension, pixel_value values[])
{
}

/* Put pixel into X11 viewport. */
void put_pixel_x11(x11_t* handle, const view_position_t position, const pixel_value value)
{
	/* Set color for next operation. */
//	XSetForeground(handle->dpy,handle->gcpx,handle->colors[value].pixel);

	/* Put a pixel into the double buffer. */
//	XDrawPoint(handle->dpy,handle->pxmap,handle->gcpx,position.x,position.y);

	/* Put a pixel into the map. */
	*(handle->iterations+position.y*handle->geometry.width+position.x)=value;
}


/* Fill rectangle in X11 viewport with color. */
void fill_rect_x11(x11_t* handle, const view_position_t position, const view_dimension_t dimension, const pixel_value value)
{
	view_position_t map_position;

	/* Set color for next operation. */
//	XSetForeground(handle->dpy,handle->gcpx,handle->colors[value].pixel);
	
	/* Fill a rectangle. */
//	XFillRectangle(handle->dpy,handle->pxmap,handle->gcpx,position.x,position.y,dimension.width,dimension.height);

	/* Put a rectangle into the map. */
	for(map_position.x=position.x;map_position.x<=position.x+dimension.width;map_position.x++)
	{
		for(map_position.y=position.y;map_position.y<=position.y+dimension.height;map_position.y++)
		{
			*(handle->iterations+map_position.y*handle->geometry.width+map_position.x)=value;
		}
	}
}

void put_map_in_buffer(x11_t* handle)
{
	int x;
	int y;

	for(x=0;x<=handle->geometry.width;x++)
	{
		for(y=0;y<=handle->geometry.height;y++)
		{
			/* Set color for next operation. */
			XSetForeground(handle->dpy,handle->gcpx,handle->colors[*(handle->iterations+y*handle->geometry.width+x)].pixel);

			/* Put a pixel into the double buffer. */
			XDrawPoint(handle->dpy,handle->pxmap,handle->gcpx,x,y);
		}
	}
}

/* Remap the window. */
void remap_x11(x11_t* handle)
{
	XWindowAttributes attributes;
	

	#ifdef DEBUG
	fprintf(stderr,"Unmap and Map the viewport.\n");
	#endif

	/* First get the window attributes. */
	XGetWindowAttributes(handle->dpy,handle->win,&attributes);
	
	/* Put double buffer onto the window. */
	XCopyArea(handle->dpy,handle->pxmap,handle->win,handle->gc,
		0,0,attributes.width,attributes.height,0,0);
	
	/* Send request to the server */
	XFlush(handle->dpy);
	
	/* Map and unmap */
	XUnmapWindow(handle->dpy,handle->win);
	XMapWindow(handle->dpy,handle->win);
}

/* Function wich allocates colors and flushes the viewport. */
void new_output_parameter(char* args,x11_t* handle)
{
	int    i;
	float  H=0;
	float  S=0;
	float  Br=0;

	char   des[3];
	int    mod[3];
	float  thres[3];
	
	int    iteration_steps;

	/* Scan the color string. */
	sscanf(args,"%c%d,%f%c%d,%f%c%d,%f",
		&des[0],&mod[0],&thres[0],
		&des[1],&mod[1],&thres[1],
		&des[2],&mod[2],&thres[2]);

	/* Just for help now. */
	iteration_steps=mod[0]*mod[1]*mod[2];
	
	/* Allocate colors */
	for(i=0;i<iteration_steps;i++)
	{
		int x=i;
		int d;
		
		for (d=0;d<3;d++)
		{
			if (thres[d]<0 || thres[d]>=1)
			{
				fprintf(stderr,"Illegal output format %s.\n",args);
				exit(EXIT_FAILURE);
			}
			switch (des[d])
			{
				case 'h':
				case 'H':
					H=(float)(x%mod[d])/mod[d]+thres[d];
					if (H>1) H=H-1;
					x=x/mod[d];
					break;
				case 's':
				case 'S':
					S=(float)(x%mod[d])/mod[d]+thres[d];
					if (S>1) S=S-1;
					x=x/mod[d];
					break;
				case 'b':
				case 'B':
					Br=(float)(x%mod[d])/mod[d]+thres[d];
					if (Br>1) Br=Br-1;
					x=x/mod[d];
					break;
				default: 
					fprintf(stderr,"Illegal output format %s.\n",args);
					exit(EXIT_FAILURE);
			}	
		}
		
		handle->colors[i].red=  R(H,S,Br)*COLOR_CEILING;
		handle->colors[i].green=G(H,S,Br)*COLOR_CEILING;
		handle->colors[i].blue= B(H,S,Br)*COLOR_CEILING;
		
		handle->colors[i].flags=DoRed|DoGreen|DoBlue;
		XAllocColor(handle->dpy,DefaultColormap(handle->dpy,DefaultScreen(handle->dpy)),
			&handle->colors[i]);
	}

	put_map_in_buffer(handle);
	remap_x11(handle);
}

/* Flush X11 viewport */
void flush_viewport_x11(x11_t* handle, button_event_t* position)
{
	XEvent event;
	XWindowAttributes attributes;

	/* Put the map into the double buffer. */
	put_map_in_buffer(handle);

	/* We want to get MapNotify events for our window. */
	//XSelectInput(handle->dpy,handle->win,StructureNotifyMask);
	XSelectInput(handle->dpy,handle->win,0xffffff);

	/* Now "map" the window (that is, make it appear on the screen). */
	XUnmapWindow(handle->dpy,handle->win);
	XMapWindow(handle->dpy,handle->win);

	/* Get window size. */
	XGetWindowAttributes(handle->dpy,handle->win,&attributes);
	for(;;)
	{
		/* Wait for an event. */
		XWindowEvent(handle->dpy,handle->win,0xffffff,&event);
		#ifdef DEBUG
		fprintf(stderr,"Event: %d\n",event.type);
		#endif
		switch (event.type)
		{
			case ButtonPress:
			
				#ifdef DEBUG
				fprintf(stderr,"Button Pressed: %d\n", event.xbutton.button);
				#endif
				switch (position->type=event.xbutton.button)
				{
					case 1:
					case 2:
					case 3:
					case 4:
					case 5:
						position->x=event.xbutton.x;
						position->y=event.xbutton.y;
						position->type=autozoom_set_center;
						goto exit_func;
					default:
						break;
				}
			case MapNotify:
			case PropertyNotify:
			case ReparentNotify:
			case ConfigureNotify:
				#ifdef DEBUG
				fprintf(stderr,"Remap the window.\n");
				#endif
				/* Put double buffer onto the window. */
				XCopyArea(handle->dpy,handle->pxmap,handle->win,handle->gc,
						0,0,attributes.width,attributes.height,0,0);
				/* Send request to the server */
				XFlush(handle->dpy);
				break;	
			default:
				break;
		}	
	}
exit_func: 

	return;
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
				remap_function:          (const plugin_output_remap_function_t*) &remap_x11,
				put_pixel_function:      (const plugin_output_put_pixel_function_t*) &put_pixel_x11,
				new_output_parameter_function: (const plugin_output_new_parameter_function_t*) &new_output_parameter,
			}
		}
	},
	{ type: plugin_facility_end }
};

