#ifndef PLUGIN_H
#define PLUGIN_H

#include "common.h"

/* Maximum number of facilities per plugin. */
#define FACILITY_NAME_MAX_SIZE 32



/* Function types for the fractal facility.*/
typedef void* (plugin_fractal_constructor_t)
	(const ordinal_number_t iteration_steps,
	 const char args[]);

typedef void (plugin_fractal_destructor_t)
	(void* handle);

typedef ordinal_number_t (plugin_fractal_calculate_function_t)
	(void* handle,
	 const complex_number_t position);


/* Function types for the output facility. */
typedef void* (plugin_output_constructor_t)
	(const view_dimension_t dimension);

typedef void (plugin_output_destructor_t)
	 (void* handle);

typedef void (plugin_output_blit_rect_function_t)
	(void* handle,
	 const view_position_t position,
	 const view_dimension_t dimension,
	 const pixel_value values[]);

typedef void (plugin_output_fill_rect_function_t)
	(void* handle,
	 const view_position_t position,
	 const view_dimension_t dimension,
	 const pixel_value value);

typedef void (plugin_output_flush_viewport_function_t)
	(void* handle);

typedef void (plugin_output_put_pixel_function_t)
	(void* handle,
	 const view_position_t position,
	 const pixel_value value);


/* Function types for the render facility */
typedef void* (plugin_render_constructor_t) 
	(const complex_number_t   center,
	 const view_dimension_t   geometry,
	 const real_number_t      scale,
	 const void*              fractal_facility,
	 const void*              output_facility,
	 const void*              fractal,
	 const void*              output,
	 const char               args[]
	 );
typedef void (plugin_render_destructor_t)
	 (void* handle);
typedef void (plugin_render_function_t)
	 (void* handle);


/* Plugin facility types. */
typedef struct
{
	plugin_fractal_constructor_t*        constructor;
	plugin_fractal_destructor_t*         destructor;
	plugin_fractal_calculate_function_t* calculate_function;
	complex_number_t                     center;
	ordinal_number_t                     iteration_steps;
	real_number_t                        scale;
} plugin_facility_fractal_t;

typedef struct
{
	plugin_output_constructor_t*             constructor;
	plugin_output_destructor_t*              destructor;
	plugin_output_blit_rect_function_t*      blit_rect_function;
	plugin_output_fill_rect_function_t*      fill_rect_function;
	plugin_output_flush_viewport_function_t* flush_viewport_function;
	plugin_output_put_pixel_function_t*      put_pixel_function;
} plugin_facility_output_t;


typedef struct
{
	plugin_render_constructor_t* constructor;
	plugin_render_destructor_t*  destructor;
	plugin_render_function_t*    render_function;
} plugin_facility_render_t;


typedef union
{
	plugin_facility_fractal_t fractal;
	plugin_facility_output_t  output;
	plugin_facility_render_t  render;
} plugin_facility_union_t;

typedef enum
{
	plugin_facility_end,
	plugin_facility_fractal,
	plugin_facility_output,
	plugin_facility_render
} plugin_facility_enum_t;

typedef struct
{
	char                    name[FACILITY_NAME_MAX_SIZE];
	plugin_facility_enum_t  type;
	plugin_facility_union_t facility;
} plugin_facility_t;

#endif
