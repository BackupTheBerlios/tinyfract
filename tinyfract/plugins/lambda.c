#include <stdlib.h>
#include "../common.h"
#include "../plugin.h"
#include "../param_parser.h"


/* Data structure for Lambda arguments and other volatile data. */
typedef struct
{
	ordinal_number_t iteration_steps;
	complex_number_t lambda;
} lambda_t;

/* Constructor and destructor for Lambda fractal. */
static lambda_t* constructor_lambda(const ordinal_number_t iteration_steps, const char args[])
{
	lambda_t* context;
	
	/* Get memory for the fractal context. */
	if (!(context=malloc(sizeof(lambda_t)))) return NULL;

	/* Set the fractal context. */
	context->iteration_steps=iteration_steps;
	Re(context->lambda)=1;
	Im(context->lambda)=1;

	/* Return the handle. */
	return context;
}

static void destructor_lambda(lambda_t* handle)
{
	free(handle);
}

/* Lambda formula: z(0)=p, lambda=const., z(n+1) = lambda*z(n)*(1 - z(n)). */
static ordinal_number_t calculate_lambda(lambda_t* handle, const complex_number_t position)
{
	/* Lambda fractal constants. */
	const real_number_t bailout_square=4;

	/* Two helper variables. */
	real_number_t    radius_square;
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
	complex_number_t lambda=handle->lambda;

	/* The calculation begins with the a point on the complex plane. */
	VARCOPY(Z,position);

	/* Now do the iteration. */
	for (step=0;step<handle->iteration_steps;step++)
	{
		/* Calculate Z_square first, as we can use a faster formula then. */
		Re(Z_square)=Re(Z)*Re(Z);
		Im(Z_square)=Im(Z)*Im(Z);

		/* Calculate the radius from complex pane origin to Z. */
		radius_square=Re(Z_square)+Im(Z_square);

 		/* Break the iteration if the mandelbrot bailout orbit is left. */
		if (radius_square>bailout_square) break;

		/* Two other speed up variables. */
		ReZs_ImZs=Re(Z_square)-Im(Z_square);
		ReZ_ImZ=Im(Z)*Re(Z);

		/* Now calculate the lambda function. */
		Re(Zn)=Re(lambda)*Re(Z)-Im(lambda)*Im(Z)+Re(lambda)*ReZs_ImZs-2*Im(lambda)*ReZ_ImZ;
		Im(Zn)=Re(lambda)*Im(Z)+Im(lambda)*Re(Z)+Im(lambda)*ReZs_ImZs+2*Re(lambda)*ReZ_ImZ;
	
		/* Copy Zn to Z */
		VARCOPY(Z,Zn);
	}

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

