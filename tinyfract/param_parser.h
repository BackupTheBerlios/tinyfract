#ifndef PARAM_PARSER_H
#define PARAM_PARSER_H

#include "common.h"

int parse_real_number_list(const char args[], real_number_t* argv[]);

int parse_options(complex_number_t* center,char* center_source,real_number_t* scale,char* scale_source,long long int prec);

int make_vinumber(complex_number_t* virtual_position,view_position_t real_position,view_dimension_t geometry,real_number_t scale,complex_number_t center,long long int prec);

#endif

