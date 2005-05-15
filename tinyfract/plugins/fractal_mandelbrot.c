#include <gmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../common.h"
#include "../plugin.h"
#include "../param_parser.h"


/* Data structure for Julia arguments and other volatile data. */
typedef struct
{
	ordinal_number_t iteration_steps;
	complex_number_t C;
	long long int    prec;
} julia_t;

/* Constructor and destructor for Julia fractal. */
static julia_t* constructor_julia(const ordinal_number_t iteration_steps, const char args[], long long int prec)
{
	julia_t* context;
	char*    help;
	char*    real_part;
	char*    imaginary_part;

	help=malloc(sizeof(char)*(prec*2+2));
	imaginary_part=malloc(sizeof(char)*prec+1);
	real_part=malloc(sizeof(char)*prec+1);

	mpf_set_default_prec(sizeof(char)*prec);
	
	
	/* Get memory for the fractal context. */
	if (!(context=malloc(sizeof(julia_t)))) return NULL;
	mpf_init(Im(context->C));
	mpf_init(Re(context->C));

	/* Set the fractal context. */
	context->iteration_steps=iteration_steps;
	
	#if 0
	if (args)
	{
		if (strchr(args, ',')==NULL)
		{
			sscanf(args,"%s", &help);
			mpf_set_str(Re(context->C),help,10);
			mpf_set_str(Im(context->C),"1",10);
		}
		else
		{
			sscanf(args,"%s",help);
			real_part=strtok(help,",");
			imaginary_part=strtok(NULL,"\0");

			mpf_set_str(Re(context->C),real_part,10);
			mpf_set_str(Im(context->C),imaginary_part,10);
		}
	}
	else
	{
		mpf_set_str(Re(context->C),"0",10);
		mpf_set_str(Im(context->C),"1",10);
	}
	#endif

	mpf_set_str(Re(context->C),"0",10);
	mpf_set_str(Im(context->C),"1",10);

	context->prec=prec;
	#ifdef DEBUG 
	gmp_printf("Julia parameter: %F.10f,%F.10f\n",Re(context->C),Im(context->C));
	#endif
	
	/* Return the handle. */
	return context;
}

static void destructor_julia(julia_t* handle)
{
	mpf_clear(Re(handle->C));
	mpf_clear(Im(handle->C));
	free(handle);
}

/* Julia formula: z(0)=p, c=const., z(n+1)=z(n)^2+c */
static ordinal_number_t calculate_julia(julia_t* handle, const complex_number_t* position)
{
	mpf_set_default_prec(sizeof(char)*handle->prec);

	/* Julia fractal constants. */
	real_number_t bailout_square;

	mpf_init(bailout_square);
	mpf_set_str(bailout_square,"4",10);

	/* Three helper variables. */
	real_number_t    radius_square;
	ordinal_number_t step;
	mpf_t            help;

	mpf_init(radius_square);
	mpf_init(help);

	/*
	 * Z stores the complex number during the iterations, Z_helper is a buffer
	 * for the new real part, because the old is required to calculate the new
	 * imaginary part. If you forget this you will get a "squeezed" fractal.
	 */
	complex_number_t Z;
	real_number_t    Z_helper;

	mpf_init(Re(Z));
	mpf_init(Im(Z));
	mpf_init(Z_helper);
	
	/* This ones accelerate the calculation. */
	complex_number_t Z_square;

	mpf_init(Re(Z_square));
	mpf_init(Im(Z_square));

	/*
	 * Julia fractals need this parameter to be set to a fixed number.
	 */
	complex_number_t C=handle->C;

	mpf_init(Re(C));
	mpf_init(Im(C));

	mpf_set(Re(C),handle->C.real_part);
	mpf_set(Im(C),handle->C.imaginary_part);

	
	/* The calculation begins with the a point on the complex plane. */
	VARCOPY(Z,*position);

	/* Now do the iteration. */
	for (step=0;step<handle->iteration_steps;step++)
	{
		/* Calculate Z_square first, as we can use a faster formula then. */
		mpf_mul(Re(Z_square),Re(Z),Re(Z));
		mpf_mul(Im(Z_square),Im(Z),Im(Z));
	
		/* Calculate the radius from complex pane origin to Z. */
		mpf_add(radius_square,Re(Z_square),Im(Z_square));

 		/* Break the iteration if the Julia bailout orbit is left. */
		if (mpf_cmp(radius_square,bailout_square)>0) break;

		/* The fast way to calculate z(n):=z(n+1)^2+c. */
		mpf_sub(help,Re(Z_square),Im(Z_square));
		mpf_add(Z_helper,help,Re(C));
		
		mpf_mul_ui(help,Re(Z),2);
		mpf_mul(help,help,Im(Z));
		mpf_add(Im(Z),help,Im(C));
		
		mpf_set(Re(Z),Z_helper);
	}

	/* Free multiple precision variables. */
	mpf_clear(bailout_square);
	mpf_clear(radius_square);
	mpf_clear(help);
	mpf_clear(Re(Z));
	mpf_clear(Im(Z));
	mpf_clear(Z_helper);
	mpf_clear(Re(Z_square));
	mpf_clear(Im(Z_square));
	mpf_clear(Re(C));
	mpf_clear(Im(C));

	return step; /* Return the iteration step in which we reached the bailout radius. */
}


/* Data structure for Mandelbrot arguments and other volatile data. */
typedef struct
{
	ordinal_number_t iteration_steps;
	long long int    prec;
} mandelbrot_t;

/* Constructor and destructor for Mandelbrot fractal. */
static mandelbrot_t* constructor_mandelbrot(const ordinal_number_t iteration_steps, long long int prec, const char args[])
{
	mandelbrot_t* context;
	
	/* Get memory for the fractal context. */
	if (!(context=malloc(sizeof(mandelbrot_t)))) return NULL;

	/* Set the fractal context. */
	context->iteration_steps=iteration_steps;
	context->prec=prec;

	/* Return the handle. */
	return context;
}

static void destructor_mandelbrot(mandelbrot_t* handle)
{
	free(handle);
}

/* Mandelbrot formula: z(0)=p, z(n+1)=z(n)^2+p */
static ordinal_number_t calculate_mandelbrot(mandelbrot_t* handle, const complex_number_t* position)
{
	/* Set default precision. */
	mpf_set_default_prec(sizeof(char)*handle->prec);
	
	/* Mandelbrot fractal constants. */
	real_number_t bailout_square;
	
	mpf_init(bailout_square);
	mpf_set_str(bailout_square,"4",10);
	
	/* Three helper variables. */
	real_number_t    radius_square;
	ordinal_number_t step;
	mpf_t            help;

	mpf_init(help);
	mpf_init(radius_square);

	/*
	 * Z stores the complex number during the iterations, Z_helper is a buffer
	 * for the new real part, because the old is required to calculate the new
	 * imaginary part. If you forget this you will get a "squeezed" fractal.
	 */
	complex_number_t Z;
	real_number_t    Z_helper;
	
	mpf_init(Re(Z));
	mpf_init(Im(Z));
	mpf_init(Z_helper);
	
	/* This ones accelerate the calculation. */
	complex_number_t Z_square;

	mpf_init(Re(Z_square));
	mpf_init(Im(Z_square));

	/* The calculation begins with the a point on the complex plane. */
	mpf_set(Re(Z),Re(*position));
	mpf_set(Im(Z),Im(*position));

	
	/* Now do the iteration. */
	for (step=0;step<handle->iteration_steps;step++)
	{
		/* Calculate Z_square first, as we can use a faster formula then. */
		mpf_mul(Re(Z_square),Re(Z),Re(Z));
		mpf_mul(Im(Z_square),Im(Z),Im(Z));
	
		/* Calculate the radius from complex pane origin to Z. */
		mpf_add(radius_square,Re(Z_square),Im(Z_square));

 		/* Break the iteration if the Mandelbrot bailout orbit is left. */
		if (mpf_cmp(radius_square,bailout_square)>0) break;

		/* The fast way to calculate z(n):=z(n+1)^2+p. */
		mpf_sub(help,Re(Z_square),Im(Z_square));
		mpf_add(Z_helper,help,Re(*position));
		
		mpf_mul_ui(help,Re(Z),2);
		mpf_mul(help,help,Im(Z));
		mpf_add(Im(Z),help,Im(*position));
		
		mpf_set(Re(Z),Z_helper);
	}

	mpf_clear(Re(Z));
	mpf_clear(Im(Z));
	mpf_clear(Re(Z_square));
	mpf_clear(Im(Z_square));
	mpf_clear(Z_helper);
	mpf_clear(help);
	mpf_clear(bailout_square);

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

