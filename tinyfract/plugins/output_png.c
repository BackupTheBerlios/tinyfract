#include <gd.h>
#include <stdlib.h>
#include <stdio.h>
#include "../common.h"
#include "../plugin.h"

#define MAX_COLORS 65536
#define COLOR_CEILING 256 


typedef struct
{
	gdImagePtr dpy;
	FILE*      output_file;
	int        colors[MAX_COLORS];
} png_t;	


/* Color mapping functions. */
float Rh(float H)
{
	if(H<1/3) return 1-(3*H);	
	else if (H>2/3) return 3*H-2;
	else return 0;
}

float Gh(float H)
{
	if(H<1/3) return (3*H);	
	else if (H>2/3) return 0;
	else return 2-(3*H);
}

float Bh(float H)
{
	if(H<1/3) return 0;	
	else if (H>2/3) return 3-(3*H);
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
static png_t* constructor_png(const view_dimension_t dimension, const char args[])
{
	png_t* context;
	int    i;
	float  H;
	float  S;
	float  Br;

	char   des[3];
	int    mod[3];
	float  thres[3];
	
	int   iteration_steps;
	char* name;
	

	/* Get memory for the output context. */
	if (!(context=malloc(sizeof(png_t)))) return NULL;

	/* Initialize the viewport. */
	
	
	/* Open the display. */
	context->dpy=gdImageCreateTrueColor(dimension.width,dimension.height);
	
	/* Save the name of the image. */
	context->output_file=fopen("test.png","wb");
	
	/* Check if parameters were given. */
	if(args==NULL)
	{
		fprintf(stderr,"Please insert output parameters\n");
		exit(EXIT_FAILURE);
	}

	/* Scan the argument string. */
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
		
		context->colors[i]=gdImageColorAllocate(context->dpy,R(H,S,Br)*COLOR_CEILING,G(H,S,Br)*COLOR_CEILING,B(H,S,Br)*COLOR_CEILING);
	}

	/* Return the handle. */
	return context;
}

void destructor_png(png_t* handle)
{
	fclose(handle->output_file);
	gdImageDestroy(handle->dpy);
	free(handle);
}

/* Blit rectangle from pixelbuffer to X11 viewport. */
void blit_rect_png(png_t* handle, const view_position_t position, const view_dimension_t dimension, pixel_value values[])
{
}


/* Fill rectangle in X11 viewport with color. */
void fill_rect_png(png_t* handle, const view_position_t position, const view_dimension_t dimension, const pixel_value value)
{
	gdImageRectangle(handle->dpy,
			position.x,
			position.y,
			dimension.width+position.x,
			dimension.height+position.y,
			handle->colors[value]);
}

/* Put pixel into X11 viewport. */
void put_pixel_png(png_t* handle, const view_position_t position, const pixel_value value)
{
	/* Put a pixel into the image. */
	gdImageSetPixel(handle->dpy,position.x,position.x,handle->colors[value]);
}

/* Flush X11 viewport */
void flush_viewport_png(png_t* handle, button_event_t* position)
{
#if 0
	XEvent event;
	XWindowAttributes attributes;
	
	/* We want to get MapNotify events for our window. */
	//XSelectInput(handle->dpy,handle->win,StructureNotifyMask);
	XSelectInput(handle->dpy,handle->win,0xffffff);

	/* Now "map" the window (that is, make it appear on the screen). */
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
				position->x=event.xbutton.x;
				position->y=event.xbutton.y;
				position->type=event.xbutton.button;
				#ifdef DEBUG
				fprintf(stderr,"c%d,%d\n",event.xbutton.x,event.xbutton.y);
				fprintf(stderr,"Button Pressed: %d\n", event.xbutton.button);
				#endif
				goto exit_func;
			case MapNotify:
			case PropertyNotify:
			case ReparentNotify:
			case ConfigureNotify:
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
#endif
	gdImagePng(handle->dpy,handle->output_file);
	
	position->type=autozoom_quit;
	return;
}


/* Enumerate plugin facilities. */
volatile const plugin_facility_t tinyfract_plugin_facilities[]=
{
	{
		name: "png",
		type: plugin_facility_output,
		facility:
		{
			output:
			{
				constructor:             (const plugin_output_constructor_t*) &constructor_png,
				destructor:              (const plugin_output_destructor_t*) &destructor_png,
				blit_rect_function:      (const plugin_output_blit_rect_function_t*) &blit_rect_png,
				fill_rect_function:      (const plugin_output_fill_rect_function_t*) &fill_rect_png,
				flush_viewport_function: (const plugin_output_flush_viewport_function_t*) &flush_viewport_png,
				put_pixel_function:      (const plugin_output_put_pixel_function_t*) &put_pixel_png,
			}
		}
	},
	{ plugin_facility_end }
};

