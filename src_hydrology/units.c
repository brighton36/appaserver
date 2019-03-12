/* $APPASERVER_HOME/src_hydrology/units.c */
/* -------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "units.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"

UNITS_CONVERTED *units_converted_new(
				void )
{
	UNITS_CONVERTED *d;

	if ( ! ( d = calloc( 1, sizeof( UNITS_CONVERTED ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return d;

} /* units_converted_new() */

UNITS_ALIAS *units_alias_new(
				void )
{
	UNITS_ALIAS *d;

	if ( ! ( d = calloc( 1, sizeof( UNITS_ALIAS ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return d;

} /* units_alias_new() */

UNITS *units_new( char *units_name )
{
	UNITS *d;

	if ( ! ( d = calloc( 1, sizeof( UNITS ) ) ) )
	{
		fprintf( stderr,
			 "Error in %s/%s/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	d->units_name = units_name;

	return d;

} /* units_new() */

LIST *units_get_units_alias_list(char *application_name,
				char *units_name )
{
	static LIST *local_units_alias_list = {0};
	LIST *units_alias_list;
	UNITS_ALIAS *units_alias;

	if ( !local_units_alias_list )
	{
		local_units_alias_list =
			units_fetch_units_alias_list(
				application_name );
	}

	if ( !list_rewind( local_units_alias_list ) ) return (LIST *)0;

	units_alias_list = list_new();

	do {
		units_alias = list_get_pointer( local_units_alias_list );

		if ( timlib_strcmp(	units_alias->units_name,
					units_name ) == 0 )
		{
			list_append_pointer( units_alias_list, units_alias );
		}

	} while ( list_next( local_units_alias_list ) );

	return units_alias_list;

} /* units_get_units_alias_list() */

LIST *units_fetch_units_alias_list( char *application_name )
{
	char sys_string[ 1024 ];
	char *folder;
	char *select;
	LIST *record_list;
	char *record;
	char piece_buffer[ 128 ];
	LIST *units_alias_list;
	UNITS_ALIAS *units_alias;

	select = "units_alias,units";
	folder = "units_alias";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder );

	record_list = pipe2list( sys_string );

	if ( !list_rewind( record_list ) ) return (LIST *)0;

	units_alias_list = list_new();

	do {
		record = list_get_pointer( record_list );

		units_alias = units_alias_new();

		piece( piece_buffer, FOLDER_DATA_DELIMITER, record, 0 );
		units_alias->units_alias = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, record, 1 );
		units_alias->units_name = strdup( piece_buffer );

		list_append_pointer( units_alias_list, units_alias );

	} while ( list_next( record_list ) );

	return units_alias_list;

} /* units_fetch_units_alias_list() */

LIST *units_fetch_units_list( char *application_name )
{
	char sys_string[ 1024 ];
	char *folder;
	char *select;
	LIST *units_name_list;
	LIST *units_list;
	UNITS *units;
	char *units_name;

	select = "units";
	folder = "units";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	",
		 application_name,
		 select,
		 folder );

	units_name_list = pipe2list( sys_string );

	if ( !list_rewind( units_name_list ) ) return (LIST *)0;

	units_list = list_new();

	do {
		units_name = list_get_pointer( units_name_list );

		units = units_new( units_name );

		units->units_alias_list =
			units_get_units_alias_list(
				application_name,
				units_name );

		list_append_pointer( units_list, units );

	} while ( list_next( units_name_list ) );

	return units_list;

} /* units_fetch_units_list() */

