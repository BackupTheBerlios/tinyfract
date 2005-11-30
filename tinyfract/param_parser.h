#ifndef PARAM_PARSER_H
#define PARAM_PARSER_H

#include "common.h"
#include "plugin.h"

int parse_real_number_list(const char args[], real_number_t* argv[]);

int parse_options(complex_number_t* center,char* center_source,real_number_t* scale,char* scale_source,long long int prec);

int make_vinumber(complex_number_t* virtual_position,view_position_t real_position,view_dimension_t geometry,real_number_t scale,complex_number_t center,long long int prec);

int calc_movie_params(char* params,long long int prec);

int render_and_flush(
			const plugin_facility_t* render_facility,
			complex_number_t         render_center,
			view_dimension_t         render_geometry,
			real_number_t            render_scale,
			const plugin_facility_t* render_fractal_facility,
			const plugin_facility_t* render_flush_output_facility,
			void*                    render_fractal,
			void*                    render_output,
			char                     render_args[],
			long long int            render_prec,
			void*                    flush_output,
			button_event_t*          flush_button_press);

#endif

