/* src_hydrology/shef_alias.c */
/* -------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "shef_alias.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "piece.h"
#include "timlib.h"

SHEF_ALIAS *new_shef_alias( char *entity )
{
	SHEF_ALIAS *s;

	s = (SHEF_ALIAS *)calloc( 1, sizeof( SHEF_ALIAS ) );
	s->shef_alias_dictionary = 
		shef_alias_get_dictionary( entity );
	return s;
}

char *shef_alias_get_shef(
			DICTIONARY *shef_alias_dictionary,
			char *shef_alias_string )
{
	return dictionary_get_string( 	shef_alias_dictionary,
					shef_alias_string );
}

DICTIONARY *shef_alias_get_dictionary(
			char *entity )
{
	DICTIONARY *shef_alias_dictionary;
	LIST *shef_name_list;
	LIST *shef_alias_record_list;
	char *shef_alias_record;
	char shef_name[ 128 ];
	char *shef_name_pointer;
	char shef_alias_name[ 128 ];
	char buffer[ 1024 ];

	sprintf( buffer, "shef_download_list.sh %s | piece '|' 0",
		 entity );
	shef_name_list = pipe2list( buffer );
	
	sprintf( buffer, "shef_alias_list.sh %s", entity );
	shef_alias_record_list = pipe2list( buffer );
	
	shef_alias_dictionary = dictionary_new();

	if ( !list_rewind( shef_name_list ) )
	{
		char msg[ 1024 ];
		sprintf(msg,
"ERROR in shef_alias_get_shef_list(%s): cannot get shef list\n",
			entity );
		appaserver_output_error_message(
			entity, msg, (char *)0 );
		return shef_alias_dictionary;
	}

	do {
		shef_name_pointer = list_get_string( shef_name_list );
		dictionary_set_string( 	shef_alias_dictionary,
					shef_name_pointer,
					shef_name_pointer );

	} while( list_next( shef_name_list ) );

	if ( !list_rewind( shef_alias_record_list ) )
	{
		char msg[ 1024 ];
		sprintf(msg,
"ERROR in shef_alias_get_shef_list(%s): cannot get shef alias list\n",
			entity );
		appaserver_output_error_message(
			entity, msg, (char *)0 );
		return shef_alias_dictionary;
	}

	do {
		shef_alias_record = 
			list_get_string( shef_alias_record_list );
		piece( shef_name, '|', shef_alias_record, 0 );
		piece( shef_alias_name, '|', shef_alias_record, 1 );
		dictionary_set_string( 	shef_alias_dictionary,
					shef_alias_name,
					shef_name );

	} while( list_next( shef_alias_record_list ) );
	return shef_alias_dictionary;
} /* shef_alias_get_dictionary() */

