#ifndef PLUGIN_H
#define PLUGIN_H

#include "common.h"

/* Maximum number of facilities per plugin. */
#define FACILITY_NAME_MAX_SIZE 32


/* Function types for the fractal function plugin.*/
typedef ordinal_number_t (plugin_fractal_calculate_function_t)(const complex_number_t position, const ordinal_number_t iteration_steps, const ordinal_number_t argc, const real_number_t argv[]);


/* Function types for the output function plugin. */
typedef int (output_initialize_view_port_t)(const view_dimension_t dimension, const char *output_args);
typedef int (output_draw_rect_t)(const view_position_t lower_left_corner, const view_position_t upper_right_corner, const pixel_properties value);



typedef struct
{
	const ordinal_number_t                     iteration_steps;
	const plugin_fractal_calculate_function_t* calculate_function;
} plugin_facility_fractal_t;

typedef struct
{
	const void* output_function;
} plugin_facility_output_t;

typedef struct
{
	const void* render_function; 
} plugin_facility_render_t;

typedef union
{
	const plugin_facility_fractal_t fractal;
	const plugin_facility_output_t  output;
	const plugin_facility_render_t  render;
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
	const char                    name[FACILITY_NAME_MAX_SIZE];
	const plugin_facility_enum_t  type;
	const plugin_facility_union_t facility;
} plugin_facility_t;

#endif
