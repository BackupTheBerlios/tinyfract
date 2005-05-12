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
	fprintf(stderr,"Render parameter is: %s\n",args);
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
	/* Volatile data. */
	ordinal_number_t* help;
	complex_number_t  complex_position;
	view_position_t   shift;
	real_number_t     scaling_factor;

	/* Check if the point has been calculated already. */
	help=handle->points+render_position.y*handle->geometry.width+render_position.x;
	if(*help==0)
	{
		/* Has not been calculated till now, calculate the iteration. */

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
	/* Volatile data. */
	ordinal_number_t ul;
	ordinal_number_t ur;
	ordinal_number_t ll;
	ordinal_number_t lr;
	view_position_t  help;
	view_dimension_t square_help;

	/* Calculate iterations for the upper left corner. */
	ul=cache_calculator(handle,start_point);

	/* Calculate iterations for the upper right corner. */
	help.x=start_point.x+square_size-1;
	help.y=start_point.y;
	ur=cache_calculator(handle,help);

	/* Calculate iterations for the fourth corner. */
	help.y=start_point.y+square_size-1;
	lr=cache_calculator(handle,help);
	
	/* Calculate iterations for the lower right corner. */
	help.x=start_point.x;
	ll=cache_calculator(handle,help);

	/* Check if we're at square size two. */
	if (square_size==2)
	{
		/* Yes. Calculate and draw the four pixels. */
		(*handle->output_facility->facility.output.put_pixel_function)(handle->output,start_point,ul);
		help.x=start_point.x+1;
		help.y=start_point.y;
		(*handle->output_facility->facility.output.put_pixel_function)(handle->output,help,ur);
		help.y++;
		(*handle->output_facility->facility.output.put_pixel_function)(handle->output,help,lr);
		help.x--;
		(*handle->output_facility->facility.output.put_pixel_function)(handle->output,help,ll);
		
		/* End of recursion. */
	}
	else
		/* Check if the corners have the same iterations. */
		if(ul==ur && ul==ll && ll==lr)
		{
			/* Yes, print the whole square in the same color. */
			square_help.width=square_size;
			square_help.height=square_size;
			(*handle->output_facility->facility.output.fill_rect_function)(handle->output,start_point,square_help,ul);
		}
		else
		{
			/* No, split the square into four new squares. */
			/* Upper left. */
			fill_square(handle,start_point,square_size/2);

			/* Upper right. */
			help.x=start_point.x+square_size/2;
			help.y=start_point.y;
			fill_square(handle,help,square_size/2);

			/* Lower right. */
			help.y=start_point.y+square_size/2;
			fill_square(handle,help,square_size/2);
			
			/* Lower left. */
			help.x=start_point.x;
			fill_square(handle,help,square_size/2);

			/* End of recursion when all fill_square functions came back. */
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
	square_size=1<<handle->param;

	/* Calculate number of squares. */
	x_square=handle->geometry.width/square_size;
	y_square=handle->geometry.height/square_size;
	
	/* Execute the fill square function for each square. */
	for(x_count=0;x_count<x_square-1;x_count++)
	{
		for(y_count=0;y_count<y_square-1;y_count++)
		{
			/* Calculate the first corner of the square. */
			start_point.x=x_count*square_size;
			start_point.y=y_count*square_size;

			/* Execute the fill square function. */
			fill_square(handle,start_point,square_size);
		}
	}
}

/* Enumerate plugin facilities. */
volatile const plugin_facility_t tinyfract_plugin_facilities[]=
{
	{
		name: "recurse",
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
