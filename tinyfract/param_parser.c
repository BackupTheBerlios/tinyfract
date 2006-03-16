#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "plugin.h"

#if 0
int parse_real_number_list(const char args[], real_number_t* argv[])
{
	int            argc;
	char*          argv_s;
	char*          argv_h;
	real_number_t* argv_p;

	/* Copy the original string, so we can chop it up. */
	argv_s=malloc(strlen(args)+sizeof(char));
	strcpy(argv_s,args);

	/* Initialize the return values. */
	argc=0;
	argv_p=NULL;
	
	/* Chop the input string. */
	argv_h=strtok(argv_s,",");
	while (argv_h!=NULL)
	{
		argc++;
		if ((argv_p=realloc(argv_p,sizeof(real_number_t)*argc))==NULL) return -1;
		sscanf(argv_h,"%lf",&argv_p[argc-1]);
		argv_h=strtok(NULL,",");
	}

	/* Return parameters and paramter count. */
	*argv=argv_p;
	return argc;
}
#endif

int parse_options(complex_number_t* center,char* center_source,real_number_t* scale,char* scale_source,long long int prec)
{
	char* help1;
	char* help2;

	/* Parse center. */
	help1=strtok(center_source,",");
	help2=strtok(NULL,",");

	mpf_set_str(center->real_part,help1,10);
	mpf_set_str(center->imaginary_part,help2,10);

	/* Parse scale. */
	mpf_set_str(*scale,scale_source,10);

	return 0;
}

int make_vinumber(complex_number_t* virtual_position,view_position_t real_position,view_dimension_t geometry,real_number_t scale,complex_number_t center,long long int prec)
{
	real_number_t   scaling_factor;
	view_position_t shift;
	real_number_t   help_two;
	real_number_t   help_one;

	/* Initialize multiple precision variables. */
	mpf_set_default_prec(sizeof(char)*prec);

	mpf_init(scaling_factor);
	mpf_init(help_two);
	mpf_init(help_one);

	/* Precalculate scaling factor and center shift for speed reasons. */
	mpf_div_ui(scaling_factor,scale,geometry.width);
	shift.x=geometry.width/2;
	shift.y=geometry.height/2;

	/* Calculate the virtual number. */
	mpf_set_si(help_two,(real_position.x-shift.x));
	mpf_mul(help_one,scaling_factor,help_two);
	mpf_add(virtual_position->real_part,help_one,Re(center));

	mpf_set_si(help_two,(real_position.y-shift.y));
	mpf_mul(help_one,scaling_factor,help_two);
	mpf_sub(virtual_position->imaginary_part,Im(center),help_one);

	/* Free multiple precision variables. */
	mpf_clear(scaling_factor);
	mpf_clear(help_one);
	mpf_clear(help_two);

	return 0;
}

int calc_movie_params(char* params,long long int prec)
{
	/* Volatile Datas. */
	complex_number_t center1;
	complex_number_t center2;
	real_number_t    scale1;
	real_number_t    scale2;
	complex_number_t center_shift;
	real_number_t    scale_shift;
	real_number_t    calc_help;
	complex_number_t new_center;
	real_number_t    new_scale;
	char*            help;
	char*            add_movie_list_format_string;
	char*            params_tok;
	int              steps;
	unsigned int     i;

	/* Set default precision and initialize the vars. */
	mpf_set_default_prec(sizeof(char)*prec);

	mpf_init(Re(center1));
	mpf_init(Im(center1));
	mpf_init(Re(center2));
	mpf_init(Im(center2));
	fprintf(stderr,"Nach init: %p\n",Im(center2));
	mpf_init(scale1);
	mpf_init(scale2);
	mpf_init(Re(center_shift));
	mpf_init(Im(center_shift));
	mpf_init(scale_shift);
	mpf_init(Re(new_center));
	mpf_init(Im(new_center));
	mpf_init(new_scale);
	mpf_init(calc_help);

	/* Copy the main string into the tok string. */
	params_tok=malloc(sizeof(params));
	strcpy(params_tok,params);

	/* Split the main string and set the vars. */
	help=strtok(params_tok,",");
	mpf_set_str(Re(center1),help,10);
	help=strtok(NULL,",");
	mpf_set_str(Im(center1),help,10);
	help=strtok(NULL,",");
	mpf_set_str(Re(center2),help,10);
	help=strtok(NULL,",");
	mpf_set_str(Im(center2),help,10);
	help=strtok(NULL,",");
	mpf_set_str(scale1,help,10);
	help=strtok(NULL,",");
	mpf_set_str(scale2,help,10);
	help=strtok(NULL,",");
	sscanf(help,"%d",&steps);

	#ifdef DEBUG
	gmp_printf("Center1: %F.100f %F.100f\nCenter2: %F.100f %F.100f\nScale1: %F.100f\nScale2: %F.100f\nSteps: %d\n",Re(center1),Im(center1),Re(center2),Im(center2),scale1,scale2,steps);
	#endif

	/* Get the difference between the single movie fraktals and calculate the shift. */
	/* Center real shift. */
	mpf_sub(calc_help,Re(center2),Re(center1));
	/* The first picture should be the first given picture, so we have to increment the steps. The movie is the one frame longer then requested but */
	/* I think that this is not a very important problem. */
	steps++;
	mpf_div_ui(Re(center_shift),calc_help,steps);

	/* Center imaginary shift. */
	mpf_sub(calc_help,Im(center2),Im(center1));
	mpf_div_ui(Im(center_shift),calc_help,steps);

	/* Scale shift. */
	mpf_sub(calc_help,scale2,scale1);
	mpf_div_ui(scale_shift,calc_help,steps);

	/* Set the format string. */
	add_movie_list_format_string=malloc(sizeof(char)*52+sizeof(long long int)*3);
	sprintf(add_movie_list_format_string,"add_movie_list %%F.%lldf %%F.%lldf %%F.%lldf %%d\n",prec,prec,prec);

	/* Print the new centers and scales. */
	for(i=0;i<=steps;i++)
	{
		/* New center real. */
		mpf_mul_ui(Re(new_center),Re(center_shift),i);
		mpf_add(Re(new_center),Re(center1),Re(new_center));

		/* New center imaginary. */
		mpf_mul_ui(Im(new_center),Im(center_shift),i);
		mpf_add(Im(new_center),Im(new_center),Im(center1));

		/* New scale. */
		mpf_mul_ui(new_scale,scale_shift,i);
		mpf_add(new_scale,new_scale,scale1);

		gmp_printf(add_movie_list_format_string,Re(new_center),Im(new_center),new_scale,steps);
		fflush(stdout);
	}
	
	/* Clear the multiple precision vars. */
	mpf_clear(Re(center1));
	mpf_clear(Im(center1));
	mpf_clear(Re(center2));
	mpf_clear(Im(center2)); // hier gibts immer SEGVs, wenn vorher gerendert wurde
	mpf_clear(scale1);
	mpf_clear(scale2);
	mpf_clear(Re(center_shift));
	mpf_clear(Im(center_shift));
	mpf_clear(scale_shift);
	mpf_clear(Re(new_center));
	mpf_clear(Im(new_center));
	mpf_clear(new_scale);
	mpf_clear(calc_help);

	return 0;
}
