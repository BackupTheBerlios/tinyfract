/*
 * This software and its doumentation may be freely distributed under
 * the terms of the GNU General Public License(GPL), Version 2
 * See http://www.fsf.org/licenses/gpl.txt for details.
 *
 * (C)2004 by Jan Kandziora <jjj@gmx.de>
 */

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "common.h"
#include "param_parser.h"
#include "plugin.h"
#include "plugin_loader.h"


/* Global variables */




/*
 * Our main function does nothing more than parsing options and checking the
 * range of their values, then loads the desired fractal and output function
 * from a plugin and calls render to actually do the job.
 */
int main(int argc, char* argv[])
{
	void*                    fractal;             /* Handle for fractal facility. */
	char*                    fractal_type=NULL;   /* Fractal type selected from command line or user input. */
	const plugin_facility_t* fractal_facility;    /* Constant of fractal plugin facilities. */
	char*                    fractal_args=NULL;   /* Fractal parameters from command line or user input. */

	void*                    output;              /* Handle for output facility. */
	char*                    output_method=NULL;  /* Output method selected from command line or user input. */
	const plugin_facility_t* output_facility;     /* Output plugin facility */
	char*                    output_args=NULL;    /* Output parameters from command line or user input. */

	void*                    render;              /* Handle for render facility. */
	char*                    render_method=NULL;  /* Render method selected from command line or user input. */
	const plugin_facility_t* render_facility;     /* Render plugin facility */
	char*                    render_args=NULL;    /* Render parameters from command line or user input. */
	
	/* Variables and constants for option parsing. */
	complex_number_t center;
	int              flags=0;
	view_dimension_t geometry;
	ordinal_number_t iteration_steps;
	real_number_t    scale;
        char*            plugin_path=NULL;

	const int CENTER_SET=1;
	const int GEOMETRY_SET=2;
	const int ITERATION_STEPS_SET=4;
	const int SCALE_SET=8;

	
	int c;
	int option_index=0;
	static struct option long_options[] =
	{
		{"center=",             1,0,'c'},
		{"fractal-type=",       1,0,'f'},
		{"fractal-parameters=", 1,0,'F'},
		{"geometry=",           1,0,'g'},
		{"iteration-steps=",    1,0,'i'},
		{"output-method=",      1,0,'o'},
		{"output-parameters=",  1,0,'O'},
		{"plugin-path=",        1,0,'P'},
		{"render-method=",      1,0,'r'},
		{"render-parameters=",  1,0,'R'},
		{"scale=",              1,0,'s'},
		{"help",                0,0,'?'},
		{"version",             0,0,'V'},
		{0,0,0,0}
	};


	/* Parse options. */
	for (;;)
	{
		c=getopt_long(argc,argv,"c:f:F:g:i:o:O:P:r:R:s:",long_options,&option_index);
		if (c==-1) break;

		switch (c)
		{
			case 'c':
				sscanf(optarg,"%lf,%lf",&Re(center),&Im(center));
				flags|=CENTER_SET;
				break;
			case 'f':
				fractal_type=optarg;
				break;
			case 'F':
				fractal_args=optarg;
				break;
			case 'g':
				sscanf(optarg,"%ux%u",&geometry.width,&geometry.height);
				flags|=GEOMETRY_SET;
				break;
			case 'i':
				sscanf(optarg,"%d",&iteration_steps);
				flags|=ITERATION_STEPS_SET;
				break;
			case 'o':
				output_method=optarg;
				break;
			case 'O':
				output_args=optarg;
				break;
			case 'P':
				plugin_path=optarg;
				break;
			case 'r':
				render_method=optarg;
				break;
			case 'R':
				render_args=optarg;
				break;
			case 's':
				sscanf(optarg,"%lf",&scale);
				flags|=SCALE_SET;
				break;
			case 'V':
				fprintf(stderr,
					"%s: Version %s-%s, build by %s at %s on %s\n"
					"(C)2004 Jan Kandziora <jjj@gmx.de>, Copy left by the means of GNU GPL v2\n"
					"See http://www.gnu.org/licenses/gpl.txt for details.\n"
					,argv[0],VERSION_INFO,BUILD_INFO,BUILD_USER,BUILD_DATE,BUILD_HOST);
				exit(EXIT_FAILURE);
			case '?':
			default:
				fprintf(stderr,
					"(C)2004 Jan Kandziora <jjj@gmx.de>, Copy left by the means of GNU GPL v2\n"
					"See http://www.gnu.org/licenses/gpl.txt for details.\n\n"
					"USAGE: %s [OPTIONS]\n"
					"  -c, --center=             Center of the picture in the complex plane. Format: Re,Im.\n"
					"                              Defaults to fractal function's choice.\n"
					"  -f, --fractal-type=       Select the fractal formula. A plugin has to provide the\n"
					"                              formula. Default is prompting the user.\n"
					"  -F, --fractal-parameters= Specifies additional fractal parameters, if the\n"
					"                              fractal formula needs any.\n"
					"  -g, --geometry=           Output picture size in pixels. Format: Width x Height.\n"
					"                              Defaults to output plugin's choice.\n"
					"  -i, --iteration-steps=    Sets the maximum count of iterations per point.\n"
					"                              Defaults to fractal function's choice.\n"
					"  -o, --output-method=      Specifies the output method. A plugin has to provide it.\n"
					"                              If no output method is given a default value from the\n"
					"                              environment variable TINYFRACT_OUTPUT_METHOD will be used.\n"
					"  -O, --output-parameters=  Specifies additional parameters passed to the output\n"
					"                              function.\n"
					"  -P, --plugin-path=        Specifies a path to search for plugins. If no plugin path\n"
					"                              is given a default value from the environment variable\n"
					"                              TINYFRACT_PLUGIN_PATH will be used.\n"
					"  -r, --render-method=      Specifies the render method. A plugin has to provide it.\n"
					"  -R, --render-parameters=  Specifies additional parameters passed to the render\n"
					"                              function.\n"
					"  -s, --scale=              Scale: Baseline with of the rendered picture in the complex\n"
					"                              coordinate system. Defaults to fractal function's choice.\n"
					"      --help                Show this help and exit.\n"
					"      --version             Show version information and exit.\n\n"
					"Example: %s -fmandelbrot --geom=640x480 -i170 -c-0.5,0.8 -s1.3\n"
					,argv[0],argv[0]); 
				exit(EXIT_FAILURE);
		}
	}

#ifdef DEBUG
	fprintf(stderr,"Command line parameters are:\n");
	fprintf(stderr,"\tcenter=%f;%f\n",Re(center),Im(center));
	fprintf(stderr,"\tfractal-type=%s\n",fractal_type);
	fprintf(stderr,"\tfractal-parameters=%s\n",fractal_args);
	fprintf(stderr,"\tgeometry=%dx%d\n",geometry.width,geometry.height);
	fprintf(stderr,"\titeration-steps=%d\n",iteration_steps);
	fprintf(stderr,"\toutput-method=%s\n",output_method);
	fprintf(stderr,"\tscale=%lf\n",scale);
#endif

	/* Test if plugin path was given. */
	if (!plugin_path)
	{
		if (!(plugin_path=getenv("TINYFRACT_PLUGIN_PATH")))
		{
			fprintf(stderr,"%s: You have to specify a plugin path.\n",argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	
	/* Test if output method was given. */
	if (!output_method)
	{
		if (!(output_method=getenv("TINYFRACT_OUTPUT_METHOD")))
		{
			fprintf(stderr,"%s: You have to specify a output method.\n",argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	
	/* Test if fractal type was given. */
	if (!fractal_type)
	{
		fprintf(stderr,"%s: You have to specify a fractal type.\n",argv[0]);
		exit(EXIT_FAILURE);
	}

	/* Test if render was given. */
	if (!render_method)
	{
		if (!(render_method=getenv("TINYFRACT_RENDER_METHOD")))
		{
			fprintf(stderr,"%s: You have to specify a render method.\n",argv[0]);
			exit(EXIT_FAILURE);
		}
	}

	/* Load fractal facility. */
	if (!(fractal_facility=load_plugin_facility(plugin_path,plugin_facility_fractal,fractal_type)))
	{
		fprintf(stderr,"%s: Could not load fractal facility %s.\n",argv[0],fractal_type);
		exit(EXIT_FAILURE);
	}

	/* Initialize the fractal facility. */
	#ifdef DEBUG
	fprintf(stderr,"Initializing fractal facility.\n");
	#endif
	
	if (!(fractal=(*fractal_facility->facility.fractal.constructor)
		((flags & ITERATION_STEPS_SET?iteration_steps:fractal_facility->facility.fractal.iteration_steps),fractal_args)))
	{
		perror("could not initialize fractal facility");
		exit(EXIT_FAILURE);
	}

	/* Load output facility. */
	if (!(output_facility=load_plugin_facility(plugin_path,plugin_facility_output,output_method)))
	{
		fprintf(stderr,"%s: Could not load output facility %s.\n",argv[0],output_method);
		exit(EXIT_FAILURE);
	}

	/* Initialize the output facility. */
	#ifdef DEBUG
	fprintf(stderr,"Initializing output facility.\n");
	#endif
	
	if (!(output=(*output_facility->facility.output.constructor)(geometry,output_args)))
	{
		perror("could not initialize output facility");
		exit(EXIT_FAILURE);
	}

	/* Load render facility. */
	if (!(render_facility=load_plugin_facility(plugin_path,plugin_facility_render,render_method)))
	{
		fprintf(stderr,"%s: Could not load render facility %s.\n",argv[0],render_method);
		exit(EXIT_FAILURE);
	}

	/* Initialize the render facility. */
	#ifdef DEBUG
	fprintf(stderr,"Initializing render facility.\n");
	#endif
	if (!(render=(*render_facility->facility.render.constructor)
		(flags & CENTER_SET?center:fractal_facility->facility.fractal.center,
		 geometry,
		 flags & SCALE_SET?scale:fractal_facility->facility.fractal.scale,
		 fractal_facility,output_facility,fractal,output,render_args)))
	{
		perror("could not initialize render facility");
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
	(*output_facility->facility.output.flush_viewport_function)(output);

	/* Sleep a while. */
	#ifdef DEBUG
	fprintf(stderr,"Waiting.\n");
	#endif

	sleep(10);

	/* Free the render facility used. */
	#ifdef DEBUG
	fprintf(stderr,"Closing render facility.\n");
	#endif
	(*render_facility->facility.render.destructor)(render);

	/* Free the output facility used. */
	#ifdef DEBUG
	fprintf(stderr,"Closing output facility.\n");
	#endif

	(*output_facility->facility.output.destructor)(output);

	/* Free the fractal facility used. */
	#ifdef DEBUG
	fprintf(stderr,"Closing fractal facility.\n");
	#endif

	(*fractal_facility->facility.fractal.destructor)(fractal);


	/* That was it. Bye! */
	exit(EXIT_SUCCESS);
	return 0;
}

