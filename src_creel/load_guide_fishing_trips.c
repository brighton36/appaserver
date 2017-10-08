/* ---------------------------------------------------	*/
/* src_creel/load_guide_fishing_trips.c			*/
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
#include "piece.h"
#include "date.h"
#include "creel_library.h"
#include "creel_load_library.h"
#include "environ.h"
#include "date_convert.h"
#include "application.h"
#include "process.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define CATCHES_INCREMENT			5
#define GUIDE_CATCHES_RESEARCHER		"none"
#define GUIDE_CATCHES_INTERVIEW_LOCATION	"none"
#define GUIDE_FISHING_PURPOSE			"guide"

/* Prototypes */
/* ---------- */
LIST *get_catches_list(		FILE *error_file,
				char *input_string,
				int line_number,
				char *application_name );

void output_catches(		FILE *catches_output_pipe,
				FILE *fishing_trips_output_pipe,
				FILE *error_file,
				int *catch_count,
				char *input_string,
				int line_number,
				char really_yn,
				char *application_name,
				char *census_date_international,
				char *interview_number );

char *get_creel_census_date_mdyy(
				char *compressed_date_string );

void delete_fishing_trips(
				char *application_name,
				char *input_filename );

void insert_fishing_trips(	int *fishing_trip_count,
				int *catch_count,
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
	int fishing_trip_count = 0;
	int catch_count = 0;

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

	printf( "<h2>Load Guide Fishing Trips\n" );
	fflush( stdout );
	if ( system( "date '+%x %H:%M'" ) );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		delete_fishing_trips( application_name, input_filename );
	}

	insert_fishing_trips(	&fishing_trip_count,
				&catch_count,
				application_name,
				login_name,
				input_filename,
				really_yn );

	if ( really_yn == 'y' )
	{
		printf(
"<p>Process complete with %d fishing trips and %d catches.\n",
			fishing_trip_count,
			catch_count );

		process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	}
	else
	{
		printf(
"<p>Process NOT executed with %d fishing trips and %d catches.\n",
			fishing_trip_count,
			catch_count );
	}

	document_close();

	return 0;

} /* main() */

#define DELETE_FIELD_LIST	"fishing_purpose,census_date,interview_location"

void delete_fishing_trips(	char *application_name,
				char *input_filename )
{
	FILE *input_file;
	FILE *fishing_trips_delete_pipe;
	FILE *catches_delete_pipe;
	char sys_string[ 1024 ];
	char input_string[ 1024 ];
	char compressed_census_date[ 256 ];
	char *creel_census_date_mdyy;
	char census_date_international[ 256 ];
	char *table_name;

	table_name =
		get_table_name(
			application_name, "fishing_trips" );

	sprintf( sys_string,
"delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FIELD_LIST );

	fishing_trips_delete_pipe = popen( sys_string, "w" );

	table_name =
		get_table_name(
			application_name, "catches" );

	sprintf( sys_string,
"delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FIELD_LIST );

	catches_delete_pipe = popen( sys_string, "w" );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	/* Skip the first line */
	/* ------------------- */
	get_line( input_string, input_file );

	while( get_line( input_string, input_file ) )
	{
		trim_character( input_string, '"', input_string );
		trim_character( input_string, '\'', input_string );

		if ( !piece(	compressed_census_date,
				',',
				input_string,
				GUIDE_CATCH_CENSUS_DATE_PIECE ) )
		{
			continue;
		}

		creel_census_date_mdyy =
			get_creel_census_date_mdyy(
				compressed_census_date );

		*census_date_international = '\0';

		date_convert_source_american(
				census_date_international,
				international,
				creel_census_date_mdyy );

		if ( !*census_date_international )
		{
			continue;
		}

/*
		fprintf(creel_census_delete_pipe,
			"%s|%s|%s\n",
			GUIDE_FISHING_PURPOSE,
		     	census_date_international,
			GUIDE_CATCHES_INTERVIEW_LOCATION );
*/

		fprintf(fishing_trips_delete_pipe,
			"%s|%s|%s\n",
			GUIDE_FISHING_PURPOSE,
		     	census_date_international,
			GUIDE_CATCHES_INTERVIEW_LOCATION );

		fprintf(catches_delete_pipe,
			"%s|%s|%s\n",
			GUIDE_FISHING_PURPOSE,
		     	census_date_international,
			GUIDE_CATCHES_INTERVIEW_LOCATION );
	}

	fclose( input_file );
	pclose( fishing_trips_delete_pipe );
	pclose( catches_delete_pipe );

} /* delete_fishing_permits() */

#define INSERT_CREEL_CENSUS_FIELD_LIST	"fishing_purpose,census_date,interview_location,researcher"
#define INSERT_FISHING_TRIPS_FIELD_LIST	"fishing_purpose,census_date,interview_location,interview_number,permit_code,hours_fishing,number_of_people_fishing,family,genus,species_preferred,fishing_area,last_changed_by,validation_date"
#define INSERT_CATCHES_FIELD_LIST	"fishing_purpose,census_date,interview_location,interview_number,family,genus,species,kept_count,released_count"
#define INSERT_PERMITS_FIELD_LIST	"permit_code"

void insert_fishing_trips(	int *fishing_trip_count,
				int *catch_count,
				char *application_name,
				char *login_name,
				char *input_filename,
				char really_yn )
{
	FILE *input_file;
	FILE *creel_census_output_pipe = {0};
	FILE *fishing_trips_output_pipe;
	FILE *catches_output_pipe = {0};
	FILE *permits_output_pipe = {0};
	char input_string[ 1024 ];
	char census_date_international[ 256 ];
	char *now_date_international;
	char permit_code[ 256 ];
	char florida_code_string[ 16 ];
	char family[ 128 ];
	char genus[ 128 ];
	char compressed_census_date[ 256 ];
	char species[ 128 ];
	char interview_number[ 256 ];
	char hours_fishing[ 256 ];
	char number_of_people_fishing[ 256 ];
	char *creel_census_date_mdyy;
	char location_area[ 16 ];
	char location_zone[ 16 ];
	char fishing_area[ 16 ];
	char sys_string[ 1024 ];
	char error_filename[ 128 ];
	char sql_error_filename[ 128 ];
	FILE *error_file;
	int line_number = 0;
	char *now_string = pipe2string( "now.sh ymd" );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	sprintf( error_filename, "/tmp/fishing_trip_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	sprintf( sql_error_filename,
		 "/tmp/sql_fishing_trip_error_%d.txt",
		 getpid() );

	if ( really_yn == 'y' )
	{
		char *table_name =
			get_table_name( application_name, "creel_census" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate",
			 table_name,
			 INSERT_CREEL_CENSUS_FIELD_LIST );
		creel_census_output_pipe = popen( sys_string, "w" );

		table_name =
			get_table_name( application_name, "fishing_trips" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate >> %s 2>&1",
			 table_name,
			 INSERT_FISHING_TRIPS_FIELD_LIST,
			 sql_error_filename );
		fishing_trips_output_pipe = popen( sys_string, "w" );

		table_name =
			get_table_name( application_name, "catches" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e >> %s 2>&1",
			 table_name,
			 INSERT_CATCHES_FIELD_LIST,
			 sql_error_filename );
		catches_output_pipe = popen( sys_string, "w" );

		table_name =
			get_table_name( application_name, "permits" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate >> %s",
			 table_name,
			 INSERT_PERMITS_FIELD_LIST,
			 sql_error_filename );
		permits_output_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
"queue_top_bottom_lines.e 50 | html_table.e 'Insert into Fishing Trips and Catches' %s,family,genus,species,kept,released '|'",
			 INSERT_FISHING_TRIPS_FIELD_LIST );
		fishing_trips_output_pipe = popen( sys_string, "w" );
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
				GUIDE_CATCH_INTERVIEW_NUMBER_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece interview number in (%s)\n",
				input_string );
			fflush( error_file );
			continue;
		}

		if ( !piece(	permit_code,
				',',
				input_string,
				GUIDE_CATCH_PERMIT_CODE_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece permit code in (%s)\n",
				input_string );
			fflush( error_file );
			continue;
		}

		if ( !piece(	compressed_census_date,
				',',
				input_string,
				GUIDE_CATCH_CENSUS_DATE_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece census date in (%s)\n",
				input_string );
			fflush( error_file );
			continue;
		}

		creel_census_date_mdyy =
			get_creel_census_date_mdyy(
				compressed_census_date );

		*census_date_international = '\0';

		if ( !date_convert_source_american(
				census_date_international,
				international,
				creel_census_date_mdyy )
		||   !*census_date_international )
		{
			fprintf(error_file,
			"Warning in line %d: invalid date format = (%s)\n",
				line_number,
				creel_census_date_mdyy );
			fflush( error_file );
			continue;
		}

		if ( strcmp(	census_date_international,
				now_date_international ) > 0 )
		{
			fprintf(error_file,
			"Warning: date is in the future = (%s)\n",
				creel_census_date_mdyy );
			fflush( error_file );
			continue;
		}

		if ( !piece(	hours_fishing,
				',',
				input_string,
				GUIDE_CATCH_FISHING_DURATION_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece fishing duration in (%s)\n",
				input_string );
			fflush( error_file );
			continue;
		}

		if ( !piece_double_quoted(
				location_area,
				',',
				input_string,
				GUIDE_CATCH_FISHING_AREA_PIECE ) )
		{
			*location_area = '\0';
		}

		if ( !piece_double_quoted(
				location_zone,
				',',
				input_string,
				GUIDE_CATCH_FISHING_AREA_ZONE_PIECE ) )
		{
			*location_zone = '\0';
		}

		sprintf( fishing_area,
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
			fflush( error_file );
			continue;
		}

		if ( !piece(	number_of_people_fishing,
				',',
				input_string,
				GUIDE_CATCH_ONBOARD_FISHING_COUNT_PIECE ) )
		{
			*number_of_people_fishing = '\0';
		}

		if ( !piece(	florida_code_string,
				',',
				input_string,
				GUIDE_CATCH_TARGETED_FLORIDA_CODE_PIECE ) )
		{
			*florida_code_string = '\0';
		}

		if ( !creel_load_library_get_family_genus_species(
				family,
				genus,
				species,
				florida_code_string,
				application_name ) )
		{
			*family = '\0';
			*genus = '\0';
			*species = '\0';
		}

		if ( creel_census_output_pipe )
		{
			fprintf(creel_census_output_pipe,
				"%s|%s|%s|%s\n",
				GUIDE_FISHING_PURPOSE,
				census_date_international,
				GUIDE_CATCHES_INTERVIEW_LOCATION,
				GUIDE_CATCHES_RESEARCHER );
		}

		fprintf(fishing_trips_output_pipe,
			"%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
			GUIDE_FISHING_PURPOSE,
			census_date_international,
			GUIDE_CATCHES_INTERVIEW_LOCATION,
			interview_number,
			permit_code,
			hours_fishing,
			number_of_people_fishing,
			family,
			genus,
			species,
			fishing_area,
			login_name,
			now_string );

		(*fishing_trip_count)++;

		if ( really_yn == 'y' )
		{
			fprintf( fishing_trips_output_pipe, "\n" );

			fprintf( permits_output_pipe,
				 "%s\n",
				 permit_code );
		}

		output_catches(	catches_output_pipe,
				fishing_trips_output_pipe,
				error_file,
				catch_count,
				input_string,
				line_number,
				really_yn,
				application_name,
				census_date_international,
				interview_number );

	}

	fclose( input_file );
	fclose( error_file );
	pclose( fishing_trips_output_pipe );

	if ( creel_census_output_pipe )
		pclose( creel_census_output_pipe );

	if ( catches_output_pipe )
		pclose( catches_output_pipe );

	if ( permits_output_pipe )
		pclose( permits_output_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'File Errors' '' '|'",
			 error_filename );
		if ( system( sys_string ) );
	}

	sprintf( sys_string, "rm %s", error_filename );
	if ( system( sys_string ) );

	if ( really_yn == 'y' )
	{
		if ( timlib_file_populated( sql_error_filename ) )
		{
			sprintf( sys_string,
"cat %s | html_table.e 'SQL Errors' '' '|'",
			 	sql_error_filename );
			if ( system( sys_string ) );
		}

		sprintf( sys_string, "rm %s", sql_error_filename );
		if ( system( sys_string ) );
	}

} /* insert_fishing_trips() */

char *get_creel_census_date_mdyy(
				char *compressed_date_string )
{
	static char return_date[ 16 ];

	if ( strlen( compressed_date_string ) == 5 )
	{
		*(return_date + 0) = '0';
	}
	else
	if ( strlen( compressed_date_string ) == 6 )
	{
		*(return_date + 0) = *compressed_date_string++;
	}
	else
	{
		return (char *)0;
	}

	*(return_date + 1) = *compressed_date_string++;
	*(return_date + 2) = '/';
	*(return_date + 3) = *compressed_date_string++;
	*(return_date + 4) = *compressed_date_string++;
	*(return_date + 5) = '/';

	if ( *compressed_date_string <= '3' )
	{
		*(return_date + 6) = '2';
		*(return_date + 7) = '0';
	}
	else
	{
		*(return_date + 6) = '1';
		*(return_date + 7) = '9';
	}
	*(return_date + 8) = *compressed_date_string++;
	*(return_date + 9) = *compressed_date_string++;

	return return_date;

} /* get_creel_census_date_mdyy() */

void output_catches(	FILE *catches_output_pipe,
			FILE *fishing_trips_output_pipe,
			FILE *error_file,
			int *catch_count,
			char *input_string,
			int line_number,
			char really_yn,
			char *application_name,
			char *census_date_international,
			char *interview_number )
{
	LIST *catches_list;
	CATCHES *catches;

	catches_list = get_catches_list(
				error_file,
				input_string,
				line_number,
				application_name );

	if ( !list_rewind( catches_list ) )
	{
		if ( really_yn != 'y' )
		{
			fprintf(fishing_trips_output_pipe,
				"\n" );
		}
		return;
	}

	do {
		catches = list_get_pointer( catches_list );

		if ( ! ( catches->kept_integer +
			 catches->released_integer ) )
		{
			continue;
		}

		if ( really_yn == 'y' )
		{
			fprintf(catches_output_pipe,
				"%s|%s|%s|%s|%s|%s|%s|%d|%d\n",
				GUIDE_FISHING_PURPOSE,
				census_date_international,
				GUIDE_CATCHES_INTERVIEW_LOCATION,
				interview_number,
				catches->family,
				catches->genus,
				catches->species,
				catches->kept_integer,
				catches->released_integer );

		}
		else
		{
			if ( list_at_head( catches_list ) )
			{
				fprintf(fishing_trips_output_pipe,
					"|%s|%s|%s|%d|%d\n",
					catches->family,
					catches->genus,
					catches->species,
					catches->kept_integer,
					catches->released_integer );
			}
			else
			{
				fprintf(fishing_trips_output_pipe,
					"|||||||||||%s|%s|%s|%d|%d\n",
					catches->family,
					catches->genus,
					catches->species,
					catches->kept_integer,
					catches->released_integer );
			}
		}
		(*catch_count)++;
	} while( list_next( catches_list ) );

	list_free_data( catches_list );
	list_free( catches_list );

} /* output_catches() */

LIST *get_catches_list(	FILE *error_file,
			char *input_string,
			int line_number,
			char *application_name )
{
	LIST *catches_list = list_new();
	CATCHES *catches;
	int catch_offset = 0;
	char florida_code_string[ 16 ];
	char kept_count[ 16 ];
	char released_count[ 16 ];
	char family[ 128 ];
	char genus[ 128 ];
	char species[ 128 ];

	while( 1 )
	{
		if ( !piece(
			florida_code_string,
			',',
			input_string,
			GUIDE_CATCH_STARTING_FLORIDA_CODE_PIECE +
			catch_offset )
		||   !*florida_code_string )
		{
			break;
		}

		if ( !creel_load_library_get_family_genus_species(
					family,
					genus,
					species,
					florida_code_string,
					application_name ) )
		{
			fprintf(error_file,
	"Warning in line %d: Cannot get florida code (%s) in (%s)\n",
				line_number,
				florida_code_string,
				input_string );
			fflush( error_file );
			catch_offset += CATCHES_INCREMENT;
			continue;
		}

		if ( !piece(
			kept_count,
			',',
			input_string,
			GUIDE_CATCH_STARTING_FLORIDA_CODE_PIECE +
			catch_offset + 1 ) )
		{
			fprintf(error_file,
		"Warning: Cannot get kept count (%s) in (%s)\n",
				florida_code_string,
				input_string );
			fflush( error_file );
			break;
		}

		if ( !piece(
			released_count,
			',',
			input_string,
			GUIDE_CATCH_STARTING_FLORIDA_CODE_PIECE +
			catch_offset + 2 ) )
		{
			fprintf(error_file,
		"Warning: Cannot get released count (%s) in (%s)\n",
				florida_code_string,
				input_string );
			fflush( error_file );
			break;
		}

		catches = creel_library_get_or_set_catches(
						catches_list,
						family,
						genus,
						species );

		catches->kept_integer += atoi( kept_count );
		catches->released_integer += atoi( released_count );

		catch_offset += CATCHES_INCREMENT;
	}
	return catches_list;
} /* get_catches_list(); */

