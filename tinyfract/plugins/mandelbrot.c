#include <stdlib.h>
#include <stdio.h>
#include "../common.h"
#include "../plugin.h"
#include "../param_parser.h"


/* Data structure for Julia arguments and other volatile data. */
typedef struct
{
	ordinal_number_t iteration_steps;
	complex_number_t C;
} julia_t;

/* Constructor and destructor for Julia fractal. */
static julia_t* constructor_julia(const ordinal_number_t iteration_steps, const char args[])
{
	julia_t* context;
	
	/* Get memory for the fractal context. */
	if (!(context=malloc(sizeof(julia_t)))) return NULL;

	/* Set the fractal context. */
	context->iteration_steps=iteration_steps;
	Re(context->C)=0;
	Im(context->C)=1;

	/* Return the handle. */
	return context;
}

static void destructor_julia(julia_t* handle)
{
	free(handle);
}

/* Julia formula: z(0)=p, c=const., z(n+1)=z(n)^2+c */
static ordinal_number_t calculate_julia(julia_t* handle, const complex_number_t position)
{
	/* Julia fractal constants. */
	const real_number_t bailout_square=4;

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
	 * Julia fractals need this parameter to be set to a fixed number.
	 */
	complex_number_t C=handle->C;

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

 		/* Break the iteration if the Julia bailout orbit is left. */
		if (radius_square>bailout_square) break;

		/* The fast way to calculate z(n):=z(n+1)^2+c. */
		Z_helper=Re(Z_square)-Im(Z_square)+Re(C);
		Im(Z)=2*Re(Z)*Im(Z)+Im(C);
		Re(Z)=Z_helper;
	}

	return step; /* Return the iteration step in which we reached the bailout radius. */
}


/* Data structure for Mandelbrot arguments and other volatile data. */
typedef struct
{
	ordinal_number_t iteration_steps;
} mandelbrot_t;

/* Constructor and destructor for Mandelbrot fractal. */
static mandelbrot_t* constructor_mandelbrot(const ordinal_number_t iteration_steps)
{
	mandelbrot_t* context;
	
	/* Get memory for the fractal context. */
	if (!(context=malloc(sizeof(mandelbrot_t)))) return NULL;

	/* Set the fractal context. */
	context->iteration_steps=iteration_steps;

	/* Return the handle. */
	return context;
}

static void destructor_mandelbrot(mandelbrot_t* handle)
{
	free(handle);
}

/* Mandelbrot formula: z(0)=p, z(n+1)=z(n)^2+p */
static ordinal_number_t calculate_mandelbrot(mandelbrot_t* handle, const complex_number_t position)
{
	/* Mandelbrot fractal constants. */
	const real_number_t bailout_square=4;

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

 		/* Break the iteration if the Mandelbrot bailout orbit is left. */
		if (radius_square>bailout_square) break;

		/* The fast way to calculate z(n):=z(n+1)^2+p. */
		Z_helper=Re(Z_square)-Im(Z_square)+Re(position);
		Im(Z)=2*Re(Z)*Im(Z)+Im(position);
		Re(Z)=Z_helper;
	}

	return step; /* Return the iteration step in which we reached the bailout radius. */
}


/* Enumerate plugin facilities. */
volatile const plugin_facility_t tinyfract_plugin_facilities[]=
{
	{
		name: "julia",
		type: plugin_facility_fractal,
		facility:
		{
			fractal:
			{
				constructor:        (const plugin_fractal_constructor_t*) &constructor_julia,
				destructor:         (const plugin_fractal_destructor_t*) &destructor_julia,
				calculate_function: (const plugin_fractal_calculate_function_t*) &calculate_julia,
				center: {0,0},
				iteration_steps: 200,
				scale: 4
			}
		}
	},
	{
		name: "mandelbrot",
		type: plugin_facility_fractal,
		facility:
		{
			fractal:
			{
				constructor:        (const plugin_fractal_constructor_t*) &constructor_mandelbrot,
				destructor:         (const plugin_fractal_destructor_t*) &destructor_mandelbrot,
				calculate_function: (const plugin_fractal_calculate_function_t*) &calculate_mandelbrot,
				center: {0,0},
				iteration_steps: 200,
				scale: 4
			}
		}
	},
	{ plugin_facility_end }
};

