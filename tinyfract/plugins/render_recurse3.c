#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../common.h"
#include "../param_parser.h"
#include "../plugin.h"
#include "../plugin_loader.h"

/*  Data structure for render arguments and other volatile data. */
typedef struct
{
	complex_number_t   center;
	view_dimension_t   geometry;
	real_number_t      scale;
	plugin_facility_t* fractal_facility;
	plugin_facility_t* output_facility;
	void*              fractal;
	void*              output;
	int                param;
	int                square_size;
	int                x_count;
	int                y_count;
	ordinal_number_t*  points;
} render_t;

/* Constructor and destructor for recurse render function. */
static render_t* constructor(
		const complex_number_t   center,
		const view_dimension_t   geometry,
		const real_number_t      scale,
		const plugin_facility_t* fractal_facility,
		const plugin_facility_t* output_facility,
		const void*              fractal,
		const void*              output,
		const char               args[])
{
	render_t*        context;
	int              x_render;
	int              y_render;
	view_position_t  render_position;
	complex_number_t complex_position;
	real_number_t    scaling_factor;
	view_position_t  shift;
	
	/* Get memory for the fractal context. */
	if (!(context=malloc(sizeof(render_t)))) return NULL;

	/* Set the fractal context. */
	VARCOPY(context->center,center);
	VARCOPY(context->geometry,geometry);
	VARCOPY(context->scale,scale);
	VARCOPY(context->fractal_facility,fractal_facility);
	VARCOPY(context->output_facility,output_facility);
	VARCOPY(context->fractal,fractal);
	VARCOPY(context->output,output);
	sscanf(args,"%d", &context->param);
	

	
	



	/* Return the handle. */
	return context;
}

static void destructor(render_t* handle)
{
	free(handle);
}

/* Render the picture by squares in one thread; very good :-P */
void render_recurse(render_t* handle)
{
	/* Volatile datas */
	complex_number_t complex_position;
	view_position_t  render_position;
	real_number_t    scaling_factor;
	view_position_t  shift;
	ordinal_number_t actual;
	int              x_square;
	int              y_square;
	int               square_size;
	ordinal_number_t* help;

	/* Get memory for points. */
	handle->points=malloc((sizeof(ordinal_number_t))*handle->geometry.width*handle->geometry.height);
	
	
	/* Calculate square_size. */
	square_size=2*handle->param-1;

	/* Calculate number of squares. */
	x_square=handle->geometry.width/square_size;
	y_square=handle->geometry.height/square_size;
	
	/* Precalculate scaling factor and center shift for speed reasons. */
	scaling_factor=handle->scale/handle->geometry.width;
	shift.x=handle->geometry.width/2;
	shift.y=handle->geometry.height/2;

	for(render_position.x=0;render_position.x<x_square*square_size;render_position.x+=square_size)
	{
		for(render_position.y=0;render_position.y<y_square*square_size;render_position.y+=square_size)
		{
			/* Calculate actual iteration. */
			Re(complex_position)=Re(handle->center)+scaling_factor*(render_position.x-shift.x);
			Im(complex_position)=Im(handle->center)-scaling_factor*(render_position.y-shift.y);

			help=handle->points
				+render_position.y*handle->geometry.width
				+render_position.x;
			
			*help=(*handle->fractal_facility->facility.fractal.calculate_function)(handle->fractal,complex_position);
		}
	}
	
	#if 0
	fprintf(stderr,"Iteration of the corner is: %d\n", actual);

	/* Test if corners have the same iterations. */
	if(actual==handle->line[handle->x_count] && actual==handle->line[handle->x_count-1] && actual==handle->latest)
	{
		#if 0
		fprintf(stderr,"render_recurse: In the if structure Corner iteration is: %d.\n",actual);
		#endif

		/* Yes. Fill the squares with color. */
		for(x_render=(handle->x_count-1)*handle->square_size+handle->starting_point.x;x_render<handle->x_count*handle->square_size+handle->starting_point.x;x_render++)
		{
			for(y_render=(handle->y_count-1)*handle->square_size+handle->starting_point.y;y_render<handle->y_count*handle->square_size+handle->starting_point.y;y_render++)
			{
				VARCOPY(render_position.x,x_render);
				VARCOPY(render_position.y,y_render);

				(*handle->output_facility->facility.output.put_pixel_function) (handle->output,render_position,actual);
			}
		}
	}

	/* Check if the line is complete. */
	if(handle->x_count>=handle->x_square)
	{
		/* Yes. Switch to next line. */
		handle->line[handle->x_count]=actual;
		handle->line[handle->x_count-1]=handle->latest;

		handle->y_count++;

		render_position.y=handle->y_count*handle->square_size+handle->starting_point.y;
		render_position.x=handle->starting_point.x;

		Re(complex_position)=Re(handle->center)+scaling_factor*(render_position.x-shift.x);
		Im(complex_position)=Im(handle->center)-scaling_factor*(render_position.y-shift.y);

		handle->latest=(*handle->fractal_facility->facility.fractal.calculate_function)(handle->fractal,complex_position);

		handle->x_count=1;
	}
	else
	{
		/* No. Switch to next x value. */
		handle->line[handle->x_count-1]=handle->latest;
		handle->latest=actual;

		handle->x_count++;
	}

	#ifdef DEBUG
	fprintf(stderr,"Durchlauf in X-Richtung: %d von %d, Durchlauf in Y-Richtung: %d von %d\n", handle->x_count,handle->x_square,handle->y_count,handle->y_square);
	#endif

	/* Check if recursion must be started. */
	if(handle->y_count<handle->y_square)
		render_recurse(handle);
	#endif
}

/* Enumerate plugin facilities. */
volatile const plugin_facility_t tinyfract_plugin_facilities[]=
{
	{
		name: "recurse2",
		type: plugin_facility_render,
		facility:
		{
			render:
			{
				constructor:     (const plugin_render_constructor_t*) &constructor,
				destructor:      (const plugin_render_destructor_t*) &destructor,
				render_function: (const plugin_render_function_t*) &render_recurse
			}
		}
	},
	{ plugin_facility_end }
};

