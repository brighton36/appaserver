/* delete_sport_creel_census.c */
/* --------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "date_convert.h"
#include "date.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "creel_library.h"
#include "creel_load_library.h"

#define FIELD_LIST		"fishing_purpose,census_date,interview_location"

int main( int argc, char **argv )
{
	char *application_name;
	char *input_filename;
	FILE *input_file;
/* Not necessary.
	FILE *creel_census_delete_pipe;
*/
	FILE *fishing_trips_delete_pipe;
	FILE *catches_delete_pipe;
	char input_string[ 1024 ];
	char census_date_mdyy[ 16 ];
	char interview_location_code_string[ 16 ];
	char *interview_location_string;
	char census_date_international[ 16 ];
	char sys_string[ 1024 ];
	char *table_name;

	if ( argc != 3 )
	{
		fprintf( stderr, 
"Usage: %s application filename\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	input_filename = argv[ 2 ];

/* Not necessary.
	table_name =
		get_table_name(
			application_name, "creel_census" );

	sprintf( sys_string,
"sort -u | delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 FIELD_LIST );

	creel_census_delete_pipe = popen( sys_string, "w" );
*/

	table_name =
		get_table_name(
			application_name, "fishing_trips" );

	sprintf( sys_string,
"sort -u | delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 FIELD_LIST );

	fishing_trips_delete_pipe = popen( sys_string, "w" );

	table_name =
		get_table_name(
			application_name, "catches" );

	sprintf( sys_string,
"sort -u | delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 FIELD_LIST );

	catches_delete_pipe = popen( sys_string, "w" );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		if ( !piece(	census_date_mdyy,
				',',
				input_string,
				SPORT_CENSUS_DATE_PIECE ) )
		{
			continue;
		}

		if ( !isdigit( *census_date_mdyy ) )
		{
			continue;
		}

		*census_date_international = '\0';

		date_convert_source_american(
				census_date_international,
				international,
				census_date_mdyy );

		if ( !*census_date_international )
		{
			continue;
		}

		if ( !piece(	interview_location_code_string,
				',',
				input_string,
				SPORT_INTERVIEW_LOCATION_PIECE ) )
		{
			continue;
		}

		interview_location_string =
			creel_library_get_interview_location(
				application_name,
				(char *)0 /* interview_location */,
				interview_location_code_string );

		if ( strcmp(	interview_location_string,
				interview_location_code_string ) == 0 )
		{
			continue;
		}

/* Not necessary.
		fprintf(creel_census_delete_pipe,
			"%s|%s|%s|%s\n",
			CREEL_CENSUS_SPORT,
			census_date_international,
			census_time,
			interview_location_string );
*/

		fprintf(fishing_trips_delete_pipe,
			"%s|%s|%s\n",
			CREEL_CENSUS_SPORT,
			census_date_international,
			interview_location_string );

		fprintf(catches_delete_pipe,
			"%s|%s|%s\n",
			CREEL_CENSUS_SPORT,
			census_date_international,
			interview_location_string );
	}

	fclose( input_file );
/* Not necessary.
	pclose( creel_census_delete_pipe );
*/
	pclose( fishing_trips_delete_pipe );
	pclose( catches_delete_pipe );

	return 0;
} /* main() */

