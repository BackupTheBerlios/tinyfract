#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

void *load_plugins(char* plugin_path, char* output_method)
{
	void *handle;
	output_initialize_view_port_t* fp;
	
	while plugin in plugin_path
	{
	
	
		
	handle=dlopen(directory, RTLD_NOW); 
	if(handle)
	{
		printf("%s\n",dlerror());
		exit(EXIT_FAILURE);
	}
	else
	{
		fp=dlsym(handle,output_method);
		if fp=!NULL break;
	}
	
	}

	if fp==NULL
	{
		printf("Error: output method %s does not exist.",output_method);
	}

	
	return fp;
}
	
