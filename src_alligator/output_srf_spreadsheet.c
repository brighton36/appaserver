/* ---------------------------------------------------	*/
/* src_alligator/output_srf_spreadsheet.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "date.h"
#include "date_convert.h"
#include "piece.h"
#include "query.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "hashtbl.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "alligator.h"
#include "appaserver_link_file.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
/*
#define OUTPUT_CODES		"%s/%s/srf_spreadsheet_with_codes_%d.csv"
#define FTP_PREPEND_CODES	"%s://%s/%s/srf_spreadsheet_with_codes_%d.csv"
#define FTP_NONPREPEND_CODES	"/%s/srf_spreadsheet_with_codes_%d.csv"
#define OUTPUT_NO_CODES		"%s/%s/srf_spreadsheet_no_codes_%d.csv"
#define FTP_PREPEND_NO_CODES	"%s://%s/%s/srf_spreadsheet_no_codes_%d.csv"
#define FTP_NONPREPEND_NO_CODES	"/%s/srf_spreadsheet_no_codes_%d.csv"
*/

#define FILENAME_STEM_CODES	"srf_spreadsheet_with_codes"
#define FILENAME_STEM_NO_CODES	"srf_spreadsheet_no_codes"

/* Prototypes */
/* ---------- */
void get_title_and_sub_title(	char *title,
				char *sub_title,
				char *process_name );

void output_srf_spreadsheet(
				char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *document_root_directory,
				int process_id,
				char *process_name );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *discovery_date_list_string;
	char *primary_researcher_list_string;

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name session discovery_date primary_researcher\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	/* session = argv[ 3 ]; */
	discovery_date_list_string = argv[ 4 ];
	primary_researcher_list_string = argv[ 5 ];

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
				appaserver_parameter_file->
					appaserver_mount_point,
				document->javascript_module_list,
				document->stylesheet_filename,
				application_get_relative_source_directory(
					application_name ),
				0 /* not with_dynarch_menu */ );
	
	document_output_body(
				document->application_name,
				document->onload_control_string );

	output_srf_spreadsheet(	application_name,
				discovery_date_list_string,
				primary_researcher_list_string,
				appaserver_parameter_file->
					document_root,
				getpid(),
				process_name );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */


void output_srf_spreadsheet(	char *application_name,
				char *discovery_date_list_string,
				char *primary_researcher_list_string,
				char *document_root_directory,
				int process_id,
				char *process_name )
{
	char *ftp_codes_filename;
	char *output_codes_filename;
	FILE *output_codes_file;
	FILE *output_no_codes_file;
	char *ftp_no_codes_filename;
	char *output_no_codes_filename;
	char title[ 128 ];
	char sub_title[ 128 ];
	char *secondary_researcher_1;
	char *secondary_researcher_2;
	ALLIGATOR *alligator;
	NEST *nest;
	APPASERVER_LINK_FILE *appaserver_link_file;

	get_title_and_sub_title(
			title,
			sub_title,
			process_name );

	printf( "<h1>%s<br>%s</h1>\n", title, sub_title );
	printf( "<h2>\n" );
	fflush( stdout );
	system( "date '+%x %H:%M'" );
	fflush( stdout );
	printf( "</h2>\n" );

	if ( !* discovery_date_list_string
	||   strcmp( discovery_date_list_string, "discovery_date" ) == 0 )
	{
		printf(
		"<h3>Please select one or more alligator censuses.</h3>\n" );
		document_close();
		exit( 0 );
	}

	alligator = alligator_new(
				application_name,
				discovery_date_list_string,
				primary_researcher_list_string,
				1 /*  with_secondary_researchers */,
				1 /* with_nest_list */,
				0 /* not with_observation_list */ );

	if ( !list_rewind( alligator->nest_list ) )
	{
		printf(
		"<h3>Warning: no alligator nests to select.</h3>\n" );
		document_close();
		exit( 0 );
	}

/*
	sprintf(output_codes_filename, 
	 	OUTPUT_CODES,
	 	appaserver_mount_point,
	 	application_name,
	 	process_id );

	sprintf(output_no_codes_filename, 
		OUTPUT_NO_CODES,
		appaserver_mount_point,
		application_name, 
		process_id );
*/

	appaserver_link_file =
		appaserver_link_file_new(
			application_get_http_prefix( application_name ),
			appaserver_library_get_server_address(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ),
	 		document_root_directory,
			(char *)0 /* filename_stem */,
			application_name,
			process_id,
			(char *)0 /* session */,
			"csv" );

	appaserver_link_file->filename_stem = FILENAME_STEM_CODES;

	output_codes_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	ftp_codes_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	appaserver_link_file->filename_stem = FILENAME_STEM_NO_CODES;

	output_no_codes_filename =
		appaserver_link_get_output_filename(
			appaserver_link_file->
				output_file->
				document_root_directory,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	ftp_no_codes_filename =
		appaserver_link_get_link_prompt(
			appaserver_link_file->
				link_prompt->
				prepend_http_boolean,
			appaserver_link_file->
				link_prompt->
				http_prefix,
			appaserver_link_file->
				link_prompt->server_address,
			appaserver_link_file->application_name,
			appaserver_link_file->filename_stem,
			appaserver_link_file->begin_date_string,
			appaserver_link_file->end_date_string,
			appaserver_link_file->process_id,
			appaserver_link_file->session,
			appaserver_link_file->extension );

	if ( ! ( output_codes_file = fopen( output_codes_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_codes_filename );
		document_close();
		exit( 1 );
	}

	if ( ! ( output_no_codes_file =
			fopen( output_no_codes_filename, "w" ) ) )
	{
		printf( "<H2>ERROR: Cannot open output file %s\n",
			output_no_codes_filename );
		document_close();
		exit( 1 );
	}

	fprintf( output_codes_file,
"waypoint,Nest Number,Year,Date,Transect #,In Transect Boundry,Basin,Nest Habitat Code,Near Pond,Nest Location UTM_E (m) NAD83, UTM_N (m) NAD83,Observer 1,Observer 2,Observer 3,Pilot\n" );

	fprintf( output_no_codes_file,
"Nest Number,Year,Date,Transect #,Within Transect,Basin,Habitat,Near Pond,UTM_E (m) NAD83, UTM_N (m) NAD83,Observer 1,Observer 2,Observer 3,Pilot\n" );


	do {
		nest = list_get_pointer( alligator->nest_list );

		alligator_get_secondary_researchers(
				&secondary_researcher_1,
				&secondary_researcher_2,
				nest->alligator_census->
					secondary_researcher_list );

		fprintf( output_no_codes_file,
			 "%s,%s,%s,%s,%c,\"%s\",\"%s\",%c,%s,%s,%s,%s,%s,%s\n",
			 nest->nest_number_key,
			 nest->alligator_census->discovery_year,
			 nest->alligator_census->discovery_date,
			 nest->transect_number,
			 nest->in_transect_boundary_yn,
			 nest->basin,
			 nest->habitat,
			 nest->near_pond_yn,
			 nest->utm_easting,
			 nest->utm_northing,
			 nest->alligator_census->primary_researcher,
			 secondary_researcher_1,
			 secondary_researcher_2,
			 nest->alligator_census->pilot );

		fprintf( output_codes_file,
			 ",%s,%s,%s,%s,%c,%s,%s,%c,%s,%s,%s,%s,%s,%s\n",
			 nest->nest_number_key,
			 nest->alligator_census->discovery_year,
			 nest->alligator_census->discovery_date_american,
			 nest->transect_number,
			 nest->in_transect_boundary_yn,
			 nest->basin_code,
			 nest->habitat_code,
			 nest->near_pond_yn,
			 nest->utm_easting,
			 nest->utm_northing,
			 nest->alligator_census->primary_researcher,
			 secondary_researcher_1,
			 secondary_researcher_2,
			 nest->alligator_census->pilot );

	} while( list_next( alligator->nest_list ) );

	fclose( output_no_codes_file );
	fclose( output_codes_file );

/*
	if ( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' )
	{
		sprintf(ftp_codes_filename, 
			FTP_PREPEND_CODES, 
			application_get_http_prefix( application_name ),
		 	appaserver_library_get_server_address(),
		 	application_name,
		 	process_id );
		sprintf(ftp_no_codes_filename, 
			FTP_PREPEND_NO_CODES,
			application_get_http_prefix( application_name ),
		 	appaserver_library_get_server_address(),
		 	application_name,
		 	process_id );
	}
	else
	{
		sprintf(ftp_codes_filename,
	 		FTP_NONPREPEND_CODES, 
	 		application_name,
	 		process_id );
		sprintf(ftp_no_codes_filename,
	 		FTP_NONPREPEND_NO_CODES, 
	 		application_name,
	 		process_id );
	}
*/

	appaserver_library_output_ftp_prompt(
			ftp_codes_filename,
"With codes: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	appaserver_library_output_ftp_prompt(
			ftp_no_codes_filename,
"Without codes: &lt;Left Click&gt; to view or &lt;Right Click&gt; to save.",
			(char *)0 /* target */,
			(char *)0 /* application_type */ );

	printf( "<br>\n" );

} /* output_srf_spreadsheet() */


void get_title_and_sub_title(
			char *title,
			char *sub_title,
			char *process_name )
{
	format_initial_capital( title, process_name );
	*sub_title = '\0';

} /* get_title_and_sub_title() */

