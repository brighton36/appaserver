/* ---------------------------------------------------	*/
/* src_creel/load_sport_measurements.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "hashtbl.h"
#include "piece.h"
#include "date.h"
#include "creel_load_library.h"
#include "creel_library.h"
#include "environ.h"
#include "date_convert.h"
#include "application.h"
#include "process.h"
#include "julian.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void insert_catches(		char *application_name,
				char *input_filename );

void delete_catch_measurements(
				char *application_name,
				char *input_filename );

int insert_catch_measurements(
				char *application_name,
				char *login_name,
				char *input_filename,
				char really_yn );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *login_name;
	char *input_filename;
	char really_yn;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	int lines_processed;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process login_name filename really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	login_name = argv[ 3 ];
	input_filename = argv[ 4 ];
	really_yn = *argv[ 5 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );

	appaserver_parameter_file = new_appaserver_parameter_file();

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_parameter_file->appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	document_output_body(
			document->application_name,
			document->onload_control_string );

	printf( "<h2>Load Sport Measurement Lengths\n" );
	fflush( stdout );
	system( "date '+%x %H:%M'" );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		delete_catch_measurements(
			application_name, input_filename );

		insert_catches(	application_name,
				input_filename );
	}

	lines_processed = insert_catch_measurements(
					application_name,
					login_name,
					input_filename,
					really_yn );

	if ( really_yn == 'y' )
	{
		printf( "<p>Process complete with %d lines.\n",
			lines_processed );
	}
	else
	{
		printf( "<p>Process not executed with %d lines.\n",
			lines_processed );
	}

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

#define DELETE_FIELD_LIST "fishing_purpose,census_date,interview_location,interview_number"

void delete_catch_measurements(	char *application_name,
				char *input_filename )
{
	FILE *input_file;
	FILE *catch_measurements_delete_pipe;
	char sys_string[ 1024 ];
	char input_string[ 1024 ];
	char interview_number[ 16 ];
	char interview_location_code[ 16 ];
	char *interview_location;
	char creel_census_date_mdyy[ 16 ];
	char census_date_international[ 256 ];
	char *table_name;

	table_name =
		get_table_name(
			application_name, "catch_measurements" );

	sprintf( sys_string,
"sort -u | delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FIELD_LIST );

	catch_measurements_delete_pipe = popen( sys_string, "w" );

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
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !piece(	creel_census_date_mdyy,
				',',
				input_string,
				SPORT_MEASUREMENTS_CENSUS_DATE_PIECE ) )
		{
			continue;
		}

		*census_date_international = '\0';

		date_convert_source_american(
				census_date_international,
				international,
				creel_census_date_mdyy );

		if ( !*census_date_international ) continue;

		if ( !piece(	interview_location_code,
				',',
				input_string,
				SPORT_MEASUREMENTS_INTERVIEW_LOCATION_PIECE ) )
		{
			continue;
		}

		interview_location =
			creel_library_get_interview_location(
				application_name,
				(char *)0 /* interview_location */,
				interview_location_code );

		if ( !interview_location ) continue;

		if ( !piece(	interview_number,
				',',
				input_string,
				SPORT_MEASUREMENTS_INTERVIEW_NUMBER_PIECE ) )
		{
			continue;
		}

		fprintf(catch_measurements_delete_pipe,
			"%s|%s|%s|%s\n",
			CREEL_CENSUS_SPORT,
		     	census_date_international,
			interview_location,
			interview_number );
	}

	fclose( input_file );
	pclose( catch_measurements_delete_pipe );

} /* delete_catch_measurements() */

#define INSERT_CATCH_MEASUREMENTS_FIELD_LIST	"fishing_purpose,census_date,interview_location,interview_number,family,genus,species,catch_measurement_number,length_millimeters"
#define INSERT_FISHING_TRIPS_FIELD_LIST	"fishing_purpose,census_date,interview_location,interview_number,fishing_area,last_changed_by,validation_date"
#define INSERT_CREEL_CENSUS_FIELD_LIST	"fishing_purpose,census_date,interview_location"

int insert_catch_measurements(	char *application_name,
				char *login_name,
				char *input_filename,
				char really_yn )
{
	FILE *input_file;
	FILE *catch_measurements_output_pipe = {0};
	FILE *fishing_trips_output_pipe = {0};
	FILE *creel_census_output_pipe = {0};
	char input_string[ 1024 ];
	char interview_number[ 16 ];
	char interview_location_code[ 16 ];
	char *interview_location;
	char census_date_mdyy[ 16 ];
	char census_date_international[ 256 ];
	char *now_date_international;
	char length_string[ 16 ];
	char florida_state_code[ 128 ];
	char family[ 128 ];
	char genus[ 128 ];
	char species[ 128 ];
	char sys_string[ 1024 ];
	char location_area[ 16 ];
	char location_zone[ 16 ];
	char fishing_area[ 16 ];
	char error_filename[ 128 ];
	FILE *error_file;
	int measurement_offset;
	int catch_measurement_number = 0;
	int line_number = 0;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( error_filename, "/tmp/fishing_trip_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	if ( really_yn == 'y' )
	{
		char *table_name =
			get_table_name(
				application_name, "catch_measurements" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate",
			 table_name,
			 INSERT_CATCH_MEASUREMENTS_FIELD_LIST );
	}
	else
	{
		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Insert into Catch Measurements' %s '|' left,left,left,right,left,left,left,right",
			 INSERT_CATCH_MEASUREMENTS_FIELD_LIST );
	}

	catch_measurements_output_pipe = popen( sys_string, "w" );

	if ( really_yn == 'y' )
	{
		char *table_name;

		table_name = get_table_name(
				application_name,
				"fishing_trips" );

		sprintf( sys_string,
"sort -u | insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate",
			 table_name,
			 INSERT_FISHING_TRIPS_FIELD_LIST );
		fishing_trips_output_pipe = popen( sys_string, "w" );

		table_name = get_table_name(
				application_name,
				"creel_census" );

		sprintf( sys_string,
"sort -u | insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate",
			 table_name,
			 INSERT_CREEL_CENSUS_FIELD_LIST );
		creel_census_output_pipe = popen( sys_string, "w" );

	}

	now_date_international = date_get_now_date_yyyy_mm_dd();

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );
	line_number++;

	while( get_line( input_string, input_file ) )
	{
		line_number++;
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !piece(	interview_number,
				',',
				input_string,
				SPORT_MEASUREMENTS_INTERVIEW_NUMBER_PIECE ) )
		{
			continue;
		}

		if ( !*interview_number ) continue;

		if ( !piece(	location_area,
				',',
				input_string,
				SPORT_MEASUREMENTS_LOCATION_AREA_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece location area in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !*location_area ) strcpy( location_area, "0" );

		if ( !piece(	location_zone,
				',',
				input_string,
				SPORT_MEASUREMENTS_LOCATION_ZONE_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece location zone in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		sprintf(	fishing_area,
				"%s%s",
				location_area,
				location_zone );

		if ( *fishing_area && !creel_library_exists_fishing_area(
				application_name,
				fishing_area ) )
		{
			fprintf(error_file,
		"Warning in line %d: invalid fishing area of (%s) in (%s)\n",
				line_number,
				fishing_area,
				input_string );
			continue;
		}

		if ( !piece(	census_date_mdyy,
				',',
				input_string,
				SPORT_MEASUREMENTS_CENSUS_DATE_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece census date in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !*census_date_mdyy ) continue;

		*census_date_international = '\0';

		date_convert_source_american(
				census_date_international,
				international,
				census_date_mdyy );

		if ( !*census_date_international )
		{
			fprintf(error_file,
			"Warning in line %d: invalid date format = (%s)\n",
				line_number,
				census_date_mdyy );
			continue;
		}

		if ( strcmp(	census_date_international,
				now_date_international ) > 0 )
		{
			fprintf(error_file,
			"Warning in line %d: date is in the future = (%s)\n",
				line_number,
				census_date_mdyy );
			continue;
		}

		if ( !piece(	interview_location_code,
				',',
				input_string,
				SPORT_MEASUREMENTS_INTERVIEW_LOCATION_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece interview location in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		interview_location =
			creel_library_get_interview_location(
				application_name,
				(char *)0 /* interview_location */,
				interview_location_code );

		if ( !interview_location )
		{
			fprintf(error_file,
"Warning in line %d: Cannot find interview location code (%s) in (%s)\n",
				line_number,
				interview_location_code,
				input_string );
			continue;
		}

		if ( !piece(	florida_state_code,
				',',
				input_string,
				SPORT_MEASUREMENTS_FLORIDA_CODE_PIECE ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot piece Florida code in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		*family = '\0';
		*species = '\0';
		*family = '\0';

		if ( !creel_load_library_get_family_genus_species(
					family,
					genus,
					species,
					florida_state_code,
					application_name ) )
		{
			fprintf(error_file,
		"Warning in line %d: cannot find Florida code = %s in (%s)\n",
				line_number,
				florida_state_code,
				input_string );
			continue;
		}

		measurement_offset = 0;
		while( 1 )
		{
			if ( !piece(
				length_string,
				',',
				input_string,
				SPORT_MEASUREMENTS_STARTING_LENGTH_PIECE +
				measurement_offset ) )
			{
				break;
			}

			if ( !atoi( length_string ) ) break;

			fprintf(catch_measurements_output_pipe,
				"%s|%s|%s|%s|%s|%s|%s|%d|%s\n",
				CREEL_CENSUS_SPORT,
				census_date_international,
				interview_location,
				interview_number,
				family,
				genus,
				species,
				++catch_measurement_number,
				length_string );

			measurement_offset++;
		}

		if ( fishing_trips_output_pipe )
		{
			fprintf(fishing_trips_output_pipe,
				"%s|%s|%s|%s|%s|%s|%s\n",
				CREEL_CENSUS_SPORT,
				census_date_international,
				interview_location,
				interview_number,
				fishing_area,
				login_name,
				now_date_international );
		}

		if ( creel_census_output_pipe )
		{
			fprintf(creel_census_output_pipe,
				"%s|%s|%s\n",
				CREEL_CENSUS_SPORT,
				census_date_international,
				interview_location );
		}
	}

	fclose( input_file );
	fclose( error_file );
	pclose( catch_measurements_output_pipe );

	if ( fishing_trips_output_pipe )
		pclose( fishing_trips_output_pipe );
	if ( creel_census_output_pipe )
		pclose( creel_census_output_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Catch Measurement Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );

	return line_number;

} /* insert_catch_measurements() */

#define INSERT_CATCHES_FIELD_LIST	"fishing_purpose,census_date,interview_location,interview_number,family,genus,species,kept_count"

void insert_catches(	char *application_name,
			char *input_filename )
{
	FILE *input_file;
	FILE *input_pipe;
	FILE *output_file;
	FILE *catches_output_pipe;
	char output_filename[ 128 ];
	char input_string[ 1024 ];
	char interview_number[ 16 ];
	char interview_location_code[ 16 ];
	char *interview_location;
	char census_date_mdyy[ 16 ];
	char census_date_international[ 256 ];
	char *now_date_international;
	char length_string[ 16 ];
	char florida_state_code[ 128 ];
	char family[ 128 ];
	char genus[ 128 ];
	char species[ 128 ];
	char sys_string[ 1024 ];
	char location_area[ 16 ];
	char location_zone[ 16 ];
	char fishing_area[ 16 ];
	int measurement_offset;
	char *table_name;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	sprintf( output_filename, "/tmp/load_%d", getpid() );

	if ( ! ( output_file = fopen( output_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", output_filename );
		fclose( input_file );
		exit( 1 );
	}

	now_date_international = date_get_now_date_yyyy_mm_dd();

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !piece(	interview_number,
				',',
				input_string,
				SPORT_MEASUREMENTS_INTERVIEW_NUMBER_PIECE ) )
		{
			continue;
		}

		if ( !*interview_number ) continue;

		if ( !piece(	location_area,
				',',
				input_string,
				SPORT_MEASUREMENTS_LOCATION_AREA_PIECE ) )
		{
			continue;
		}

		if ( !*location_area ) strcpy( location_area, "0" );

		if ( !piece(	location_zone,
				',',
				input_string,
				SPORT_MEASUREMENTS_LOCATION_ZONE_PIECE ) )
		{
			continue;
		}

		sprintf(	fishing_area,
				"%s%s",
				location_area,
				location_zone );

		if ( *fishing_area && !creel_library_exists_fishing_area(
				application_name,
				fishing_area ) )
		{
			continue;
		}

		if ( !piece(	census_date_mdyy,
				',',
				input_string,
				SPORT_MEASUREMENTS_CENSUS_DATE_PIECE ) )
		{
			continue;
		}

		if ( !*census_date_mdyy ) continue;

		*census_date_international = '\0';

		date_convert_source_american(
				census_date_international,
				international,
				census_date_mdyy );

		if ( !*census_date_international )
		{
			continue;
		}

		if ( strcmp(	census_date_international,
				now_date_international ) > 0 )
		{
			continue;
		}

		if ( !piece(	interview_location_code,
				',',
				input_string,
				SPORT_MEASUREMENTS_INTERVIEW_LOCATION_PIECE ) )
		{
			continue;
		}

		interview_location =
			creel_library_get_interview_location(
				application_name,
				(char *)0 /* interview_location */,
				interview_location_code );

		if ( !interview_location )
		{
			continue;
		}

		if ( !piece(	florida_state_code,
				',',
				input_string,
				SPORT_MEASUREMENTS_FLORIDA_CODE_PIECE ) )
		{
			continue;
		}

		*family = '\0';
		*species = '\0';
		*family = '\0';

		if ( !creel_load_library_get_family_genus_species(
					family,
					genus,
					species,
					florida_state_code,
					application_name ) )
		{
			continue;
		}

		measurement_offset = 0;
		while( 1 )
		{
			if ( !piece(
				length_string,
				',',
				input_string,
				SPORT_MEASUREMENTS_STARTING_LENGTH_PIECE +
				measurement_offset ) )
			{
				break;
			}

			if ( !atoi( length_string ) ) break;

			fprintf(output_file,
				"%s|%s|%s|%s|%s|%s|%s\n",
				CREEL_CENSUS_SPORT,
				census_date_international,
				interview_location,
				interview_number,
				family,
				genus,
				species );

			measurement_offset++;
		}

	}

	fclose( input_file );
	fclose( output_file );

	sprintf( sys_string, 
		 "sort %s | group.e | piece_shift_left.e '|'",
		 output_filename );

	input_pipe = popen( sys_string, "r" );

	table_name = get_table_name(
			application_name,
			"catches" );

	sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate",
		 table_name,
		 INSERT_CATCHES_FIELD_LIST );

	catches_output_pipe = popen( sys_string, "w" );

	while( get_line( input_string, input_pipe ) )
	{
		fprintf( catches_output_pipe, "%s\n", input_string );
	}
	pclose( input_pipe );
	pclose( catches_output_pipe );

	sprintf( sys_string, "rm -f %s", output_filename );
	system( sys_string );

} /* insert_catches() */

