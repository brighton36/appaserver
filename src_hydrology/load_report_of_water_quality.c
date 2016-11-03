/* ---------------------------------------------------	*/
/* src_hydrology/load_report_of_water_quality.c		*/
/* ---------------------------------------------------	*/
/* Obsolete. See $APPASERVER_HOME/src_waterquality	*/
/* ---------------------------------------------------	*/
/* The results from the lab sometimes comes in report	*/
/* format that needs to be parsed and loaded.		*/
/*							*/
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
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "dictionary.h"
#include "application.h"
#include "session.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define UPDATE_WATER_QUALITY_RESULTS_LIST_STRING 	"station,collection_date,collection_time,laboratory,sample_id,water_parameter"

#define INSERT_WATER_QUALITY_RESULTS_LIST_STRING 	"station,collection_date,collection_time,laboratory,sample_id,water_parameter,concentration"

#define INSERT_WATER_QUALITY_TESTS_LIST_STRING 		"station,collection_date,collection_time,laboratory,sample_id"

#define INSERT_WATER_COLLECTIONS_LIST_STRING 		"station,collection_date,collection_time"

#define INSERT_WATER_QUALITY_STATION_LIST_STRING 	"station"

#define PROCESS_NAME			"load_report_of_water_quality"

/* Prototypes */
/* ---------- */
int fetch_sfwmd_primary_key(
			char **station,
			char **collection_date,
			char **collection_time,
			char **sample_id,
			DICTIONARY *dictionary,
			char *database_management_system );

char *subtract_colon_from_hrmi(
			char *buffer, char *hrmi );

char *insert_dashes_in_yyyymmdd(
			char *buffer,
			char *yyyymmdd );

void load_report_of_water_quality(
			char *application_name,
			char *laboratory,
			DICTIONARY *water_parameter_load_headings_dictionary,
			char *input_filename,
			char really_yn );

DICTIONARY *get_water_parameter_load_headings_dictionary(
			char *application_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *laboratory;
	char really_yn;
	char *input_filename;
	DOCUMENT *document;
	DICTIONARY *water_parameter_load_headings_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *session;
	char *login_name;
	char *role_name;
	char *database_string = {0};

	if ( argc != 8 )
	{
		fprintf( stderr, 
"Usage: %s application session login_name role laboratory filename really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	login_name = argv[ 3 ];
	role_name = argv[ 4 ];
	laboratory = argv[ 5 ];
	input_filename = argv[ 6 ];
	really_yn = *argv[ 7 ];

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

	printf( "<h2>Load Report of Water Quality\n" );
	fflush( stdout );
	system( "date '+%x %H:%M'" );
	printf( "</h2>\n" );
	fflush( stdout );

	water_parameter_load_headings_dictionary =
		get_water_parameter_load_headings_dictionary(
			application_name );

	if ( !water_parameter_load_headings_dictionary
	||   !dictionary_length( water_parameter_load_headings_dictionary ) )
	{
		printf(
		"<h2>Warning: empty water_parameter_load_headings</h2>\n" );
		document_close();
		exit( 0 );
	}

	load_report_of_water_quality(
			application_name,
			laboratory,
			water_parameter_load_headings_dictionary,
			input_filename,
			really_yn );

	if ( really_yn == 'y' )
		printf( "<p>Process complete\n" );
	else
		printf( "<p>Process not executed\n" );

	document_close();

	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

DICTIONARY *get_water_parameter_load_headings_dictionary(
			char *application_name )
{
	char buffer[ 1024 ];
	char load_heading[ 512 ];
	char water_parameter[ 512 ];
	FILE *input_pipe;
	char *select = "water_parameter,load_heading";
	DICTIONARY *water_parameter_load_headings_dictionary;

	water_parameter_load_headings_dictionary =
		dictionary_new_dictionary();

	sprintf( buffer,
		 "get_folder_data	application=%s			    "
		 "			folder=water_parameter_load_headings"
		 "			select=%s			    ",
		 application_name,
		 select );

	input_pipe = popen( buffer, "r" );
	while( get_line( buffer, input_pipe ) )
	{
		piece( water_parameter, FOLDER_DATA_DELIMITER, buffer, 0 );
		piece( load_heading, FOLDER_DATA_DELIMITER, buffer, 1 );

		dictionary_set_string(
			water_parameter_load_headings_dictionary,
			strdup( load_heading ), strdup( water_parameter ) );
	}
	pclose( input_pipe );
			
	return water_parameter_load_headings_dictionary;
} /* get_water_parameter_load_headings_dictionary() */

char *insert_dashes_in_yyyymmdd( char *buffer, char *yyyymmdd )
{
	if ( strlen( yyyymmdd ) != 8 ) return "";

	strncpy( buffer, yyyymmdd, 4 );
	*(buffer + 4) = '-';
	strncpy( buffer + 5, yyyymmdd + 4, 2 );
	*(buffer + 7) = '-';
	strncpy( buffer + 8, yyyymmdd + 6, 2 );
	*(buffer + 10) = '\0';

	return buffer;

} /* insert_dashes_in_yyyymmdd() */

char *subtract_colon_from_hrmi( char *buffer, char *hrmi )
{
	if ( strlen( hrmi ) != 5 ) return "";

	strncpy( buffer, hrmi, 2 );
	strncpy( buffer + 2, hrmi + 3, 2 );
	*(buffer + 4) = '\0';

	return buffer;

} /* subtract_colon_from_hrmi() */

void load_report_of_water_quality(
			char *application_name,
			char *laboratory,
			DICTIONARY *water_parameter_load_headings_dictionary,
			char *input_filename,
			char really_yn )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	FILE *water_quality_results_insert_pipe = {0};
	FILE *water_quality_results_update_pipe = {0};
	DICTIONARY *dictionary;
	LIST *load_heading_list;
	char *load_heading;
	char *station;
	char *collection_date;
	char *collection_time;
	char *sample_id;
	char *water_parameter;
	char *concentration;
	char *water_quality_results_table_name;
	DICTIONARY *water_parameter_count_dictionary;
	char station_water_parameter[ 512 ];
	char *water_quality_tests_table_name;
	char *water_collections_table_name;
	char *water_quality_station_table_name;
	FILE *water_quality_tests_pipe = {0};
	FILE *water_collections_pipe = {0};
	FILE *water_quality_station_pipe = {0};
	char *database_management_system;

	/* Make sure we have read access */
	/* ----------------------------- */
	if ( ! ( input_pipe = fopen( input_filename, "r" ) ) )
	{
		printf( "<h2>ERROR: cannot open %s for read</h2>\n",
			input_filename );
		return;
	}
	else
	{
		fclose( input_pipe );
	}

	sprintf( sys_string,
		 "cat %s | parse_report '^'",
		 input_filename );
	input_pipe = popen( sys_string, "r" );

	water_quality_results_table_name =
		get_table_name(	application_name,
				"water_quality_results" );

	water_quality_tests_table_name =
		get_table_name(	application_name,
				"water_quality_tests" );

	water_collections_table_name =
		get_table_name(	application_name,
				"water_collections" );

	water_quality_station_table_name =
		get_table_name(	application_name,
				"water_quality_station" );

	water_parameter_count_dictionary = dictionary_new_dictionary();

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
"insert_statement.e %s %s '^' | sql 2>&1 | grep -v Duplicate | grep -v \"unique constraint\" | html_paragraph_wrapper.e",
		 	water_quality_results_table_name,
		 	INSERT_WATER_QUALITY_RESULTS_LIST_STRING );

		water_quality_results_insert_pipe = popen( sys_string, "w" );

		sprintf( sys_string,
"update_statement.e %s %s \"^^\" | sql 2>&1 | html_paragraph_wrapper.e",
		 	water_quality_results_table_name,
		 	UPDATE_WATER_QUALITY_RESULTS_LIST_STRING );

		water_quality_results_update_pipe = popen( sys_string, "w" );

		sprintf( sys_string,
"sort -u | insert_statement.e %s %s '^' | sql 2>&1 | grep -v Duplicate | grep -v \"unique constraint\" | html_paragraph_wrapper.e",
		 	water_quality_tests_table_name,
		 	INSERT_WATER_QUALITY_TESTS_LIST_STRING );

		water_quality_tests_pipe = popen( sys_string, "w" );

		sprintf( sys_string,
"sort -u | insert_statement.e %s %s '^' | sql 2>&1 | grep -v Duplicate | grep -v \"unique constraint\" | html_paragraph_wrapper.e",
		 	water_collections_table_name,
		 	INSERT_WATER_COLLECTIONS_LIST_STRING );

		water_collections_pipe = popen( sys_string, "w" );

		sprintf( sys_string,
"sort -u | insert_statement.e %s %s '^' | sql 2>&1 | grep -v Duplicate | grep -v \"unique constraint\" | html_paragraph_wrapper.e",
		 	water_quality_station_table_name,
		 	INSERT_WATER_QUALITY_STATION_LIST_STRING );

		water_quality_station_pipe = popen( sys_string, "w" );
	}

	load_heading_list =
		dictionary_get_key_list(
			water_parameter_load_headings_dictionary );

	database_management_system =
		appaserver_parameter_file_get_dbms();

	while( ( dictionary = dictionary_load_record_dictionary(
				input_pipe,
				'^' ) ) )
	{
		if ( strcmp( laboratory, "sfwmd" ) == 0 )
		{
			if ( !fetch_sfwmd_primary_key(
					&station,
					&collection_date,
					&collection_time,
					&sample_id,
					dictionary,
					database_management_system ) )
			{
				continue;
			}
		}
		else
		{
			continue;
		}

		if ( !list_rewind( load_heading_list ) )
		{
			continue;
		}

		do {
			load_heading =
				list_get_pointer(
					load_heading_list );

			if ( ! ( concentration = dictionary_get_pointer(
				dictionary,
				load_heading ) ) )
			{
				continue;
			}

			if ( strncmp( concentration, "--", 2 ) == 0 )
				continue;

			water_parameter =
				dictionary_get_pointer(
				water_parameter_load_headings_dictionary,
				load_heading );

			if ( really_yn == 'y' )
			{
				fprintf(water_quality_station_pipe,
				 	"%s\n",
				 	station );

				fprintf(water_collections_pipe,
				 	"%s^%s^%s\n",
				 	station,
				 	collection_date,
				 	collection_time );

				fprintf(water_quality_tests_pipe,
				 	"%s^%s^%s^%s^%s\n",
				 	station,
				 	collection_date,
				 	collection_time,
				 	laboratory,
				 	sample_id );

				fprintf(water_quality_results_insert_pipe,
				 	"%s^%s^%s^%s^%s^%s^%s\n",
				 	station,
				 	collection_date,
				 	collection_time,
				 	laboratory,
				 	sample_id,
				 	water_parameter,
				 	concentration );

				fprintf(water_quality_results_update_pipe,
				 	"%s^%s^%s^%s^%s^%s^concentration^%s\n",
				 	station,
				 	collection_date,
				 	collection_time,
				 	laboratory,
				 	sample_id,
				 	water_parameter,
				 	concentration );
			}

			sprintf( station_water_parameter,
				 "%s | %s",
				 station,
				 water_parameter );

			dictionary_increment_count(
				water_parameter_count_dictionary,
				station_water_parameter );

		} while( list_next( load_heading_list ) );

		dictionary_free( dictionary );
		free( station );
		free( collection_date );
		free( collection_time );
		free( sample_id );
	}
	pclose( input_pipe );

	if ( really_yn == 'y' )
	{
		pclose( water_quality_results_insert_pipe );
		pclose( water_quality_results_update_pipe );
		pclose( water_quality_tests_pipe );
		pclose( water_collections_pipe );
		pclose( water_quality_station_pipe );
	}

	dictionary_output_html_table(
		water_parameter_count_dictionary,
		"Station | Water Parameter",
		"Count",
		1 /* align_right */ );

} /* load_report_of_water_quality() */

int fetch_sfwmd_primary_key(
					char **station,
					char **collection_date,
					char **collection_time,
					char **sample_id,
					DICTIONARY *dictionary,
					char *database_management_system )
{
	char *data;
	char buffer[ 1024 ];

	if ( ! ( data = dictionary_get_pointer(
			dictionary, "Station Id" ) ) )
	{
		return 0;
	}

	*station = strdup( data );

	if ( ! ( data = dictionary_get_pointer(
			dictionary, "Date YYYYMMDD" ) ) )
	{
		return 0;
	}

	if ( strcmp( database_management_system, "oracle" ) == 0 )
	{
		insert_dashes_in_yyyymmdd( buffer, data );
		*collection_date =
			strdup( timlib_yyyymmdd_to_ora_date( buffer ) );
	}
	else
	{
		*collection_date =
			strdup( insert_dashes_in_yyyymmdd( buffer, data ) );
	}

	if ( ! ( data = dictionary_get_pointer(
			dictionary, "Time HR:MI" ) ) )
	{
		return 0;
	}

	*collection_time =
		strdup( subtract_colon_from_hrmi( buffer, data ) );

	if ( ! ( data = dictionary_get_pointer(
			dictionary, "Sample Id" ) ) )
	{
		return 0;
	}

	*sample_id = strdup( data );

	return 1;
} /* fetch_sfwmd_primary_key() */

