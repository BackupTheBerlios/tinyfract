#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include "common.h"
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>


int main(int argc, char *argv[])
{ 
	struct dirent* dir_stream;
	DIR*           plugin_dir;
	char*          plugin_path;
	char*          plugin_path_component;
	char*          plugin_path_help;

	if ((plugin_path=malloc(strlen(argv[1])+1))==NULL)
	{
		perror("plugin_loader, malloc");
		exit(EXIT_FAILURE);
	}
	plugin_path_help=plugin_path;
	strcpy(plugin_path,argv[1]);

	plugin_path_component=strtok(plugin_path_help,":");
	do
	{
	

		if ((plugin_dir=opendir(plugin_path_component))==NULL)
		{
			switch (errno)
			{
				case EACCES:
				case ENOENT:
				case ENOTDIR:
					break;
				case EMFILE:
				case ENFILE:
				case ENOMEM:
				default:
					perror("plugin_loader, opendir");
					exit(EXIT_FAILURE);
			}
		
		}
		else
		{
			do
			{
				dir_stream=readdir(plugin_dir);
				if (dir_stream==NULL) break;
				printf("%s\n",dir_stream->d_name);
			}
			while (dir_stream);
		}

		}
	while ((plugin_path_component=strtok(NULL,":"))!=NULL);
	
	free(plugin_path);
	return 0;
	}
	


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
	
