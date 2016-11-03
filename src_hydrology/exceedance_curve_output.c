/* --------------------------------------------------- 	*/
/* src_hydrology/exceedance_curve_output.c	       	*/
/* --------------------------------------------------- 	*/
/* 						       	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "timlib.h"
#include "application.h"
#include "piece.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "html_table.h"
#include "grace.h"
#include "environ.h"
#include "make_date_time_between_compatable.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "process.h"

/* Constants */
/* --------- */
#define PROCESS_NAME		"output_exceedance_curve"
#define PDF_PROMPT		"Press to view chart."

#define VALUE_PIECE			0
#define REAL_TIME_DATE_PIECE		1
#define REAL_TIME_TIME_PIECE		2
#define REAL_TIME_COUNT_BELOW_PIECE	4
#define REAL_TIME_PERCENT_BELOW_PIECE	5
#define AGGREGATION_COUNT_PIECE		2
#define AGGREGATION_DATE_PIECE		3
#define AGGREGATION_TIME_PIECE		4
#define AGGREGATION_COUNT_BELOW_PIECE	5
#define AGGREGATION_PERCENT_BELOW_PIECE	6

/* Prototypes */
/* ---------- */
void piece_input_buffer(
				char *measurement_value,
				char *count_below,
				char *percent_below,
				char *measurement_date,
				char *measurement_time,
				char *aggregation_count,
				char *input_buffer,
				enum aggregate_level aggregate_level );

int main( int argc, char **argv )
{
	char *application_name;
	char *station;
	char *datatype;
	char *begin_date;
	char *end_date;
	char *aggregate_level_string;
	char *aggregate_statistic_string;
	char *chart_yn;
	char input_buffer[ 4096 ];
	char buffer[ 4096 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	FILE *input_pipe;
	HTML_TABLE *html_table = {0};
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char measurement_value[ 128 ];
	char aggregation_count[ 128 ];
	char count_below[ 128 ];
	char percent_below[ 128 ];
	LIST *heading_list;
	char *ftp_agr_filename;
	char *ftp_output_filename;
	char *output_filename;
	char *agr_filename;
	char *postscript_filename;
	char title[ 256 ];
	char sub_title[ 1024 ] = {0};
	GRACE *grace = {0};
	char graph_identifier[ 128 ];
	char where_clause[ 1024 ];
	char *units_converted;
	char *units;
	char *units_display;
	char legend[ 128 ];
	char value_label[ 128 ];
	enum aggregate_level aggregate_level;
	enum aggregate_statistic aggregate_statistic;
	int page_width_pixels;
	int page_length_pixels;
	char *distill_landscape_flag;
	char *email_address;
	char *chart_email_command_line;
	char *database_string = {0};
	int count = 0;
	boolean bar_chart;
	char value_heading[ 128 ];
	char *horizontal_line_at_point;

	if ( argc != 15 )
	{
		fprintf(stderr,
"Usage: %s application ignored ignored ignored station datatype begin_date end_date aggregate_level aggregate_statistic chart_yn email_address units_converted horizontal_line_at_point\n",
			argv[ 0 ] );
		exit( 1 );
	}

	application_name = argv[ 1 ];
	/* session = argv[ 2 ]; */
	/* login_name = argv[ 3 ]; */
	/* role_name = argv[ 4 ]; */
	station = argv[ 5 ];
	datatype = argv[ 6 ];
	begin_date = argv[ 7 ];
	end_date = argv[ 8 ];
	aggregate_level_string = argv[ 9 ];
	aggregate_statistic_string = argv[ 10 ];
	chart_yn = argv[ 11 ];
	email_address = argv[ 12 ];
	units_converted = argv[ 13 ];
	horizontal_line_at_point = argv[ 14 ];

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

	if ( strcmp( chart_yn, "chart_yn" ) == 0 ) chart_yn = "y";

	appaserver_parameter_file = new_appaserver_parameter_file();

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					station,
					datatype );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					appaserver_parameter_file->
						database_management_system,
					(PROCESS_GENERIC_OUTPUT *)0,
					(DICTIONARY *)0 /* post_dictionary */) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: no data available for these dates.\n" );
		document_close();
		exit( 0 );
	}

	aggregate_level =
		aggregate_level_get_aggregate_level( aggregate_level_string );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
			aggregate_statistic_string,
			aggregate_level );

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
					application_name,
					datatype,
					aggregate_statistic_none );
	}

	units = hydrology_library_get_units_string(
				&bar_chart,
				application_name,
				datatype );

	units_display =
		hydrology_library_get_datatype_units_display(
			application_name,
			datatype,
			units,
			units_converted,
			aggregate_statistic );

	if ( !units_display )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );
		printf( "%s\n",
			hydrology_library_get_output_invalid_units_error(
				datatype,
				units,
				units_converted ) );
		document_close();
		exit( 0 );
	}

	sprintf( where_clause,
"station = '%s' and datatype = '%s' and measurement_date between '%s' and '%s' and measurement_value is not null",
		 station,
		 datatype,
		 begin_date,
		 end_date );

	sprintf(buffer,
"%s/%s/exceedance_curve %s \"%s\" \"%s\" %s \"%s\" \"%s\" 2>>%s",
		appaserver_parameter_file->appaserver_mount_point,
		application_get_relative_source_directory( application_name ),
		application_name,
		where_clause,
		aggregate_level_get_string( aggregate_level ),
		aggregate_statistic_get_string( aggregate_statistic ),
		units,
		units_converted,
		appaserver_error_get_filename( application_name ) );

	input_pipe = popen( buffer, "r" );

	sprintf( title, "Exceedance Curve for %s/%s", station, datatype );
	format_initial_capital( title, title );

	if ( aggregate_level != real_time )
	{
		format_initial_capital(
			value_label,
		 	aggregate_statistic_get_string(
				aggregate_statistic ) );
	}
	else
		*value_label = '\0';

	sprintf( sub_title,
		 "%s %s Values Begin: %s End: %s",
		 format_initial_capital(
				buffer,
		 		aggregate_level_get_string( aggregate_level ) ),
		 value_label,
		 begin_date,
		 end_date );

	if ( *chart_yn == 'y' )
	{
		grace = grace_new_xy_grace(
				(char *)0 /* application_name */,
				(char *)0 /* role_name */,
				title,
				sub_title,
				units_display,
				format_initial_capital( legend, datatype ) );

		grace->xaxis_ticklabel_precision = 0;
		grace->world_min_x = 0.0;
		grace->world_max_x = 100.0;

		sprintf(	graph_identifier,
				"%s_%s_%d",
				station,
				datatype,
				getpid() );

		grace->grace_output =
			application_get_grace_output( application_name );

		grace_get_filenames(
			&agr_filename,
			&ftp_agr_filename,
			&postscript_filename,
			&output_filename,
			&ftp_output_filename,
			application_name,
			appaserver_parameter_file->
				document_root,
			graph_identifier,
			grace->grace_output );
	}
	else
	/* ----------------------------- */
	/* Must be *chart_yn != 'y'	 */
	/* ----------------------------- */
	{
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

		sprintf( title + strlen( title ), "<br>%s", sub_title );
	 	html_table = new_html_table(
				title,
				(char *)0 /* sub_title */ );
	
		heading_list = new_list();

		sprintf( value_heading,
			 "Value<br>(%s)",
			 units_display );
		list_append_string( heading_list, value_heading );

		list_append_string( heading_list, "Count Below" );
		list_append_string( heading_list, "Percent Below" );
	
		if ( aggregate_level == real_time )
		{
			list_append_string( heading_list, "Date" );
			list_append_string( heading_list, "Time" );
		}
		else
		if ( aggregate_level == half_hour
		||   aggregate_level == hourly )
		{
			list_append_string(	heading_list,
						"Date" );
			list_append_string( heading_list, "Time" );
			list_append_string( 	heading_list,
						"Aggregation Count" );
		}
		else
		if ( aggregate_level == daily )
		{
			list_append_string(	heading_list,
						"Date" );
			list_append_string(	heading_list,
						"Aggregation Count" );
		}
		else
		{
			list_append_string(	heading_list,
						"Representative Date" );
			list_append_string(	heading_list,
						"Aggregation Count" );
		}

		html_table_set_heading_list( html_table, heading_list );
		html_table_output_table_heading(
						html_table->title,
						html_table->sub_title );

		html_table->number_left_justified_columns = 0;
		html_table->number_right_justified_columns = 99;

		html_table_output_data_heading(
				html_table->heading_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->justify_list );
	}

	while( get_line( input_buffer, input_pipe ) )
	{
		piece_input_buffer(
				measurement_value,
				count_below,
				percent_below,
				measurement_date,
				measurement_time,
				aggregation_count,
				input_buffer,
				aggregate_level );

		if ( *chart_yn == 'y' )
		{
			grace_set_xy_to_point_list(
				grace->graph_list, 
				atof( percent_below ),
				strdup( measurement_value ),
				(char *)0 /* optional_label */,
				grace->dataset_no_cycle_color );
		}
		else
		{
			html_table_set_data(	html_table->data_list,
						strdup( measurement_value ) );
			html_table_set_data(	html_table->data_list,
						strdup( count_below ) );
			html_table_set_data(	html_table->data_list,
						strdup( percent_below ) );
			html_table_set_data(	html_table->data_list,
						strdup( measurement_date ) );
	
			if ( aggregate_level == real_time )
			{
				html_table_set_data(
						html_table->data_list,
						strdup( measurement_time ) );
			}
			else
			if ( aggregate_level == half_hour
			||   aggregate_level == hourly )
			{
				html_table_set_data(
						html_table->data_list,
						strdup( measurement_time ) );
				html_table_set_data(
						html_table->data_list,
						strdup( aggregation_count ) );
			}
			else
			{
				html_table_set_data(
						html_table->data_list,
						strdup( aggregation_count ) );
			}

			if ( ++count == 20 )
			{
				html_table_output_data_heading(
					html_table->heading_list,
					html_table->
					number_left_justified_columns,
					html_table->
					number_right_justified_columns,
					html_table->justify_list );
				count = 0;
			}

			html_table_output_data(
				html_table->data_list,
				html_table->number_left_justified_columns,
				html_table->number_right_justified_columns,
				html_table->background_shaded,
				html_table->justify_list );

			list_free_string_list( html_table->data_list );
			html_table->data_list = list_new();
		}
	} /* while( get_line() */

	pclose( input_pipe );

	if ( *chart_yn == 'y' )
	{
		GRACE_GRAPH *grace_graph;

		grace_graph =
			(GRACE_GRAPH *)
				list_get_first_pointer(
					grace->graph_list );
		grace_graph->xaxis_label = "Percent Below";
		grace_graph->horizontal_line_at_point =
			atof( horizontal_line_at_point );

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
			DOCUMENT *document;
	
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
	
			printf( "<p>Warning: no graphs to display.\n" );
			document_close();
			exit( 0 );
		}

		grace->symbols = 1;

		if ( !grace_output_charts(
				output_filename, 
				postscript_filename,
				agr_filename,
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
			document_quick_output_body(
						application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

			printf( "<h2>No data for selected parameters.</h2>\n" );
			document_close();
			exit( 0 );
		}

		chart_email_command_line =
				application_get_chart_email_command_line(
					application_name );

		if ( !*chart_email_command_line
		||   *chart_email_command_line == '#'
		||   strcmp( email_address, "email_address" ) == 0 )
		{
			grace_output_graph_window(
				application_name,
				ftp_output_filename,
				ftp_agr_filename,
				appaserver_parameter_file->
					appaserver_mount_point,
				1 /* with_document_output */,
				(char *)0 /* where_clause */ );
		}
		else
		{
			grace_email_graph(
				application_name,
				email_address,
				chart_email_command_line,
				output_filename,
				appaserver_parameter_file->
					appaserver_mount_point,
				1 /* with_document_output */,
				(char *)0 /* where_clause */ );
		}
	}
	else
	{
		html_table_close();
	}
	document_close();
	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );

	exit( 0 );
} /* main() */

void piece_input_buffer(
				char *measurement_value,
				char *count_below,
				char *percent_below,
				char *measurement_date,
				char *measurement_time,
				char *aggregation_count,
				char *input_buffer,
				enum aggregate_level aggregate_level )
{
	piece(	measurement_value,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		VALUE_PIECE );

	if ( aggregate_level == real_time )
	{
		piece(	measurement_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			REAL_TIME_DATE_PIECE );

		piece(	measurement_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			REAL_TIME_TIME_PIECE );

		piece(	count_below,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			REAL_TIME_COUNT_BELOW_PIECE );

		piece(	percent_below,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			REAL_TIME_PERCENT_BELOW_PIECE );
		return;
	}

	if ( aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		piece(	measurement_time,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			AGGREGATION_TIME_PIECE );

	}
	piece(	measurement_date,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		AGGREGATION_DATE_PIECE );

	piece(	aggregation_count,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		AGGREGATION_COUNT_PIECE );

	piece(	count_below,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		AGGREGATION_COUNT_BELOW_PIECE );

	piece(	percent_below,
		FOLDER_DATA_DELIMITER,
		input_buffer,
		AGGREGATION_PERCENT_BELOW_PIECE );

} /* piece_input_buffer() */

