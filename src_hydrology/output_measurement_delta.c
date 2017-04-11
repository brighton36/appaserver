/* ---------------------------------------------------	*/
/* src_hydrology/output_measurement_delta.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "column.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "aggregate_level.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "application_constants.h"
#include "document.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "process.h"
#include "application.h"
#include "easycharts.h"
#include "datatype.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */
#define QUEUE_LINES		100
#define SELECT_LIST		"station,datatype,measurement_date,measurement_time,measurement_value"
#define EASYCHART_DATATYPE_PIECE	0
#define EASYCHART_DATE_TIME_PIECE	1
#define EASYCHART_VALUE_PIECE		2
#define DELTA_VALUE_PIECE		4
#define DELIMITER			','
#define STATION_DATATYPE_DELIMITER	'/'
#define DATE_TIME_DELIMITER		':'

#define OUTPUT_FILE_SPREADSHEET	"%s/%s/measurement_delta_%s_%s_%s_%s%s.csv"
#define FTP_PREPEND_FILE_SPREADSHEET "%s://%s/%s/measurement_delta_%s_%s_%s_%s%s.csv"
#define FTP_NONPREPEND_FILE_SPREADSHEET "/%s/measurement_delta_%s_%s_%s_%s%s.csv"

#define OUTPUT_FILE_TEXT_FILE	"%s/%s/measurement_delta_%s_%s_%s_%s%s.txt"
#define FTP_PREPEND_FILE_TEXT_FILE "%s://%s/%s/measurement_delta_%s_%s_%s_%s%s.txt"
#define FTP_NONPREPEND_FILE_TEXT_FILE "/%s/measurement_delta_%s_%s_%s_%s%s.txt"

/* Prototypes */
/* ---------- */
boolean populate_input_chart_list_data(
				LIST *input_chart_list,
				LIST *station_list,
				LIST *datatype_list,
				char *application_name,
				char *begin_date,
				char *end_date,
				double delta_threshold );

void populate_input_chart_list_datatypes(
				LIST *input_chart_list,
				LIST *station_list,
				LIST *datatype_list,
				char *application_name );

void output_easycharts(		char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				char *document_root_directory,
				char *process_name,
				double delta_threshold );

void output_table(		char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				double delta_threshold );

void output_spreadsheet(	char *application_name,
				char *session,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				char *appaserver_mount_point,
				double delta_threshold );

void output_text_file(		char *application_name,
				char *session,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				char *appaserver_mount_point,
				double delta_threshold );

void output_stdout(		char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				double delta_threshold );

char *get_where_clause(		char *begin_date,
				char *end_date,
				char *station,
				char *datatype );

char *get_heading_string(
			char *application_name,
			char *datatype );

char *get_heading_sys_string(
			char *output_filename,
			char *application_name,
			char *datatype );

char *get_sys_string(	char *output_filename,
			char *table_name,
			char *where_clause,
			double delta_threshold,
			boolean order_by_delta_value );

int main( int argc, char **argv )
{
	char *application_name;
	char *parameter_begin_date, *parameter_end_date;
	char *station_list_string;
	char *datatype_list_string;
	LIST *station_list;
	LIST *datatype_list;
	char *session;
	char *process_name;
	char buffer[ 128 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	char *database_string = {0};
	char *output_medium;
	char *delta_threshold_string;

	if ( argc != 10 )
	{
		fprintf( stderr,
"Usage: %s application session process station datatype begin_date end_date delta_threshold output_medium\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	session = argv[ 2 ];
	process_name = argv[ 3 ];
	station_list_string = argv[ 4 ];
	datatype_list_string = argv[ 5 ];
	parameter_begin_date = argv[ 6 ];
	parameter_end_date = argv[ 7 ];
	delta_threshold_string = argv[ 8 ];
	output_medium = argv[ 9 ];

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

	if ( strcmp( output_medium, "stdout" ) != 0
	&&   strcmp( station_list_string, "station" ) == 0 )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: please choose a station/datatype.\n" );
		document_close();
		exit( 0 );
	}

	station_list = list_string2list( station_list_string, ',' );
	datatype_list = list_string2list( datatype_list_string, ',' );

	if ( strcmp( output_medium, "stdout" ) != 0
	&&   list_length( station_list ) != list_length( datatype_list ) )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf( "<p>ERROR: station and datatype list don't match.\n" );
		document_close();
		exit( 0 );
	}

	hydrology_library_get_clean_begin_end_date(
					&parameter_begin_date,
					&parameter_end_date,
					application_name,
					(char *)0 /* station_name */,
					(char *)0 /* datatype_name */ );

	if ( strcmp( output_medium, "stdout" ) != 0
	&&   !appaserver_library_validate_begin_end_date(
					&parameter_begin_date,
					&parameter_end_date,
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

	if ( strcmp( output_medium, "stdout" ) != 0 )
	{
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
	
		printf( "<h1>%s<br></h1>\n",
			format_initial_capital( buffer, process_name ) );
		printf( "<h2>\n" );
		fflush( stdout );
		system( "date '+%x %H:%M'" );
		fflush( stdout );
		printf( "</h2>\n" );
	}

	if ( strcmp( output_medium, "table" ) == 0 )
	{
		output_table(
				application_name,
				station_list,
				datatype_list,
				parameter_begin_date,
				parameter_end_date,
				atof( delta_threshold_string ) );
	}
	else
	if ( strcmp( output_medium, "spreadsheet" ) == 0 )
	{
		output_spreadsheet(
				application_name,
				session,
				station_list,
				datatype_list,
				parameter_begin_date,
				parameter_end_date,
				appaserver_parameter_file->
					appaserver_mount_point,
				atof( delta_threshold_string ) );
	}
	else
	if ( strcmp( output_medium, "text_file" ) == 0 )
	{
		output_text_file(
				application_name,
				session,
				station_list,
				datatype_list,
				parameter_begin_date,
				parameter_end_date,
				appaserver_parameter_file->
					appaserver_mount_point,
				atof( delta_threshold_string ) );
	}
	else
	if ( strcmp( output_medium, "stdout" ) == 0 )
	{
		output_stdout(
				application_name,
				station_list,
				datatype_list,
				parameter_begin_date,
				parameter_end_date,
				atof( delta_threshold_string ) );
	}
	else
	if ( strcmp( output_medium, "easychart" ) == 0 )
	{
		output_easycharts(
				application_name,
				station_list,
				datatype_list,
				parameter_begin_date,
				parameter_end_date,
				appaserver_parameter_file->
					document_root,
				process_name,
				atof( delta_threshold_string ) );
	}
	else
	{
		printf( "<h3>Error: invalid output medium.</h3>\n" );
		document_close();
		exit( 0 );
	}

	if ( strcmp( output_medium, "stdout" ) != 0 )
		document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file->
					database_management_system );
	return 0;

} /* main() */

void output_stdout(		char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				double delta_threshold )
{
	char *station;
	char *datatype;
	char *where_clause;
	char sys_string[ 1024 ];
	char *table_name;
	char *heading;
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];

	if ( !list_rewind( station_list ) ) return;

	list_rewind( datatype_list );

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );

		hydrology_library_output_station_text_file(
			stdout,
			application_name,
			station );

		where_clause = get_where_clause(
				begin_date,
				end_date,
				station,
				datatype );

		table_name = get_table_name( application_name, "measurement" );

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e ',' 2" );
*/
		sprintf(sys_string,
		 	"tr ',' '%c'",
			OUTPUT_TEXT_FILE_DELIMITER );

		output_pipe = popen( sys_string, "w" );

		heading = get_heading_string(
				application_name,
				datatype );

		search_replace_character( heading, ' ', '_' );

		fprintf( output_pipe, "#%s\n", heading );

		strcpy( sys_string,
			get_sys_string(
				(char *)0 /* output_filename */,
				table_name,
				where_clause,
				delta_threshold,
				0 /* not order_by_delta_value */ ) );

		input_pipe = popen( sys_string, "r" );

		while( get_line( input_buffer, input_pipe ) )
			fprintf( output_pipe, "%s\n", input_buffer );

		pclose( input_pipe );
		pclose( output_pipe );

		list_next( datatype_list );
	} while( list_next( station_list ) );

} /* output_stdout() */

void output_text_file(		char *application_name,
				char *session,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				char *appaserver_mount_point,
				double delta_threshold )
{
	char *station;
	char *datatype;
	char output_filename[ 256 ] = {0};
	char ftp_filename[ 256 ];
	char end_date_suffix[ 128 ];
	char *where_clause;
	int record_count;
	char sys_string[ 1024 ];
	char *table_name;
	char *heading;
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];

	printf( "<table>\n" );

	list_rewind( station_list );
	list_rewind( datatype_list );

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );

		if ( strcmp( end_date, "end_date" ) != 0 )
			sprintf( end_date_suffix, "%s_", end_date );
		else
			*end_date_suffix = '\0';

		sprintf(output_filename,
	 		OUTPUT_FILE_TEXT_FILE,
	 		appaserver_mount_point,
	 		application_name, 
			station,
			datatype,
	 		begin_date,
	 		end_date_suffix,
	 		session );

		if ( ! ( output_pipe = fopen( output_filename, "w" ) ) )
		{
			printf( "<H2>ERROR: Cannot open output file %s\n",
				output_filename );
			document_close();
			exit( 1 );
		}
		else
		{
			fclose( output_pipe );
		}

		hydrology_library_output_station_text_filename(
			output_filename,
			application_name,
			station,
			0 /* not with_zap_file */ );

		if ( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' )
		{
			sprintf(ftp_filename, 
		 		FTP_PREPEND_FILE_TEXT_FILE, 
				application_get_http_prefix( application_name ),
		 		appaserver_library_get_server_address(),
		 		application_name,
				station,
				datatype,
		 		begin_date,
		 		end_date_suffix,
		 		session );
		}
		else
		{
			sprintf(ftp_filename, 
		 		FTP_NONPREPEND_FILE_TEXT_FILE, 
		 		application_name,
				station,
				datatype,
		 		begin_date,
		 		end_date_suffix,
		 		session );
		}

		where_clause = get_where_clause(
				begin_date,
				end_date,
				station,
				datatype );

		table_name = get_table_name( application_name, "measurement" );

/*
		sprintf( sys_string,
			 "delimiter2padded_columns.e ',' 2 >> %s",
			 output_filename );
*/
		sprintf(sys_string,
		 	"tr ',' '%c' >> %s",
			OUTPUT_TEXT_FILE_DELIMITER,
		 	output_filename );

		output_pipe = popen( sys_string, "w" );

		heading = get_heading_string(
				application_name,
				datatype );

		fprintf( output_pipe, "#%s\n", heading );

		strcpy( sys_string,
			get_sys_string(
				(char *)0 /* output_filename */,
				table_name,
				where_clause,
				delta_threshold,
				0 /* not order_by_delta_value */ ) );

		input_pipe = popen( sys_string, "r" );

		while( get_line( input_buffer, input_pipe ) )
			fprintf( output_pipe, "%s\n", input_buffer );

		pclose( input_pipe );
		pclose( output_pipe );

		sprintf( sys_string,
		 	"cat %s | grep -v '^#' | grep -v '^$' | wc -l",
		 	output_filename );
		record_count = atoi( pipe2string( sys_string ) );

		printf( "<tr><td>For %s/%s generated %d differences:\n",
			station,
			datatype,
			record_count );
		
		printf( "<tr><td><a href=\"%s\">%s</a></p>\n",
			ftp_filename,
			TRANSMIT_PROMPT );

		list_next( datatype_list );
	} while( list_next( station_list ) );

	printf( "</table>\n" );

} /* output_text_file() */

void output_spreadsheet(	char *application_name,
				char *session,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				char *appaserver_mount_point,
				double delta_threshold )
{
	char *station;
	char *datatype;
	char output_filename[ 256 ] = {0};
	FILE *output_file;
	char ftp_filename[ 256 ];
	char end_date_suffix[ 128 ];
	char *where_clause;
	int record_count;
	char sys_string[ 1024 ];
	char *table_name;

	printf( "<table>\n" );

	list_rewind( station_list );
	list_rewind( datatype_list );

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );

		if ( strcmp( end_date, "end_date" ) != 0 )
			sprintf( end_date_suffix, "%s_", end_date );
		else
			*end_date_suffix = '\0';

		sprintf(output_filename,
	 		OUTPUT_FILE_SPREADSHEET,
	 		appaserver_mount_point,
	 		application_name, 
			station,
			datatype,
	 		begin_date,
	 		end_date_suffix,
	 		session );

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

		hydrology_library_output_station_text_filename(
			output_filename,
			application_name,
			station,
			0 /* not with_zap_file */ );

		if ( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' )
		{
			sprintf(ftp_filename, 
		 		FTP_PREPEND_FILE_SPREADSHEET, 
				application_get_http_prefix( application_name ),
		 		appaserver_library_get_server_address(),
		 		application_name,
				station,
				datatype,
		 		begin_date,
		 		end_date_suffix,
		 		session );
		}
		else
		{
			sprintf(ftp_filename, 
		 		FTP_NONPREPEND_FILE_SPREADSHEET, 
		 		application_name,
				station,
				datatype,
		 		begin_date,
		 		end_date_suffix,
		 		session );
		}

		where_clause = get_where_clause(
				begin_date,
				end_date,
				station,
				datatype );

		table_name = get_table_name( application_name, "measurement" );

		strcpy( sys_string,
			get_heading_sys_string(
				output_filename,
				application_name,
				datatype ) );

		system( sys_string );

		strcpy( sys_string,
			get_sys_string(
				output_filename,
				table_name,
				where_clause,
				delta_threshold,
				0 /* not order_by_delta_value */ ) );

		system( sys_string );

		sprintf( sys_string,
		 	"cat %s | grep -v '^#' | grep -v '^$' | wc -l",
		 	output_filename );
		record_count = atoi( pipe2string( sys_string ) );

		printf( "<tr><td>For %s/%s generated %d differences:\n",
			station,
			datatype,
			record_count );
		
		printf( "<tr><td><a href=\"%s\">%s</a></p>\n",
			ftp_filename,
			TRANSMIT_PROMPT );

		list_next( datatype_list );
	} while( list_next( station_list ) );

	printf( "</table>\n" );
} /* output_spreadsheet() */

char *get_sys_string(	char *output_filename,
			char *table_name,
			char *where_clause,
			double delta_threshold,
			boolean order_by_delta_value )
{
	static char sys_string[ 1024 ];
	char output_clause[ 256 ];
	char *additional_sort;

	if ( order_by_delta_value )
		additional_sort = "sort -t',' -n -k5";
	else
		additional_sort = "cat";

	if ( output_filename )
	{
		sprintf( output_clause,
			 "cat >> %s",
			 output_filename );
	}
	else
	{
		strcpy( output_clause, "cat" );
	}

	sprintf(sys_string,
	 	"echo \"select %s from %s where %s order by %s;\"	|"
		"sql.e '%c'						|"
		"delta_values.e %d previous '%c'			|"
		"filter_anomaly.e '%c' %d %.3lf				|"
		"%s							|"
		"%s							 ",
		SELECT_LIST,
		table_name,
		where_clause,
		SELECT_LIST,
		DELIMITER,
		DELTA_VALUE_PIECE,
		DELIMITER,
		DELIMITER,
		DELTA_VALUE_PIECE,
		delta_threshold,
		additional_sort,
		output_clause );

	return sys_string;
} /* get_sys_string() */

char *get_where_clause(		char *begin_date,
				char *end_date,
				char *station,
				char *datatype )
{
	static char where_clause[ 1024 ];
	char date_where_clause[ 128 ];

	if ( strcmp( end_date, "end_date" ) != 0 )
	{
		sprintf(date_where_clause,
			"measurement_date between '%s' and '%s'",
			begin_date,
			end_date );
	}
	else
	{
		sprintf(date_where_clause,
			"measurement_date = '%s'",
			begin_date );
	}

	sprintf(where_clause,
	 	"station = '%s' and datatype = '%s' and %s",
	 	station,
	 	datatype,
	 	date_where_clause );

	return where_clause;

} /* get_where_clause() */

char *get_heading_string(	char *application_name,
				char *datatype )
{
	static char heading[ 1024 ];
	boolean bar_graph;

	sprintf(heading,
	 	"station,datatype,date,time,delta value (%s)",
		datatype_get_units_string(
				&bar_graph,
				application_name,
				datatype ) );
	return heading;
} /* get_heading_string() */

char *get_heading_sys_string(	char *output_filename,
				char *application_name,
				char *datatype )
{
	static char sys_string[ 1024 ];

	sprintf(sys_string,
	 	"echo \"#%s\" >> %s",
		get_heading_string( application_name, datatype ),
	 	output_filename );
	return sys_string;
} /* get_heading_sys_string() */

void output_easycharts(		char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				char *document_root_directory,
				char *process_name,
				double delta_threshold )
{
	EASYCHARTS *easycharts;
	char *chart_filename;
	char *prompt_filename;
	FILE *chart_file;
	char applet_library_archive[ 128 ];
	char title[ 256 ];
	char sub_title[ 256 ];
	int easycharts_width;
	int easycharts_height;

	easycharts_get_chart_filename(
			&chart_filename,
			&prompt_filename,
			application_name,
			document_root_directory,
			getpid() );

	chart_file = fopen( chart_filename, "w" );

	if ( !chart_file )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot open %s\n",
			__FILE__,
			__FUNCTION__,
			chart_filename );
		exit( 1 );
	}

	sprintf(applet_library_archive,
		"/appaserver/%s/%s",
		application_name,
		EASYCHARTS_JAR_FILE );

	application_constants_get_easycharts_width_height(
			&easycharts_width,
			&easycharts_height,
			application_name );

	easycharts =
		easycharts_new_timeline_easycharts(
			easycharts_width, easycharts_height );

	easycharts->point_highlight_size = 0;
	easycharts->applet_library_archive = applet_library_archive;
	easycharts->legend_on = 0;
	easycharts->bold_labels = 0;
	easycharts->bold_legends = 0;
	easycharts->set_y_lower_range = 1;
	easycharts->sample_scroller_on = 1;
	easycharts->range_scroller_on = 1;

	hydrology_library_get_title(
		title,
		sub_title,
		either /* validation_level */,
		aggregate_statistic_none,
		delta /* aggregate_level */,
		(char *)0 /* station */,
		(char *)0 /* datatype */,
		begin_date,
		end_date,
		'n' /* accumulate_yn */ );
	
	sprintf( title + strlen( title ),
		 "\\n%s",
		 sub_title );
	easycharts->title = title;

	populate_input_chart_list_datatypes(
			easycharts->input_chart_list,
			station_list,
			datatype_list,
			application_name );

	if ( !populate_input_chart_list_data(
			easycharts->input_chart_list,
			station_list,
			datatype_list,
			application_name,
			begin_date,
			end_date,
			delta_threshold ) )
	{
		printf(
		"<h2>Warning: nothing was selected to display.</h2>\n" );
		document_close();
		exit( 0 );
	}

	easycharts->output_chart_list =
		easycharts_timeline_get_output_chart_list(
			easycharts->input_chart_list );

	easycharts->yaxis_decimal_count =
		easycharts_get_yaxis_decimal_count(
			easycharts->output_chart_list );

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
				prompt_filename,
				(char *)0 /* where_clause */ );

} /* output_easycharts() */

void populate_input_chart_list_datatypes(
			LIST *input_chart_list,
			LIST *station_list,
			LIST *datatype_list,
			char *application_name )
{
	char y_axis_label[ 128 ];
	EASYCHARTS_INPUT_CHART *input_chart;
	EASYCHARTS_INPUT_DATATYPE *input_datatype;
	char station_datatype[ 128 ];
	char *station;
	char *datatype;
	boolean bar_graph;

	list_rewind( station_list );
	list_rewind( datatype_list );

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );

		sprintf( station_datatype,
			 "%s%c%s",
			 station,
			 STATION_DATATYPE_DELIMITER,
			 datatype );

		/* Build the chart */
		/* --------------- */
		input_chart = easycharts_new_input_chart();
		list_append_pointer(	input_chart_list,
					input_chart );

		input_chart->applet_library_code =
			EASYCHARTS_APPLET_LIBRARY_LINE_CHART;

		sprintf(y_axis_label,
			"%s (%s)",
			station_datatype,
			datatype_get_units_string(
				&bar_graph,
				application_name,
				datatype ) );
	
		input_chart->y_axis_label = strdup( y_axis_label );
	
		/* Build the datatype */
		/* ------------------ */
		input_datatype = easycharts_new_input_datatype(
					strdup( station_datatype ),
					(char *)0 /* units */ );

		list_append_pointer(	input_chart->datatype_list,
					input_datatype );

		list_next( datatype_list );
	} while( list_next( station_list ) );
} /* populate_input_chart_list_datatypes() */

boolean populate_input_chart_list_data(
			LIST *input_chart_list,
			LIST *station_list,
			LIST *datatype_list,
			char *application_name,
			char *begin_date,
			char *end_date,
			double delta_threshold )
{
	boolean got_input = 0;
	char sys_string[ 1024 ];
	char *table_name;
	char *station;
	char *datatype;
	char *where_clause;

	list_rewind( station_list );
	list_rewind( datatype_list );

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );

		where_clause = get_where_clause(
				begin_date,
				end_date,
				station,
				datatype );

		table_name = get_table_name( application_name, "measurement" );

		strcpy( sys_string,
			get_sys_string(
				(char *)0 /* output_filename */,
				table_name,
				where_clause,
				delta_threshold,
				0 /* not order_by_delta_value */ ) );

		sprintf( sys_string + strlen( sys_string ),
			 " | sed 's|%c|%c|1' | sed 's|%c|%c|2'",
			 DELIMITER,
			 STATION_DATATYPE_DELIMITER,
			 DELIMITER,
			 DATE_TIME_DELIMITER );

		if ( easycharts_set_all_input_values(
				input_chart_list,
				sys_string,
				EASYCHART_DATATYPE_PIECE,
				EASYCHART_DATE_TIME_PIECE,
				EASYCHART_VALUE_PIECE,
				DELIMITER ) )
		{
			got_input = 1;
		}

		list_next( datatype_list );
	} while( list_next( station_list ) );
	return got_input;
} /* populate_input_chart_list_data() */

void output_table(		char *application_name,
				LIST *station_list,
				LIST *datatype_list,
				char *begin_date,
				char *end_date,
				double delta_threshold )
{
	char *station;
	char *datatype;
	char *where_clause;
	char sys_string[ 1024 ];
	char *table_name;
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char *heading;
	char *justify;
	char title[ 256 ];
	LIST *done_station_name_list = list_new();

	if ( list_rewind( station_list ) )
	{
		do {
			station = list_get_pointer( station_list );

			if ( !list_exists_string(
				done_station_name_list,
				station ) )
			{
				hydrology_library_output_station_table(
					application_name,
					station );

				list_append_pointer(	done_station_name_list,
							station );
			}
		} while( list_next( station_list ) );
	}

	heading = "station,datatype,date,time,delta_value";
	justify = "left,left,left,left,right";

	printf(
"<p>Only the top/bottom %d records are displayed in delta value order.\n",
		QUEUE_LINES );
	fflush( stdout );

	list_rewind( station_list );
	list_rewind( datatype_list );

	do {
		station = list_get_pointer( station_list );
		datatype = list_get_pointer( datatype_list );

		where_clause = get_where_clause(
				begin_date,
				end_date,
				station,
				datatype );

		table_name = get_table_name( application_name, "measurement" );

		sprintf( title,
			 "%s/%s",
			 station,
			 datatype );

		sprintf( sys_string,
			 "queue_top_bottom_lines.e %d			|"
			 "html_table.e '^^%s' '%s' '%c' '%s' 		|"
			 "cat						 ",
			 QUEUE_LINES,
			 title,
			 heading,
			 DELIMITER,
			 justify );

		output_pipe = popen( sys_string, "w" );

		strcpy( sys_string,
			get_sys_string(
				(char *)0 /* output_filename */,
				table_name,
				where_clause,
				delta_threshold,
				1 /* order_by_delta_value */ ) );

		input_pipe = popen( sys_string, "r" );

		while( get_line( input_buffer, input_pipe ) )
		{
			fprintf( output_pipe, "%s\n", input_buffer );
		}

		pclose( input_pipe );
		pclose( output_pipe );

		list_next( datatype_list );
	} while( list_next( station_list ) );

} /* output_table() */

