/* dictionary2file.c 							*/
/* -------------------------------------------------------------------- */
/* This is the dictionary2file function.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "dictionary.h"
#include "piece.h"
#include "timlib.h"
#include "appaserver_library.h"
#include "dictionary2file.h"

DICTIONARY *dictionary2file_get_dictionary(
				pid_t process_id,
				char *appaserver_data_directory,
				char *insert_update_key,
				char *folder_name,
				char *optional_related_attribute_name,
				char dictionary_delimiter )
{
	DICTIONARY *dictionary;
	char *filename;
	char key[ 512 ];
	char data[ MAX_INPUT_LINE ];
	char buffer[ MAX_INPUT_LINE ];
	FILE *f;

	filename = dictionary2file_get_filename(
				process_id,
				appaserver_data_directory,
				insert_update_key,
				folder_name,
				optional_related_attribute_name );

	f = fopen( filename, "r" );
	if ( !f ) return (DICTIONARY *)0;

	dictionary = dictionary_new_medium_dictionary();

	while( get_line( buffer, f ) )
	{
		piece( key, dictionary_delimiter, buffer, 0 );
		piece( data, dictionary_delimiter, buffer, 1 );
		dictionary_set_pointer(	dictionary,
					strdup( key ),
					strdup( data ) );
	}

	fclose( f );

/* Can't do. This prevents the back button from functioning.
	sprintf( sys_string, "rm -f %s", filename );
	system( sys_string );
*/
	return dictionary;

} /* dictionary2file_get_dictionary() */

char *dictionary2file_get_filename(
				pid_t process_id,
				char *appaserver_data_directory,
				char *insert_update_key,
				char *folder_name,
				char *optional_related_attribute_name )
{
	char filename[ 1024 ];
	char local_folder_name[ 512 ];

	if ( !appaserver_data_directory )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty appaserver_data_directory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !insert_update_key )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty insert_update_key.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Sometimes the folder name is really a comma separated list */
	/* ---------------------------------------------------------- */
	timlib_strcpy( local_folder_name, folder_name, 0 );
	search_replace_string( local_folder_name, ",", "_" );

	if ( optional_related_attribute_name
	&&   *optional_related_attribute_name
	&&   strcmp( optional_related_attribute_name, "null" ) != 0 )
	{
		sprintf( local_folder_name + strlen( local_folder_name ),
			 "_%s",
			 optional_related_attribute_name );
	}

/*
	sprintf( filename,
		 "%s/%s/%s_%s_%d.txt",
		 appaserver_mount_point,
		 DICTIONARY2FILE_DIRECTORY,
		 insert_update_key,
		 local_folder_name,
		 process_id );
*/

	sprintf( filename,
		 "%s/%s_%s_%d.txt",
		 appaserver_data_directory,
		 insert_update_key,
		 local_folder_name,
		 process_id );

	return strdup( filename );

} /* dictionary2file_get_filename() */

