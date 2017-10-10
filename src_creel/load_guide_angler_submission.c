/* ---------------------------------------------------	*/
/* src_creel/load_guide_angler_submission.c		*/
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
#define PERMITS_PRIMARY_KEY			"permit_code"
#define QUEUE_TOP_BOTTOM_LINES			50
#define PERMIT_CODE_INTERVIEW_NUMBER_DELIMITER	'|'
#define CATCHES_INCREMENT			5
#define GUIDE_CATCHES_RESEARCHER		"none"
#define GUIDE_CATCHES_INTERVIEW_LOCATION	"none"
#define GUIDE_FISHING_PURPOSE			"guide"
#define HEADER_COLUMN_A				"Permitee Name"

/* Prototypes */
/* ---------- */
DICTIONARY *get_interview_number_dictionary(
				char *application_name,
				char *input_filename );

LIST *get_catches_list(		FILE *error_file,
				char *input_string,
				int line_number,
				char *application_name );

int get_starting_next_reference_number(
				char *application_name,
				int input_record_count,
				char really_yn );

int get_input_record_count(	char *input_filename );

void delete_fishing_trips(
				char *application_name,
				char *input_filename );

void insert_fishing_trips(	char *application_name,
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

	if ( !input_filename || strcmp( input_filename, "filename" ) == 0 )
	{
		printf( "<h3>Please transmit a file.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( really_yn == 'y' )
	{
		delete_fishing_trips( application_name, input_filename );
	}

	insert_fishing_trips(	application_name,
				login_name,
				input_filename,
				really_yn );

	if ( really_yn == 'y' )
		printf( "<h3>Process complete.</h3>\n" );
	else
		printf( "<h3>Process NOT executed.</h3>\n" );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

#define INSERT_CREEL_CENSUS_FIELD_LIST	"fishing_purpose,census_date,interview_location,researcher"
#define INSERT_FISHING_TRIPS_FIELD_LIST	"fishing_purpose,census_date,interview_location,interview_number,permit_code,hours_fishing,number_of_people_fishing,family,genus,species_preferred,fishing_area,guide_angler_submission_yn,last_changed_by,validation_date"
#define INSERT_CATCHES_FIELD_LIST	"fishing_purpose,census_date,interview_location,interview_number,family,genus,species,kept_count,released_count"
#define INSERT_GUIDE_ANGLERS_FIELD_LIST	"guide_angler_name"
#define INSERT_PERMITS_FIELD_LIST	"permit_code,guide_angler_name"

void insert_fishing_trips(	char *application_name,
				char *login_name,
				char *input_filename,
				char really_yn )
{
	FILE *input_file;
	FILE *creel_census_output_pipe = {0};
	FILE *fishing_trips_output_pipe;
	FILE *catches_output_pipe = {0};
	FILE *guide_anglers_output_pipe = {0};
	FILE *permits_insert_pipe = {0};
	FILE *permits_update_pipe = {0};
	char input_string[ 4096 ];
	char census_date_international[ 256 ];
	char *now_date_international;
	char permit_code[ 256 ];
	char common_name_string[ 128 ];
	char family[ 128 ];
	char genus[ 128 ];
	char species[ 128 ];
	char hours_fishing[ 256 ];
	char number_of_people_fishing[ 256 ];
	char guide_angler_name[ 256 ];
	char census_date_mmddyyyy[ 256 ];
	char location_area[ 16 ];
	char location_zone[ 16 ];
	char fishing_area[ 16 ];
	char sys_string[ 1024 ];
	char error_filename[ 128 ];
	FILE *error_file;
	char sql_error_filename[ 128 ];
	int line_number = 0;
	int input_record_count;
	int next_reference_number = 0;
	LIST *catches_list;
	CATCHES *catches;
	char *now_string = pipe2string( "now.sh ymd" );

	input_record_count = get_input_record_count( input_filename );

	next_reference_number =
		get_starting_next_reference_number(
			application_name,
			input_record_count,
			really_yn );

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

	sprintf(	sql_error_filename,
			"/tmp/fishing_trip_sql_error_%d.txt",
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
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | cat >> %s",
			 table_name,
			 INSERT_FISHING_TRIPS_FIELD_LIST,
			 sql_error_filename );
		fishing_trips_output_pipe = popen( sys_string, "w" );

		table_name =
			get_table_name( application_name, "catches" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | cat >> %s",
			 table_name,
			 INSERT_CATCHES_FIELD_LIST,
			 sql_error_filename );
		catches_output_pipe = popen( sys_string, "w" );

		table_name =
			get_table_name( application_name, "guide_anglers" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate >> %s",
			 table_name,
			 INSERT_GUIDE_ANGLERS_FIELD_LIST,
			 sql_error_filename );
		guide_anglers_output_pipe = popen( sys_string, "w" );

		table_name =
			get_table_name( application_name, "permits" );

		sprintf( sys_string,
"insert_statement.e t=%s f=%s d='|' | sql.e 2>&1 | grep -vi duplicate >> %s",
			 table_name,
			 INSERT_PERMITS_FIELD_LIST,
			 sql_error_filename );
		permits_insert_pipe = popen( sys_string, "w" );

		sprintf( sys_string,
"update_statement.e t=%s k=%s carrot=y | sql.e 2>&1 | cat >> %s",
			 table_name,
			 PERMITS_PRIMARY_KEY,
			 sql_error_filename );
		permits_update_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
"queue_top_bottom_lines.e %d | html_table.e 'Insert into Fishing Trips' %s,family,genus,species,kept,released '|'",
			 QUEUE_TOP_BOTTOM_LINES,
			 INSERT_FISHING_TRIPS_FIELD_LIST );
		fishing_trips_output_pipe = popen( sys_string, "w" );
	}

	now_date_international =
		date_get_now_date_yyyy_mm_dd(
			HOURS_WEST_GMT );

	while( get_line( input_string, input_file ) )
	{
		line_number++;

		if ( ! piece_double_quoted(
			guide_angler_name,
			',',
			input_string,
			GUIDE_SUBMISSION_ANGLER_NAME_PIECE ) )
		{
			continue;
		}

		if ( !*guide_angler_name ) continue;

		if ( strcmp( guide_angler_name, HEADER_COLUMN_A ) == 0 )
			continue;

		if ( !piece_double_quoted(
					permit_code,
					',',
					input_string,
					GUIDE_SUBMISSION_PERMIT_CODE_PIECE ) )
		{
			continue;
		}

		if ( !piece_double_quoted(
					census_date_mmddyyyy,
					',',
					input_string,
					GUIDE_SUBMISSION_CENSUS_DATE_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece census date in (%s)\n",
				input_string );
			continue;
		}

		*census_date_international = '\0';

		if ( !date_convert_source_american(
				census_date_international,
				international,
				census_date_mmddyyyy )
		||   !*census_date_international )
		{
			fprintf(error_file,
		"Warning in line %d: invalid date format = (%s). Skipping...\n",
				line_number,
				census_date_mmddyyyy );
			continue;
		}

		if ( timlib_strncmp(	census_date_international,
					"invalid" ) == 0 )
		{
			fprintf(error_file,
			"Warning: invalid date format = (%s). Skipping...\n",
				census_date_mmddyyyy );
			continue;
		}

		if ( strcmp(	census_date_international,
				now_date_international ) > 0 )
		{
			fprintf(error_file,
			"Warning: date is in the future = (%s). Skipping...\n",
				census_date_mmddyyyy );
			continue;
		}

		if ( !piece_double_quoted(
				hours_fishing,
				',',
				input_string,
				GUIDE_SUBMISSION_FISHING_DURATION_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece fishing duration in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_double_quoted(
				location_area,
				',',
				input_string,
				GUIDE_SUBMISSION_FISHING_AREA_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece fishing area in (%s)\n",
				input_string );
			continue;
		}

		if ( !piece_double_quoted(
				location_zone,
				',',
				input_string,
				GUIDE_SUBMISSION_FISHING_AREA_ZONE_PIECE ) )
		{
			fprintf(error_file,
			"Warning: Cannot piece fishing zone in (%s)\n",
				input_string );
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
"Warning in line %d: invalid fishing area of (%s) in (%s). Skipping...\n",
				line_number,
				fishing_area,
				input_string );
			continue;
		}

		if ( !piece_double_quoted(
				number_of_people_fishing,
				',',
				input_string,
				GUIDE_SUBMISSION_ONBOARD_FISHING_COUNT_PIECE ) )
		{
			*number_of_people_fishing = '\0';
		}

		if ( !piece_double_quoted(
				common_name_string,
				',',
				input_string,
				GUIDE_SUBMISSION_TARGETED_COMMON_NAME_PIECE ) )
		{
			*common_name_string = '\0';
		}

		if ( *common_name_string )
		{
			if ( !creel_load_library_common_get_species(
					family,
					genus,
					species,
					common_name_string,
					application_name ) )
			{
				fprintf(error_file,
"Ignoring in line %d: Cannot get common name (%s) in (%s). Ignoring species preferred.\n",
					line_number,
					common_name_string,
					input_string );

				*family = '\0';
				*genus = '\0';
				*species = '\0';
			}
		}
		else
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
			"%s|%s|%s|%d|%s|%s|%s|%s|%s|%s|%s|y|%s|%s",
			GUIDE_FISHING_PURPOSE,
			census_date_international,
			GUIDE_CATCHES_INTERVIEW_LOCATION,
			next_reference_number,
			permit_code,
			hours_fishing,
			number_of_people_fishing,
			family,
			genus,
			species,
			fishing_area,
			login_name,
			now_string );

		if ( really_yn == 'y' )
		{
			fprintf( fishing_trips_output_pipe, "\n" );

			fprintf( guide_anglers_output_pipe,
				 "%s\n",
				 guide_angler_name );

			fprintf( permits_insert_pipe,
				 "%s|%s\n",
				 permit_code,
				 guide_angler_name );

			fprintf( permits_update_pipe,
				 "%s^guide_angler_name^%s\n",
				 permit_code,
				 guide_angler_name );
		}

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
			next_reference_number++;
			list_free( catches_list );
			continue;
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
					"%s|%s|%s|%d|%s|%s|%s|%d|%d\n",
					GUIDE_FISHING_PURPOSE,
					census_date_international,
					GUIDE_CATCHES_INTERVIEW_LOCATION,
					next_reference_number,
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
					"||||||||||||||%s|%s|%s|%d|%d\n",
						catches->family,
						catches->genus,
						catches->species,
						catches->kept_integer,
						catches->released_integer );
				}
			}
		} while( list_next( catches_list ) );

		list_free_data( catches_list );
		list_free( catches_list );

		next_reference_number++;
	}

	fclose( input_file );
	fclose( error_file );
	pclose( fishing_trips_output_pipe );

	if ( creel_census_output_pipe )
		pclose( creel_census_output_pipe );

	if ( catches_output_pipe )
		pclose( catches_output_pipe );

	if ( guide_anglers_output_pipe )
		pclose( guide_anglers_output_pipe );

	if ( permits_insert_pipe )
		pclose( permits_insert_pipe );

	if ( permits_update_pipe )
		pclose( permits_update_pipe );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Fishing Trips Errors' '' '|'",
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
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'SQL Errors' '' '|'",
			 	sql_error_filename );
			if ( system( sys_string ) );
		}
		sprintf( sys_string, "rm %s", sql_error_filename );
		if ( system( sys_string ) );
	}

} /* insert_fishing_trips() */

int get_input_record_count( char *input_filename )
{
	FILE *input_file;
	char input_string[ 4096 ];
	char guide_angler_name[ 128 ];
	int input_record_count = 0;

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	while( get_line( input_string, input_file ) )
	{
		if ( !piece_double_quoted(
				guide_angler_name,
				',',
				input_string,
				GUIDE_SUBMISSION_ANGLER_NAME_PIECE ) )
		{
			continue;
		}

		if ( !*guide_angler_name ) continue;

		if ( strcmp( guide_angler_name, HEADER_COLUMN_A ) == 0 )
			continue;

		input_record_count++;
	}
	fclose( input_file );
	return input_record_count;

} /* get_input_record_count() */

int get_starting_next_reference_number(
			char *application_name,
			int input_record_count,
			char really_yn )
{
	char sys_string[ 1024 ];

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
		 	"reference_number.sh %s %d",
		 	application_name,
		 	input_record_count );
	}
	else
	{
		sprintf( sys_string,
		 	"reference_number.sh %s %d noupdate",
		 	application_name,
		 	input_record_count );
	}

	return atoi( pipe2string( sys_string ) );
} /* get_starting_next_reference_number() */

#define DELETE_FISHING_TRIPS	"fishing_purpose,census_date,permit_code"
#define DELETE_CATCHES		"fishing_purpose,census_date,interview_number"

void delete_fishing_trips(	char *application_name,
				char *input_filename )
{
	FILE *input_file;
	FILE *fishing_trips_delete_pipe;
	FILE *catches_delete_pipe;
	char sys_string[ 1024 ];
	char input_string[ 4096 ];
	char guide_angler_name[ 256 ];
	char permit_code[ 256 ];
	char census_date_mmddyyyy[ 256 ];
	char census_date_international[ 256 ];
	char *table_name;
	char interview_number_key[ 128 ];
	DICTIONARY *interview_number_dictionary;
	char *interview_number;

	if ( ! ( interview_number_dictionary =
		get_interview_number_dictionary(
			application_name,
			input_filename ) ) )
	{
		return;
	}

	table_name =
		get_table_name(
			application_name, "fishing_trips" );

	sprintf( sys_string,
"delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FISHING_TRIPS );

	fishing_trips_delete_pipe = popen( sys_string, "w" );

	table_name =
		get_table_name(
			application_name, "catches" );

	sprintf( sys_string,
"delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_CATCHES );

	catches_delete_pipe = popen( sys_string, "w" );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	while( get_line( input_string, input_file ) )
	{
		if ( !piece_double_quoted(
				guide_angler_name,
				',',
				input_string,
				GUIDE_SUBMISSION_ANGLER_NAME_PIECE ) )
		{
			continue;
		}

		if ( !*guide_angler_name ) continue;

		if ( strcmp( guide_angler_name, HEADER_COLUMN_A ) == 0 )
			continue;

		if ( !piece_double_quoted(
				census_date_mmddyyyy,
				',',
				input_string,
				GUIDE_SUBMISSION_CENSUS_DATE_PIECE ) )
		{
			continue;
		}

		*census_date_international = '\0';

		date_convert_source_american(
				census_date_international,
				international,
				census_date_mmddyyyy );

		if ( !*census_date_international )
		{
			continue;
		}

		if ( !piece_double_quoted(
					permit_code,
					',',
					input_string,
					GUIDE_SUBMISSION_PERMIT_CODE_PIECE ) )
		{
			continue;
		}

		fprintf(fishing_trips_delete_pipe,
			"%s|%s|%s\n",
			GUIDE_FISHING_PURPOSE,
		     	census_date_international,
			permit_code );

		sprintf( interview_number_key,
			 "%s%c%s",
			 permit_code,
			 PERMIT_CODE_INTERVIEW_NUMBER_DELIMITER,
			 census_date_international );

		if ( ( interview_number =
				dictionary_fetch( 
					interview_number_dictionary,
					interview_number_key ) ) )
		{
			fprintf(catches_delete_pipe,
				"%s|%s|%s\n",
				GUIDE_FISHING_PURPOSE,
		     		census_date_international,
				interview_number );
		}
	}

	fclose( input_file );
	pclose( fishing_trips_delete_pipe );
	pclose( catches_delete_pipe );
	dictionary_free( interview_number_dictionary );

} /* delete_fishing_trips() */

LIST *get_catches_list(		FILE *error_file,
				char *input_string,
				int line_number,
				char *application_name )
{
	int catch_offset;
	CATCHES *catches;
	char kept_count[ 16 ];
	char released_count[ 16 ];
	char family[ 128 ];
	char genus[ 128 ];
	char species[ 128 ];
	char common_name_string[ 128 ];
	LIST *catches_list = list_new();

	catch_offset = 0;
	while( 1 )
	{
		if ( !piece_double_quoted(
			common_name_string,
			',',
			input_string,
			GUIDE_SUBMISSION_STARTING_COMMON_NAME_PIECE +
			catch_offset )
		||   !*common_name_string )
		{
			break;
		}

		if ( !creel_load_library_common_get_species(
					family,
					genus,
					species,
					common_name_string,
					application_name ) )
		{
			fprintf(error_file,
"Warning in line %d: Cannot get common name (%s) in (%s). Skipping...\n",
				line_number,
				common_name_string,
				input_string );

			catch_offset += CATCHES_INCREMENT;
			continue;
		}

		if ( !piece_double_quoted(
			kept_count,
			',',
			input_string,
			GUIDE_SUBMISSION_STARTING_COMMON_NAME_PIECE +
			catch_offset + 1 ) )
		{
			fprintf(error_file,
		"Warning: Cannot get kept count (%s) in (%s)\n",
				common_name_string,
				input_string );
			break;
		}

		if ( !piece_double_quoted(
			released_count,
			',',
			input_string,
			GUIDE_SUBMISSION_STARTING_COMMON_NAME_PIECE +
			catch_offset + 2 ) )
		{
			fprintf(error_file,
		"Warning: Cannot get released count (%s) in (%s)\n",
				common_name_string,
				input_string );
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
} /* get_catches_list() */

DICTIONARY *get_interview_number_dictionary(
			char *application_name,
			char *input_filename )
{
	LIST *census_date_list = list_new();
	LIST *permit_code_list = list_new();
	char sys_string[ 65536 ];
	FILE *input_file;
	char input_string[ 4096 ];
	char guide_angler_name[ 256 ];
	char permit_code[ 256 ];
	char census_date_mmddyyyy[ 256 ];
	char census_date_international[ 256 ];
	char *table_name;
	char *permit_code_in_clause;
	char *census_date_in_clause;
	char select[ 128 ];

	/* Select */
	/* ------ */
	sprintf(select,
		"concat( permit_code, '%c', census_date ), interview_number",
		PERMIT_CODE_INTERVIEW_NUMBER_DELIMITER );

	/* From */
	/* ---- */
	table_name = get_table_name( application_name, "fishing_trips" );

	/* Where */
	/* ----- */
	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	while( get_line( input_string, input_file ) )
	{
		if ( ! piece_double_quoted(
			guide_angler_name,
			',',
			input_string,
			GUIDE_SUBMISSION_ANGLER_NAME_PIECE ) )
		{
			continue;
		}

		if ( !*guide_angler_name ) continue;

		if ( strcmp( guide_angler_name, HEADER_COLUMN_A ) == 0 )
			continue;

		if ( !piece_double_quoted(
					permit_code,
					',',
					input_string,
					GUIDE_SUBMISSION_PERMIT_CODE_PIECE ) )
		{
			continue;
		}

		if ( !piece_double_quoted(
				census_date_mmddyyyy,
				',',
				input_string,
				GUIDE_SUBMISSION_CENSUS_DATE_PIECE ) )
		{
			continue;
		}

		*census_date_international = '\0';

		date_convert_source_american(
				census_date_international,
				international,
				census_date_mmddyyyy );

		if ( !*census_date_international )
		{
			continue;
		}

		list_append_pointer(	census_date_list,
					strdup( census_date_international ) );

		list_append_pointer(	permit_code_list,
					strdup( permit_code ) );

	}

	fclose( input_file );

	if ( !list_length( census_date_list ) ) return (DICTIONARY *)0;

	census_date_in_clause =
		timlib_with_list_get_in_clause( census_date_list );

	permit_code_in_clause =
		timlib_with_list_get_in_clause( permit_code_list );

	list_free_string_list( census_date_list );
	list_free_string_list( permit_code_list );

	/* System */
	/* ------ */
	sprintf(sys_string,
"echo \"select %s from %s where fishing_purpose = '%s' and census_date in (%s) and permit_code in (%s);\" | sql.e '%c'",
		select,
		table_name,
		GUIDE_FISHING_PURPOSE,
		census_date_in_clause,
		permit_code_in_clause,
		FOLDER_DATA_DELIMITER );

	free( permit_code_in_clause );
	free( census_date_in_clause );
	
	return pipe2dictionary( sys_string, FOLDER_DATA_DELIMITER );
} /* get_interview_number_dictionary() */

