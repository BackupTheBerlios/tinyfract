#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "common.h"

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

