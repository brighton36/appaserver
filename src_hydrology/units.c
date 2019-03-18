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

UNITS_ALIAS *units_alias_seek(	LIST *units_alias_list,
				char *units_alias_name )
{
	UNITS_ALIAS *a;

	if ( !list_rewind( units_alias_list ) )
		return (UNITS_ALIAS *)0;

	do {
		a = list_get( units_alias_list );

		if ( timlib_strcmp(	a->units_alias_name,
					units_alias_name ) == 0 ) 
		{
			return a;
		}

	} while( list_next( units_alias_list ) );

	return (UNITS_ALIAS *)0;

} /* units_alias_seek() */

UNITS_ALIAS *units_alias_new( void )
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

UNITS *units_new( void )
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

	return d;

} /* units_new() */

UNITS *units_seek_alias_new(	char *application_name,
				char *units_name )
{
	UNITS *u;
	static LIST *local_units_alias_list = {0};
	UNITS_ALIAS *a;

	if ( !units_name ) return (UNITS *)0;

	if ( !local_units_alias_list )
	{
		local_units_alias_list =
			units_fetch_local_units_alias_list(
				application_name );
	}

	u = units_new();

	if ( ( a = units_alias_seek(
			local_units_alias_list,
			units_name /* units_alias_name */ ) ) )
	{
		u->units_name = a->units_name;
	}

	u->units_alias_list =
		units_fetch_units_alias_list(
			application_name,
			u->units_name );

	return u;

} /* units_seek_alias_new() */

LIST *units_fetch_units_alias_list(char *application_name,
				 char *units_name )
{
	static LIST *local_units_alias_list = {0};
	LIST *units_alias_list;
	UNITS_ALIAS *units_alias;

	if ( !units_name ) return (LIST *)0;

	if ( !local_units_alias_list )
	{
		local_units_alias_list =
			units_fetch_local_units_alias_list(
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

} /* units_fetch_units_alias_list() */

LIST *units_fetch_local_units_alias_list( char *application_name )
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
		units_alias->units_alias_name = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, record, 1 );
		units_alias->units_name = strdup( piece_buffer );

		list_append_pointer( units_alias_list, units_alias );

	} while ( list_next( record_list ) );

	return units_alias_list;

} /* units_fetch_local_units_alias_list() */

UNITS *units_fetch(		char *application_name,
				char *units_name )
{
	static LIST *units_list = {0};

	if ( !units_list )
	{
		units_list = units_fetch_units_list( application_name );
	}

	return units_seek( units_list, units_name );

} /* units_fetch() */

UNITS *units_seek(	LIST *units_list,
			char *units_name )
{
	UNITS *units;

	if ( !list_rewind( units_list ) ) return (UNITS *)0;

	do {
		units = list_get( units_list );

		if ( timlib_strcmp( units->units_name, units_name ) == 0 )
			return units;

	} while ( list_next( units_list ) );

	return (UNITS *)0;

} /* units_seek() */

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

		units = units_new();
		units->units_name = units_name;

		units->units_alias_list =
			units_fetch_units_alias_list(
				application_name,
				units_name );

		list_append_pointer( units_list, units );

	} while ( list_next( units_name_list ) );

	return units_list;

} /* units_fetch_units_list() */

void units_free( UNITS *units )
{
	if ( !units ) return;

	if ( units->units_alias_list )
	{
		list_free_container( units->units_alias_list );
	}

	if ( units->units_converted_list )
	{
		list_free_container( units->units_converted_list );
	}

	free( units );

} /* units_free() */

char *units_translate_units_name(
				LIST *units_alias_list,
				char *units_name,
				char *units_seek_phrase )
{
	UNITS *units;

	if ( !units_name || !units_seek_phrase ) return (char *)0;

	if ( strcmp( units_name, units_seek_phrase ) == 0 )
	{
		/* Could also return units_seek_phrase */
		/* ----------------------------------- */
		return units_name;
	}

	if ( ( units = units_seek(
				units_alias_list,
				units_seek_phrase ) ) )
	{
		/* Could also return units_seek_phrase */
		/* ----------------------------------- */
		return units->units_name;
	}

	return (char *)0;

} /* units_translate_units_name() */

