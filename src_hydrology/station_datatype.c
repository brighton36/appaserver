/* $APPASERVER_HOME/src_hydrology/station_datatype.c */
/* ------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "datatype.h"
#include "units.h"
#include "shef_datatype_code.h"
#include "station_datatype.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"

LIST *station_datatype_get_station_datatype_list(
				char *application_name,
				LIST *station_list,
				LIST *datatype_list )
{
	char *station, *datatype;
	STATION_DATATYPE *station_datatype;
	LIST *station_datatype_list;

	if (!list_length( station_list )
	||  list_length( station_list ) != list_length( datatype_list ) )
	{
		return (LIST *)0;
	}

	station_datatype_list = list_new();

	list_rewind( station_list );
	list_rewind( datatype_list );

	do {
		station = list_get_string( station_list );
		datatype = list_get_string( datatype_list );

		if ( ! ( station_datatype =
				station_datatype_get_station_datatype(
						application_name,
						station,
						datatype ) ) )
		{
			return (LIST *)0;
		}

		list_append_pointer(	station_datatype_list,
					station_datatype );

		list_next( datatype_list );
	} while( list_next( station_list ) );

	return station_datatype_list;

} /* station_datatype_get_station_datatype_list() */

STATION_DATATYPE *station_datatype_get_station_datatype(
				char *application_name,
				char *station_name,
				char *datatype_name )
{
	char *station_datatype_string;
	char sys_string[ 1024 ];
	char buffer[ 256 ];
	char *datatype_table, *station_datatype_table;
	STATION_DATATYPE *station_datatype;

	station_datatype_table =
		get_table_name( application_name, "station_datatype" );

	datatype_table = get_table_name( application_name, "datatype" );

	if ( station_name && *station_name )
	{
		sprintf( sys_string,
	"echo \"select 							   "
"units,aggregation_sum_yn,bar_graph_yn,scale_graph_to_zero_yn		   "
	"	from  %s,%s						   "
	"	where %s.datatype = %s.datatype				   "
	"	  and %s.datatype = '%s'				   "
	"	  and %s.station = '%s';\"				  |"
	"sql.e '%c'							   ",
		datatype_table, station_datatype_table,
		datatype_table, station_datatype_table,
		datatype_table, datatype_name,
		station_datatype_table, station_name,
		FOLDER_DATA_DELIMITER );
	}
	else
	{
		sprintf( sys_string,
	"echo \"select 							   "
"units,aggregation_sum_yn,bar_graph_yn,scale_graph_to_zero_yn,''	   "
	"	from  %s						   "
	"	where datatype = '%s';\"				  |"
	"sql.e '%c'							   ",
		datatype_table,
		datatype_name,
		FOLDER_DATA_DELIMITER );
	}

	station_datatype_string = pipe2string( sys_string );

	if ( !station_datatype_string )
	{
		return (STATION_DATATYPE *)0;
	}

	station_datatype = station_datatype_new();

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		station_datatype_string,
		0 );
	station_datatype->units = strdup( buffer );

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		station_datatype_string,
		1 );
	station_datatype->aggregation_sum_yn = *buffer;

	piece(	buffer,
		FOLDER_DATA_DELIMITER,
		station_datatype_string,
		2 );
	station_datatype->bar_graph_yn = *buffer;

	piece( 	buffer,
		FOLDER_DATA_DELIMITER,
		station_datatype_string,
		3 );
	station_datatype->scale_graph_zero_yn = *buffer;

	station_datatype->station_name = station_name;

	station_datatype->datatype = datatype_new( datatype_name );

	return station_datatype;

} /* station_datatype_get_station_datatype() */

LIST *station_datatype_list_get_station_datatype_list(
				char *application_name )
{
	return station_datatype_fetch_list( application_name );
}

LIST *station_datatype_fetch_list(
				char *application_name )
{
	char sys_string[ 1024 ];
	char buffer[ 256 ];
	char *station_datatype_table;
	STATION_DATATYPE *station_datatype;
	FILE *input_pipe;
	char station_name[ 128 ];
	char datatype_name[ 128 ];
	LIST *station_datatype_list = list_new();

	station_datatype_table =
		get_table_name(
			application_name,
			"station_datatype" );

	sprintf( sys_string,
	"echo \"select station,datatype					   "
	"	from  %s;\"						  |"
	"sql.e '%c'							   ",
		station_datatype_table,
		FOLDER_DATA_DELIMITER );

	input_pipe = popen( sys_string, "r" );

	while( get_line( buffer, input_pipe ) )
	{
		station_datatype = station_datatype_new();

		piece(	station_name,
			FOLDER_DATA_DELIMITER,
			buffer,
			0 );

		station_datatype->station_name = strdup( station_name );

		piece(	datatype_name,
			FOLDER_DATA_DELIMITER,
			buffer,
			1 );

		station_datatype->datatype = datatype_new( datatype_name );

		list_append_pointer(
			station_datatype_list,
			station_datatype );
	}
	pclose( input_pipe );
	return station_datatype_list;

} /* station_datatype_fetch_list() */

STATION_DATATYPE *station_datatype_list_seek(
				LIST *station_datatype_list,
				char *station_name,
				char *datatype_name )
{
	STATION_DATATYPE *station_datatype;

	if ( !list_rewind( station_datatype_list ) )
		return (STATION_DATATYPE *)0;

	do {
		station_datatype = list_get_pointer( station_datatype_list );

		if ( station_name )
		{
			if ( timlib_strcmp(	station_datatype->
							station_name,
						station_name ) == 0
			&&   station_datatype->datatype
			&&   timlib_strcmp(	station_datatype->
							datatype->
							datatype_name,
						datatype_name ) == 0 )
			{
				return station_datatype;
			}
		}
		else
		/* --------------- */
		/* No station_name */
		/* --------------- */
		{
			if ( station_datatype->datatype
			&&   timlib_strcmp(	station_datatype->
							datatype->
							datatype_name,
						datatype_name ) == 0 )
			{
				return station_datatype;
			}
		}

	} while( list_next( station_datatype_list ) );

	return (STATION_DATATYPE *)0;

} /* station_datatype_list_seek() */

void station_datatype_free( STATION_DATATYPE *station_datatype )
{
	if ( !station_datatype ) return;

	if ( station_datatype->datatype )
	{
		datatype_free( station_datatype->datatype );
	}

	free( station_datatype );

} /* station_datatype_free() */

STATION_DATATYPE *station_datatype_fetch_new(	char *application_name,
						char *station_name,
						char *datatype_name )
{
	STATION_DATATYPE *station_datatype;
	static LIST *datatype_list = {0};

	if ( !datatype_list )
	{
		datatype_list =
			datatype_with_station_name_get_datatype_list(
				application_name,
				station_name );
	}

	station_datatype = station_datatype_new();
	station_datatype->station_name = station_name;

	station_datatype->datatype =
		datatype_list_seek(
			datatype_list,
			datatype_name );

	station_datatype->datatype->datatype_alias_list =
		datatype_fetch_alias_list(
			application_name,
			datatype_name );

	return station_datatype;

} /* station_datatype_fetch_new() */

STATION_DATATYPE *station_datatype_new( void )
{
	STATION_DATATYPE *station_datatype;

	if ( ! ( station_datatype =
			(STATION_DATATYPE *)
				calloc( 1, sizeof( STATION_DATATYPE ) ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s/%d: cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return station_datatype;

} /* station_datatype_new() */

char *station_datatype_get_manipulate_agency(
				char *application_name,
				char *station,
				char *datatype )
{
	char sys_string[ 1024 ];
	char where[ 256 ];

	sprintf(	where,
			"station = '%s' and datatype = '%s'",
			station,
			datatype );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=manipulate_agency	"
		 "			folder=station_datatype		"
		 "			where=\"%s\"			",
		 application_name,
		 where );

	return pipe2string( sys_string );

} /* station_datatype_get_manipulate_agency() */

char *station_datatype_translate_datatype_name(
				DATATYPE *datatype,
				char *shef_upload_code,
				char *datatype_seek_phrase )
{
	DATATYPE_ALIAS *datatype_alias;

	if ( !datatype ) return (char *)0;

	if ( timlib_strcmp(	datatype->datatype_name,
				datatype_seek_phrase ) == 0 )
	{
		return datatype->datatype_name;
	}

	/* Shef takes precedence over alias. */
	/* --------------------------------- */
	if ( timlib_strcmp(	shef_upload_code,
				datatype_seek_phrase ) == 0 )
	{
		return datatype->datatype_name;
	}

	if ( ( datatype_alias =
			datatype_alias_seek(
				datatype->datatype_alias_list,
				datatype_seek_phrase
					/* datatype_alias_name */ ) ) )
	{
		return datatype_alias->datatype_name;
	}

	return (char *)0;

} /* station_datatype_translate_datatype_name() */

UNITS *station_datatype_list_seek_units(
				LIST *station_datatype_list,
				char *units_seek_phrase )
{
	STATION_DATATYPE *station_datatype;
	DATATYPE *datatype;
	char *units_name;

	if ( !list_rewind( station_datatype_list ) )
		return (UNITS *)0;

	do {
		station_datatype = list_get_pointer( station_datatype_list );

		if ( !station_datatype->datatype )
		{
			fprintf( stderr,
			"Warning in %s/%s()/%d: empty datatype for %s\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 station_datatype->station_name );
			continue;
		}

		datatype = station_datatype->datatype;

		if ( !datatype->units )
		{
			fprintf( stderr,
			"Warning in %s/%s()/%d: empty units for %s/%s\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 station_datatype->station_name,
				 datatype->datatype_name );
			continue;
		}

		if ( ( units_name =
				units_translate_units_name(
					datatype->units->units_alias_list,
					datatype->units->units_name,
					units_seek_phrase ) ) )
		{
			return datatype->units;
		}

	} while( list_next( station_datatype_list ) );

	return (UNITS *)0;

} /* station_datatype_list_seek_units() */

