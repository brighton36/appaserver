/* src_everglades/datatype.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdlib.h>
#include "datatype.h"

DATATYPE *datatype_new_datatype(
			char *datatype_name,
			char *units )
{
	DATATYPE *datatype;

	if ( ! ( datatype = (DATATYPE *)calloc( 1, sizeof( DATATYPE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	datatype->datatype_name = datatype_name;
	datatype->units = units;
	return datatype;

} /* datatype_new_datatype() */

DATATYPE *datatype_unit_record2datatype( char *record )
{
	DATATYPE *datatype;
	char datatype_name[ 128 ];
	char units[ 128 ];
	char bar_graph_yn[ 16 ];
	char scale_graph_to_zero_yn[ 16 ];
	char aggregation_sum_yn[ 16 ];
	char set_negative_values_to_zero_yn[ 16 ];

	piece( units, '|', record, 0 );
	piece( datatype_name, '|', record, 1 );
	/* station is piece( 2 ) */
	piece( bar_graph_yn, '|', record, 3 );
	piece( scale_graph_to_zero_yn, '|', record, 4 );
	piece( aggregation_sum_yn, '|', record, 5 );
	piece( set_negative_values_to_zero_yn, '|', record, 6 );

	datatype =
		datatype_new_datatype(
			strdup( datatype_name ),
			strdup( units ) );

	datatype->bar_chart =
		( tolower( *bar_graph_yn ) == 'y' );

	datatype->scale_graph_to_zero =
		( tolower( *scale_graph_to_zero_yn ) == 'y' );

	datatype->aggregation_sum =
		( tolower( *aggregation_sum_yn ) == 'y' );

	datatype->set_negative_values_to_zero =
		( tolower( *set_negative_values_to_zero_yn ) == 'y' );

	return datatype;

} /* datatype_unit_record2datatype() */

DATATYPE *datatype_list_seek(	LIST *datatype_list,
				char *datatype_name )
{
	DATATYPE *datatype;

	if ( !list_rewind( datatype_list ) ) return 0;

	do {
		datatype = list_get_pointer( datatype_list );

		if ( strcmp( datatype_name, datatype->datatype_name ) == 0 )
			return datatype;

	} while( list_next( datatype_list ) );

	return (DATATYPE *)0;

} /* datatype_list_seek() */

boolean datatype_list_exists(	LIST *datatype_list,
				char *datatype_name )
{
	DATATYPE *datatype;

	if ( !list_rewind( datatype_list ) ) return 0;

	do {
		datatype = list_get_pointer( datatype_list );

		if ( strcmp( datatype_name, datatype->datatype_name ) == 0 )
			return 1;

	} while( list_next( datatype_list ) );

	return 0;

} /* datatype_list_exists() */

LIST *datatype_with_station_name_list_get_datatype_bar_graph_list(
			char *application_name,
			LIST *station_name_list )
{
	DATATYPE *datatype;
	char input_buffer[ 256 ];
	char buffer[ 256 ];
	FILE *input_pipe;
	LIST *datatype_list;
	char *station_name;

	if ( !list_rewind( station_name_list ) )
		return (LIST *)0;

	datatype_list = list_new_list();

	do {
		station_name = list_get_pointer( station_name_list );

		sprintf(buffer,
			"datatype_bar_graph4station_list.sh %s %s",
			application_name,
			station_name );

		input_pipe = popen( buffer, "r" );

		while( get_line( input_buffer, input_pipe ) )
		{
			piece( buffer, '^', input_buffer, 0 );

			if ( !datatype_list_exists(
					datatype_list,
					buffer ) )
			{
				datatype = datatype_new_datatype(
						strdup( buffer ),
						(char *)0 /* units */ );
				piece( buffer, '^', input_buffer, 1 );
				datatype->bar_chart = ( *buffer == 'y' );
				list_append_pointer( datatype_list, datatype );
			}
		}

		pclose( input_pipe );

	} while( list_next( station_name_list ) );

	return datatype_list;

} /* datatype_with_station_name_list_get_datatype_bar_graph_list() */

LIST *datatype_with_station_name_list_get_datatype_list(
			char *application_name,
			LIST *station_name_list )
{
	DATATYPE *datatype;
	char buffer[ 1024 ];
	FILE *input_pipe;
	LIST *datatype_list;
	char *station_name;

	if ( !list_rewind( station_name_list ) )
		return (LIST *)0;

	datatype_list = list_new_list();

	do {
		station_name = list_get_pointer( station_name_list );

		sprintf(	buffer,
				"datatype_unit_record_list.sh %s %s %c %s",
				application_name,
				station_name,
				'n',
				"aggregate_statistic" );

		input_pipe = popen( buffer, "r" );

		while( get_line( buffer, input_pipe ) )
		{
			datatype = datatype_unit_record2datatype( buffer );

			if ( !datatype_list_exists(
					datatype_list,
					datatype->datatype_name ) )
			{
				list_append_pointer( datatype_list, datatype );
			}
		}

		pclose( input_pipe );

	} while( list_next( station_name_list ) );

	return datatype_list;

} /* datatype_with_station_name_list_get_datatype_list() */

LIST *datatype_get_datatype_list(
			char *application_name,
			char *station_name,
			char plot_for_station_check_yn,
			enum aggregate_statistic aggregate_statistic )
{
	DATATYPE *datatype;
	char buffer[ 1024 ];
	FILE *input_pipe;
	LIST *datatype_list = list_new_list();

	sprintf(	buffer,
			"datatype_unit_record_list.sh %s %s %c %s",
			application_name,
			station_name,
			plot_for_station_check_yn,
			aggregate_statistic_get_string(
				aggregate_statistic ) );

	input_pipe = popen( buffer, "r" );

	while( get_line( buffer, input_pipe ) )
	{
		datatype = datatype_unit_record2datatype( buffer );
		list_append_pointer( datatype_list, datatype );
	}

	pclose( input_pipe );
	return datatype_list;

} /* datatype_get_datatype_list() */

char *datatype_get_units_string(		boolean *bar_graph,
						char *application_name,
						char *datatype_name )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *results;
	char bar_graph_yn[ 16 ];

	table_name = get_table_name( application_name, "datatype" );

	sprintf( sys_string,
		 "echo \"select units,bar_graph_yn			 "
		 "       from %s					 "
		 "	 where datatype = '%s';\"			|"
		 "sql.e '^'						 ",
		 table_name,
		 datatype_name );

	results = pipe2string( sys_string );
	*bar_graph = 0;
	if ( !results )
	{
		return "";
	}
	else
	{
		piece( bar_graph_yn, '^', results, 1 );
		*bar_graph = (*bar_graph_yn == 'y');

		if ( *( results + strlen( results ) - 1 ) == '^' )
			*(results + strlen( results ) - 1) = '\0';
		else
			*(results + strlen( results ) - 2) = '\0';

		return results;
	}
} /* datatype_get_units_string() */

char *datatype_list_display( LIST *datatype_list )
{
	DATATYPE *datatype;
	char buffer[ 65536 ];
	char *ptr = buffer;

	if ( !list_rewind( datatype_list ) ) return "";
	*ptr = '\0';

	do {
		datatype = list_get_pointer( datatype_list );

		if ( !list_at_head( datatype_list ) )
			ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "%s", datatype->datatype_name );

	} while( list_next( datatype_list ) );

	return strdup( buffer );
} /* datatype_list_display() */

boolean datatype_bar_chart(
			char *application_name,
			char *datatype_name )
{
	char sys_string[ 1024 ];
	char *results;
	char where[ 256 ];

	sprintf( where, "datatype = '%s'", datatype_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=bar_graph_yn	"
		 "			folder=datatype		"
		 "			where=\"%s\"		",
		 application_name,
		 where );

	results = pipe2string( sys_string );

	if ( !results )
		return 0;
	else
		return ( *results == 'y' );

} /* datatype_bar_chart() */

DATATYPE *datatype_record2datatype( char *record )
{
	DATATYPE *datatype;
	char datatype_name[ 128 ];
	char bar_graph_yn[ 16 ];
	char scale_graph_to_zero_yn[ 16 ];
	char aggregation_sum_yn[ 16 ];
	char set_negative_values_to_zero_yn[ 16 ];

	piece( datatype_name, '|', record, 0 );
	piece( bar_graph_yn, '|', record, 1 );
	piece( scale_graph_to_zero_yn, '|', record, 2 );
	piece( aggregation_sum_yn, '|', record, 3 );
	piece( set_negative_values_to_zero_yn, '|', record, 4 );

	datatype =
		datatype_new_datatype(
			strdup( datatype_name ),
			(char *)0 /* units */ );

	datatype->bar_chart =
		( tolower( *bar_graph_yn ) == 'y' );

	datatype->scale_graph_to_zero =
		( tolower( *scale_graph_to_zero_yn ) == 'y' );

	datatype->aggregation_sum =
		( tolower( *aggregation_sum_yn ) == 'y' );

	datatype->set_negative_values_to_zero =
		( tolower( *set_negative_values_to_zero_yn ) == 'y' );

	return datatype;

} /* datatype_record2datatype() */

LIST *datatype_get_list( char *application_name )
{
	return datatype_list_get( application_name );
}

LIST *datatype_list_get( char *application_name )
{
	DATATYPE *datatype;
	char buffer[ 1024 ];
	FILE *input_pipe;
	LIST *datatype_list;

	datatype_list = list_new_list();

	sprintf(	buffer,
			"datatype_record_list.sh %s",
			application_name );

	input_pipe = popen( buffer, "r" );

	while( get_line( buffer, input_pipe ) )
	{
		datatype = datatype_record2datatype( buffer );
		list_append_pointer( datatype_list, datatype );
	}

	pclose( input_pipe );

	return datatype_list;

} /* datatype_list_get() */

boolean datatype_get_bypass_data_collection_frequency(
				char *application_name,
				char *station,
				char *datatype )
{
	char *select;
	char *results;
	char where[ 128 ];
	char sys_string[ 512 ];

	select = "bypass_data_collection_frequency_yn";
	sprintf( where,
		 "station = '%s' and datatype = '%s'",
		 station,
		 datatype );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=station_datatype	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	results = pipe2string( sys_string );

	if ( !results ) return 0;

	return ( *results == 'y' );

} /* datatype_get_bypass_data_collection_frequency() */

