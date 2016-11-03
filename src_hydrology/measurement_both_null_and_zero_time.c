/* measurement_both_null_and_zero_time.c	        */
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "appaserver_library.h"

void delete_zero_time(
				char *application_name,
				char *station,
				char *datatype,
				LIST *date_both_null_and_zero_time_list );

LIST *get_date_both_null_and_zero_time_list(
				char *application_name,
				char *station,
				char *datatype );

int main( int argc, char **argv )
{
	char *application_name;
	FILE *station_datatype_input_pipe;
	char station[ 128 ];
	char datatype[ 128 ];
	char input_buffer[ 512 ];
	char sys_string[ 1024 ];
	LIST *date_both_null_and_zero_time_list;
	char *with_delete_yn;
	char *optional_station;

	if ( argc < 3 )
	{
		fprintf( stderr,
			 "Usage: %s application with_delete_yn [station]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	with_delete_yn = argv[ 2 ];

	if ( argc == 4 )
		optional_station = argv[ 3 ];
	else
		optional_station = (char *)0;

	if ( optional_station )
	{
		sprintf(sys_string,
			"station_datatype_list_all.sh %s 2>/dev/null	|"
			"grep '^%s|'					 ",
			application_name,
			optional_station );
	}
	else
	{
		sprintf(sys_string,
			"station_datatype_list_all.sh %s 2>/dev/null",
			application_name );
	}

	station_datatype_input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, station_datatype_input_pipe ) )
	{
		piece( station, '|', input_buffer, 0 );
		piece( datatype, '|', input_buffer, 1 );

		date_both_null_and_zero_time_list =
			get_date_both_null_and_zero_time_list(
				application_name,
				station,
				datatype );

		if ( date_both_null_and_zero_time_list )
		{
			if ( list_length( date_both_null_and_zero_time_list ) )
			{
				printf( "%s/%s: %s\n",
					station,
					datatype,
					list_display(
					date_both_null_and_zero_time_list ) );

				if ( *with_delete_yn == 'y' )
				{
					delete_zero_time(
					application_name,
					station,
					datatype,
					date_both_null_and_zero_time_list );
				}
			}

			list_free_string_list(
				date_both_null_and_zero_time_list );
		}
	}

	pclose( station_datatype_input_pipe );
	exit( 0 );

} /* main() */

LIST *get_date_both_null_and_zero_time_list(
				char *application_name,
				char *station,
				char *datatype )
{
	LIST *date_with_two_measurements_list;
	char sys_string[ 1024 ];
	char *table_name;
	LIST *date_both_null_and_zero_time_list = list_new_list();
	char *date_with_two_measurements = {0};

	table_name = get_table_name( application_name, "measurement" );

	sprintf( sys_string,
		 "echo \"select measurement_date			 "
		 "	 from %s					 "
		 "	 where station = '%s'				 "
		 "	   and datatype = '%s'				 "
		 "	 order by measurement_date;\"			|"
		 "sql							|"
		 "group.e						|"
		 "grep '^2|'						|"
		 "piece.e '|' 1						 ",
		 table_name,
		 station,
		 datatype );

	date_with_two_measurements_list = pipe2list( sys_string );

	if ( list_rewind( date_with_two_measurements_list ) )
	{
		char where[ 1024 ];
		char *input_string;

		do {
			date_with_two_measurements =
				list_get_pointer(
					date_with_two_measurements_list );

			sprintf( where,
				 "station = '%s' and			"
				 "datatype = '%s' and			"
				 "measurement_date = '%s' and		"
				 "measurement_time = 'null'		",
				 station,
				 datatype,
				 date_with_two_measurements );

			sprintf( sys_string,
				 "get_folder_data application=%s	"
				 "		  select=count		"
				 "		  folder=measurement	"
				 "		  where=\"%s\"		",
				 application_name,
				 where );

			input_string = pipe2string( sys_string );

			if ( !input_string || !atoi( input_string ) )
			{
				break;
			}

			list_append_pointer(
				date_both_null_and_zero_time_list,
				date_with_two_measurements );
				
		} while( list_next( date_with_two_measurements_list ) );
	}

	return date_both_null_and_zero_time_list;

} /* get_date_both_null_and_zero_time_list() */

void delete_zero_time(
		char *application_name,
		char *station,
		char *datatype,
		LIST *date_both_null_and_zero_time_list )
{
	char *date_both_null_and_zero_time;
	char where[ 1024 ];
	FILE *output_pipe;
	char *table_name;

	if ( !list_rewind( date_both_null_and_zero_time_list ) ) return;

	table_name = get_table_name( application_name, "measurement" );
	output_pipe = popen( "sql", "w" );

	do {
		date_both_null_and_zero_time =
			list_get_pointer(
				date_both_null_and_zero_time_list );

		sprintf( where,
			 "station = '%s' and			"
			 "datatype = '%s' and			"
			 "measurement_date = '%s' and		"
			 "measurement_time != 'null'		",
			 station,
			 datatype,
			 date_both_null_and_zero_time );

		fprintf( output_pipe,
			 "delete from %s where %s;\n",
			 table_name,
			 where );

	} while( list_next( date_both_null_and_zero_time_list ) );
	pclose( output_pipe );
} /* delete_zero_time() */

