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
	long long int      prec;
} render_t;

/* Constructor and destructor for dumb render function. */
static render_t* constructor(
		char                     center_real[],
		char                     center_imaginary[],
		const view_dimension_t   geometry,
		char                     scale[],
		const plugin_facility_t* fractal_facility,
		const plugin_facility_t* output_facility,
		const void*              fractal,
		const void*              output,
		long long int            prec)
{
	render_t* context;
	
	/* Get memory for the fractal context. */
	if (!(context=malloc(sizeof(render_t)))) return NULL;

	mpf_set_default_prec(sizeof(char)*prec);

	mpf_init(Re(context->center));
	mpf_init(Im(context->center));
	mpf_init(context->scale);

	/* Set the fractal context. */
	mpf_set_str(Re(context->center),center_real,10);
	mpf_set_str(Im(context->center),center_imaginary,10);
	mpf_set_str(context->scale,scale,10);
	VARCOPY(context->prec,prec);
	VARCOPY(context->geometry,geometry);
	VARCOPY(context->fractal_facility,fractal_facility);
	VARCOPY(context->output_facility,output_facility);
	VARCOPY(context->fractal,fractal);
	VARCOPY(context->output,output);

	#ifdef DEBUG
	gmp_printf("Re: %F.10f, Im: %F.10f\nScale: %F.10f\n", Re(context->center), Im(context->center), context->scale);
	#endif	

	/* Return the handle. */
	return context;
}

static void destructor(render_t* handle)
{
	mpf_clear(Re(handle->center));
	mpf_clear(Im(handle->center));
	mpf_clear(handle->scale);
	free(handle);
}

/* Render the picture pixel by pixel in one thread; pretty dumb :-P */
void render_dumb(render_t* handle)
{
	complex_number_t complex_position;
	view_position_t  render_position;
	real_number_t    scaling_factor;
	view_position_t  shift;
	mpf_t            help;
	mpf_t            help_two;
	ordinal_number_t steps;
	int count=1;


	mpf_set_default_prec(sizeof(char)*handle->prec);

	mpf_init(Re(complex_position));
	mpf_init(Im(complex_position));
	mpf_init(scaling_factor);
	mpf_init(help);
	mpf_init(help_two);

	/* Precalculate scaling factor and center shift for speed reasons. */
	mpf_div_ui(scaling_factor,handle->scale,handle->geometry.width);
	shift.x=handle->geometry.width/2;
	shift.y=handle->geometry.height/2;
	for(render_position.y=0;render_position.y<handle->geometry.height;render_position.y++)
	{
		for(render_position.x=0;render_position.x<handle->geometry.width;render_position.x++)
		{
			//gmp_printf("Help before: %F.10\n", help);
			mpf_set_si(help_two,(render_position.x-shift.x));
			mpf_mul(help,scaling_factor,help_two);
			//gmp_printf("(%d-%d)*%F.10f=%F.10f\n",render_position.x, shift.x, scaling_factor, help);
			mpf_add(Re(complex_position),Re(handle->center),help);

			mpf_set_si(help_two,(render_position.y-shift.y));
			mpf_mul(help,scaling_factor,help_two);
			mpf_sub(Im(complex_position),Im(handle->center),help);

			steps=(*handle->fractal_facility->facility.fractal.calculate_function)(handle->fractal,&complex_position);
			#ifdef DEBUG
			fprintf(stderr,"Steps: %d of Number: %d ", steps, count);
			gmp_printf("On position: %F.10f %F.10f\n", Re(complex_position), Im(complex_position));
			#endif

			(*handle->output_facility->facility.output.put_pixel_function)
				(handle->output,render_position,steps);
			count++;
		}
	}
	mpf_clear(Re(complex_position));
	mpf_clear(Im(complex_position));
	mpf_clear(scaling_factor);
	mpf_clear(help);
	mpf_clear(help_two);
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
				constructor:     (const plugin_render_constructor_t*) &constructor,
				destructor:      (const plugin_render_destructor_t*) &destructor,
				render_function: (const plugin_render_function_t*) &render_dumb
			}
		}
	},
	{ plugin_facility_end }
};

