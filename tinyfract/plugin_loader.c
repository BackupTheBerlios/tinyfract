#include <dlfcn.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "common.h"

void *load_symbol(const char* plugin_path, const char* symbol)
{ 
	struct dirent* directory_entry;
	void*          dl_handle;
	char*          plugin_directory;
	DIR*           plugin_directory_handle;
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

	#ifdef DEBUG
	fprintf(stderr,"Plugin path is %s\n",plugin_path_help);
	#endif

	plugin_directory=strtok(plugin_path_help,":");
	do
	{
		#ifdef DEBUG
		fprintf(stderr,"Considering plugin directory %s\n",plugin_directory);
		#endif
		if ((plugin_directory_handle=opendir(plugin_directory))==NULL)
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
			while ((directory_entry=readdir(plugin_directory_handle))!=NULL)
			{
				dl_handle=dlopen(directory_entry->d_name,RTLD_NOW);
				if (dl_handle!=NULL)
				{
					fp=dlsym(dl_handle,symbol);
					if (fp!=NULL) goto end;
					dlclose(dl_handle);
						
				}
				#ifdef DEBUG
				else fprintf(stderr,"%s\n",dlerror());
				#endif
			}
		}
	}
	while ((plugin_directory=strtok(NULL,":"))!=NULL);

end:
	free(plugin_path_scratch);
	return fp;
}

