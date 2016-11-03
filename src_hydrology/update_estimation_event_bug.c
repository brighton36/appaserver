/* update_estimation_event_bug.c */
/*
-------------------------------------------------------------
This script fixes the bug which resulted in storing the
source station instead of the destination station for
nearest neighbor estimations.

The process is to scan through the estimation_event_parameter
table for the "destination_station" parameter, and then
updating both estimation_event.station and
estimation_event_parameter.station.
-------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "piece.h"
#include "appaserver_library.h"

/* Constants */
/* --------- */
#define SELECT_CLAUSE		 "station,datatype,measurement_update_date,measurement_update_time,login_name,parameter_value"
#define WHERE_CLAUSE		 "measurement_update_method = 'nearest_neighbor' and parameter = 'destination_station' and parameter_value != station"

/* Prototypes */
/* ---------- */
void output_update_estimation_event_sys_string(
				char *station,
				char *datatype,
				char *measurement_update_date,
				char *measurement_update_time,
				char *login_name,
				char *new_station );

char *get_fetch_sys_string(	void );

void get_update_statement(	char *statement,
				char *table_name,
				char *station,
				char *new_station,
				char *datatype,
				char *measurement_update_date,
				char *measurement_update_time,
				char *login_name );

void piece_record(		char *station,
				char *datatype,
				char *measurement_update_date,
				char *measurement_update_time,
				char *login_name,
				char *parameter_value,
				char *record );

int main( int argc, char **argv )
{
	LIST *record_list;
	char *sys_string;
	char station[ 128 ];
	char datatype[ 128 ];
	char measurement_update_date[ 128 ];
	char measurement_update_time[ 128 ];
	char login_name[ 128 ];
	char parameter_value[ 128 ];
	char *record;

	sys_string = get_fetch_sys_string();
	record_list = pipe2list( sys_string );

	if ( list_rewind( record_list ) )
	{
		do {
			record = list_get_pointer( record_list );

			piece_record(	station,
					datatype,
					measurement_update_date,
					measurement_update_time,
					login_name,
					parameter_value,
					record );

			output_update_estimation_event_sys_string(
					station,
					datatype,
					measurement_update_date,
					measurement_update_time,
					login_name,
					parameter_value );

		} while( list_next( record_list ) );
	}

	exit( 0 );

} /* main() */

char *get_fetch_sys_string( void )
{
	char sys_string[ 1024 ];

	sprintf(sys_string,
		"get_folder_data	application=everglades		  "
		"			folder=estimation_event_parameter "
		"			select=\"%s\"			  "
		"			where=\"%s\"			  ",
		SELECT_CLAUSE,
		WHERE_CLAUSE );
		
	return strdup( sys_string );
} /* get_fetch_sys_string() */

void piece_record(	char *station,
			char *datatype,
			char *measurement_update_date,
			char *measurement_update_time,
			char *login_name,
			char *parameter_value,
			char *record )
{
	piece(	station,
		FOLDER_DATA_DELIMITER,
		record,
		0 );

	piece(	datatype,
		FOLDER_DATA_DELIMITER,
		record,
		1 );

	piece(	measurement_update_date,
		FOLDER_DATA_DELIMITER,
		record,
		2 );

	piece(	measurement_update_time,
		FOLDER_DATA_DELIMITER,
		record,
		3 );

	piece(	login_name,
		FOLDER_DATA_DELIMITER,
		record,
		4 );

	piece(	parameter_value,
		FOLDER_DATA_DELIMITER,
		record,
		5 );

} /* piece_record() */

void output_update_estimation_event_sys_string(
				char *station,
				char *datatype,
				char *measurement_update_date,
				char *measurement_update_time,
				char *login_name,
				char *new_station )
{
	char statement[ 1024 ];

	get_update_statement(	statement,
				"estimation_event",
				station,
				new_station,
				datatype,
				measurement_update_date,
				measurement_update_time,
				login_name );

	printf( "%s\n", statement );

	get_update_statement(	statement,
				"estimation_event_parameter",
				station,
				new_station,
				datatype,
				measurement_update_date,
				measurement_update_time,
				login_name );

	printf( "%s\n", statement );

} /* output_update_estimation_event_sys_string() */


void get_update_statement(	char *statement,
				char *table_name,
				char *station,
				char *new_station,
				char *datatype,
				char *measurement_update_date,
				char *measurement_update_time,
				char *login_name )
{
	sprintf(statement,
		"update %s					 	 "
		"	set station = '%s'				 "
		"	where station = '%s' 				 "
		"	  and datatype = '%s' 				 "
		"	  and measurement_update_date = '%s'			 "
		"	  and measurement_update_time = '%s'			 "
		"	  and measurement_update_method = 'nearest_neighbor' 	 "
		"	  and login_name = '%s';			 ",
		table_name,
		new_station,
		station,
		datatype,
		measurement_update_date,
		measurement_update_time,
		login_name );
		
} /* get_update_statement() */

