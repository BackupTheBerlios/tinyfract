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
} render_dumb_t;

/* Constructor and destructor for dumb render function. */
static render_dumb_t* constructor_dumb(
		const complex_number_t   center,
		const view_dimension_t   geometry,
		const real_number_t      scale,
		const plugin_facility_t* fractal_facility,
		const plugin_facility_t* output_facility,
		const void*              fractal,
		const void*              output)
{
	render_dumb_t* context;
	
	/* Get memory for the fractal context. */
	if (!(context=malloc(sizeof(render_dumb_t)))) return NULL;

	/* Set the fractal context. */
	VARCOPY(context->center,center);
	VARCOPY(context->geometry,geometry);
	VARCOPY(context->scale,scale);
	VARCOPY(context->fractal_facility,fractal_facility);
	VARCOPY(context->output_facility,output_facility);
	VARCOPY(context->fractal,fractal);
	VARCOPY(context->output,output);
	
	/* Return the handle. */
	return context;
}

static void destructor_dumb(render_dumb_t* handle)
{
	free(handle);
}

/* Render the picture pixel by pixel in one thread; pretty dumb :-P */
void render_dumb(render_dumb_t* handle)
{
	complex_number_t complex_position;
	view_position_t  render_position;
	real_number_t    scaling_factor;
	view_position_t  shift;

	/* Precalculate scaling factor and center shift for speed reasons. */
	scaling_factor=handle->scale/handle->geometry.width;
	shift.x=handle->geometry.width/2;
	shift.y=handle->geometry.height/2;
	for(render_position.y=0;render_position.y<handle->geometry.height;render_position.y++)
	{
		for(render_position.x=0;render_position.x<handle->geometry.width;render_position.x++)
		{
			Re(complex_position)=Re(handle->center)+scaling_factor*(render_position.x-shift.x);
			Im(complex_position)=Im(handle->center)-scaling_factor*(render_position.y-shift.y);

			(*handle->output_facility->facility.output.put_pixel_function)
				(handle->output,render_position,(*handle->fractal_facility->facility.fractal.calculate_function)(handle->fractal,complex_position));
		}
	}
}

/* Enumerate plugin facilities. */
volatile const plugin_facility_t tinyfract_plugin_facilities[]=
{
	{
		name: "dumb",
		type: plugin_facility_render,
		facility:
		{
			render:
			{
				constructor:     (const plugin_render_constructor_t*) &constructor_dumb,
				destructor:      (const plugin_render_destructor_t*) &destructor_dumb,
				render_function: (const plugin_render_function_t*) &render_dumb
			}
		}
	},
	{ plugin_facility_end }
};

