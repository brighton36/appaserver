/* ---------------------------------------------------	*/
/* src_waterquality/load_sfwmd_file.c			*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

/* Includes */
/* -------- */
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
#include "column.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "process.h"
#include "date_convert.h"
#include "application.h"
#include "load_sfwmd_file.h"
#include "water_quality.h"

/* Constants */
/* --------- */
/* #define BDL_HALF_MDL		1 */

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char really_yn;
	char delete_yn;
	char *input_filename;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	int load_count;
	char buffer[ 128 ];

	if ( argc != 6 )
	{
		fprintf( stderr, 
"Usage: %s application process_name filename delete_yn really_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	input_filename = argv[ 3 ];
	delete_yn = *argv[ 4 ];
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

	appaserver_parameter_file = appaserver_parameter_file_new();

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

	printf( "<h2>%s\n", format_initial_capital( buffer, process_name ) );
	fflush( stdout );
	system( "date '+%x %H:%M'" );
	printf( "</h2>\n" );
	fflush( stdout );

	if ( really_yn == 'y' )
	{
		if ( delete_yn == 'y' )
		{
			delete_waterquality(	application_name,
						input_filename );
		}

		insert_waterquality_parameters(
					application_name,
					input_filename );
	}

	load_count = load_sfwmd_file(
			application_name,
			input_filename,
			really_yn );

	if ( really_yn == 'y' )
		printf( "<p>Process complete with %d concentrations.\n",
			load_count );
	else
		printf( "<p>Process did not load %d concentrations.\n",
			load_count );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

char *subtract_colon_from_hrmi( char *hrmi )
{
	static char buffer[ 128 ];

	int str_len = strlen( hrmi );

	*buffer = '\0';

	if ( str_len == 4 )
	{
		*buffer = '0';
		strncpy( buffer + 1, hrmi, 1 );
		strncpy( buffer + 2, hrmi + 2, 2 );
		*(buffer + 4) = '\0';
		return buffer;
	}
	else
	if ( str_len == 5 )
	{
		strncpy( buffer, hrmi, 2 );
		strncpy( buffer + 2, hrmi + 3, 2 );
		*(buffer + 4) = '\0';
		return buffer;
	}
	else
	{
		return (char *)0;
	}

} /* subtract_colon_from_hrmi() */

int load_sfwmd_file(
			char *application_name,
			char *input_filename,
			char really_yn )
{
	char sys_string[ 1024 ];
	FILE *input_file;
	FILE *error_file;
	char input_string[ 4096 ];
	char *missing_heading;
	char *collection_table_name;
	char *station_parameter_table_name;
	char *station_table_name;
	char *water_project_station_table_name;
	char *results_table_name;
	char *results_exception_table_name;
	char error_filename[ 128 ];
	char project_code[ 128 ];
	char *project_name;
	char sample_id[ 128 ];
	char station[ 128 ];
	char collection_date_time[ 128 ];
	char collection_date[ 128 ];
	char collection_date_international[ 128 ];
	char collection_time[ 128 ];
	char *collection_time_without_colon;
	char collection_depth_meters_string[ 128 ];
	double collection_depth_meters;
	char collection_depth_unit[ 128 ];
	char lab_lims_id[ 128 ];
	char lab_test_code[ 128 ];
	char parameter_string[ 128 ];
	char *aliased_parameter;
	char minimum_detection_limit[ 128 ];
	char concentration[ 128 ];
	char sample_comments[ 4096 ];
	char results_comments[ 4096 ];
	FILE *table_output_pipe = {0};
	FILE *results_insert_pipe = {0};
	FILE *results_exception_insert_pipe = {0};
	FILE *collection_insert_pipe = {0};
	FILE *station_parameter_insert_pipe = {0};
	FILE *water_project_station_insert_pipe = {0};
	FILE *station_insert_pipe = {0};
	int load_count = 0;
	char matrix_code[ 128 ];
	char units[ 128 ];
	char *matrix;
	char flow_no_flow_code[ 128 ];
	char *flow_no_flow;
	char up_down_stream_code[ 128 ];
	char *up_down_stream;
	char exception_code_multiple[ 128 ];
	int line_number = 1;
	DICTIONARY *heading_piece_dictionary = {0};
#ifdef BLD_HALF_MDL
	double concentration_double;
#endif

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		printf( "<h2>ERROR: cannot open %s for read</h2>\n",
			input_filename );
		return 0;
	}

	if ( timlib_get_line( input_string, input_file, 4096 ) )
	{
		if ( ! ( heading_piece_dictionary =
			get_heading_piece_dictionary(
						&missing_heading,
						input_string ) ) )
		{
			printf(
		"<h3>ERROR: cannot get the heading of %s.</h3>\n",
				missing_heading );
			document_close();

			exit( 0 );
		}
	}

	sprintf(error_filename,
		"/tmp/load_sfwmd_file_error_%d.txt", getpid() );
	if ( ! ( error_file = fopen( error_filename, "w" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", error_filename );
		fclose( input_file );
		exit( 1 );
	}

	results_table_name =
		get_table_name(	application_name,
				"results" );

	results_exception_table_name =
		get_table_name(	application_name,
				"results_exception" );

	collection_table_name =
		get_table_name(	application_name,
				"collection" );

	station_parameter_table_name =
		get_table_name(	application_name,
				"station_parameter" );

	station_table_name =
		get_table_name(	application_name,
				"station" );

	water_project_station_table_name =
		get_table_name(	application_name,
				"water_project_station" );

	if ( really_yn == 'y' )
	{
		sprintf( sys_string,
			 "insert_statement.e %s %s '|'			|"
			 "count.e %d 'WQ Load count'			|"
			 "sql.e 2>&1					|"
			 "grep -vi duplicate				|"
			 "html_paragraph_wrapper.e			 ",
		 	results_table_name,
		 	INSERT_RESULTS,
			STDERR_COUNT );

		results_insert_pipe = popen( sys_string, "w" );

		sprintf( sys_string,
			 "insert_statement.e %s %s '|'			|"
			 "sql.e 2>&1					|"
			 "grep -vi duplicate				|"
			 "html_paragraph_wrapper.e			 ",
		 	results_exception_table_name,
		 	INSERT_RESULTS_EXCEPTION );

		results_exception_insert_pipe = popen( sys_string, "w" );

		sprintf( sys_string,
			 "sort -u					|"
			 "insert_statement.e %s %s '|'			|"
			 "sql.e 2>&1					|"
			 "grep -vi duplicate				|"
			 "html_paragraph_wrapper.e			 ",
		 	station_parameter_table_name,
		 	INSERT_STATION_PARAMETER );

		station_parameter_insert_pipe = popen( sys_string, "w" );

		sprintf( sys_string,
			 "sort -u					|"
			 "insert_statement.e %s %s '|'			|"
			 "sql.e 2>&1					|"
			 "grep -vi duplicate				|"
			 "html_paragraph_wrapper.e			 ",
		 	station_table_name,
		 	INSERT_STATION );

		station_insert_pipe = popen( sys_string, "w" );

		sprintf( sys_string,
			 "sort -u					|"
			 "insert_statement.e %s %s '|'			|"
			 "sql.e 2>&1					|"
			 "grep -vi duplicate				|"
			 "html_paragraph_wrapper.e			 ",
		 	water_project_station_table_name,
		 	INSERT_WATER_PROJECT_STATION );

		water_project_station_insert_pipe = popen( sys_string, "w" );

		sprintf( sys_string,
			 "sort -u					|"
			 "insert_statement.e %s %s '|'			|"
			 "sql.e 2>&1					|"
			 "grep -vi duplicate				|"
			 "html_paragraph_wrapper.e			 ",
		 	collection_table_name,
		 	INSERT_COLLECTION );

		collection_insert_pipe = popen( sys_string, "w" );
	}
	else
	{
		sprintf( sys_string,
		"queue_top_bottom_lines.e 50				|"
		 "html_table.e 'Insert into Water Quality Results' %s '|'",
			 INSERT_RESULTS );

		table_output_pipe = popen( sys_string, "w" );
	}

	while( timlib_get_line( input_string, input_file, 4096 ) )
	{
		line_number++;
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !get_heading_piece_string(
					project_code,
					heading_piece_dictionary,
					PROJECT_CODE_HEADING,
					input_string ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot get project code in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( ! ( project_name = water_quality_get_project_name(
				application_name,
				project_code ) ) )
		{
			fprintf(error_file,
"Warning in line %d: Invalid project code of (%s) in (%s). You need to insert a new water project and set the code.\n",
				line_number,
				project_code,
				input_string );

			continue;
		}

		if ( !get_heading_piece_string(
					sample_id,
					heading_piece_dictionary,
					SAMPLE_ID_HEADING,
					input_string ) )
		{
			fprintf(error_file,
"Warning in line %d: Cannot piece sample ID in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !get_heading_piece_string(
					station,
					heading_piece_dictionary,
					STATION_HEADING,
					input_string ) )
		{
			fprintf(error_file,
			"Warning in line %d: Cannot piece station in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !get_heading_piece_string(
					collection_date_time,
					heading_piece_dictionary,
					COLLECTION_DATE_TIME1_HEADING,
					input_string ) )
		{
			if ( !get_heading_piece_string(
						collection_date_time,
						heading_piece_dictionary,
						COLLECTION_DATE_TIME2_HEADING,
						input_string ) )
			{
				fprintf(error_file,
	"Warning in line %d: Cannot piece collection date/time in (%s)\n",
					line_number,
					input_string );
				continue;
			}
		}

		column( collection_date, 0, collection_date_time );

		*collection_date_international = '\0';

		if ( !date_convert_source_unknown(
				collection_date_international,
				international,
				collection_date ) )
		{
			fprintf(error_file,
	"Warning in line %d: Unrecognized date format in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( !column( collection_time, 1, collection_date_time ) )
		{
			fprintf(error_file,
		"Warning in line %d: Cannot get collection time in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		collection_time_without_colon =
			subtract_colon_from_hrmi(
				collection_time );

		if ( !collection_time_without_colon )
		{
			fprintf(error_file,
		"Warning in line %d: Invalid collection time in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		get_heading_piece_string(
					collection_depth_meters_string,
					heading_piece_dictionary,
					COLLECTION_DEPTH_METERS_HEADING,
					input_string );

		get_heading_piece_string(
					collection_depth_unit,
					heading_piece_dictionary,
					COLLECTION_DEPTH_UNIT_HEADING,
					input_string );

		if ( *collection_depth_unit
		&&   strcasecmp( collection_depth_unit, "m" ) != 0
		&&   strcasecmp( collection_depth_unit, "ft" ) != 0 )
		{
			fprintf(error_file,
"Warning in line %d: Unrecognized collection depth unit of (%s) in (%s)\n",
				line_number,
				collection_depth_unit,
				input_string );
			continue;
		}

		if ( strcasecmp( collection_depth_unit, "ft" ) == 0 )
		{
			collection_depth_meters =
				atof( collection_depth_meters_string ) *
					0.3048;
		}
		else
		{
			collection_depth_meters =
				atof( collection_depth_meters_string );
		}

		get_heading_piece_string(
					up_down_stream_code,
					heading_piece_dictionary,
					UP_DOWN_STREAM_CODE_HEADING,
					input_string );

		if ( ! ( up_down_stream =
				get_up_down_stream(
					application_name,
					up_down_stream_code ) ) )
		{
			up_down_stream = "";
		}

		get_heading_piece_string(
					flow_no_flow_code,
					heading_piece_dictionary,
					FLOW_NO_FLOW_CODE_HEADING,
					input_string );

		if ( ! ( flow_no_flow =
				get_flow_no_flow(
					application_name,
					flow_no_flow_code ) ) )
		{
			flow_no_flow = "";
		}

		get_heading_piece_string(
					minimum_detection_limit,
					heading_piece_dictionary,
					MINIMUM_DETECTION_LIMIT_HEADING,
					input_string );

		get_heading_piece_string(
					exception_code_multiple,
					heading_piece_dictionary,
					EXCEPTION_CODE_HEADING,
					input_string );

		if ( !get_heading_piece_string(
					concentration,
					heading_piece_dictionary,
					CONCENTRATION_HEADING,
					input_string ) )
		{
			if ( !*exception_code_multiple )
			{
				fprintf(error_file,
"Warning in line %d: Empty concentration and exception in (%s)\n",
					line_number,
					input_string );
			}
		}

		if ( !*minimum_detection_limit
		&&   *concentration == '-' )
		{
			strcpy( minimum_detection_limit,
				concentration + 1 );
		}

		/* A null value is no concentration and no MDL. */
	        /* -------------------------------------------- */	
		if ( !*concentration )
		{
			*minimum_detection_limit = '\0';
		}

		/* BDL is identified by a negative concentration. */
		/* ---------------------------------------------- */
		if ( *concentration == '-' )
		{
#ifdef BDL_HALF_MDL
			concentration_double =
				atof( minimum_detection_limit ) / 2.0;

			sprintf( concentration, "%.5lf", concentration_double );

#else
			*concentration = '\0';
#endif
		}

		get_heading_piece_string(
					lab_lims_id,
					heading_piece_dictionary,
					LAB_LIMS_ID_HEADING,
					input_string );

		get_heading_piece_string(
					lab_test_code,
					heading_piece_dictionary,
					LAB_TEST_CODE_HEADING,
					input_string );

		if ( !get_heading_piece_string(
					parameter_string,
					heading_piece_dictionary,
					PARAMETER_HEADING,
					input_string ) )
		{
			continue;
		}

		aliased_parameter =
			get_aliased_parameter(
				application_name,
				parameter_string );

		get_heading_piece_string(
					sample_comments,
					heading_piece_dictionary,
					SAMPLE_COMMENTS_HEADING,
					input_string );

		search_replace_special_characters( sample_comments );

		get_heading_piece_string(
					results_comments,
					heading_piece_dictionary,
					RESULTS_COMMENTS_HEADING,
					input_string );

		search_replace_special_characters( results_comments );

		get_heading_piece_string(
					matrix_code,
					heading_piece_dictionary,
					MATRIX_CODE_HEADING,
					input_string );

		if ( ! ( matrix = get_matrix(
					application_name,
					matrix_code ) ) )
		{
			fprintf(error_file,
"Warning in line %d: Cannot get matrix using code (%s). Loading anyway.\n",
				line_number,
				matrix_code );

			matrix = "";
		}

		if ( !units_piece(	units,
					input_string,
					heading_piece_dictionary ) )
		{

			fprintf(error_file,
		"Warning in line %d: Cannot get units in (%s)\n",
				line_number,
				input_string );
			continue;
		}

		if ( really_yn == 'y' )
		{
			fprintf( results_insert_pipe,
				 "%s|%s|%s|%s|%s|%s|%s|%s\n",
				 station,
				 collection_date_international,
				 collection_time_without_colon,
				 aliased_parameter,
				 units,
				 matrix,
				 concentration,
				 minimum_detection_limit );

			if ( *exception_code_multiple )
			{
				insert_results_exception_code_multiple(
				 	results_exception_insert_pipe,
					error_file,
				 	application_name,
				 	station,
				 	collection_date_international,
				 	collection_time_without_colon,
				 	aliased_parameter,
				 	units,
				 	exception_code_multiple,
				 	concentration,
					line_number );
			}

			fprintf( station_parameter_insert_pipe,
				 "%s|%s|%s\n",
				 station,
				 aliased_parameter,
				 units );

			fprintf( station_insert_pipe,
				 "%s\n",
				 station );

			fprintf( water_project_station_insert_pipe,
				 "%s|%s\n",
				 project_name,
				 station );

			fprintf( collection_insert_pipe,
				 "%s|%s|%s|%.2lf|%s|%s|%s|%s|%s|%s\n",
				 station,
				 collection_date_international,
				 collection_time_without_colon,
				 collection_depth_meters,
				 up_down_stream,
				 flow_no_flow,
				 sample_id,
				 lab_lims_id,
				 sample_comments,
				 results_comments );
		}
		else
		{
			fprintf( table_output_pipe,
				 "%s|%s|%s|%s|%s|%s|%s|%s\n",
				 station,
				 collection_date_international,
				 collection_time_without_colon,
				 aliased_parameter,
				 units,
				 matrix,
				 concentration,
				 minimum_detection_limit );

			if ( *exception_code_multiple )
			{
				/* Send bad codes to the error file. */
				/* --------------------------------- */
				insert_results_exception_code_multiple(
				 	(FILE *)0 /* results_exception_pipe */,
					error_file,
				 	application_name,
				 	station,
				 	collection_date_international,
				 	collection_time_without_colon,
				 	aliased_parameter,
				 	units,
				 	exception_code_multiple,
				 	concentration,
					line_number );
			}

		}
		load_count++;
	}

	fclose( input_file );
	fclose( error_file );

	close_pipes(
		results_insert_pipe,
		results_exception_insert_pipe,
		station_parameter_insert_pipe,
		station_insert_pipe,
		water_project_station_insert_pipe,
		collection_insert_pipe,
		table_output_pipe,
		really_yn );

	if ( timlib_file_populated( error_filename ) )
	{
		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 300 | html_table.e 'Water Quality Errors' '' '|'",
			 error_filename );
		system( sys_string );
	}

	sprintf( sys_string, "rm %s", error_filename );
	system( sys_string );

	return load_count;

} /* load_sfwmd_file() */

#define DELETE_FIELD_LIST	"station,collection_date,collection_time"

void delete_waterquality(	char *application_name,
				char *input_filename )
{
	FILE *input_file;
	FILE *collection_delete_pipe;
	FILE *results_delete_pipe;
	FILE *results_exception_delete_pipe;
	char sys_string[ 1024 ];
	char input_string[ 4096 ];
	char *missing_heading;
	char *table_name;
	char station[ 128 ];
	char collection_date_time[ 128 ];
	char collection_date[ 128 ];
	char collection_date_international[ 128 ];
	char collection_time[ 128 ];
	char *collection_time_without_colon;
	DICTIONARY *heading_piece_dictionary = {0};

	/* Collection */
	/* ---------- */
	table_name =
		get_table_name(
			application_name, "collection" );

	sprintf( sys_string,
"sort -u | delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FIELD_LIST );

	collection_delete_pipe = popen( sys_string, "w" );

	/* Results */
	/* ------- */
	table_name =
		get_table_name(
			application_name, "results" );

	sprintf( sys_string,
		 "sort -u						|"
		 "delete_statement.e t=%s f=%s d='|'			|"
		 "count.e %d 'WQ delete collection count'		|"
		 "sql.e 2>&1						 ",
		 table_name,
		 DELETE_FIELD_LIST,
		 STDERR_COUNT );

	results_delete_pipe = popen( sys_string, "w" );

	/* Results_Exception */
	/* ----------------- */
	table_name =
		get_table_name(
			application_name, "results_exception" );

	sprintf( sys_string,
"sort -u | delete_statement.e t=%s f=%s d='|' | sql.e 2>&1",
		 table_name,
		 DELETE_FIELD_LIST );

	results_exception_delete_pipe = popen( sys_string, "w" );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		pclose( collection_delete_pipe );
		pclose( results_delete_pipe );
		pclose( results_exception_delete_pipe );
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	if ( timlib_get_line( input_string, input_file, 4096 ) )
	{
		heading_piece_dictionary =
			get_heading_piece_dictionary(
				&missing_heading,
				input_string );
	}

	if ( !heading_piece_dictionary )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get the heading of %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 missing_heading );

		fclose( input_file );
		return;
	}

	while( get_line( input_string, input_file ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !get_heading_piece_string(
					station,
					heading_piece_dictionary,
					STATION_HEADING,
					input_string ) )
		{
			continue;
		}

		if ( !get_heading_piece_string(
					collection_date_time,
					heading_piece_dictionary,
					COLLECTION_DATE_TIME1_HEADING,
					input_string ) )
		{
			if ( !get_heading_piece_string(
						collection_date_time,
						heading_piece_dictionary,
						COLLECTION_DATE_TIME2_HEADING,
						input_string ) )
			{
				continue;
			}
		}

		column( collection_date, 0, collection_date_time );

		*collection_date_international = '\0';

		if ( !date_convert_source_unknown(
				collection_date_international,
				international,
				collection_date ) )
		{
			continue;
		}

		if ( !column( collection_time, 1, collection_date_time ) )
		{
			continue;
		}

		collection_time_without_colon =
			subtract_colon_from_hrmi(
				collection_time );

		if ( !collection_time_without_colon )
		{
			continue;
		}

		fprintf( collection_delete_pipe,
			 "%s|%s|%s\n",
			 station,
			 collection_date_international,
			 collection_time_without_colon );

		fprintf( results_delete_pipe,
			 "%s|%s|%s\n",
			 station,
			 collection_date_international,
			 collection_time_without_colon );

		fprintf( results_exception_delete_pipe,
			 "%s|%s|%s\n",
			 station,
			 collection_date_international,
			 collection_time_without_colon );
	}

	fclose( input_file );
	pclose( collection_delete_pipe );
	pclose( results_delete_pipe );
	pclose( results_exception_delete_pipe );

} /* delete_waterquality() */

void insert_waterquality_parameters(
				char *application_name,
				char *input_filename )
{
	FILE *input_file;
	FILE *parameter_unit_insert_pipe;
	FILE *parameter_insert_pipe;
	char sys_string[ 1024 ];
	char input_string[ 4096 ];
	char *missing_heading;
	char parameter[ 128 ];
	char lab_test_code[ 128 ];
	char units[ 128 ];
	char lab_storet_code[ 128 ];
	char *parameter_unit_table_name;
	char *parameter_table_name;
	DICTIONARY *heading_piece_dictionary = {0};

	parameter_table_name =
		get_table_name(
			application_name, "parameter" );

	parameter_unit_table_name =
		get_table_name(
			application_name, "parameter_unit" );

	sprintf( sys_string,
"sort -u | insert_statement.e %s %s '|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
	 	parameter_table_name,
	 	INSERT_PARAMETER );

	parameter_insert_pipe = popen( sys_string, "w" );

	sprintf( sys_string,
"sort -u | insert_statement.e %s %s '|' | sql.e 2>&1 | grep -vi duplicate | html_paragraph_wrapper.e",
	 	parameter_unit_table_name,
	 	INSERT_PARAMETER_UNIT );

	parameter_unit_insert_pipe = popen( sys_string, "w" );

	if ( ! ( input_file = fopen( input_filename, "r" ) ) )
	{
		fprintf( stderr, "File open error: %s\n", input_filename );
		exit( 1 );
	}

	if ( get_line( input_string, input_file ) )
	{
		heading_piece_dictionary =
			get_heading_piece_dictionary(
				&missing_heading,
				input_string );
	}

	if ( !heading_piece_dictionary )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get the heading of %s.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 missing_heading );

		fclose( input_file );
		return;
	}

	while( get_line( input_string, input_file ) )
	{
		trim( input_string );
		if ( !*input_string ) continue;

		if ( !get_heading_piece_string(
					parameter,
					heading_piece_dictionary,
					PARAMETER_HEADING,
					input_string ) )
		{
			continue;
		}

		if ( !units_piece(	units,
					input_string,
					heading_piece_dictionary ) )
		{
			continue;
		}

		get_heading_piece_string(
					lab_test_code,
					heading_piece_dictionary,
					LAB_TEST_CODE_HEADING,
					input_string );

		get_heading_piece_string(
					lab_storet_code,
					heading_piece_dictionary,
					LAB_STORET_CODE_HEADING,
					input_string );

		fprintf( parameter_unit_insert_pipe,
			 "%s|%s\n",
			 parameter,
			 units );

		fprintf( parameter_insert_pipe,
			 "%s|%s|%s\n",
			 parameter,
			 lab_test_code,
			 lab_storet_code );
	}

	fclose( input_file );
	pclose( parameter_unit_insert_pipe );
	pclose( parameter_insert_pipe );

} /* insert_waterquality_parameters() */

char *get_matrix(	char *application_name,
			char *matrix_code )
{
	static char matrix[ 256 ];
	static LIST *matrix_list = {0};
	char *record;
	int str_len;

	if ( !*matrix_code ) return (char *)0;

	str_len = strlen( matrix_code );

	if ( !matrix_list )
	{
		char sys_string[ 1024 ];
		char *select = "matrix_code,matrix";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=matrix			",
		application_name,
		select );

		matrix_list = pipe2list( sys_string );
	}

	if ( !list_rewind( matrix_list ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: empty matrix_list.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );
		exit( 1 );
	}

	do {
		record = list_get_pointer( matrix_list );

		if ( strncmp(	matrix_code,
				record,
				str_len ) == 0 )
		{
			piece(	matrix,
				FOLDER_DATA_DELIMITER,
				record,
				1 );
			return matrix;
		}
	} while( list_next( matrix_list ) );

	return (char *)0;
} /* get_matrix() */

char *get_flow_no_flow(	char *application_name,
			char *code )
{
	static char flow_no_flow[ 256 ];
	static LIST *flow_no_flow_list = {0};
	char *record;
	int str_len;

	if ( !*code ) return (char *)0;

	str_len = strlen( code );

	if ( !flow_no_flow_list )
	{
		char sys_string[ 1024 ];
		char *select = "code,flow_no_flow";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=flow_no_flow		",
		application_name,
		select );

		flow_no_flow_list = pipe2list( sys_string );
	}

	if ( !list_rewind( flow_no_flow_list ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: empty flow_no_flow_list.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );
		exit( 1 );
	}

	do {
		record = list_get_pointer( flow_no_flow_list );

		if ( strncmp(	code,
				record,
				str_len ) == 0 )
		{
			piece(	flow_no_flow,
				FOLDER_DATA_DELIMITER,
				record,
				1 );
			return flow_no_flow;
		}
	} while( list_next( flow_no_flow_list ) );

	return (char *)0;
} /* get_flow_no_flow() */

char *get_up_down_stream(	char *application_name,
				char *code )
{
	static char up_down_stream[ 256 ];
	static LIST *up_down_stream_list = {0};
	char *record;
	int str_len;

	if ( !*code ) return (char *)0;

	str_len = strlen( code );

	if ( !up_down_stream_list )
	{
		char sys_string[ 1024 ];
		char *select = "code,up_down_stream";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=up_down_stream		",
		application_name,
		select );

		up_down_stream_list = pipe2list( sys_string );
	}

	if ( !list_rewind( up_down_stream_list ) )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: empty up_down_stream_list.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );
		exit( 1 );
	}

	do {
		record = list_get_pointer( up_down_stream_list );

		if ( strncmp(	code,
				record,
				str_len ) == 0 )
		{
			piece(	up_down_stream,
				FOLDER_DATA_DELIMITER,
				record,
				1 );
			return up_down_stream;
		}
	} while( list_next( up_down_stream_list ) );

	return (char *)0;

} /* get_up_down_stream() */

void close_pipes(
		FILE *results_insert_pipe,
		FILE *results_exception_insert_pipe,
		FILE *station_parameter_insert_pipe,
		FILE *station_insert_pipe,
		FILE *water_project_station_insert_pipe,
		FILE *collection_insert_pipe,
		FILE *table_output_pipe,
		int really_yn )
{
	if ( really_yn == 'y' )
	{
		pclose( results_insert_pipe );
		pclose( results_exception_insert_pipe );
		pclose( station_parameter_insert_pipe );
		pclose( station_insert_pipe );
		pclose( water_project_station_insert_pipe );
		pclose( collection_insert_pipe );
	}
	else
	{
		pclose( table_output_pipe );
	}

} /* close_pipes() */

char *get_aliased_parameter(	char *application_name,
				char *parameter_string )
{
	static DICTIONARY *alias_dictionary = {0};
	char *alias_parameter;

	if ( !alias_dictionary )
	{
		alias_dictionary = get_alias_dictionary( application_name );
	}

	if ( ! ( alias_parameter =
			dictionary_get_pointer(
				alias_dictionary,
				parameter_string ) ) )
	{
		return parameter_string;
	}
	else
	{
		return alias_parameter;
	}

} /* get_aliased_parameter() */

DICTIONARY *get_alias_dictionary( char *application_name )
{
	char sys_string[ 1024 ];
	char *select = "parameter_alias,parameter";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=parameter_alias	",
		 application_name,
		 select );

	return dictionary_pipe2dictionary( sys_string, FOLDER_DATA_DELIMITER );

} /* get_alias_dictionary() */

boolean units_piece(	char *units,
			char *input_string,
			DICTIONARY *heading_piece_dictionary )
{
	if ( !get_heading_piece_string(
					units,
					heading_piece_dictionary,
					UNITS_HEADING,
					input_string ) )
	{
		return 0;
	}

	if ( !*units ) strcpy( units, "null" );
	return 1;

} /* units_piece() */

void insert_results_exception_code_multiple(
				 FILE *results_exception_insert_pipe,
				 FILE *error_file,
				 char *application_name,
				 char *station,
				 char *collection_date_international,
				 char *collection_time_without_colon,
				 char *aliased_parameter,
				 char *units,
				 char *exception_code_multiple,
				 char *concentration,
				 int line_number )
{
	LIST *exception_name_list;
	char *exception_name;

	if ( !*concentration )
	{
		insert_results_exception(
				 results_exception_insert_pipe,
				 station,
				 collection_date_international,
				 collection_time_without_colon,
				 aliased_parameter,
				 units,
				 NULL_CONCENTRATION );
	}
	else
	if ( *concentration == '-' )
	{
		insert_results_exception(
				 results_exception_insert_pipe,
				 station,
				 collection_date_international,
				 collection_time_without_colon,
				 aliased_parameter,
				 units,
				 BELOW_DETECTION_LIMIT );
/*
		*exception_code = BELOW_DETECTION_LIMIT_CODE;
		*(exception_code + 1) = '\0';

		insert_results_exception_code(
				 results_exception_insert_pipe,
				 error_file,
				 application_name,
				 station,
				 collection_date_international,
				 collection_time_without_colon,
				 aliased_parameter,
				 units,
				 exception_code,
				 line_number );
*/
	}

	exception_name_list =
		get_exception_name_list(
			exception_code_multiple,
			error_file,
			line_number,
			application_name );

	if ( !list_rewind( exception_name_list ) ) return;

	do {
		exception_name = list_get_pointer( exception_name_list );

		insert_results_exception(
				 results_exception_insert_pipe,
				 station,
				 collection_date_international,
				 collection_time_without_colon,
				 aliased_parameter,
				 units,
				 exception_name );

	} while( list_next( exception_name_list ) );

	list_free_string_list( exception_name_list );

} /* insert_results_exception_code_multiple() */

void insert_results_exception(
				 FILE *results_exception_insert_pipe,
				 char *station,
				 char *collection_date_international,
				 char *collection_time_without_colon,
				 char *aliased_parameter,
				 char *units,
				 char *exception )
{
	if ( results_exception_insert_pipe )
	{
		fprintf( results_exception_insert_pipe,
		 	"%s|%s|%s|%s|%s|%s\n",
		 	station,
		 	collection_date_international,
		 	collection_time_without_colon,
		 	aliased_parameter,
		 	units,
		 	exception );
	}

} /* insert_results_exception() */

DICTIONARY *get_heading_piece_dictionary(	char **missing_heading,
						char *heading_line )
{
	DICTIONARY *heading_piece_dictionary;
	int piece_offset;
	char piece_offset_string[ 8 ];
	char heading[ 128 ];

	heading_piece_dictionary = dictionary_small_new();

	for( piece_offset = 0 ;; piece_offset++ )
	{
		if ( !piece_quoted(	heading,
					',',
					heading_line,
					piece_offset,
					'"' ) )
		{
			break;
		}

		sprintf( piece_offset_string, "%d", piece_offset );

		if ( timlib_strcmp( heading, PROJECT_CODE_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				PROJECT_CODE_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp( heading, STATION_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				STATION_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp( heading, SAMPLE_ID_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				SAMPLE_ID_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					COLLECTION_DATE_TIME1_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				COLLECTION_DATE_TIME1_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					COLLECTION_DATE_TIME2_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				COLLECTION_DATE_TIME2_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					COLLECTION_DEPTH_METERS_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				COLLECTION_DEPTH_METERS_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					COLLECTION_DEPTH_UNIT_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				COLLECTION_DEPTH_UNIT_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					MATRIX_CODE_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				MATRIX_CODE_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					LAB_TEST_CODE_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				LAB_TEST_CODE_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					PARAMETER_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				PARAMETER_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					LAB_STORET_CODE_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				LAB_STORET_CODE_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					EXCEPTION_CODE_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				EXCEPTION_CODE_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					CONCENTRATION_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				CONCENTRATION_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					MINIMUM_DETECTION_LIMIT_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				MINIMUM_DETECTION_LIMIT_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					UNITS_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				UNITS_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					LAB_LIMS_ID_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				LAB_LIMS_ID_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					FLOW_NO_FLOW_CODE_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				FLOW_NO_FLOW_CODE_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					UP_DOWN_STREAM_CODE_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				UP_DOWN_STREAM_CODE_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					SAMPLE_COMMENTS_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				SAMPLE_COMMENTS_HEADING,
				strdup( piece_offset_string ) );
		}
		else
		if ( timlib_strcmp(	heading,
					RESULTS_COMMENTS_HEADING ) == 0 )
		{
			dictionary_set_pointer(
				heading_piece_dictionary,
				RESULTS_COMMENTS_HEADING,
				strdup( piece_offset_string ) );
		}
	}

	if ( !dictionary_key_exists(	heading_piece_dictionary,
					STATION_HEADING ) )
	{
		*missing_heading = STATION_HEADING;
		return (DICTIONARY *)0;
	}
	else
	if ( !dictionary_key_exists(	heading_piece_dictionary,
					COLLECTION_DATE_TIME1_HEADING ) )
	{
		if ( !dictionary_key_exists(
				heading_piece_dictionary,
				COLLECTION_DATE_TIME2_HEADING ) )
		{
			*missing_heading =
				COLLECTION_DATE_TIME1_HEADING;
			return (DICTIONARY *)0;
		}
	}
	else
	if ( !dictionary_key_exists(	heading_piece_dictionary,
					PARAMETER_HEADING ) )
	{
		*missing_heading = PARAMETER_HEADING;
		return (DICTIONARY *)0;
	}
	else
	if ( !dictionary_key_exists(	heading_piece_dictionary,
					UNITS_HEADING ) )
	{
		*missing_heading = UNITS_HEADING;
		return (DICTIONARY *)0;
	}
	else
	if ( !dictionary_key_exists(	heading_piece_dictionary,
					CONCENTRATION_HEADING ) )
	{
		*missing_heading = CONCENTRATION_HEADING;
		return (DICTIONARY *)0;
	}

	return heading_piece_dictionary;

} /* get_heading_piece_dictionary() */

boolean get_heading_piece_string(
				char *destination,
				DICTIONARY *heading_piece_dictionary,
				char *heading_string,
				char *input_buffer )
{
	int piece_offset;
	char *piece_offset_string;

	*destination = '\0';

	if ( !( piece_offset_string =
		dictionary_get_pointer(
			heading_piece_dictionary,
			heading_string ) ) )
	{
		return 0;
	}

	piece_offset = atoi( piece_offset_string );

	if ( !piece_quoted(	destination,
				',',
				input_buffer,
				piece_offset,
				'"' ) )
	{
		return 0;
	}

	if ( strcmp( heading_string, PARAMETER_HEADING ) == 0
	&&    strcmp( destination, "NO BOTTLE SAMPLE" ) == 0 )
	{
		*destination = '\0';
	}

	if ( !*destination )
		return 0;
	else
		return 1;

} /* get_heading_piece_string() */

LIST *get_exception_name_list(
			char *exception_code_multiple,
			FILE *error_file,
			int line_number,
			char *application_name )
{
	LIST *return_list = {0};
	static LIST *exception_record_list = {0};
	char *exception_record;
	char exception_code[ 16 ];
	char exception_name[ 64 ];
	int position;
	int all_done = 0;

	if ( !*exception_code_multiple ) return (LIST *)0;

	if ( !exception_record_list )
	{
		char sys_string[ 1024 ];
		char *select = "exception_code,exception";
		char *order = "length( exception_code ) desc";

		sprintf(
		sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=exception		"
		"			order=\"%s\"			",
		application_name,
		select,
		order );

		exception_record_list = pipe2list( sys_string );
	}

	if ( !list_length( exception_record_list ) ) return (LIST *)0;

	while( !all_done )
	{
		all_done = 1;
		list_rewind( exception_record_list );

		do {
			exception_record =
				list_get_pointer(
					exception_record_list );

			piece(	exception_code,
				FOLDER_DATA_DELIMITER,
				exception_record,
				0 );

			if ( !*exception_code ) continue;

			position = instr(	exception_code,
						exception_code_multiple,
						1 );

			if ( position > -1 )
			{
				piece(	exception_name,
					FOLDER_DATA_DELIMITER,
					exception_record,
					1 );

				if ( !return_list ) return_list = list_new();

				list_append_unique_string(
					return_list,
					strdup( exception_name ) );

				delete_str(	exception_code_multiple,
						position,
						strlen( exception_code ) );

				all_done = 0;
			}

		} while( list_next( exception_record_list ) );
	}

	search_replace_character(
			exception_code_multiple,
			',',
			' ' );

	trim( exception_code_multiple );

	if ( strlen( exception_code_multiple ) )
	{
		fprintf(error_file,
	"Warning in line %d: Cannot find exception code: %s. Loading anyway.\n",
				line_number,
				exception_code_multiple );
	}

	return return_list;

} /* get_exception_name_list() */

