/* $APPASERVER_HOME/src_appaserver/output_google_chart.c		*/
/* --------------------------------------------------------------	*/
/* This process is triggered if you select the google chart radio	*/
/* button in some of the lookup forms.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* --------------------------------------------------------------	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "folder.h"
#include "operation.h"
#include "dictionary.h"
#include "query.h"
#include "google_chart.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "appaserver_link_file.h"
#include "environ.h"
#include "decode_html_post.h"
#include "appaserver.h"
#include "dictionary_appaserver.h"

/* Constants */
/* --------- */
#define FILENAME_STEM		"google_chart"

/* Prototypes */
/* ---------- */
void output_chart(		char *application_name,
				char *login_name,
				char *role_name,
				char *document_root_directory,
				char *appaserver_mount_point,
				char *folder_name,
				DICTIONARY *query_dictionary,
				char *time_attribute_name,
				LIST *float_integer_attribute_name_list,
				LIST *select_attribute_name_list );

int main( int argc, char **argv )
{
	char *login_name, *application_name, *session, *folder_name;
	char *role_name;
	char decoded_dictionary_string[ MAX_INPUT_LINE ];
	char dictionary_string[ MAX_INPUT_LINE ];
	DICTIONARY *original_post_dictionary;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FOLDER *folder;
	LIST *date_primary_attribute_name_list;
	LIST *time_primary_attribute_name_list;
	LIST *float_integer_attribute_name_list;
	char *date_attribute_name = {0};
	char *time_attribute_name = {0};
	LIST *select_attribute_name_list;
	DICTIONARY_APPASERVER *dictionary_appaserver;

	if ( ! ( application_name =
			environ_get_environment(
				APPASERVER_DATABASE_ENVIRONMENT_VARIABLE ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot get environment of %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE );
		exit( 1 );
	}

	appaserver_error_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc < 7 )
	{
		fprintf( stderr, 
"Usage: %s login_name ignored session folder role ignored [dictionary_stdin]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	login_name = argv[ 1 ];
	session = argv[ 3 ];
	folder_name = argv[ 4 ];
	role_name = argv[ 5 ];
	/* state = argv[ 6 ]; */

	if ( argc == 8 && strcmp( argv[ 7 ], "dictionary_stdin" ) == 0 )
	{
		get_line( dictionary_string, stdin );
		decode_html_post(	decoded_dictionary_string, 
					dictionary_string );

		original_post_dictionary = 
			dictionary_index_string2dictionary( 
				decoded_dictionary_string );

		if ( ! ( dictionary_appaserver =
				dictionary_appaserver_new(
					original_post_dictionary,
					(char *)0 /* application_name */,
					(LIST *)0 /* attribute_list */,
					(LIST *)0 /* operation_name_list */) ) )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: exiting early.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__ );
			exit( 1 );
		}
	}
	else
	{
		dictionary_appaserver =
				dictionary_appaserver_new(
					(DICTIONARY *)0,
					(char *)0 /* application_name */,
					(LIST *)0 /* attribute_list */,
					(LIST *)0 /* operation_name_list */ );
	}

	appaserver_parameter_file = appaserver_parameter_file_new();

	folder = folder_new_folder( 	application_name,
					session,
					folder_name );

	folder->attribute_list =
		attribute_get_attribute_list(
			folder->application_name,
			folder->folder_name,
			(char *)0 /* attribute_name */,
			(LIST *)0 /* mto1_isa_related_folder_list */,
			role_name );

	date_primary_attribute_name_list =
	    attribute_list_get_primary_datatype_attribute_string_list(
		folder->attribute_list,
		"current_date,current_date_time,date,date_time,current_date" );

	if ( !list_length( date_primary_attribute_name_list ) )
		return 0;
	else
		date_attribute_name =
			list_get_first_pointer(
				date_primary_attribute_name_list );

	time_primary_attribute_name_list =
		attribute_list_get_primary_datatype_attribute_string_list(
						folder->attribute_list,
						"time,current_time" );

	if ( list_length( time_primary_attribute_name_list ) )
	{
		time_attribute_name =
			list_get_first_pointer(
				time_primary_attribute_name_list );
	}

	float_integer_attribute_name_list =
		attribute_get_time_chart_attribute_name_list(
					folder->attribute_list );

	if ( !list_length( float_integer_attribute_name_list ) )
	{
		return 0;
	}

	select_attribute_name_list = list_new();
	list_append_pointer(
			select_attribute_name_list,
			date_attribute_name );

	if ( time_attribute_name )
	{
		list_append_pointer(
				select_attribute_name_list,
				time_attribute_name );
	}

	list_append_list(
			select_attribute_name_list,
			float_integer_attribute_name_list );

	output_chart(	application_name,
			login_name,
			role_name,
			appaserver_parameter_file->document_root,
			appaserver_parameter_file->appaserver_mount_point,
			folder_name,
			dictionary_appaserver->query_dictionary,
			time_attribute_name,
			float_integer_attribute_name_list,
			select_attribute_name_list );
	return 0;
} /* main() */

void output_chart(	char *application_name,
			char *login_name,
			char *role_name,
			char *document_root_directory,
			char *appaserver_mount_point,
			char *folder_name,
			DICTIONARY *query_dictionary,
			char *time_attribute_name,
			LIST *float_integer_attribute_name_list,
			LIST *select_attribute_name_list )
{
	char *output_filename;
	char *prompt_filename;
	char title[ 256 ];
	char buffer[ 256 ];
	char sub_title[ 1024 ];
	char initcap_buffer[ 256 ];
	DOCUMENT *document;
	LIST *query_record_list;
	char begin_date_string[ 128 ];
	char date_data[ 128 ];
	char time_data[ 128 ];
	char value_data[ 128 ];
	char *float_integer_attribute_name;
	GOOGLE_OUTPUT_CHART *google_output_chart;
	QUERY *query;
	int first_time = 1;
	int data_piece_offset;
	char *query_record;
	int value_piece_offset = 0;
	FILE *output_file;

	appaserver_link_get_pid_filename(
			&output_filename,
			&prompt_filename,
			application_name,
			document_root_directory,
			getpid(),
			FILENAME_STEM,
			"html" /* extension */ );

	output_file = fopen( output_filename, "w" );

	if ( !output_file )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot open %s\n",
			__FILE__,
			__FUNCTION__,
			output_filename );
		exit( 1 );
	}

	document = document_new( "", application_name );
	document_set_output_content_type( document );

	document_output_head(
			document->application_name,
			document->title,
			document->output_content_type,
			appaserver_mount_point,
			document->javascript_module_list,
			document->stylesheet_filename,
			application_get_relative_source_directory(
				application_name ),
			0 /* not with_dynarch_menu */ );

	query = query_folder_new(
			application_name,
			login_name,
			folder_name,
			query_dictionary,
			role_new( application_name, role_name ) );

	query_record_list =
		query_get_record_list(
			application_name,
			query->query_output,
			list_display( select_attribute_name_list ),
			query->query_output->order_clause );

	if ( !query_record_list || !list_length( query_record_list ) )
	{
		printf( "<h2>No data for selected parameters.</h2>\n" );
		document_close();
		exit( 0 );
	}

	sprintf(title,
		"Chart %s", 
		format_initial_capital( initcap_buffer,
			format_plural( buffer, folder_name ) ) );

	strcpy( sub_title,
		query_get_display_where_clause(
				query->query_output->where_clause,
				application_name,
				folder_name,
				application_get_is_primary_application(
					application_name ) ) );

	if ( strcmp( sub_title, "1 = 1" ) == 0 )
		*sub_title = '\0';

	strcpy( buffer, "'" );
	search_replace_string( sub_title, buffer, "" );

	strcpy( sub_title, format_initial_capital( buffer, sub_title ) );

	google_output_chart =
		google_output_chart_new(
			GOOGLE_CHART_POSITION_LEFT,
			GOOGLE_CHART_POSITION_TOP,
			GOOGLE_CHART_WIDTH,
			GOOGLE_CHART_HEIGHT );

	if ( !list_rewind( float_integer_attribute_name_list ) ) return;

	if ( time_attribute_name )
		data_piece_offset = 2;
	else
		data_piece_offset = 1;

	do {
		float_integer_attribute_name =
			list_get_pointer(
				float_integer_attribute_name_list );

		list_append_pointer(	google_output_chart->datatype_name_list,
					float_integer_attribute_name );

	} while( list_next( float_integer_attribute_name_list ) );

	if ( !list_rewind( float_integer_attribute_name_list ) ) return;

	do {
		float_integer_attribute_name =
			list_get_pointer(
				float_integer_attribute_name_list );

		if ( !list_rewind( query_record_list ) ) return;

		do {
			query_record = list_get_pointer( query_record_list );

			piece(	date_data,
				FOLDER_DATA_DELIMITER,
				query_record,
				0 );

			if ( strlen( date_data ) == 4 )
				strcat( date_data, "-12-31" );

			if ( first_time )
			{
				strcpy( begin_date_string, date_data );
				first_time = 0;
			}

			if ( time_attribute_name )
			{
				piece(	time_data,
					FOLDER_DATA_DELIMITER,
					query_record,
					1 );
			}
			else
			{
				strcpy( time_data, "2359" );
			}

			piece(	value_data,
				FOLDER_DATA_DELIMITER,
				query_record,
				data_piece_offset + value_piece_offset );

			if ( *value_data )
			{
				google_timeline_set_point(
					google_output_chart->timeline_list,
					google_output_chart->datatype_name_list,
					date_data,
					time_data,
					float_integer_attribute_name
						/* datatype_name */,
					atof( value_data ) /* point */ );
			}

		} while( list_next( query_record_list ) );

		value_piece_offset++;

	} while( list_next( float_integer_attribute_name_list ) );

/*
	fprintf( output_file, "<html>\n" );
*/
	document_output_html_stream( output_file );

	fprintf( output_file, "<head>\n" );

	google_chart_output_include( output_file );

	google_chart_output_visualization_function(
				output_file,
				google_output_chart->google_chart_type,
				google_output_chart->timeline_list,
				google_output_chart->barchart_list,
				google_output_chart->datatype_name_list,
				title,
				(char *)0 /* yaxis_label */,
				google_output_chart->width,
				google_output_chart->height,
				google_output_chart->background_color,
				google_output_chart->legend_position_bottom,
				0 /* not chart_type_bar */,
				google_output_chart->google_package_name,
				0 /* not dont_display_range_selector */,
				aggregate_level_none,
				google_output_chart->chart_number );

	fprintf( output_file, "</head>\n" );
	fprintf( output_file, "<body>\n" );

	google_chart_float_chart(
				output_file,
				(char *)0 /* chart_title */,
				google_output_chart->width,
				google_output_chart->height,
				google_output_chart->chart_number );

	google_chart_output_chart_instantiation(
		output_file,
		google_output_chart->chart_number );

	fprintf( output_file, "</body>\n" );
	fprintf( output_file, "</html>\n" );

	fclose( output_file );

	google_chart_output_prompt(
		application_name,
		prompt_filename,
		FILENAME_STEM /* process_name */,
		query->query_output->where_clause );

	document_close();

} /* output_chart() */

