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
	real_number_t center1_real;
	real_number_t center1_imaginary;
	real_number_t center2_real;
	real_number_t center2_imaginary;
	real_number_t scale1;
	real_number_t scale2;
	real_number_t center_real_shift;
	real_number_t center_imaginary_shift;
	real_number_t scale_shift;
	real_number_t calc_help;
	real_number_t new_center_real;
	real_number_t new_center_imaginary;
	real_number_t new_scale;
	char*         help;
	char*         format_string;
	char*         params_tok;
	unsigned int  steps;
	unsigned int  i;

	/* Set default precision and initialize the vars. */
	mpf_set_default_prec(sizeof(char)*prec);

	mpf_init(center1_real);
	mpf_init(center1_imaginary);
	mpf_init(center2_real);
	mpf_init(center2_imaginary);
	mpf_init(scale1);
	mpf_init(scale2);
	mpf_init(center_real_shift);
	mpf_init(center_imaginary_shift);
	mpf_init(scale_shift);
	mpf_init(calc_help);
	mpf_init(new_center_real);
	mpf_init(new_center_imaginary);
	mpf_init(new_scale);

	/* Copy the main string into the tok string. */
	params_tok=malloc(sizeof(params));
	strcpy(params_tok,params);

	/* Split the main string and set the vars. */
	help=strtok(params_tok,",");
	mpf_set_str(center1_real,help,10);
	help=strtok(NULL,",");
	mpf_set_str(center1_imaginary,help,10);
	help=strtok(NULL,",");
	mpf_set_str(center2_real,help,10);
	help=strtok(NULL,",");
	mpf_set_str(center2_imaginary,help,10);
	help=strtok(NULL,",");
	mpf_set_str(scale1,help,10);
	help=strtok(NULL,",");
	mpf_set_str(scale2,help,10);
	help=strtok(NULL,",");
	sscanf(help,"%d",&steps);

	#ifdef DEBUG
	fprintf(stderr,"Steps: %d\n",steps);
	#endif

	/* Get the difference between the single movie fraktals and calculate the shift. */
	/* Center real shift. */
	mpf_sub(calc_help,center2_real,center1_real);
	mpf_div_ui(center_real_shift,calc_help,steps);

	/* Center imaginary shift. */
	mpf_sub(calc_help,center2_imaginary,center1_imaginary);
	mpf_div_ui(center_imaginary_shift,calc_help,steps);

	/* Scale shift. */
	mpf_sub(calc_help,scale2,scale1);
	mpf_div_ui(scale_shift,calc_help,steps);

	/* Set the format string. */
	format_string=malloc(sizeof(char)*32+sizeof(long long int)*3);
	sprintf(format_string,"add_movie_list %%F.%lldf %%F.%lldf %%F.%lldf %%d\n",prec,prec,prec);

	/* Print the new centers and scales. */
	for(i=1;i<=steps;i++)
	{
		/* New center real. */
		mpf_mul_ui(new_center_real,center_real_shift,i);
		mpf_add(new_center_real,center1_real,new_center_real);

		/* New center imaginary. */
		mpf_mul_ui(new_center_imaginary,center_imaginary_shift,i);
		mpf_add(new_center_imaginary,new_center_imaginary,center1_imaginary);

		/* New scale. */
		mpf_mul_ui(new_scale,scale_shift,i);
		mpf_add(new_scale,new_scale,scale1);

		#ifdef DEBUG
		fprintf(stderr,"Step %d:", i);
		#endif

		gmp_printf(format_string,new_center_real,new_center_imaginary,new_scale,steps);
		fflush(stdout);
	}

	/* Clear the multiple precision vars. */
	mpf_clear(center1_real);
	mpf_clear(center1_imaginary);
	mpf_clear(center2_real);

	mpf_clear(center2_imaginary);
	fprintf(stderr,"Hallo\n");

	mpf_clear(scale1);
	mpf_clear(scale2);
	mpf_clear(center_real_shift);
	mpf_clear(center_imaginary_shift);
	mpf_clear(scale_shift);
	mpf_clear(new_center_real);
	mpf_clear(new_center_imaginary);
	mpf_clear(new_scale);
	mpf_clear(calc_help);

	return 0;
}

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
		button_event_t*          flush_button_press)
{
	void* render;	/* Handle for render facility. */

	/* Initialize the render facility. */
	#ifdef DEBUG
	fprintf(stderr,"Initializing render facility.\n");
	#endif
	if (!(render=(*render_facility->facility.render.constructor)
		(render_center,
		 render_geometry,
		 render_scale,
		 render_fractal_facility,render_flush_output_facility,render_fractal,render_output,render_args,render_prec)))
	{
		perror("Could not initialize render facility");
		exit(EXIT_FAILURE);
	}

	/* Render the fractal. */
	#ifdef DEBUG
	fprintf(stderr,"Rendering the fractal.\n");
	#endif
	(*render_facility->facility.render.render_function)(render);

	/* Flush and close the output viewport. */
	#ifdef DEBUG
	fprintf(stderr,"Flushing viewport.\n");
	#endif
	(*render_flush_output_facility->facility.output.flush_viewport_function)(flush_output,flush_button_press);

	/* Free the render facility used. */
	#ifdef DEBUG
	fprintf(stderr,"Closing render facility.\n");
	#endif
	(*render_facility->facility.render.destructor)(render);

	return 0;
}
