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
#include "plugin_loader.h"


/* Global variables */








/*
 * This function renders the picture. It connects the fractal function and the
 * output function.
 */
void render(render_param_t render_param, plugin_fractal_calculate_function_t *fractal_calculate_point, ordinal_number_t fractal_argc, real_number_t fractal_argv[])
{
	complex_number_t complex_position;
	view_position_t  render_position;
	real_number_t    scaling_factor;
	view_position_t  shift;

	/* Precalculate scaling factor and center shift for speed reasons. */
	scaling_factor=render_param.scale/render_param.output_dimension.width;
	shift.x=render_param.output_dimension.width/2;
	shift.y=render_param.output_dimension.height/2;


#ifdef DEBUG
	fprintf(stderr,"Fractal:\n");
	for(render_position.y=0;render_position.y<render_param.output_dimension.height;render_position.y++)
	{
		for(render_position.x=0;render_position.x<render_param.output_dimension.width;render_position.x++)
		{
			Re(complex_position)=Re(render_param.center)+scaling_factor*(render_position.x-shift.x);
			Im(complex_position)=Im(render_param.center)+scaling_factor*(render_position.y-shift.y);
//			fprintf(stderr,"%lf %lf %lf\n",Re(complex_position),Im(complex_position),(*fractal_calculate_point)(complex_position,render_param.iteration_steps,fractal_argc,fractal_argv));
			fprintf(stderr,"%c",' '+(char)(
			
			(*fractal_calculate_point)(complex_position,render_param.iteration_steps,fractal_argc,fractal_argv))
			);
		}
		fprintf(stderr,"\n");
	}
#endif

}






/*
 * Our main function does nothing more than parsing options and checking the
 * range of their values, then loads the desired fractal and output function
 * from a plugin and calls render to actually do the job.
 */
int main(int argc, char* argv[])
{
	render_param_t    render_param;        /* Rendering parameters from command line or user input */
	char             *fractal_type=NULL;   /* Fractal type selected from command line or user input */
	char             *fractal_argv_string; /* Fractal parameters from command line or user input */
	char             *fractal_argv_helper; /* Helper variable for tokenizing fractal_argv_string */
	ordinal_number_t  fractal_argc=0;      /* Number of fractal parameters */ 
	real_number_t    *fractal_argv=NULL;   /* Array of fractal parameters */
	
	plugin_fractal_calculate_function_t *plugin_fractal_calculate_function=NULL; /* Pointer to fractal calculation function */
	char             *output_method=NULL;  /* Output method selected from command line or user input */
	char             *output_args=NULL;    /* Output parameters from command line or user input */

#ifdef DEBUG
	int debug_i;
#endif

	char            *plugin_fractal_calculate_function_name;

	
	const char plugin_name[]   ="plugin_";
	const char fractal_name[]  ="fractal_";
	const char calculate_name[]="calculate_";
	

	/* Parse options. */
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
		{"plugin-path",         1,0,'P'},
		{"recursion-depth=",    1,0,'r'},
		{"scale=",              1,0,'s'},
		{"help",                0,0,'?'},
		{"version",             0,0,'V'},
		{0,0,0,0}
	};

	/* Initialize render_param */
	VARZERO(render_param);

	for (;;)
	{

		c=getopt_long(argc,argv,"c:f:F:g:i:o:O:P:r:s:",long_options,&option_index);
		if (c==-1) break;

		switch (c)
		{
			case 'c':
				sscanf(optarg,"%lf,%lf",&render_param.center.real_part,&render_param.center.imaginary_part);
				break;
			case 'f':
				fractal_type=optarg;
				break;
			case 'F':
				fractal_argv_string=malloc(strlen(optarg)+sizeof(char));
				strcpy(fractal_argv_string,optarg);
				fractal_argc=0;
				fractal_argv=NULL;
				fractal_argv_helper=strtok(fractal_argv_string,",");
				while (fractal_argv_helper!=NULL)
				{
					fractal_argc++;
					fractal_argv=realloc(fractal_argv,sizeof(real_number_t)*fractal_argc);
					if (fractal_argv==NULL)
					{
						perror("parsing of parameters failed:");
						exit(EXIT_FAILURE);
					}
					sscanf(fractal_argv_helper,"%lf",&fractal_argv[fractal_argc-1]);
					fractal_argv_helper=strtok(NULL,",");
				}
				break;
			case 'g':
				sscanf(optarg,"%ux%u",&render_param.output_dimension.width,&render_param.output_dimension.height);
				break;
			case 'i':
				sscanf(optarg,"%d",&render_param.iteration_steps);
				break;
			case 'o':
				output_method=optarg;
				break;
			case 'O':
				output_args=optarg;
				break;
			case 'r':
				sscanf(optarg,"%u",&render_param.recursion_depth);
				break;
			case 's':
				sscanf(optarg,"%lf",&render_param.scale);
				break;
			case 'V':
				fprintf(stderr,
					"%s: Version %s-%s, build %s on %s\n"
					"(C)2004 Jan Kandziora <jjj@gmx.de>, Copy left by the means of GNU GPL v2\n"
					"See http://www.gnu.org/licenses/gpl.txt for details.\n"
					,argv[0],VERSION_INFO,BUILD_INFO,BUILD_DATE,BUILD_HOST);
				exit(EXIT_FAILURE);
			case '?':
			default:
				fprintf(stderr,
					"(C)2004 Jan Kandziora <jjj@gmx.de>, Copy left by the means of GNU GPL v2\n"
					"See http://www.gnu.org/licenses/gpl.txt for details.\n\n"
					"USAGE: %s [OPTIONS]\n"
					"  -c, --center=        Center of the picture in the complex plane. Format: Re,Im.\n"
					"                         Defaults to fractal function's choice.\n"
					"  -f, --fractal-type=  Select the fractal formula. A plugin has to provide the\n"
					"                         formula. Default is prompting the user.\n"
					"  -F, --fractal-parameters=  Specifies additional fractal parameters, if the\n"
					"                         fractal formula needs any. All are zero by default.\n"
					"  -g, --geometry=      Output picture size in pixels. Format: Width x Height.\n"
					"                         Defaults to output plugin's choice.\n"
					"  -i, --iteration-steps= Sets the maximum count of iterations per point.\n"
					"                         Defaults to fractal function's choice.\n"
					"  -o, --output-method= Specifies the output method. A plugin has to provide it.\n"
					"                         Default is prompting the user.\n"
					"  -O, --output-parameters=  Specifies additional parameters passed to the output\n"
					"                         function.\n"
					"  -P, --plugin-path=   Specifies a path to search for plugins.\n"
					"  -r, --recursion-depth=  Set the recursion depth of the render algorithm. A high\n"
					"                         number increases the speed large areas of similar result\n"
					"                         values are filled but may result in rendering artefacs in\n"
					"                         other areas. A value of 0 choses a non-recursive algorithm.\n"
					"                         Defaults to 4 (squares of 9x9 pixel).\n"
					"  -s, --scale=         Scale: Baseline with of the rendered picture in the complex\n"
					"                         coordinate system. Defaults to fractal function's choice.\n"
					"      --help           Show this help and exit.\n"
					"      --version        Show version information and exit.\n\n"
					"Example: %s -fmandelbrot --geom=640x480 -c-0.5,0.8 -s1.3\n"
					,argv[0],argv[0]); 
				exit(EXIT_FAILURE);
		}
	}

#ifdef DEBUG
	fprintf(stderr,"Command line parameters are:\n");
	fprintf(stderr,"\tcenter=%f;%f\n",render_param.center.real_part,render_param.center.imaginary_part);
	fprintf(stderr,"\tfractal-type=%s\n",fractal_type);
	fprintf(stderr,"\tfractal-parameters=");
	for(debug_i=0;debug_i<fractal_argc;debug_i++) fprintf(stderr,"%lf,",fractal_argv[debug_i]);
	fprintf(stderr,"\n\tgeometry=%dx%d\n",render_param.output_dimension.width,render_param.output_dimension.height);
	fprintf(stderr,"\titeration-steps=%d\n",render_param.iteration_steps);
	fprintf(stderr,"\toutput-method=%s\n",output_method);
	fprintf(stderr,"\trecursion-depth=%d\n",render_param.recursion_depth);
	fprintf(stderr,"\tscale=%lf\n",render_param.scale);
	fflush(stdout);
	fflush(stderr);
#endif

	/* Test if fractal type was given. */
	if (fractal_type==NULL)
	{
		fprintf(stderr,"%s: You have to specify a fractal type.\n",argv[0]);
		exit(EXIT_FAILURE);
	}
	
	/* Load fractal function. */
	if ((plugin_fractal_calculate_function_name=
		malloc(strlen(plugin_name)+strlen(fractal_name)+strlen(calculate_name)
			+strlen(fractal_type))+1)==NULL)
	{
		perror("fractal_function, malloc");
		exit(EXIT_FAILURE);
	}
	
	strcpy(plugin_fractal_calculate_function_name,plugin_name);
	strcat(plugin_fractal_calculate_function_name,fractal_name);
	strcat(plugin_fractal_calculate_function_name,calculate_name);
	strcat(plugin_fractal_calculate_function_name,fractal_type);

	#ifdef DEBUG
	fprintf(stderr,"Considering %s\n",plugin_fractal_calculate_function_name);
	#endif
	
	if ((plugin_fractal_calculate_function=load_symbol("./plugins",fractal_type))==NULL)
	//if ((plugin_fractal_calculate_function=load_symbol("./plugins",plugin_fractal_calculate_function_name))==NULL)
	{
		fprintf(stderr,"%s: Could not load fractal function %s.\n",argv[0],fractal_type);
		exit(EXIT_FAILURE);
	}


	#ifdef DEBUG
	fprintf(stderr,"Render function %p\n",plugin_fractal_calculate_function);
	#endif
			
	/* Render the fractal. */
	//render(render_param,plugin_fractal_type_calculate_f,fractal_argc,fractal_argv); 

	/* That was it. Bye! */
	exit(EXIT_SUCCESS);
	return 0;
}

