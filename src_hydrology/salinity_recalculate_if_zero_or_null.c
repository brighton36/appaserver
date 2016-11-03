/* salinity_recalculate_if_zero_or_null.c	*/
/* -------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "appaserver_library.h"
#include "conductivity2salinity.h"

typedef struct
{
	char *date_string;
	char *time_string;
	double salinity;
	double conductivity;
} SALINITY_RECORD;

#define KEY_COMMA_LIST_STRING	 			"station,datatype,measurement_date,measurement_time"

void populate_conductivity_values(
				char *application_name,
				char *station,
				LIST *salinity_record_list );

void output_to_process(
			FILE *output_pipe,
			char *station,
			LIST *salinity_record_list );

char *get_station_salinity_sys_string(
				char *application_name,
				char *optional_station );

LIST *get_salinity_record_list(
				char *application_name,
				char *station );

SALINITY_RECORD *new_salinity_record( void );

int main( int argc, char **argv )
{
	char *application_name;
	FILE *station_salinity_input_pipe;
	char station[ 128 ];
	LIST *salinity_record_list;
	char *really_yn;
	char *sql_process;
	char output_process_string[ 512 ];
	FILE *output_pipe;
	char *table_name;
	char *optional_station = {0};

	if ( argc < 3 )
	{
		fprintf( stderr,
			 "Usage: %s application really_yn [station]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	really_yn = argv[ 2 ];

	if ( argc == 4 ) optional_station = argv[ 3 ];
		
	if ( *really_yn == 'y' )
		sql_process = "sql";
	else
		sql_process = "cat";

	table_name = get_table_name( application_name, "measurement" );

	sprintf( output_process_string,
		 "update_statement.e %s %s '^'			|"
		 "%s						 ",
		 table_name,
		 KEY_COMMA_LIST_STRING,
		 sql_process );

	output_pipe = popen( output_process_string, "w" );

	station_salinity_input_pipe = 
		popen(	get_station_salinity_sys_string( application_name,
							 optional_station ),
			"r" );

	while( get_line( station, station_salinity_input_pipe ) )
	{
		salinity_record_list =
			get_salinity_record_list(
				application_name,
				station );

		if ( list_length( salinity_record_list ) )
		{
			populate_conductivity_values( 	application_name,
							station,
							salinity_record_list );

			output_to_process(
				output_pipe,
				station,
				salinity_record_list );

		}
	}

	pclose( output_pipe );
	pclose( station_salinity_input_pipe );
	exit( 0 );

} /* main() */

LIST *get_salinity_record_list(
				char *application_name,
				char *station )
{
	char sys_string[ 1024 ];
	char *table_name;
	LIST *salinity_record_list = list_new_list();
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char date[ 128 ], time[ 128 ];
	SALINITY_RECORD *salinity_record;

	table_name = get_table_name( application_name, "measurement" );

	sprintf( sys_string,
		 "echo \"select measurement_date,measurement_time	 "
		 "	 from %s					 "
		 "	 where station = '%s'				 "
		 "	   and datatype = 'salinity'			 "
		 "	   and ( measurement_value = 0.0		 "
		 "	    or   measurement_value is null )		 "
		 "	 order by measurement_date;\"			|"
		 "sql							|"
		 "cat							 ",
		 table_name,
		 station );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece( date, '^', input_buffer, 0 );
		piece( time, '^', input_buffer, 1 );
		salinity_record = new_salinity_record();
		salinity_record->date_string = strdup( date );
		salinity_record->time_string = strdup( time );
		list_append_pointer( salinity_record_list, salinity_record );
	}
	pclose( input_pipe );
	return salinity_record_list;
} /* get_salinity_record_list() */

char *get_station_salinity_sys_string(	char *application_name,
					char *optional_station )
{
	char sys_string[ 1024 ];
	char grep_station_process[ 128 ];

	if ( optional_station && *optional_station )
	{
		sprintf( grep_station_process,
			 "grep '^%s|'",
			 optional_station );
	}
	else
	{
		strcpy( grep_station_process, "cat" );
	}

	sprintf(sys_string,
		"station_datatype_list_all.sh %s 2>/dev/null		|"
		"grep '|salinity$'					|"
		"%s							|"
		"piece.e '|' 0 						 ",
		application_name,
		grep_station_process );

	return strdup( sys_string );
} /* get_station_salinity_sys_string() */

void output_to_process(
			FILE *output_pipe,
			char *station,
			LIST *salinity_record_list )
{
	SALINITY_RECORD *salinity_record;

	list_rewind( salinity_record_list );

	do {
		salinity_record =
			list_get_pointer(
				salinity_record_list );

		if ( salinity_record->conductivity )
		{
			salinity_record->salinity =
				conductivity2salinity(
					salinity_record->conductivity );
			
			fprintf( output_pipe,
			 	"%s,salinity,%s,%s^measurement_value=%lf\n",
			 	station,
			 	salinity_record->date_string,
			 	salinity_record->time_string,
			 	salinity_record->salinity );
		}
	} while( list_next( salinity_record_list ) );

} /* output_to_process() */

void populate_conductivity_values(	char *application_name,
					char *station,
					LIST *salinity_record_list )
{
	char sys_string[ 1024 ];
	char where[ 1024 ];
	SALINITY_RECORD *salinity_record;
	char *results_string;

	if ( !list_rewind( salinity_record_list ) ) return;

	do {
		salinity_record = list_get_pointer( salinity_record_list );

		sprintf(where,
			"station = '%s' and			"
			"datatype = 'conductivity' and		"
			"measurement_date = '%s' and		"
			"measurement_time = '%s' and		"
			"measurement_value != 0 and		"
			"measurement_value is not null		",
			station,
			salinity_record->date_string,
			salinity_record->time_string );

		sprintf( sys_string,
		 	 "get_folder_data	application=%s		"
			 "			select=measurement_value"
			 "			folder=measurement	"
			 "			where=\"%s\"		",
			application_name,
			where );

		results_string = pipe2string( sys_string );

		if ( results_string )
		{
			salinity_record->conductivity = atof( results_string );
			free( results_string );
		}
	} while( list_next( salinity_record_list ) );

} /* populate_conductivity_values() */

SALINITY_RECORD *new_salinity_record( void )
{
	return (SALINITY_RECORD *)calloc( 1, sizeof( SALINITY_RECORD ) );

} /* new_salinity_record() */

