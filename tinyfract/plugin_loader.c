#include <dlfcn.h>
#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "common.h"
#include "plugin.h"

/*
 * Check all plugins for a specific facility.
 */
void *load_plugin_facility(const char* path, const plugin_facility_enum_t facility_type, const char* facility_name)
{
	struct dirent*     directory_entry;
	char*              directory_name;
	DIR*               directory_handle;
	void*              dl_handle;
	plugin_facility_t* facility=NULL;
	char*              file_name;
	char*              path_help;
	char*              path_scratch;

	/* Check if a facility_name name is given. */
	if (facility_name==NULL)
	{
		fprintf(stderr,"plugin_loader: no facility name given.\n");
		exit(EXIT_FAILURE);
	}
	
	/* Arrange the search path. */
	if ((path_scratch=malloc(strlen(path)+1))==NULL)
	{
		perror("plugin_loader, malloc");
		exit(EXIT_FAILURE);
	}
	path_help=path_scratch;
	strcpy(path_scratch,path);

	#ifdef DEBUG
	fprintf(stderr,"plugin_loader: plugin path is %s\n",path_help);
	#endif

	/* Parse the search path. */
	directory_name=strtok(path_help,":");
	do
	{
		#ifdef DEBUG
		fprintf(stderr,"plugin_loader: considering plugin directory %s\n",directory_name);
		#endif

		/* Open the next directory in the search path. */
		if ((directory_handle=opendir(directory_name))==NULL)
		{
			/* Check whether to ignore the error or exit. */
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
			/* Ok, no error. Go through all files in the directory in question. */
			while ((directory_entry=readdir(directory_handle))!=NULL)
			{
				/* Arrange the file name for dlopen(). */
				if ((file_name=malloc(strlen(directory_name)+strlen(directory_entry->d_name)+2))==NULL)
				{
					perror("plugin_loader, malloc");
					exit(EXIT_FAILURE);
				}

				strcpy(file_name,directory_name);
				strcat(file_name,"/");
				strcat(file_name,directory_entry->d_name);
				
				#ifdef DEBUG
				fprintf(stderr,"plugin_loader: considering plugin file %s\n",file_name);
				#endif
				
				/* dlopen() the file. */
				if ((dl_handle=dlopen(file_name,RTLD_NOW))!=NULL)
				{
					/* Free the space for the file name. */
					free(file_name);
					
					/* Check if there are tinyfract facilities in the library file. */
					if ((facility=dlsym(dl_handle,"tinyfract_plugin_facilities"))!=NULL)
					{
						#ifdef DEBUG
						fprintf(stderr,"plugin_loader: is a tinyfract plugin.\n");
						#endif 
						
						/* Yes. Check if the given facility is defined in the library file. */
						while (facility->type!=plugin_facility_end)
						{
							#ifdef DEBUG
							fprintf(stderr,"plugin_loader: facility %s found.\n",facility->name);
							#endif

							/* Check if type and name match the request. */
							if ((facility->type==facility_type)
								&& (strcmp(facility->name,facility_name)==0))
							{
								#ifdef DEBUG
								fprintf(stderr,"plugin_loader: type and name match the request!\n");
								#endif
								
								goto end;
							}	
								
							/* Check next facility. */
							facility++;
						}	
					}

					/* No. Close the library file. */
					dlclose(dl_handle);
				}
				#ifdef DEBUG
				else fprintf(stderr,"plugin_loader, dlopen %s\n",dlerror());
				#endif
			}
		}
	}
	while ((directory_name=strtok(NULL,":"))!=NULL);

end: /* Target for successful search. */

	/* Free the space for the arranged path. */
	free(path_scratch);

	#ifdef DEBUG
	fprintf(stderr,"plugin_loader: found matching facility %s at address %p.\n",facility_name,facility);
	#endif

	return facility;
}
