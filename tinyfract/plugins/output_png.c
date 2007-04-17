#include <gd.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../common.h"
#include "../plugin.h"


#define MAX_COLORS 65536
#define COLOR_CEILING 65536 


typedef struct
{
	gdImagePtr im;
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

/* Constructor and destructor for png output. */
static png_t* constructor_png(const view_dimension_t dimension, char args[])
{
	png_t* context;
	int    i;
	float  H=0;
	float  S=0;
	float  Br=0;

	char   des[3];
	int    mod[3];
	float  thres[3];

	int   iteration_steps;
	char* output_args;
	char* name;
	

	/* Get memory for the output context. */
	if (!(context=malloc(sizeof(png_t)))) return NULL;

	/* Parse the output args and the safing file name. */
	output_args=strtok(args,"-");
	name=strtok(NULL,"-");
	#ifdef DEBUG
	fprintf(stderr,"Output args: %s\n", output_args);
	fprintf(stderr,"Safing file: %s\n", name);
	#endif

	
	/* Check if parameters were given. */
	if(output_args==NULL)
	{
		fprintf(stderr,"Please insert output parameters\n");
		exit(EXIT_FAILURE);
	}

	/* Check safing file was given. */
	if(name==NULL)
	{
		fprintf(stderr,"Please insert a safing file for the image.\n");
		exit(EXIT_FAILURE);
	}

	/* Initialize the viewport. */
	
	/* Open the display. */
	context->im=gdImageCreateTrueColor(dimension.width,dimension.height);

	/* Open the saving file. */
	context->output_file=fopen(name,"wb");
	
	/* Scan the argument string. */
	sscanf(output_args,"%c%d,%f%c%d,%f%c%d,%f",
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
				fprintf(stderr,"Illegal output format %s.\n",output_args);
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
		
		context->colors[i]=gdImageColorAllocate(
			context->im,
			round((R(H,S,Br)+1)*127.5),
			round((G(H,S,Br)+1)*127.5),
			round((B(H,S,Br)+1)*127.5));
	}

	/* Return the handle. */
	return context;
}

void destructor_png(png_t* handle)
{
	/* Close open image and safing file. */
	fclose(handle->output_file);
	gdImageDestroy(handle->im);
	
	free(handle);
}

/* Blit rectangle from pixelbuffer to X11 viewport. */
void blit_rect_png(png_t* handle, const view_position_t position, const view_dimension_t dimension, pixel_value values[])
{
}


/* Fill rectangle in image with color. */
void fill_rect_png(png_t* handle, const view_position_t position, const view_dimension_t dimension, const pixel_value value)
{
	/* Put a filled rectangle in the image. */
	gdImageFilledRectangle(handle->im,
			position.x,
			position.y,
			dimension.width+position.x,
			dimension.height+position.y,
			handle->colors[value]);
}

/* Put pixel into the image viewport. */
void put_pixel_png(png_t* handle, const view_position_t position, const pixel_value value)
{
	/* Put a pixel into the image. */
	gdImageSetPixel(handle->im,position.x,position.y,handle->colors[value]);
}

/* Safe the image */
void flush_viewport_png(png_t* handle, button_event_t* position)
{
	/* Safe the image. */
	gdImagePng(handle->im,handle->output_file);
	
	/* Call the main function that the programm is over. */
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
	{ type: plugin_facility_end }
};

