#include <gmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../common.h"
#include "../plugin.h"
#include "../param_parser.h"


/* Data structure for Lambda arguments and other volatile data. */
typedef struct
{
	ordinal_number_t iteration_steps;
	complex_number_t lambda;
	long long int    prec;
} lambda_t;

/* Constructor and destructor for Lambda fractal. */
static lambda_t* constructor_lambda(const ordinal_number_t iteration_steps, long long int prec, const char args[])
{
	lambda_t* context;
	char*     real_param;
	char*     imaginary_param;
	char*     args_help;
	
	/* Get memory for the fractal context. */
	if (!(context=malloc(sizeof(lambda_t)))) return NULL;

	mpf_set_default_prec(sizeof(char)*prec);

	mpf_init(Re(context->lambda));
	mpf_init(Im(context->lambda));

	/* Set the fractal context. */
	context->iteration_steps=iteration_steps;
	
	if(args!=NULL)
	{
		if (strchr(args, ',')==NULL)
		{
			real_param=malloc(sizeof(char)*(prec+1));	
			sscanf(args,"%s",real_param);
			mpf_set_str(Re(context->lambda),real_param,10);
			mpf_set_str(Im(context->lambda),"0",10);
			free(real_param);
		}
		else
		{
			args_help=malloc(sizeof(args));
			strcpy(args_help,args);
			real_param=strtok(args_help,",");
			imaginary_param=strtok(NULL,"\0");

			mpf_set_str(Re(context->lambda),real_param,10);
			mpf_set_str(Im(context->lambda),imaginary_param,10);
			free(args_help);
		}
	}
	else
	{
		mpf_set_si(Re(context->lambda),1);
		mpf_set_si(Im(context->lambda),0);
	}

	context->prec=prec;

 	#ifdef DEBUG 
	gmp_fprintf(stderr,"Lambda parameter: %F.10f,%F.10f\n",Re(context->lambda),Im(context->lambda));
	#endif

	/* Return the handle. */
	return context;
}

static void destructor_lambda(lambda_t* handle)
{
	mpf_clear(Re(handle->lambda));
	mpf_clear(Im(handle->lambda));
	free(handle);
}

/* Lambda formula: z(0)=p, lambda=const., z(n+1) = lambda*z(n)*(1 - z(n)). */
static ordinal_number_t calculate_lambda(lambda_t* handle, const complex_number_t* position)
{


	/* Lambda fractal constants. */
	real_number_t bailout_square;

	/* Three helper variables. */
	real_number_t    radius_square;
	real_number_t    help;
	real_number_t    help_two;
	ordinal_number_t step;
	
	/*
	 * Z stores the complex number during the iterations, Zn is the next iteration step.
	 */
	complex_number_t Z;
	complex_number_t Zn;

	/* These ones accelerate the calculation. */
	complex_number_t Z_square;
	real_number_t    ReZs_ImZs;
	real_number_t    ReZ_ImZ;

	/* Lambda parameter. */
	complex_number_t lambda;

	/* Init multiple precision vars. */
	mpf_set_default_prec(sizeof(char)*handle->prec);
	
	mpf_init(bailout_square);
	mpf_init(Re(lambda));
	mpf_init(Im(lambda));
	mpf_init(Re(Z_square));
	mpf_init(Im(Z_square));
	mpf_init(ReZs_ImZs);
	mpf_init(ReZ_ImZ);
	mpf_init(Re(Z));
	mpf_init(Im(Z));
	mpf_init(Re(Zn));
	mpf_init(Im(Zn));
	mpf_init(radius_square);
	mpf_init(help);
	mpf_init(help_two);

	/* Set the bailout square constant. */
	mpf_set_d(bailout_square,4);
	
	mpf_set(Re(lambda),Re(handle->lambda));
	mpf_set(Im(lambda),Im(handle->lambda));

	/* The calculation begins with the a point on the complex plane. */
	mpf_set(Re(Z),position->real_part);
	mpf_set(Im(Z),position->imaginary_part);

	//fprintf(stderr,"Hallo\n");
	/* Now do the iteration. */
	for (step=0;step<handle->iteration_steps;step++)
	{
		/* Calculate Z_square first, as we can use a faster formula then. */
		mpf_mul(Re(Z_square),Re(Z),Re(Z));
		mpf_mul(Im(Z_square),Im(Z),Im(Z));

		/* Calculate the radius from complex pane origin to Z. */
		mpf_add(radius_square,Re(Z_square),Im(Z_square));

 		/* Break the iteration if the mandelbrot bailout orbit is left. */
		if (mpf_cmp(radius_square,bailout_square)>0) break;

		/* Two other speed up variables. */
		mpf_sub(ReZs_ImZs,Re(Z_square),Im(Z_square));
		mpf_mul(ReZ_ImZ,Im(Z),Re(Z));

		/* Now calculate the lambda function. */
		mpf_mul(help,Re(lambda),Re(Z));
		mpf_mul(help_two,Im(lambda),Im(Z));
		mpf_sub(help,help,help_two);
		mpf_mul(help_two,Re(lambda),ReZs_ImZs);
		mpf_add(help,help,help_two);
		mpf_mul_ui(help_two,Im(lambda),2);
		mpf_mul(help_two,help_two,ReZ_ImZ);
		mpf_sub(Re(Zn),help,help_two);

		mpf_mul(help,Re(lambda),Im(Z));
		mpf_mul(help_two,Im(lambda),Re(Z));
		mpf_add(help,help,help_two);
		mpf_mul(help_two,Im(lambda),ReZs_ImZs);
		mpf_add(help,help,help_two);
		mpf_mul_ui(help_two,Re(lambda),2);
		mpf_mul(help_two,help_two,ReZ_ImZ);
		mpf_add(Im(Zn),help,help_two);

		/* Copy Zn to Z */
		mpf_set(Re(Z),Re(Zn));
		mpf_set(Im(Z),Im(Zn));
	}

	/* Clear multiple precision Variables. */
	mpf_clear(bailout_square);
	mpf_clear(radius_square);
	mpf_clear(help);
	mpf_clear(help_two);
	mpf_clear(Re(Z));
	mpf_clear(Im(Z));
	mpf_clear(Re(Zn));
	mpf_clear(Im(Zn));
	mpf_clear(Re(Z_square));
	mpf_clear(Im(Z_square));
	mpf_clear(ReZs_ImZs);
	mpf_clear(ReZ_ImZ);
	mpf_clear(Re(lambda));
	mpf_clear(Im(lambda));

	return step; /* Return the iteration step in which we reached the bailout radius. */
}


/* Enumerate plugin facilities. */
volatile const plugin_facility_t tinyfract_plugin_facilities[]=
{
	{
		name: "lambda",
		type: plugin_facility_fractal,
		facility:
		{
			fractal:
			{
				constructor:        (const plugin_fractal_constructor_t*) &constructor_lambda,
				destructor:         (const plugin_fractal_destructor_t*) &destructor_lambda,
				calculate_function: (const plugin_fractal_calculate_function_t*) &calculate_lambda,
				center: {0,0},
				iteration_steps: 200,
				scale: 4
			}
		}
	},
	{ plugin_facility_end }
};

