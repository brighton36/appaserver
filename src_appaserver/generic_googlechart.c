/* ----------------------------------------------------------	*/
/* $APPASERVER_HOME/src_appaserver/generic_googlechart.c	*/
/* ----------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ----------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "application.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "process_generic_output.h"
#include "appaserver.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *process_set_name;
	char *begin_date = {0};
	char *end_date = {0};
	DOCUMENT *document;
	char *process_name;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *post_dictionary;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	char *sys_string = {0};
	char *where_clause = {0};
	char *database_string = {0};
	int length_select_list = 0;
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	char *units_label;
	char delimiter = FOLDER_DATA_DELIMITER;
	char *accumulate_yn;
	boolean accumulate;
	int time_piece = -1;
	FILE *input_pipe;
	char input_buffer[ 512 ];
	FILE *output_pipe;
	boolean first_time = 1;

	if ( argc >= 1 )
	{
		application_name = argv[ 1 ];

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
	}

	if ( argc != 4 )
	{
		fprintf( stderr,
			 "Usage: %s application process_set dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	process_set_name = argv[ 2 ];

	post_dictionary =
		dictionary_string2dictionary( argv[ 3 ] );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_STARTING_LABEL );

	appaserver_parameter_file = appaserver_parameter_file_new();

	accumulate_yn =
		dictionary_fetch_index_zero(
			post_dictionary,
			"accumulate_yn" );

	accumulate = (accumulate_yn && *accumulate_yn == 'y' );

	process_generic_output =
		process_generic_output_new(
			application_name,
			(char *)0 /* process_name */,
			process_set_name,
			accumulate );

	process_generic_output->value_folder->datatype =
		process_generic_datatype_new(
			application_name,
			process_generic_output->
				value_folder->
					foreign_folder->
						foreign_attribute_name_list,
			process_generic_output->
				value_folder->
					datatype_folder->
						datatype_folder_name,
			process_generic_output->
				value_folder->
					datatype_folder->
						primary_attribute_name_list,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_aggregation_sum,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_bar_graph,
			process_generic_output->
				value_folder->
					datatype_folder->
						exists_scale_graph_zero,
			process_generic_output->
				value_folder->
					units_folder_name,
			post_dictionary,
			0 /* dictionary_index */ );

	if ( !process_generic_output->value_folder->datatype )
	{
		char buffer[ 1024 ];

		format_initial_capital( buffer, process_set_name );

		document_quick_output_body(
					application_name,
					appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<h1>%s</h1><h3>ERROR: insufficient input.</h3>\n",
			buffer );
		document_close();
		exit( 0 );
	}

	process_name =
		process_generic_output_get_process_name(
			process_set_name,
			post_dictionary );

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_level" ) );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_statistic" ),
			aggregate_level );

	if ( aggregate_statistic == aggregate_statistic_none
	&&   aggregate_level != aggregate_level_none
	&&   aggregate_level != real_time )
	{
		aggregate_statistic =
			process_generic_output_get_database_aggregate_statistic(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point,
				process_generic_output->
					value_folder->
					datatype->
					primary_attribute_data_list,
				process_generic_output->
					value_folder->
					datatype_folder->
					exists_aggregation_sum );
	}

	sys_string = process_generic_output_get_text_file_sys_string(
			&begin_date,
			&end_date,
			&where_clause,
			&units_label,
			(int *)0 /* datatype_entity_piece */,
			(int *)0 /* datatype_piece */,
			(int *)0 /* date_piece */,
			&time_piece,
			(int *)0 /* value_piece */,
			&length_select_list,
			application_name,
			process_generic_output,
			post_dictionary,
			delimiter,
			aggregate_level,
			aggregate_statistic,
			0 /* append_low_high */,
			0 /* not concat_datatype_entity */,
			0 /* not concat_datatype */,
			process_generic_output->accumulate );

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

	input_pipe = popen( sys_string, "r" );

	strcpy( sys_string, "html_paragraph_wrapper.e" );

	output_pipe = popen( sys_string, "w" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( first_time )
		{
			search_replace_character(
				input_buffer,
				' ',
				'_' );
			fprintf( output_pipe, "%s\n", input_buffer );
			first_time = 0;
			continue;
		}

		if ( process_generic_output->
			value_folder->
			time_attribute_name
		&&   *process_generic_output->
			value_folder->
			time_attribute_name
		&&   aggregate_level != aggregate_level_none
		&&   aggregate_level != real_time )
		{
			piece_inverse( input_buffer, '^', time_piece );
		}

		fprintf( output_pipe, "%s\n", input_buffer );
	}

	pclose( input_pipe );
	pclose( output_pipe );

	process_increment_execution_count(
		application_name,
		process_name,
		appaserver_parameter_file_get_dbms() );

	return 0;

} /* main() */

