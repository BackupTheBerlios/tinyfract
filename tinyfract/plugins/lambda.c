#include "../common.h"
#include "../plugin.h"


/* Lambda fractal function */
/* Lambda formula: z(0)=p, lambda=const., z(n+1) = lambda*z(n)*(1 - z(n)). */
static ordinal_number_t calculate_lambda(const complex_number_t position, const ordinal_number_t iteration_steps, const ordinal_number_t argc, const real_number_t argv[])
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
	complex_number_t lambda;

	/* Determine if lambda parameter is given. */
	if (argc>1) 
	{
		/* Yes. */
		Re(lambda)=argv[0];
		Im(lambda)=argv[1];
	} else
	{
		/* No, initialize lambda parameter to (1,0). */
		Re(lambda)=1;
		Im(lambda)=0;
	}	

	/* The calculation begins with the a point on the complex plane. */
	VARCOPY(Z,position);

	/* Now do the iteration. */
	for (step=0;step<iteration_steps;step++)
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
		"lambda",
		plugin_facility_fractal,
		{{
			iteration_steps: 170,
			calculate_function: (const plugin_fractal_calculate_function_t*) &calculate_lambda,
		}}
	},
	{ plugin_facility_end }
};

