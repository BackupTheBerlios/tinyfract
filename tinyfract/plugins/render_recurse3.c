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
	int                param_help;
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

	/* Check if parameter was given, */
	if(args==NULL)
	{
		fprintf(stderr,"You have to specify render parameters\n");
		exit(EXIT_FAILURE);
	}

	#ifdef DEBUG
	fprintf(stderr,"My parameter is: %s", args);
	#endif
	
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

/* New calculate function. */
ordinal_number_t cache_calculator(render_t* handle,const view_position_t render_position)
{
	/* Volatile datas */
	ordinal_number_t*  help;
	complex_number_t complex_position;
	view_position_t    shift;
	real_number_t      scaling_factor;

	/* Check if the point is also claculated. */
	help=handle->points+render_position.y*handle->geometry.width+render_position.x;
	if(*help==0)
	{
		/* No the has not been calculated till now, calculate the iteration. */

		/* Precalculate scaling factor and center shift for speed reasons. */
		scaling_factor=handle->scale/handle->geometry.width;
		shift.x=handle->geometry.width/2;
		shift.y=handle->geometry.height/2;

		/* Calculate the iteration. */
		Re(complex_position)=Re(handle->center)+scaling_factor*(render_position.x-shift.x);
		Im(complex_position)=Im(handle->center)-scaling_factor*(render_position.y-shift.y);
		*help=(*handle->fractal_facility->facility.fractal.calculate_function)(handle->fractal,complex_position);
	}

	/* Return the iteration. */
	return(*help);
}

void fill_square(render_t* handle,const view_position_t start_point,const int square_size)
{
	/* Volatile datas */
	ordinal_number_t ol;
	ordinal_number_t or;
	ordinal_number_t ul;
	ordinal_number_t ur;
	view_position_t  help;
	view_dimension_t square_help;
	int              new_square_size;

	/* Calculate iterations for the first corner. */
	help.x=start_point.x;
	help.y=start_point.y;
	ol=cache_calculator(handle,help);

	/* Calculate iterations for the second corner. */
	help.x=start_point.x+square_size;
	help.y=start_point.y;
	or=cache_calculator(handle,help);

	/* Calculate iterations for the third corner. */
	help.x=start_point.x+square_size;
	help.y=start_point.y+square_size;
	ur=cache_calculator(handle,help);

	/* Calculate iterations for the fourth corner. */
	help.x=start_point.x;
	help.y=start_point.y+square_size;
	ul=cache_calculator(handle,help);
	
	/* Check if the corners have the same iterations. */
	if(ol==or && ol==ul && ol==ur)
	{
		/* Yes, print the hole square. */
		square_help.width=square_size;
		square_help.height=square_size;
		(*handle->output_facility->facility.output.fill_rect_function)(handle->output,start_point,square_help,ol);
	}
	else
	{
		/* No, split the square into four new squares. */

		/* Calculate the new square size. */ 
		if(square_size==1)
			new_square_size=0;
		else
		{
			handle->param_help--;
			new_square_size=handle->param_help*2-1;
		}
		
		/* Execute the fill square function for each new square. */
		fill_square(handle,start_point,new_square_size);
		handle->param_help=(new_square_size+1)/2;

		help.x=start_point.x+new_square_size;
		help.y=start_point.y;
		fill_square(handle,help,new_square_size);
		handle->param_help=(new_square_size+1)/2;

		help.x=start_point.x;
		help.y=start_point.y+new_square_size;
		fill_square(handle,help,new_square_size);
		handle->param_help=(new_square_size+1)/2;

		help.x=start_point.x+new_square_size;
		help.y=start_point.y+new_square_size;
		fill_square(handle,help,new_square_size);
		handle->param_help=(new_square_size+1)/2;
	}
}



/* Render the picture by spliting into squares */
void render_recurse(render_t* handle)
{
	/* Volatile datas */
	view_position_t   render_position;
	view_position_t   start_point;
	int               x_square;
	int               y_square;
	int               x_count;
	int               y_count;
	int               square_size;
	ordinal_number_t  *help;

	/* Get memory for points. */
	handle->points=malloc((sizeof(ordinal_number_t))*handle->geometry.width*handle->geometry.height);
	
	/* Calculate square_size. */
	square_size=2*handle->param-1;

	/* Calculate number of squares. */
	x_square=handle->geometry.width/square_size;
	y_square=handle->geometry.height/square_size;
	
	/* Calulate iterations of each square corner. */
	for(render_position.x=0;render_position.x<x_square*square_size;render_position.x+=square_size)
	{
		for(render_position.y=0;render_position.y<y_square*square_size;render_position.y+=square_size)
		{
			help=handle->points
				+render_position.y*handle->geometry.width
				+render_position.x;
			*help=cache_calculator(handle,render_position);
		}
	}

	/* Execute the fill square function for each square. */
	for(x_count=0;x_count<x_square-1;x_count++)
	{
		for(y_count=0;y_count<y_square-1;y_count++)
		{
			/* Calculate the first corner of the square. */
			start_point.x=x_count*square_size;
			start_point.y=y_count*square_size;

			/* Set the correct param into param_help. */
			handle->param_help=handle->param;

			/* Execute the fill square function. */
			fill_square(handle,start_point,square_size);
		}
	}
}

/* Enumerate plugin facilities. */
volatile const plugin_facility_t tinyfract_plugin_facilities[]=
{
	{
		name: "recurse3",
		type: plugin_facility_render,
		facility:
		{
			render:
			{
				constructor:      (const plugin_render_constructor_t*) &constructor,
				destructor:       (const plugin_render_destructor_t*) &destructor,
				render_function:  (const plugin_render_function_t*) &render_recurse,
				cache_calculator: (const plugin_render_calculate_function_t*) &cache_calculator,
				fill_square:      (const plugin_render_fill_square_function_t*) &fill_square
			}
		}
	},
	{ plugin_facility_end }
};

