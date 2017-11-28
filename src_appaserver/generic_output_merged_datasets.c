/* ---------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_appaserver/generic_output_merged_datasets.c    	*/
/* ---------------------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "hashtbl.h"
#include "dictionary.h"
#include "timlib.h"
#include "piece.h"
#include "query.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "grace.h"
#include "application.h"
#include "environ.h"
#include "list_usage.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "easycharts.h"
#include "document.h"
#include "process_generic_output.h"
#include "application_constants.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define KEY_DELIMITER			'/'
#define ROWS_BETWEEN_HEADING		20

/*
#define OUTPUT_FILE_SPREADSHEET		"%s/%s/merged_%s_%s_%s_%d.csv"
#define FTP_FILE_SPREADSHEET		"/%s/merged_%s_%s_%s_%d.csv"
#define FTP_WITH_HTTP_FILE_SPREADSHEET	"%s://%s/%s/merged_%s_%s_%s_%d.csv"

#define OUTPUT_FILE_TEXT_FILE		"%s/%s/merged_%s_%s_%s_%d.txt"
#define FTP_FILE_TEXT_FILE		"/%s/merged_%s_%s_%s_%d.txt"
#define FTP_WITH_HTTP_FILE_TEXT_FILE	"%s://%s/%s/merged_%s_%s_%s_%d.txt"
*/

/* Structures */
/* ---------- */

/* Prototypes */
/* ---------- */
LIST *merged_datasets_easycharts_get_output_chart_list(
			LIST *compare_datatype_list,
			PROCESS_GENERIC_DATATYPE_FOLDER *datatype_folder,
			boolean accumulate );

char *get_title(			char *medium,
					enum aggregate_level,
					char *value_folder_name,
					boolean accumulate );

boolean merged_datasets_output_gracechart(
			LIST *compare_datatype_list,
			PROCESS_GENERIC_DATATYPE_FOLDER *datatype_folder,
			enum aggregate_level,
			char *value_folder_name,
			char *where_clause,
			char *application_name,
			char *role_name,
			char *begin_date_string,
			char *end_date_string,
			char *argv_0,
			char *document_root_directory,
			boolean accumulate );

boolean merged_datasets_output_easychart(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			LIST *compare_datatype_list,
			PROCESS_GENERIC_DATATYPE_FOLDER *datatype_folder,
			enum aggregate_level aggregate_level,
			char *document_root_directory,
			char *process_name,
			char *value_folder_name,
			char *where_clause,
			boolean accumulate );

boolean any_missing_measurements(	LIST *datatype_entity_datatype_list,
					char *date_comma_time_string );

boolean get_has_bar_graph(		LIST *compare_datatype_list );

HASH_TABLE *get_merged_hash_table(	LIST *compare_datatype_list );

boolean merged_datasets_output_table(
					LIST *compare_datatype_list,
					enum aggregate_level,
					char *value_folder_name,
					char *where_clause,
					char *application_name,
					boolean accumulate );

boolean merged_datasets_output_transmit(
					FILE *output_pipe,
					LIST *compare_datatype_list,
					boolean accumulate );

int main( int argc, char **argv )
{
	char *application_name;
	char *role_name;
	char *begin_date_string = {0};
	char *end_date_string = {0};
	char *merged_output = {0};
	char *results_string;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	DICTIONARY *post_dictionary;
	char *process_name;
	PROCESS_GENERIC_OUTPUT *process_generic_output;
	char *database_string = {0};
	enum aggregate_level aggregate_level;
	char *prompt_prefix;
	char *where_clause;
	boolean accumulate_flag;

	if ( argc != 6 )
	{
		fprintf( stderr,
	"Usage: %s application role process filler dictionary\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	role_name = argv[ 2 ];
	process_name = argv[ 3 ];

	post_dictionary =
		dictionary_string2dictionary( argv[ 5 ] );

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

/*
	add_dot_to_path();
	add_utility_to_path();
	add_src_appaserver_to_path();
	add_relative_source_directory_to_path( application_name );
*/

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_FROM_STARTING_LABEL );

	dictionary_add_elements_by_removing_prefix(
				    	post_dictionary,
				    	QUERY_STARTING_LABEL );

	appaserver_parameter_file = new_appaserver_parameter_file();

	results_string = dictionary_get_index_zero(
				post_dictionary,
				"accumulate_yn" );
	accumulate_flag = (results_string && *results_string == 'y' );

	process_generic_output =
		process_generic_output_new(
			application_name,
			process_name,
			(char *)0 /* process_set_name */,
			accumulate_flag );

	if ( !process_generic_output )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot process_generic_output_new()\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	dictionary_get_index_data( 	&merged_output, 
					post_dictionary, 
					"merged_output", 
					0 );

	if ( !*merged_output
	||   strcmp( merged_output, "merged_output" ) == 0 )
	{
		merged_output = "text_file";
	}

	aggregate_level =
		aggregate_level_get_aggregate_level(
			dictionary_get_index_zero(
				post_dictionary,
				"aggregate_level" ) );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date_string,
					&end_date_string,
					application_name,
					appaserver_parameter_file->
						database_management_system,
					process_generic_output,
					post_dictionary ) )
	{
		document_quick_output_body(
				application_name,
				appaserver_parameter_file->
					appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( merged_output, "stdout" ) != 0 )
	{
		DOCUMENT *document = document_new( "", application_name );
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
	}

	prompt_prefix =
		list_display_delimited(
			process_generic_output->
				value_folder->
				foreign_folder->
				foreign_attribute_name_list,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER );

	if ( ! ( process_generic_output->
			value_folder->
			compare_datatype_list =
				process_generic_get_compare_datatype_list(
					&where_clause,
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
					prompt_prefix,
					1 /* with_data_select */,
					aggregate_level,
					begin_date_string,
					end_date_string,
					process_generic_output->
						value_folder->
						value_folder_name,
					process_generic_output->
						value_folder->
						date_attribute_name,
					process_generic_output->
						value_folder->
						time_attribute_name,
					process_generic_output->
						value_folder->
						value_attribute_name,
					process_generic_output->accumulate ) ) )
	{
		printf( "<h2>ERROR: insufficient input.</h2>\n" );
		document_close();
		exit( 0 );
	}

	if ( !where_clause )
	{
		where_clause = "";
	}

	if ( strcmp( merged_output, "easychart" ) == 0 )
	{
		if ( !merged_datasets_output_easychart(
			application_name,
			begin_date_string,
			end_date_string,
			process_generic_output->
				value_folder->
				compare_datatype_list,
			process_generic_output->
				value_folder->
				datatype_folder,
			aggregate_level,
			appaserver_parameter_file->
				document_root,
			process_name,
			process_generic_output->
				value_folder->
					value_folder_name,
			where_clause,
			process_generic_output->accumulate ) )
		{
			printf(
			"<h3>Warning: nothing selected to output.</h3>\n" );
			document_close();
			exit( 0 ); 
		}
	}
	else
	if ( strcmp( merged_output, "gracechart" ) == 0 )
	{
		if ( !merged_datasets_output_gracechart(
					process_generic_output->
						value_folder->
						compare_datatype_list,
					process_generic_output->
						value_folder->
						datatype_folder,
					aggregate_level,
					process_generic_output->
						value_folder->
							value_folder_name,
					where_clause,
					application_name,
					role_name,
					begin_date_string,
					end_date_string,
					argv[ 0 ],
					appaserver_parameter_file->
						document_root,
					process_generic_output->accumulate ) )
		{
			printf(
			"<h3>Warning: nothing selected to output.</h3>\n" );
			document_close();
			exit( 0 ); 
		}
	}
	else
	if ( strcmp( merged_output, "table" ) == 0 )
	{
		if ( !merged_datasets_output_table(
					process_generic_output->
						value_folder->
						compare_datatype_list,
					aggregate_level,
					process_generic_output->
						value_folder->
							value_folder_name,
					where_clause,
					application_name,
					process_generic_output->accumulate ) )
		{
			printf(
			"<h3>Warning: nothing selected to output.</h3>\n" );
			document_close();
			exit( 0 ); 
		}
	}
	else
	if ( strcmp( merged_output, "spreadsheet" ) == 0 )
	{
		char *ftp_filename;
		char *output_filename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		FILE *output_file;
		char sys_string[ 1024 ];
		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_get_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_get_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->document_root,
	 			process_generic_output->
					value_folder->
					value_folder_name /* filename_stem */,
				application_name,
				process_id,
				(char *)0 /* session */,
				"csv" /* extension */ );

		appaserver_link_file->application_name = application_name;
		appaserver_link_file->begin_date_string = begin_date_string;
		appaserver_link_file->end_date_string = end_date_string;

		output_filename =
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

		ftp_filename =
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

/*
		sprintf( output_filename, 
			 OUTPUT_FILE_SPREADSHEET,
			 appaserver_parameter_file->appaserver_mount_point,
			 application_name, 
		 	 process_generic_output->
				value_folder->
				value_folder_name,
			 begin_date_string,
			 end_date_string,
			 process_id );
*/
	
		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_file );
		}

		sprintf( sys_string,
			 "tr '|' ',' > %s",
			 output_filename );

		output_pipe = popen( sys_string, "w" );

/*
		if ( application_get_prepend_http_protocol_yn(
					application_name ) == 'y' )
		{
			sprintf(ftp_filename, 
			 	FTP_WITH_HTTP_FILE_SPREADSHEET,
				application_get_http_prefix( application_name ),
			 	appaserver_library_get_server_address(),
			 	application_name,
		 	 	process_generic_output->
					value_folder->
					value_folder_name,
			 	begin_date_string,
			 	end_date_string,
			 	process_id );
		}
		else
		{
			sprintf(ftp_filename, 
			 	FTP_FILE_SPREADSHEET,
			 	application_name,
		 	 	process_generic_output->
					value_folder->
					value_folder_name,
			 	begin_date_string,
			 	end_date_string,
			 	process_id );
		}
*/
	
		if ( !merged_datasets_output_transmit(
				output_pipe,
		  		process_generic_output->
					value_folder->
					compare_datatype_list,
				process_generic_output->accumulate ) )
		{
			printf(
			"<h3>Warning: nothing selected to output.</h3>\n" );
			pclose( output_pipe );
			document_close();
			exit( 0 ); 
		}

		pclose( output_pipe );

		printf( "<h1>Merged Datasets Transmission<br></h1>\n" );
		printf( "<h1>\n" );
		fflush( stdout );
		system( "date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h1>\n" );

		printf( "<br>Search criteria: %s\n",
			query_get_display_where_clause(
				where_clause,
				application_name,
				process_generic_output->
					value_folder->value_folder_name,
				application_get_is_primary_application(
					application_name ) ) );

		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( merged_output, "transmit" ) == 0
	||   strcmp( merged_output, "text_file" ) == 0 )
	{
		char *ftp_filename;
		char *output_filename;
		pid_t process_id = getpid();
		FILE *output_pipe;
		FILE *output_file;
		char sys_string[ 1024 ];

		APPASERVER_LINK_FILE *appaserver_link_file;

		appaserver_link_file =
			appaserver_link_file_new(
				application_get_http_prefix( application_name ),
				appaserver_library_get_server_address(),
				( application_get_prepend_http_protocol_yn(
					application_name ) == 'y' ),
	 			appaserver_parameter_file->document_root,
	 			process_generic_output->
					value_folder->
					value_folder_name /* filename_stem */,
				application_name,
				process_id,
				(char *)0 /* session */,
				"txt" /* extension */ );

		appaserver_link_file->application_name = application_name;
		appaserver_link_file->begin_date_string = begin_date_string;
		appaserver_link_file->end_date_string = end_date_string;

		output_filename =
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

		ftp_filename =
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

/*
		sprintf( output_filename, 
			 OUTPUT_FILE_TEXT_FILE,
			 appaserver_parameter_file->appaserver_mount_point,
			 application_name, 
		 	 process_generic_output->
				value_folder->
				value_folder_name,
			 begin_date_string,
			 end_date_string,
			 process_id );
*/
	
		if ( ! ( output_file = fopen( output_filename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_file );
		}

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e '|' > %s",
			 output_filename );
*/
		sprintf( sys_string,
			 "tr '|' '%c' > %s",
			 OUTPUT_TEXT_FILE_DELIMITER,
			 output_filename );

		output_pipe = popen( sys_string, "w" );

/*
		if ( application_get_prepend_http_protocol_yn(
					application_name ) == 'y' )
		{
			sprintf(ftp_filename, 
			 	FTP_WITH_HTTP_FILE_TEXT_FILE,
				application_get_http_prefix( application_name ),
			 	appaserver_library_get_server_address(),
			 	application_name,
		 	 	process_generic_output->
					value_folder->
					value_folder_name,
			 	begin_date_string,
			 	end_date_string,
			 	process_id );
		}
		else
		{
			sprintf(ftp_filename, 
			 	FTP_FILE_TEXT_FILE,
			 	application_name,
		 	 	process_generic_output->
					value_folder->
					value_folder_name,
			 	begin_date_string,
			 	end_date_string,
			 	process_id );
		}
*/
	
		if ( !merged_datasets_output_transmit(
				output_pipe,
		  		process_generic_output->
					value_folder->
					compare_datatype_list,
				process_generic_output->accumulate ) )
		{
			printf(
			"<h3>Warning: nothing selected to output.</h3>\n" );
			pclose( output_pipe );
			document_close();
			exit( 0 ); 
		}

		pclose( output_pipe );

		printf( "<h1>Merged Datasets Transmission<br></h1>\n" );
		printf( "<h1>\n" );
		fflush( stdout );
		system( "date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h1>\n" );

		printf( "<br>Search criteria: %s\n",
			query_get_display_where_clause(
				where_clause,
				application_name,
				process_generic_output->
					value_folder->value_folder_name,
				application_get_is_primary_application(
					application_name ) ) );

		appaserver_library_output_ftp_prompt(
				ftp_filename,
				TRANSMIT_PROMPT,
				(char *)0 /* target */,
				(char *)0 /* application_type */ );
	}
	else
	if ( strcmp( merged_output, "stdout" ) == 0 )
	{
		FILE *output_pipe;
		char sys_string[ 512 ];

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e '|'" );
*/

		sprintf( sys_string,
			 "tr '|' '%c'",
			 OUTPUT_TEXT_FILE_DELIMITER );

		output_pipe = popen( sys_string, "w" );

		merged_datasets_output_transmit(
				output_pipe,
		  		process_generic_output->
					value_folder->
					compare_datatype_list,
				process_generic_output->accumulate );
		pclose( output_pipe );
	}
	else
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid output medium = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			merged_output );
		exit( 0 );
	}

	if ( strcmp( merged_output, "stdout" ) != 0 )
	{
		document_close();
	}

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	exit( 0 );
} /* main() */

boolean merged_datasets_output_transmit(
					FILE *output_pipe,
					LIST *compare_datatype_list,
					boolean accumulate )
{
	LIST *key_list;
	char *key;
	PROCESS_GENERIC_DATATYPE *datatype;
	PROCESS_GENERIC_VALUE *value;
	char buffer[ 512 ];
	char initial_capital_buffer[ 512 ];
	HASH_TABLE *merged_hash_table;

	if ( !list_rewind( compare_datatype_list ) ) return 0;

	fprintf( output_pipe, "Date" );

	do {
		datatype = list_get_pointer( compare_datatype_list );

		fprintf(
			output_pipe,
			"|%s(%s)",
			search_replace_character(
				format_initial_capital(
					initial_capital_buffer,
					list_display_delimited(
						datatype->
						foreign_attribute_data_list,
						'/' ) ),
				' ', '_' ),
			datatype->units );

		if ( accumulate )
		{
			fprintf( output_pipe, "|Accumulate" );
		}

	} while( list_next( compare_datatype_list ) );

	fprintf( output_pipe, "\n" );

	merged_hash_table =
		get_merged_hash_table(
			compare_datatype_list );

	key_list =
		 hash_table_get_ordered_key_list(
			merged_hash_table );

	if ( !list_length( key_list ) )
	{
		return 0;
	}

	list_rewind( key_list );

	do {
		key = list_get_pointer( key_list );

		fprintf( output_pipe, "%s", key );

		list_rewind( compare_datatype_list );

		do {
			datatype = list_get_pointer( compare_datatype_list );

			value = hash_table_get_pointer(
					datatype->values_hash_table,
					key );

			if ( value && !value->is_null )
			{
				sprintf(buffer,
					"%.3lf",
					value->value );
			}
			else
			{
				strcpy( buffer, "null" );
			}

			fprintf( output_pipe, "|%s", buffer );

			if ( accumulate )
			{
				if ( value && !value->is_null )
				{
					sprintf(buffer,
						"%.3lf",
						value->accumulate );
				}
				else
				{
					strcpy( buffer, "null" );
				}
	
				fprintf( output_pipe, "|%s", buffer );
			}

		} while( list_next( compare_datatype_list ) );

		fprintf( output_pipe, "\n" );

	} while( list_next( key_list ) );	

	list_free_container( key_list );
	return 1;
} /* merged_datasets_output_transmit() */

boolean merged_datasets_output_gracechart(
			LIST *compare_datatype_list,
			PROCESS_GENERIC_DATATYPE_FOLDER *datatype_folder,
			enum aggregate_level aggregate_level,
			char *value_folder_name,
			char *where_clause,
			char *application_name,
			char *role_name,
			char *begin_date_string,
			char *end_date_string,
			char *argv_0,
			char *document_root_directory,
			boolean accumulate )
{
	char datatype_entity_datatype_input_buffer[ 512 ];
	char *title;
	char sub_title[ 512 ];
	GRACE *grace;
	char graph_identifier[ 16 ];
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	GRACE_GRAPH *grace_graph;
	GRACE_DATATYPE *grace_datatype;
	boolean bar_graph = 0;
	char legend[ 128 ];
	char buffer[ 512 ];
	LIST *key_list;
	char *key;
	PROCESS_GENERIC_DATATYPE *datatype;
	PROCESS_GENERIC_VALUE *value;
	HASH_TABLE *merged_hash_table;
	char *datatype_entity_name;

	if ( !list_rewind( compare_datatype_list ) ) return 0;

	merged_hash_table =
		get_merged_hash_table(
			compare_datatype_list );

	key_list =
		 hash_table_get_ordered_key_list(
			merged_hash_table );

	if ( !list_length( key_list ) ) return 0;

	title = get_title(	"Chart",
				aggregate_level,
				value_folder_name,
				accumulate );

	sprintf(sub_title,
		"From %s To %s",
		begin_date_string,
		end_date_string );

	grace = grace_new_unit_graph_grace(
				application_name,
				role_name,
				(char *)0 /* infrastructure_process */,
				(char *)0 /* data_process */,
				argv_0,
				0 /* datatype_entity_piece */,
				1 /* datatype_piece */,
				2 /* date_piece */,
				-1 /* time_piece */,
				3 /* value_piece */,
				title,
				sub_title,
				0 /* not datatype_type_xyhilo */,
				no_cycle_colors_if_multiple_datatypes );

	grace->dataset_no_cycle_color = 1;

	if ( !grace_set_begin_end_date(grace,
					begin_date_string,
					end_date_string ) )
	{
		printf(
		"<h2>ERROR: Invalid date format format.</h2>" );
		document_close();
		exit( 1 );
	}

	grace_graph = grace_new_grace_graph();

	list_append_pointer( grace->graph_list, grace_graph );

	if ( !list_rewind( compare_datatype_list ) ) return 0;

	do {
		datatype = list_get_pointer( compare_datatype_list );

		datatype_entity_name =
			process_generic_get_datatype_entity(
				datatype->foreign_attribute_data_list,
				datatype_folder->primary_attribute_name_list,
				' ' /* delimiter */ );

		grace_datatype =
			grace_new_grace_datatype(
				strdup( datatype_entity_name ),
				process_generic_get_datatype_name(
			 		datatype->
					primary_attribute_data_list,
					' ' /* delimiter */ ) );

		grace_graph->xaxis_ticklabel_angle = 45;

		if ( datatype->units )
		{
			sprintf(legend,
				"%s/%s (%s)",
				datatype_entity_name,
				process_generic_get_datatype_name(
					datatype->
					primary_attribute_data_list,
					' ' /* delimiter */ ),
				datatype->units );
		}
		else
		{
			sprintf(legend,
				"%s/%s",
				datatype_entity_name,
				process_generic_get_datatype_name(
					datatype->
					primary_attribute_data_list,
					' ' /* delimiter */ ) );
		}

		strcpy(	legend,
			format_initial_capital( buffer, legend ) );

		grace_datatype->legend = strdup( legend );

		bar_graph = datatype->bar_graph;

		if ( bar_graph )
		{
			grace_datatype->line_linestyle = 0;
			grace_datatype->datatype_type_bar_xy_xyhilo = "bar";
		}
		else
		{
			grace_datatype->datatype_type_bar_xy_xyhilo = "xy";
		}

		list_append_pointer(	grace_graph->datatype_list,
					grace_datatype );

	} while( list_next( compare_datatype_list ) );

	list_rewind( key_list );

	do {
		key = list_get_pointer( key_list );

		list_rewind( compare_datatype_list );

		do {
			datatype = list_get_pointer( compare_datatype_list );

			datatype_entity_name =
			process_generic_get_datatype_entity(
				datatype->foreign_attribute_data_list,
				datatype_folder->primary_attribute_name_list,
				' ' /* delimiter */ );

			sprintf(datatype_entity_datatype_input_buffer,
			 	"%s|%s|%s",
			 	datatype_entity_name,
				process_generic_get_datatype_name(
			 		datatype->
					primary_attribute_data_list,
					' ' /* delimiter */ ),
			 	key );

			value = hash_table_get_pointer(
					datatype->values_hash_table,
					key );

			if ( value && !value->is_null )
			{
				if ( accumulate )
				{
					sprintf(buffer,
						"|%.3lf",
						value->accumulate );
				}
				else
				{
					sprintf(buffer,
						"|%.3lf",
						value->value );
				}
			}
			else
			{
				strcpy( buffer, "|null" );
			}

			strcat( datatype_entity_datatype_input_buffer, buffer );

			grace_set_string_to_point_list(
				grace->graph_list, 
				grace->datatype_entity_piece,
				grace->datatype_piece,
				grace->date_piece,
				grace->time_piece,
				grace->value_piece,
				datatype_entity_datatype_input_buffer,
				unit_graph,
				grace->datatype_type_xyhilo,
				grace->dataset_no_cycle_color,
				(char *)0 /* optional_label */ );

		} while( list_next( compare_datatype_list ) );

	} while( list_next( key_list ) );	

	grace->grace_output =
		application_get_grace_output( application_name );

	sprintf( graph_identifier, "%d", getpid() );

	grace_get_filenames(
			&grace->agr_filename,
			&grace->ftp_agr_filename,
			&grace->postscript_filename,
			&grace->output_filename,
			&grace->ftp_output_filename,
			application_name,
			document_root_directory,
			graph_identifier,
			grace->grace_output );

	if ( !grace_set_structures(
				&page_width_pixels,
				&page_length_pixels,
				&distill_landscape_flag,
				&grace->landscape_mode,
				grace,
				grace->graph_list,
				grace->anchor_graph_list,
				grace->begin_date_julian,
				grace->end_date_julian,
				grace->number_of_days,
				grace->grace_graph_type,
				0 /* not force_landscape_mode */ ) )
	{
		return 0;
	}

	/* grace->symbols = 1; */
	grace_move_legend_bottom_left(
			grace_graph,
			grace->landscape_mode );

	grace_graph->legend_char_size = GRACE_DEFAULT_LEGEND_CHAR_SIZE;

/*
	grace_lower_legend(	grace->graph_list,
				-0.05 );
	grace_lower_legend(	grace->graph_list,
				0.30 );
*/

	if ( grace_contains_multiple_datatypes( grace->graph_list ) )
		grace->dataset_no_cycle_color = 1;


	if ( !grace_output_charts(
				grace->output_filename, 
				grace->postscript_filename,
				grace->agr_filename,
				grace->title,
				grace->sub_title,
				grace->xaxis_ticklabel_format,
				grace->grace_graph_type,
				grace->x_label_size,
				page_width_pixels,
				page_length_pixels,
				application_get_grace_home_directory(
					application_name ),
				application_get_grace_execution_directory(
					application_name ),
				application_get_grace_free_option_yn(
					application_name ),
				grace->grace_output,
				application_get_distill_directory(
					application_name ),
				distill_landscape_flag,
				application_get_ghost_script_directory(
					application_name ),
				(LIST *)0 /* quantum_datatype_name_list */,
				grace->symbols,
				grace->world_min_x,
				grace->world_max_x,
				grace->xaxis_ticklabel_precision,
				grace->graph_list,
				grace->anchor_graph_list ) )
	{
		printf( "<h2>No data for selected parameters.</h2>\n" );
		document_close();
		exit( 0 );
	}
	else
	{
		grace_output_graph_window(
				application_name,
				grace->ftp_output_filename,
				grace->ftp_agr_filename,
				(char *)0 /* appaserver_mount_point */,
				0 /* with_document_output */,
			 	query_get_display_where_clause(
					where_clause,
					application_name,
					value_folder_name,
					application_get_is_primary_application(
						application_name ) ) );
	}
	return 1;
} /* merged_datasets_output_gracechart() */

char *get_title(	char *medium,
			enum aggregate_level aggregate_level,
			char *value_folder_name,
			boolean accumulate )
{
	char title[ 512 ];

	if ( aggregate_level != aggregate_level_none
	&&   aggregate_level != real_time )
	{
		sprintf(title,
		 	"%s %s %s",
			medium,
			aggregate_level_get_string( aggregate_level ),
			value_folder_name );
	}
	else
	{
		sprintf(title,
		 	"%s %s",
			medium,
			value_folder_name );
	}

	if ( accumulate ) sprintf( title + strlen( title ), " (accumulate)" );

	format_initial_capital( title, title );
	return strdup( title );

} /* get_title() */

boolean merged_datasets_output_table(
					LIST *compare_datatype_list,
					enum aggregate_level aggregate_level,
					char *value_folder_name,
					char *where_clause,
					char *application_name,
					boolean accumulate )
{
	LIST *key_list;
	char *key;
	PROCESS_GENERIC_DATATYPE *datatype;
	PROCESS_GENERIC_VALUE *value;
	LIST *heading_list;
	HTML_TABLE *html_table = {0};
	char buffer[ 512 ];
	HASH_TABLE *merged_hash_table;
	register int count = 0;

	if ( !list_rewind( compare_datatype_list ) ) return 0;

	merged_hash_table =
		get_merged_hash_table(
			compare_datatype_list );

	key_list =
		 hash_table_get_ordered_key_list(
			merged_hash_table );

	if ( !list_length( key_list ) ) return 0;

	html_table =
		new_html_table(
			get_title(	"Table",
					aggregate_level,
					value_folder_name,
					accumulate ),
			(char *)0 /* sub_title */ );

	heading_list = list_new_list();

	list_append_pointer( heading_list, "Date" );

	list_rewind( compare_datatype_list );
	do {
		datatype = list_get_pointer( compare_datatype_list );

		sprintf(buffer,
			"%s (%s)",
			list_display_delimited(
				datatype->foreign_attribute_data_list,
				'/' ),
			datatype->units );

		list_append_pointer(
				heading_list,
				strdup( format_initial_capital(
						buffer,
						buffer ) ) );

		if ( accumulate )
		{
			list_append_pointer(
					heading_list,
					"Accumulate" );
		}

	} while( list_next( compare_datatype_list ) );

	html_table->number_left_justified_columns = 1;
	html_table->number_right_justified_columns =
		list_length( heading_list ) - 1;

	html_table_set_heading_list( html_table, heading_list );
	html_table_output_table_heading(
					html_table->title,
					html_table->sub_title );

	printf( "<br>Search criteria: %s",
		 query_get_display_where_clause(
			where_clause,
			application_name,
			value_folder_name,
			application_get_is_primary_application(
				application_name ) ) );

	html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				(LIST *)0 /* justify_list */ );

	list_rewind( key_list );
	do {
		key = list_get_pointer( key_list );

		html_table_set_data(	html_table->data_list,
					strdup( key ) );

		list_rewind( compare_datatype_list );

		do {
			datatype =
				list_get_pointer(
					compare_datatype_list );

			value = hash_table_get_pointer(
					datatype->values_hash_table,
					key );

			if ( value && !value->is_null )
			{
				sprintf(buffer,
					"%.3lf",
					value->value );
			}
			else
			{
				strcpy( buffer, "null" );
			}

			html_table_set_data(	html_table->data_list,
						strdup( buffer ) );

			if ( accumulate )
			{
				if ( value && !value->is_null )
				{
					sprintf(buffer,
						"%.3lf",
						value->accumulate );
				}
				else
				{
					strcpy( buffer, "null" );
				}

				html_table_set_data(	html_table->data_list,
							strdup( buffer ) );
			}

		} while( list_next( compare_datatype_list ) );

		if ( !(++count % ROWS_BETWEEN_HEADING ) )
		{
			html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				(LIST *)0 /* justify_list */ );
		}

		html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				(LIST *)0 /* justify_list */ );

		list_free_string_list( html_table->data_list );
		html_table->data_list = list_new_list();

	} while( list_next( key_list ) );	

	html_table_close();
	return 1;

} /* merged_datasets_output_table() */

HASH_TABLE *get_merged_hash_table( LIST *compare_datatype_list )
{
	HASH_TABLE *merged_hash_table;
	PROCESS_GENERIC_DATATYPE *datatype;
	LIST *key_list;
	char *key;

	merged_hash_table = hash_table_new_hash_table( hash_table_large );

	if ( !list_rewind( compare_datatype_list ) )
		return merged_hash_table;

	do {
		datatype =
			list_get_pointer(
				compare_datatype_list );

		key_list =
			hash_table_get_key_list(
				datatype->values_hash_table );

		if ( list_rewind( key_list ) )
		{
			do {
				key = list_get_pointer( key_list );

				hash_table_set_pointer(
					merged_hash_table,
					key,
					"" );
			} while( list_next( key_list ) );
		}

		list_free_container( key_list );

	} while( list_next( compare_datatype_list ) );

	return merged_hash_table;

} /* get_merged_hash_table() */

boolean get_has_bar_graph( LIST *compare_datatype_list )
{
	PROCESS_GENERIC_DATATYPE *datatype;

	if ( !list_rewind( compare_datatype_list ) ) return 0;

	do {
		datatype =
			list_get_pointer(
				compare_datatype_list );

		if ( datatype->bar_graph ) return 1;
	} while( list_next( compare_datatype_list ) );

	return 0;
} /* get_has_bar_graph() */

boolean merged_datasets_output_easychart(
			char *application_name,
			char *begin_date_string,
			char *end_date_string,
			LIST *compare_datatype_list,
			PROCESS_GENERIC_DATATYPE_FOLDER *datatype_folder,
			enum aggregate_level aggregate_level,
			char *document_root_directory,
			char *process_name,
			char *value_folder_name,
			char *where_clause,
			boolean accumulate )
{
	EASYCHARTS *easycharts;
	FILE *chart_file;
	char title[ 512 ];
	boolean has_bar_graph;
	char applet_library_archive[ 128 ];
	int easycharts_width;
	int easycharts_height;

	if ( !list_rewind( compare_datatype_list ) ) return 0;

	application_constants_get_easycharts_width_height(
			&easycharts_width,
			&easycharts_height,
			application_name );

	easycharts =
		easycharts_new_timeline_easycharts(
			easycharts_width, easycharts_height );

	has_bar_graph = get_has_bar_graph( compare_datatype_list );

	sprintf(title, 
		"%s\\nFrom: %s to %s\n",
		get_title(	"Chart",
				aggregate_level,
				value_folder_name,
				accumulate ),
		begin_date_string,
		end_date_string );

	easycharts_get_chart_filename(
			&easycharts->chart_filename,
			&easycharts->prompt_filename,
			application_name,
			document_root_directory,
			getpid() );

	chart_file = fopen( easycharts->chart_filename, "w" );

	if ( !chart_file )
	{
		printf( "<p>ERROR: cannot open %s for write.\n",
			easycharts->chart_filename );
		document_close();
		exit( 1 );
	}

	sprintf(applet_library_archive,
		"/appaserver/%s/%s",
		application_name,
		EASYCHARTS_JAR_FILE );
	easycharts->applet_library_archive = applet_library_archive;

	easycharts->output_chart_list =
		merged_datasets_easycharts_get_output_chart_list(
			compare_datatype_list,
			datatype_folder,
			accumulate );

	if ( !easycharts->output_chart_list
	||   !list_length( easycharts->output_chart_list ) )
	{
		printf( "<p>There are no charts to display\n" );
		document_close();
		exit( 0 );
	}

	easycharts->yaxis_decimal_count =
		easycharts_get_yaxis_decimal_count(
			easycharts->output_chart_list );

	easycharts->range_step =
		easycharts_get_range_step(
			easycharts->output_chart_list );

	/* easycharts->sample_label_angle = 90; */
	easycharts->bold_labels = 0;
	easycharts->bold_legends = 0;
	easycharts->font_size = 12;
	easycharts->sample_scroller_on = 1;
	easycharts->range_scroller_on = 1;

	if ( has_bar_graph )
		easycharts->series_line_off = 1;

	easycharts->title = title;

	easycharts_output_all_charts(
			chart_file,
			easycharts->output_chart_list,
			easycharts->highlight_on,
			easycharts->highlight_style,
			easycharts->point_highlight_size,
			easycharts->series_labels,
			easycharts->series_line_off,
			easycharts->applet_library_archive,
			easycharts->width,
			easycharts->height,
			easycharts->title,
			easycharts->set_y_lower_range,
			easycharts->legend_on,
			easycharts->value_labels_on,
			easycharts->sample_scroller_on,
			easycharts->range_scroller_on,
			easycharts->xaxis_decimal_count,
			easycharts->yaxis_decimal_count,
			easycharts->range_labels_off,
			easycharts->value_lines_off,
			easycharts->range_step,
			easycharts->sample_label_angle,
			easycharts->bold_labels,
			easycharts->bold_legends,
			easycharts->font_size,
			easycharts->label_parameter_name,
			1 /* include_sample_series_output */ );

	easycharts_output_html( chart_file );

	fclose( chart_file );

	easycharts_output_graph_window(
				application_name,
				(char *)0 /* appaserver_mount_point */,
				0 /* not with_document_output */,
				process_name,
				easycharts->prompt_filename,
			 	query_get_display_where_clause(
					where_clause,
					application_name,
					value_folder_name,
					application_get_is_primary_application(
						application_name ) ) );

	return 1;
} /* merged_datasets_output_easychart() */

LIST *merged_datasets_easycharts_get_output_chart_list(
			LIST *compare_datatype_list,
			PROCESS_GENERIC_DATATYPE_FOLDER *datatype_folder,
			boolean accumulate )
{
	PROCESS_GENERIC_DATATYPE *datatype;
	LIST *output_chart_list;
	char output_datatype_name[ 128 ];
	LIST *key_list;
	PROCESS_GENERIC_VALUE *value;
	EASYCHARTS_OUTPUT_CHART *output_chart;
	EASYCHARTS_OUTPUT_DATATYPE *output_datatype;
	HASH_TABLE *merged_hash_table;
	char *key;
	char *datatype_entity_name;
	double y_value;

	if ( !compare_datatype_list
	||   !list_length( compare_datatype_list ) )
	{
		return (LIST *)0;
	}

	output_chart_list = list_new_list();
	output_chart = easycharts_new_output_chart();
	list_append_pointer( output_chart_list, output_chart );

/*
	output_chart->bar_chart = input_chart->bar_chart;
	output_chart->bar_labels_on = input_chart->bar_labels_on;
	output_chart->x_axis_label = input_chart->x_axis_label;
	output_chart->y_axis_label = input_chart->y_axis_label;

*/
	output_chart->applet_library_code =
			EASYCHARTS_APPLET_LIBRARY_LINE_CHART;

	merged_hash_table =
		get_merged_hash_table(
			compare_datatype_list );

	key_list =
		 hash_table_get_ordered_key_list(
			merged_hash_table );

	if ( !list_length( key_list ) )
	{
		return 0;
	}

	list_rewind( compare_datatype_list );
	do {
		datatype =
			list_get_pointer(
				compare_datatype_list );

		datatype_entity_name =
			process_generic_get_datatype_entity(
				datatype->foreign_attribute_data_list,
				datatype_folder->primary_attribute_name_list,
				' ' /* delimiter */ );

/*
			process_generic_get_datatype_entity_name(
				datatype->foreign_attribute_data_list );
*/

		sprintf( output_datatype_name,
			 "%s/%s",
			 datatype_entity_name,
			 process_generic_get_datatype_name(
			 	datatype->
				primary_attribute_data_list,
				' ' /* delimiter */ ) );

		output_datatype =
			easycharts_new_output_datatype(
				strdup( output_datatype_name ),
				datatype->units );

		list_append_pointer(	output_chart->output_datatype_list,
					output_datatype );

		list_rewind( key_list );

		do {
			key = list_get_pointer( key_list );

			value = hash_table_get_pointer(
					datatype->values_hash_table,
					key );

			if ( !value )
			{
				if ( !easycharts_set_point(
						output_datatype->point_list,
						0.0,
						0.0,
						key /* x_label */,
						0 /* dont display_value */ ) )
				{
					return (LIST *)0;
				}
			}
			else
			{
				if ( accumulate )
				{
					y_value = value->accumulate;
				}
				else
				{
					y_value = value->value;
				}

				if ( !easycharts_set_point(
					output_datatype->point_list,
					0.0 /* x_value */,
					y_value,
					key /* x_label */,
					1 - value->is_null ) )
				{
					return (LIST *)0;
				}
			}
		} while( list_next( key_list ) );
	} while( list_next( compare_datatype_list ) );
	return output_chart_list;
} /* merged_datasets_easycharts_get_output_chart_list() */

