/* src_everglades/station_alias.c */
/* ------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "station_alias.h"
#include "appaserver_library.h"
#include "piece.h"

STATION_ALIAS *new_station_alias( char *application )
{
	STATION_ALIAS *s;

	s = (STATION_ALIAS *)calloc( 1, sizeof( STATION_ALIAS ) );
	s->station_alias_dictionary = 
		station_alias_get_dictionary( application );
	return s;
}

char *station_alias_get_station(
			DICTIONARY *station_alias_dictionary,
			char *station_alias_string )
{
	return dictionary_get_string( 	station_alias_dictionary,
					station_alias_string );
}

DICTIONARY *station_alias_get_dictionary( char *application )
{
	DICTIONARY *station_alias_dictionary;
	LIST *station_name_list;
	LIST *station_alias_record_list;
	char *station_alias_record;
	char station_name[ 128 ];
	char *station_name_pointer;
	char station_alias_name[ 128 ];
	char buffer[ 1024 ];

	sprintf( buffer, "station_list_all.sh %s", application );
	station_name_list = pipe2list( buffer );
	
	sprintf( buffer, "station_alias_list.sh %s", application );
	station_alias_record_list = pipe2list( buffer );
	
	station_alias_dictionary = dictionary_new_dictionary();
	if ( !list_rewind( station_name_list ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s(%s): cannot get station list\n",
			__FILE__,
			__FUNCTION__,
			application );
		return station_alias_dictionary;
	}

	do {
		station_name_pointer = list_get_string( station_name_list );
		dictionary_set_string( 	station_alias_dictionary,
					station_name_pointer,
					station_name_pointer );

	} while( list_next( station_name_list ) );

	if ( !list_rewind( station_alias_record_list ) )
		return station_alias_dictionary;

	do {
		station_alias_record = 
			list_get_string( station_alias_record_list );
		piece( station_name, '|', station_alias_record, 0 );
		piece( station_alias_name, '|', station_alias_record, 1 );

		/* -------------------------------------------------- */
		/* Set the station to the station.		      */
		/* There might be orphans in the SHEF_DATATYPE table. */
		/* -------------------------------------------------- */
		dictionary_set_string( 	station_alias_dictionary,
					station_name,
					station_name );

		/* Set the station to the station alias */
		/* ------------------------------------ */
		dictionary_set_string( 	station_alias_dictionary,
					station_alias_name,
					station_name );

	} while( list_next( station_alias_record_list ) );
	return station_alias_dictionary;

} /* station_alias_get_dictionary() */

