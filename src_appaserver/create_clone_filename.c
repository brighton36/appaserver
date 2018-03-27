/* create_clone_filename.c 						*/
/* -------------------------------------------------------------------- */
/* This is the appaserver create_clone_filename ADT.			*/
/* It manages the creation of zipped filenames for both			*/
/* the create and clone application processes.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "create_clone_filename.h"
#include "list.h"
#include "dictionary2file.h"

CREATE_CLONE_FILENAME *create_clone_filename_new(
			char *application_name,
			char *appaserver_data_directory )
{
	CREATE_CLONE_FILENAME *c;

	c = (CREATE_CLONE_FILENAME *)
		calloc( 1, sizeof( CREATE_CLONE_FILENAME ) );
	c->application_name = application_name;
	c->appaserver_data_directory = appaserver_data_directory;
	c->folder_name_list = list_new();
	return c;
}

char *create_clone_filename_get_directory_name(
			char *appaserver_data_directory,
			char *directory_name )
{
	if ( directory_name )
		return directory_name;
	else
		return appaserver_data_directory;
}

char *create_clone_filename_get_clone_application_compressed_filename(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char system_folders_yn,
			char *export_output )
{
	char buffer[ 1024 ];

	*directory_name = 
		create_clone_filename_get_directory_name(
			appaserver_data_directory,
			*directory_name );

	if ( system_folders_yn == 'y' )
	{
		if ( export_output
		&&   strcmp( export_output, "shell_script" ) == 0 )
		{
			sprintf(	buffer,
					"%s/insert_%s_appaserver.sh.gz",
					*directory_name,
					application_name );
		}
		else
		if ( export_output
		&&   strcmp( export_output, "spreadsheet" ) == 0 )
		{
			sprintf(	buffer,
					"%s/%s_appaserver.csv.gz",
					*directory_name,
					application_name );
		}
		else
		{
			sprintf(	buffer,
					"%s/insert_%s_appaserver.sql.gz",
					*directory_name,
					application_name );
		}
	}
	else
	{
		if ( export_output
		&&   strcmp( export_output, "shell_script" ) == 0 )
		{
			sprintf(	buffer,
					"%s/insert_%s_application.sh.gz",
					*directory_name,
					application_name );
		}
		else
		if ( export_output
		&&   strcmp( export_output, "spreadsheet" ) == 0 )
		{
			sprintf(	buffer,
					"%s/%s.csv.gz",
					*directory_name,
					application_name );
		}
		else
		{
			sprintf(	buffer,
					"%s/insert_%s_application.sql.gz",
					*directory_name,
					application_name );
		}
	}

	return strdup( buffer );

} /* create_clone_filename_get_clone_application_compressed_filename() */

char *create_clone_filename_get_create_application_filename(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char system_folders_yn )
{
	char buffer[ 1024 ];

	*directory_name = 
		create_clone_filename_get_directory_name(
			appaserver_data_directory,
			*directory_name );

	if ( system_folders_yn == 'y' )
	{
		sprintf(	buffer,
				"%s/create_%s_appaserver.sh",
				*directory_name,
				application_name );
	}
	else
	{
		sprintf(	buffer,
				"%s/create_%s.sh",
				*directory_name,
				application_name );
	}
	return strdup( buffer );
} /* create_clone_filename_get_create_application_filename() */

char *create_clone_filename_get_shell_script_filename(
			char *destination,
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name )
{
	return create_clone_filename_get_clone_folder_filename(
			destination,
			directory_name,
			application_name,
			appaserver_data_directory,
			folder_name,
			"shell_script" );
} /* create_clone_filename_get_shell_script_filename() */

char *create_clone_filename_get_clone_folder_filename(
			char *destination,
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name,
			char *export_output )
{
	*directory_name =
		create_clone_filename_get_directory_name(
			appaserver_data_directory,
			*directory_name );

	if ( export_output
	&&   strcmp( export_output, "shell_script" ) == 0 )
	{
		sprintf(	destination,
				"%s/insert_%s_%s.sh",
				*directory_name,
				application_name,
				folder_name );
	}
	else
	{
		sprintf(	destination,
				"%s/insert_%s_%s.sql",
				*directory_name,
				application_name,
				folder_name );
	}
	
	return destination;
} /* create_clone_filename_get_clone_folder_filename() */

char *create_clone_filename_get_clone_folder_compressed_filename(
			char *destination,
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name )
{
	*directory_name =
		create_clone_filename_get_directory_name(
			appaserver_data_directory,
			*directory_name );

	sprintf(	destination,
			"%s/insert_%s_%s.sql.gz",
			*directory_name,
			application_name,
			folder_name );
	
	return destination;
} /* create_clone_filename_get_clone_folder_compressed_filename() */

char *create_clone_filename_get_create_folder_filename(
			char *destination,
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name )
{
	*directory_name = 
		create_clone_filename_get_directory_name(
			appaserver_data_directory,
			*directory_name );

	sprintf(	destination,
			"%s/create_%s_%s.sh",
			*directory_name,
			application_name,
			folder_name );
	
	return destination;
} /* create_clone_filename_get_create_folder_filename() */

char *create_clone_filename_get_clone_folder_tee_process_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name )
{
	char buffer[ 1024 ];
	char folder_filename[ 128 ];

	sprintf(	buffer,
			"tee_process.e \"gzip - > %s\"",
		create_clone_filename_get_clone_folder_compressed_filename(
				folder_filename,
				directory_name,
				application_name,
				appaserver_data_directory,
				folder_name ) );
	return strdup( buffer );
} /* create_clone_filename_get_clone_folder_tee_process_string() */

char *create_clone_filename_get_create_table_tee_process_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			char *folder_name )
{
	char buffer[ 1024 ];
	char folder_filename[ 128 ];

	sprintf(	buffer,
"tee_process.e \"sed 's/<p>//' | grep -v 'Error:' | grep -v Content-type > %s\"",
			create_clone_filename_get_create_folder_filename(
				folder_filename,
				directory_name,
				application_name,
				appaserver_data_directory,
				folder_name ) );
	return strdup( buffer );
}

char *create_clone_filename_get_create_application_combine_sys_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			LIST *folder_name_list,
			char system_folders_yn )
{
	char buffer[ 65536 ];
	char clone_folder_filename[ 1024 ];
	char *buffer_pointer = buffer;

	*directory_name =
		create_clone_filename_get_directory_name(
			appaserver_data_directory,
			*directory_name );

	buffer_pointer += sprintf( buffer_pointer, "cat " );

	if ( !list_rewind( folder_name_list ) ) return (char *)0;

	do {
		buffer_pointer += sprintf(
			buffer_pointer,
			" %s",
			create_clone_filename_get_create_folder_filename(
				clone_folder_filename,
				directory_name,
				application_name,
				appaserver_data_directory,
				list_get_string( folder_name_list ) ) );

	} while( list_next( folder_name_list ) );

	buffer_pointer += sprintf(
			buffer_pointer,
			" | cat - > %s",
			create_clone_filename_get_create_application_filename(
				directory_name,
				application_name,
				appaserver_data_directory,
				system_folders_yn ) );

	return strdup( buffer );
} /* create_clone_filename_get_create_application_compress_sys_string() */

char *create_clone_filename_get_create_application_remove_sys_string(
			char **directory_name,
			char *application_name,
			char *appaserver_data_directory,
			LIST *folder_name_list )
{
	char buffer[ 65536 ];
	char clone_folder_filename[ 1024 ];
	char *buffer_pointer = buffer;

	*directory_name =
		create_clone_filename_get_directory_name(
			appaserver_data_directory,
			*directory_name );

	buffer_pointer += sprintf( buffer_pointer, "rm -f " );

	if ( !list_rewind( folder_name_list ) ) return (char *)0;

	do {
		buffer_pointer += sprintf(
			buffer_pointer,
			" %s",
			create_clone_filename_get_create_folder_filename(
				clone_folder_filename,
				directory_name,
				application_name,
				appaserver_data_directory,
				list_get_string( folder_name_list ) ) );

	} while( list_next( folder_name_list ) );

	return strdup( buffer );
} /* clone_filename_get_create_application_remove_sys_string() */

