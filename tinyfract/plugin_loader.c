#include <dlfcn.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "common.h"

void *load_plugins(char* plugin_path, char* output_method)
{ 
	struct dirent* dir_stream;
	void*          dl_handle;
	DIR*           plugin_dir;
	char*          plugin_directory;
	char*          plugin_path_help;
	char*          plugin_path_scratch;
	output_initialize_view_port_t* fp=NULL;
	

	if ((plugin_path_scratch=malloc(strlen(plugin_path)+1))==NULL)
	{
		perror("plugin_loader, malloc");
		exit(EXIT_FAILURE);
	}
	plugin_path_help=plugin_path_scratch;
	strcpy(plugin_path_scratch,plugin_path);

	plugin_directory=strtok(plugin_path_help,":");
	do
	{
		if ((plugin_dir=opendir(plugin_directory))==NULL)
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
				dl_handle=dlopen(plugin_directory, RTLD_NOW); 
				if (dl_handle)
				{
					printf("%s\n",dlerror());
					exit(EXIT_FAILURE);
				}
				else
				{
					fp=dlsym(dl_handle,output_method);
					if (fp!=NULL) goto end;
					dlclose(dl_handle);
						
				}
			}
			while (dir_stream);
		}
	}
	while ((plugin_directory=strtok(NULL,":"))!=NULL);

end:
	free(plugin_path_scratch);
	return fp;
}

