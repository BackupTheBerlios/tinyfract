#include <aalib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../common.h"
#include "../plugin.h"


typedef struct
{
	aa_context* context_aa; /* Information about currently initialized device.  */
	aa_palette  palette;    /* Emulated palette (optional). */
} aa_t;	

/* Constructor and destructor for asciiart output. */
static aa_t* constructor_aa(const view_dimension_t dimension, const char args[])
{
	aa_t* context;
	struct aa_hardware_params hardware_params;
  		
	/* Get memory for the output context. */
	if (!(context=malloc(sizeof(aa_t)))) return NULL;

	/* Parse options for aalib given in AAOPTS environment variable. */
	if (!aa_parseoptions(NULL,NULL,NULL,NULL))
        {
		#ifdef DEBUG
		fprintf(stderr,"aa: error parsing options, %s\n",aa_help);
		#endif
		return NULL;
	}

	/* Initialize the viewport. */
	VARCOPY(hardware_params,aa_defparams);
  	hardware_params.width=dimension.width/2;	
  	hardware_params.height=dimension.width/2;	
	if ((context->context_aa=aa_autoinit(&hardware_params))==NULL)
        {
		#ifdef DEBUG
		fprintf(stderr,"aa: error initializing viewport, %s\n",aa_help);
		#endif
		return NULL;
	}

	/* Hide the cursor if necessary/possible. */
	aa_hidecursor(context->context_aa);

	/* Return the handle. */
	return context;
}

void destructor_aa(aa_t* handle)
{
	aa_close(handle->context_aa);
	free(handle);
}

unsigned char colormap(const pixel_value value)
{
	return (255-(value*10))&0xff;
}

/* Blit rectangle from pixelbuffer to asciiart viewport. */
void blit_rect_aa(aa_t* handle, const view_position_t position, const view_dimension_t dimension, pixel_value values[])
{
}


/* Fill rectangle in asciiart viewport with color. */
void fill_rect_aa(aa_t* handle, const view_position_t position, const view_dimension_t dimension, const pixel_value value)
{
	unsigned int x,y;

	for (y=position.y;y<(position.y+dimension.height);y++)
		for (x=position.x;x<(position.x+dimension.width);x++)
			aa_putpixel(handle->context_aa,x,y,colormap(value));
}

/* Flush asciiart viewport */
void flush_viewport_aa(aa_t* handle, button_event_t* position)
{
	aa_render(handle->context_aa,&aa_defrenderparams,0,0,aa_imgwidth(handle->context_aa),aa_imgheight(handle->context_aa));
	aa_flush(handle->context_aa);
	position->type=autozoom_do_nothing;
}

/* Put pixel into asciiart viewport. */
void put_pixel_aa(aa_t* handle, const view_position_t position, const pixel_value value)
{
	aa_putpixel(handle->context_aa,position.x,position.y,colormap(value));
}


/* Enumerate plugin facilities. */
volatile const plugin_facility_t tinyfract_plugin_facilities[]=
{
	{
		name: "aa",
		type: plugin_facility_output,
		facility:
		{
			output:
			{
				constructor:             (const plugin_output_constructor_t*) &constructor_aa,
				destructor:              (const plugin_output_destructor_t*) &destructor_aa,
				blit_rect_function:      (const plugin_output_blit_rect_function_t*) &blit_rect_aa,
				fill_rect_function:      (const plugin_output_fill_rect_function_t*) &fill_rect_aa,
				flush_viewport_function: (const plugin_output_flush_viewport_function_t*) &flush_viewport_aa,
				put_pixel_function:      (const plugin_output_put_pixel_function_t*) &put_pixel_aa,
			}
		}
	},
	{ type: plugin_facility_end }
};

