/*
 * This software and its doumentation may be freely distributed under
 * the terms of the GNU General Public License(GPL), Version 2
 * See http://www.fsf.org/licenses/gpl.txt for details.
 *
 * (C)2004 by Jan Kandziora <jjj@gmx.de>
 */

#include <getopt.h>
#include <gmp.h>
#include <math.h>
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
	long long int            prec=0;              /* Precision from command line or user input. */
	
	/* Variables and constants for option parsing. */
	char             command;
	int              flags=0;
	view_dimension_t geometry;
	ordinal_number_t iteration_steps;
	char*            scale_str=NULL;
	real_number_t    scale;
        char*            plugin_path=NULL;
	char*            center_str=NULL;
	complex_number_t center;
	button_event_t   new_center_real;
	complex_number_t new_center_virtual;
	double           zoom_factor=10;
	view_position_t  help;
	real_number_t    convert;
	
	const int CENTER_SET=1;
	const int GEOMETRY_SET=2;
	const int ITERATION_STEPS_SET=4;
	const int SCALE_SET=8;

	geometry.width=0;
	geometry.height=0;

	
	int c;
	int option_index=0;
	static struct option long_options[] =
	{
//		{"center=",             1,0,'c'},
		{"fractal-type=",       1,0,'f'},
		{"fractal-parameters=", 1,0,'F'},
		{"geometry=",           1,0,'g'},
//		{"iteration-steps=",    1,0,'i'},
		{"output-method=",      1,0,'o'},
		{"output-parameters=",  1,0,'O'},
		{"plugin-path=",        1,0,'P'},
		{"render-method=",      1,0,'r'},
		{"render-parameters=",  1,0,'R'},
		{"precision=",          1,0,'p'},
		{"scale-factor",        1,0,'S'},
//		{"scale=",              1,0,'s'},
		{"help",                0,0,'?'},
		{"version",             0,0,'V'},
		{0,0,0,0}
	};


	/* Parse options. */
	for (;;)
	{
//		c=getopt_long(argc,argv,"c:f:F:g:i:o:O:P:r:R:p:s:S:",long_options,&option_index);
		c=getopt_long(argc,argv,"f:F:g:o:O:P:r:R:p:S:",long_options,&option_index);
		if (c==-1) break;

		switch (c)
		{
/*			case 'c':
				sscanf(optarg,"%lf,%lf",&Re(center),&Im(center));
				flags|=CENTER_SET;
				break;*/
			case 'p':
				sscanf(optarg,"%lld", &prec);
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
/*			case 'i':
				sscanf(optarg,"%d",&iteration_steps);
				flags|=ITERATION_STEPS_SET;
				break;*/
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
/*			case 's':
				sscanf(optarg,"%lf",&scale);
				flags|=SCALE_SET;
				break;*/
			case 'S':
				sscanf(optarg,"%lf", &zoom_factor);
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
//					"  -c, --center=             Center of the picture in the complex plane. Format: Re,Im.\n"
					"                              Defaults to fractal function's choice.\n"
					"  -f, --fractal-type=       Select the fractal formula. A plugin has to provide the\n"
					"                              formula. Default is prompting the user.\n"
					"  -F, --fractal-parameters= Specifies additional fractal parameters, if the\n"
					"                              fractal formula needs any.\n"
					"  -g, --geometry=           Output picture size in pixels. Format: Width x Height.\n"
					"                              Defaults to output plugin's choice.\n"
//					"  -i, --iteration-steps=    Sets the maximum count of iterations per point.\n"
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
//					"  -s, --scale=              Scale: Baseline with of the rendered picture in the complex\n"
					"  -S  --scale-factor=       Set the zomming factor for auto zoom. Default value is 10.\n"
					"                              coordinate system. Defaults to fractal function's choice.\n"
					"  -p, --precision=          Set the number of Nachkommastellen.\n"
					"      --help                Show this help and exit.\n"
					"      --version             Show version information and exit.\n\n"
					"Example: %s -fmandelbrot --geom=640x480 -i170 -c-0.5,0.8 -s1.3\n"
					,argv[0],argv[0]); 
				exit(EXIT_FAILURE);
		}
	}

#ifdef DEBUG
	fprintf(stderr,"Command line parameters are:\n");
//	fprintf(stderr,"\tcenter=%f;%f\n",Re(center),Im(center));
	fprintf(stderr,"\tfractal-type=%s\n",fractal_type);
	fprintf(stderr,"\tfractal-parameters=%s\n",fractal_args);
	fprintf(stderr,"\tgeometry=%dx%d\n",geometry.width,geometry.height);
	fprintf(stderr,"\titeration-steps=%d\n",iteration_steps);
	fprintf(stderr,"\toutput-method=%s\n",output_method);
	fprintf(stderr,"\tprecision=%lld\n",prec);
//	fprintf(stderr,"\tscale=%lf\n",scale);
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

	/* Test if geometry was given. */
	if(geometry.width==0 || geometry.height==0)
	{
		fprintf(stderr,"%s: You have to specify a geometry.\n", argv[0]);
		exit(EXIT_FAILURE);
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
//		((flags & ITERATION_STEPS_SET?iteration_steps:fractal_facility->facility.fractal.iteration_steps),fractal_args)))
		(fractal_facility->facility.fractal.iteration_steps,prec,fractal_args)))
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


/* Hier muss die Hauptschleife beginnen. */

	#ifdef DEBUG
	fprintf(stderr,"parameter from stdin:\n");
	#endif

	while (!feof(stdin))
	{	
		scanf("%c",&command);
		switch (command)
		{
			case 'p':
				if(prec==0)
				{
					fprintf(stderr,"%s: You have to specify a precision greater than zero!\n", argv[0]);
					exit(EXIT_FAILURE);
				}
				center_str=malloc(sizeof(char)*(prec*2+2));
				scanf("%s", center_str);
				flags|=CENTER_SET;
				break;
			case 's':
				if(prec==0)
				{
					fprintf(stderr,"%s: You have to specify a precision greater than zero!\n", argv[0]);
					exit(EXIT_FAILURE);
				}
				scale_str=malloc(sizeof(char)*(prec+1));
				scanf("%s",scale_str);
				flags|=SCALE_SET;
				break;
			case 'i':
				scanf("%u",&iteration_steps);
				break;
			case 'r':
				printf("position: %s\n",center_str);
				printf("scale: %s\n",scale_str);
				printf("iteration_steps: %u\n",iteration_steps);

	/* Parse options. */
	mpf_set_default_prec(sizeof(char)*prec);

	mpf_init(Re(center));
	mpf_init(Im(center));
	mpf_init(scale);
	mpf_init(Re(new_center_virtual));
	mpf_init(Im(new_center_virtual));
	mpf_init(convert);

	parse_options(&center,center_str,&scale,scale_str,prec);

	for(;;)
	{
		gmp_printf("position: %F.10f %F.10f\n",Re(center),Im(center));
		gmp_printf("scale: %F.10f\n",scale);
		printf("iteration_steps: %u\n",iteration_steps);
		
		/* Initialize the render facility. */
		#ifdef DEBUG
		fprintf(stderr,"Initializing render facility.\n");
		#endif
		if (!(render=(*render_facility->facility.render.constructor)
			(center,
//			(fractal_facility->facility.fractal.center,
			 geometry,
			 scale,
//			 fractal_facility->facility.fractal.scale,
			 fractal_facility,output_facility,fractal,output,render_args,100)))
		{
			perror("Could not initialize render facility");
			exit(EXIT_FAILURE);
		}

		/* Render the fractal. */
		#ifdef DEBUG
		fprintf(stderr,"Rendering the fractal.\n");
		#endif
		(*render_facility->facility.render.render_function)(render);

		/* Infoem the user about watching the fractal. */
		fprintf(stderr,"%s: Ready with rendering. You can look at the fractal know.\n", argv[0]);

		/* Flush and close the output viewport. */
		#ifdef DEBUG
		fprintf(stderr,"Flushing viewport.\n");
		#endif
		(*output_facility->facility.output.flush_viewport_function)(output,&new_center_real);

		/* Calculate new centre. */
		#ifdef DEBUG
		fprintf(stderr,"%s: Pressed Button is: %d.\n", argv[0], new_center_real.type);
		#endif
		switch (new_center_real.type)
		{
			case 1:
				help.x=new_center_real.x;
				help.y=new_center_real.y;
				make_vinumber(&new_center_virtual,help,geometry,scale,center,prec);
				VARCOPY(Re(center),Re(new_center_virtual));
				VARCOPY(Im(center),Im(new_center_virtual));
				mpf_set_d(convert,zoom_factor);
				mpf_div(scale,scale,convert);
				break;
			case 4:
				help.x=new_center_real.x;
				help.y=new_center_real.y;
				make_vinumber(&new_center_virtual,help,geometry,scale,center,prec);
				VARCOPY(Re(center),Re(new_center_virtual));
				VARCOPY(Im(center),Im(new_center_virtual));
				break;
			case 3:
				help.x=new_center_real.x;
				help.y=new_center_real.y;
				make_vinumber(&new_center_virtual,help,geometry,scale,center,prec);
				VARCOPY(Re(center),Re(new_center_virtual));
				VARCOPY(Im(center),Im(new_center_virtual));
				mpf_set_d(convert,zoom_factor);
				mpf_mul(scale,scale,convert);
				break;
			case 5:
				help.x=new_center_real.x;
				help.y=new_center_real.y;
				make_vinumber(&new_center_virtual,help,geometry,scale,center,prec);
				VARCOPY(Re(center),Re(new_center_virtual));
				VARCOPY(Im(center),Im(new_center_virtual));
				break;
			case 2:
				goto exit_func;
			default:
				break;
		}

		/* Sleep a while. */
		#ifdef DEBUG
		fprintf(stderr,"Waiting.\n");
		#endif

		/* Free the render facility used. */
		#ifdef DEBUG
		fprintf(stderr,"Closing render facility.\n");
		#endif
		(*render_facility->facility.render.destructor)(render);
	}			
				
				break;
			case '\n':
				break;
			default:
				printf("fehler\n");
		}
	}
/* Hier ist das Ende der Hauptschleife. */

exit_func:
	
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

	/* Free the multiple precision variables. */
	mpf_clear(Re(center));
	mpf_clear(Im(center));
	mpf_clear(scale);
	mpf_clear(Re(new_center_virtual));
	mpf_clear(Im(new_center_virtual));
	mpf_clear(convert);
	
	/* That was it. Bye! */
	exit(EXIT_SUCCESS);
	return 0;
}

