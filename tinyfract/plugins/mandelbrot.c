#include "common.h"

/* Mandelbrot fractal function */
real_number_t plugin_mandelbrot_calculate(const complex_number_t position, const ordinal_number_t iteration_steps, const ordinal_number_t argc, const real_number_t argv[])
{
	/* Mandelbrot fractal constants. */
	const real_number_t  bailout_square=4;

	/* Two helper variables. */
	real_number_t    radius_square;
	ordinal_number_t step;

	/*
	 * Z stores the complex number during the iterations, Z_helper is a buffer
	 * for the new real part, because the old is required to calculate the new
	 * imaginary part. If you forget this you will get a "squeezed" fractal.
	 */
	complex_number_t Z;
	real_number_t    Z_helper;

	/* This ones accelerate the calculation. */
	complex_number_t Z_square;

	/*
	 * For the Mandelbrot fractal the parameter C is set to the starting point
	 * on the complex plane. Julia fractals needs this parameter to be set to
	 * a fixed number.
     */
	complex_number_t C;

	/* Determine if Julia mode is requested. */
	if (argc>1) 
	{
		/* Yes, Julia mode */
		Re(C)=argv[0];
		Im(C)=argv[1];
	}
	else VARCOPY(C,position); /* No, Mandelbrot mode */

	/* The calculation begins with the a point on the complex plane. */
	VARCOPY(Z,position);

	/* Now do the iteration. */
	for (step=0;step<iteration_steps;step++)
	{
		/* Calculate Z_square first, as we can use a faster formula then. */
		Re(Z_square)=Re(Z)*Re(Z);
		Im(Z_square)=Im(Z)*Im(Z);
	
		/* Calculate the radius from complex pane origin to Z */
		radius_square=Re(Z_square)+Im(Z_square);

 		/* Break the iteration if the mandelbrot bailout orbit is left */
		if (radius_square>bailout_square) break;

		/* The fast way to calculate Z:=Z^2+C */
		Z_helper=Re(Z_square)-Im(Z_square)+Re(C);
		Im(Z)=2*Re(Z)*Im(Z)+Im(C);
		Re(Z)=Z_helper;
	}

	return (real_number_t) step; /* Return the iteration step which reached the bailout radius */
}
