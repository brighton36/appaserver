/* ----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/output_measurement_googlecharts.c	*/
/* ----------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* ----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "folder.h"
#include "query.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "aggregate_level.h"
#include "validation_level.h"
#include "aggregate_statistic.h"
#include "hydrology_library.h"
#include "document.h"
#include "datatype.h"
#include "google_chart.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "statistics_weighted.h"
#include "application_constants.h"
#include "appaserver_link_file.h"

/* Constants */
/* --------- */
#define SOURCE_FOLDER				"measurement"
#define DATE_PIECE				0
#define SELECT_LIST				"measurement_date,measurement_time,datatype,measurement_value"

/* Prototypes */
/* ---------- */
boolean populate_point_array(
				LIST *xaxis_list,
				LIST *google_datatype_name_list,
				char *sys_string );

void output_measurement_googlecharts(
			char *application_name,
			char *where_clause,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *station_name,
			char *datatype_name,
			int date_piece,
			char *begin_date_string,
			char *end_date_string,
			char *units,
			char *units_converted,
			char *end_date,
			char accumulate_yn,
			boolean bypass_data_collection_frequency,
			char *yaxis_label,
			boolean bar_chart,
			char *process_name,
			char *chart_title,
			char *document_root_directory );

GOOGLE_CHART *get_google_chart(
			char *application_name,
			char *where_clause,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *station_name,
			char *datatype_name,
			int date_piece,
			char *begin_date_string,
			char *end_date_string,
			char *units,
			char *units_converted,
			char *end_date,
			char accumulate_yn,
			boolean bypass_data_collection_frequency );

char *get_sys_string(	char *application_name,
			char *where_clause,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *station_name,
			char *datatype_name,
			int date_piece,
			char *begin_date_string,
			char *end_date_string,
			char *units,
			char *units_converted,
			char *end_date,
			char accumulate_yn,
			boolean bypass_data_collection_frequency );

int main( int argc, char **argv )
{
	char *application_name;
	char *process_name;
	char *begin_date, *end_date;
	char *aggregate_level_string;
	char *validation_level_string;
	char *aggregate_statistic_string;
	char *station_name;
	char *datatype_name;
	char where_clause[ 4096 ];
	char yaxis_label[ 256 ];
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	enum aggregate_level aggregate_level;
	enum validation_level validation_level;
	enum aggregate_statistic aggregate_statistic;
	char title[ 256 ];
	char sub_title[ 256 ];
	char chart_title[ 256 ];
	char *units_converted;
	char *units;
	char *accumulate_yn;
	char *database_string = {0};
	char *units_display;
	boolean bypass_data_collection_frequency;
	boolean bar_chart = 0;

	if ( argc != 12 )
	{
		fprintf( stderr, 
	"Usage: %s application process_name station datatype begin_date end_date aggregate_level validation_level aggregate_statistic units_converted accumulate_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	process_name = argv[ 2 ];
	station_name = argv[ 3 ];
	datatype_name = argv[ 4 ];
	begin_date = argv[ 5 ];
	end_date = argv[ 6 ];
	aggregate_level_string = argv[ 7 ];
	validation_level_string = argv[ 8 ];
	aggregate_statistic_string = argv[ 9 ];
	units_converted = argv[ 10 ];
	accumulate_yn = argv[ 11 ];

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

	appaserver_parameter_file = appaserver_parameter_file_new();

	hydrology_library_get_clean_begin_end_date(
		&begin_date,
		&end_date,
		application_name,
		station_name,
		datatype_name );

	if ( !appaserver_library_validate_begin_end_date(
		&begin_date,
		&end_date,
		application_name,
		appaserver_parameter_file->database_management_system,
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

	if ( !*station_name || strcmp( station_name, "station" ) == 0
	||   !*datatype_name || strcmp( datatype_name, "datatype" ) == 0 )
	{
		document_quick_output_body(	application_name,
						appaserver_parameter_file->
						appaserver_mount_point );

		printf(
		"<p>ERROR: Please select a station/datatype\n" );
		document_close();
		exit( 0 );
	}

	aggregate_level =
		aggregate_level_get_aggregate_level(
			aggregate_level_string );

	validation_level =
		validation_level_get_validation_level(
			validation_level_string);

	bypass_data_collection_frequency =
		datatype_get_bypass_data_collection_frequency(
				application_name,
				station_name,
				datatype_name );

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
				aggregate_statistic_string,
				aggregate_level );

	if ( strcmp( end_date, "end_date" ) != 0 )
	{
		sprintf( where_clause,
		 	"station = '%s' and			"
		 	"datatype = '%s' and			"
			"measurement_date between '%s' and '%s'	",
			station_name,
			datatype_name,
			begin_date,
			end_date );
	}
	else
	{
		sprintf( where_clause,
		 	"station = '%s' and			"
		 	"datatype = '%s' and			"
			"measurement_date = '%s'		",
			station_name,
			datatype_name,
			begin_date );
	}

	if ( validation_level == provisional )
	{
		strcat( where_clause,
			" and last_validation_date is null" );
	}
	else
	if ( validation_level == validated )
	{
		strcat( where_clause,
			" and last_validation_date is not null" );
	}

	hydrology_library_get_title(
				title,
				sub_title,
				validation_level,
				aggregate_statistic,
				aggregate_level,
				station_name,
				(char *)0 /* datatype */,
				begin_date,
				end_date,
				*accumulate_yn );

	sprintf( chart_title, "%s %s", title, sub_title );

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
	
/*
	document_output_body(
				document->application_name,
				document->onload_control_string );

	printf( "<h1>%s</h1><h2>%s</h2>\n", title, sub_title );
*/

	units = hydrology_library_get_units_string(
				&bar_chart,
				application_name,
				datatype_name );

	units_display =
		hydrology_library_get_datatype_units_display(
			application_name,
			datatype_name,
			units,
			units_converted,
			aggregate_statistic );

	if ( !units_display )
	{
		printf( "%s\n",
			hydrology_library_get_output_invalid_units_error(
				datatype_name,
				units,
				units_converted ) );
		document_close();
		exit( 0 );
	}

	sprintf(yaxis_label,
		"%s (%s)",
		datatype_name,
		units_display );

	format_initial_capital( yaxis_label, yaxis_label );

	output_measurement_googlecharts(
			application_name,
			where_clause,
			aggregate_level,
			aggregate_statistic,
			station_name,
			datatype_name,
			DATE_PIECE,
			begin_date,
			end_date,
			units,
			units_converted,
			end_date,
			*accumulate_yn,
			bypass_data_collection_frequency,
			yaxis_label,
			bar_chart,
			process_name,
			chart_title,
			appaserver_parameter_file->document_root );

	document_close();

	process_increment_execution_count(
				application_name,
				process_name,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

char *get_sys_string(	char *application_name,
			char *where_clause,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *station_name,
			char *datatype_name,
			int date_piece,
			char *begin_date_string,
			char *end_date_string,
			char *units,
			char *units_converted,
			char *end_date,
			char accumulate_yn,
			boolean bypass_data_collection_frequency )
{
	char sys_string[ 4096 ];
	char real_time_aggregation_process[ 1024 ];
	char accumulate_process[ 1024 ];
	char trim_time_process[ 1024 ];
	char *select_list_string;
	char units_converted_process[ 128 ];

if ( station_name ){};
if ( begin_date_string ){};
if ( end_date_string ){};
if ( bypass_data_collection_frequency ){};

	select_list_string = SELECT_LIST;

	if ( aggregate_statistic == aggregate_statistic_none )
	{
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype_name,
				aggregate_statistic );
	}

	if ( aggregate_level == real_time )
	{
		strcpy( real_time_aggregation_process, "cat" );
	}
	else
	{
		sprintf( real_time_aggregation_process, 
		"real_time2aggregate_value.e %s %d %d %d '^' %s n %s",
			 aggregate_statistic_get_string( aggregate_statistic ),
			 date_piece,
			 date_piece + 1,
			 date_piece + 3,
			 aggregate_level_get_string( aggregate_level ),
			 end_date );

		if ( aggregate_level != half_hour
		&&   aggregate_level != hourly )
		{
			sprintf( trim_time_process,
			 	"piece_inverse.e %d ','",
			 	date_piece + 1 );
		}
		else
		{
			strcpy( trim_time_process, "cat" );
		}
	}

	if ( accumulate_yn != 'y' )
	{
		strcpy( accumulate_process, "cat" );
	}
	else
	{
		sprintf( accumulate_process, 
			 "accumulate.e %d '^' replace",
			 date_piece + 3 );
	}

	if ( units_converted
	&&   *units_converted
	&&   strcmp( units_converted, "units_converted" ) != 0 )
	{
		sprintf( units_converted_process,
			 "measurement_convert_units.e %s %s %s %d '^'",
			 application_name,
			 units,
			 units_converted,
			 date_piece + 3 );
	}
	else
	{
		strcpy( units_converted_process, "cat" );
	}

/*
	if ( !bypass_data_collection_frequency )
	{
		sprintf( sys_string,
		 "get_folder_data	application=%s			  "
		 "			folder=%s			  "
		 "			select='%s'			  "
		 "			where=\"%s\"			  "
		 "			quick=yes			 |"
		 "%s							 |"
		 "%s							 |"
		 "%s							 |"
		 "pad_missing_times.e '^' 0,1,2 %s %s 0000 %s 2359 0 '%s'|"
		 "sed 's/\\^/:/1'					 |"
		 "sed 's/:null//1'					 |"
		 "cat							  ",
		 	application_name,
		 	SOURCE_FOLDER,
		 	select_list_string,
		 	where_clause,
		 	real_time_aggregation_process,
		 	accumulate_process,
		 	units_converted_process,
		 	aggregate_level_get_string( aggregate_level ),
		 	begin_date_string,
		 	end_date_string,
		 	hydrology_library_get_expected_count_list_string(
				application_name,
				station_name,
				datatype_name,
				'|' ) );
	}
	else
	{
		sprintf( sys_string,
		 "get_folder_data	application=%s			  "
		 "			folder=%s			  "
		 "			select='%s'			  "
		 "			where=\"%s\"			  "
		 "			quick=yes			 |"
		 "%s							 |"
		 "%s							 |"
		 "%s							 |"
		 "sed 's/\\^/:/1'					 |"
		 "sed 's/:null//1'					 |"
		 "cat							  ",
		 	application_name,
		 	SOURCE_FOLDER,
		 	select_list_string,
		 	where_clause,
		 	real_time_aggregation_process,
		 	accumulate_process,
		 	units_converted_process );
	}
*/

	sprintf( sys_string,
		 "get_folder_data	application=%s			  "
		 "			folder=%s			  "
		 "			select='%s'			  "
		 "			where=\"%s\"			  "
		 "			quick=yes			 |"
		 "%s							 |"
		 "%s							 |"
		 "%s							 |"
		 "sed 's/\\^/:/1'					 |"
		 "sed 's/:null//1'					 |"
		 "cat							  ",
		 	application_name,
		 	SOURCE_FOLDER,
		 	select_list_string,
		 	where_clause,
		 	real_time_aggregation_process,
		 	accumulate_process,
		 	units_converted_process );

	return strdup( sys_string );

} /* get_sys_string() */

boolean populate_point_array(
				LIST *xaxis_list,
				LIST *google_datatype_name_list,
				char *sys_string )
{
	char input_buffer[ 1024 ];
	FILE *input_pipe;
	boolean got_one = 0;

fprintf( stderr, "%s/%s()/%d: sys_string = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
sys_string );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
fprintf( stderr, "%s/%s()/%d: input_buffer = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
input_buffer );
		got_one = 1;

		google_chart_set_point_string(
				xaxis_list,
				google_datatype_name_list,
				input_buffer,
				FOLDER_DATA_DELIMITER );

#ifdef NOT_DEFINED
	char xaxis_label[ 128 ];
	char point_string[ 128 ];
	double total = 0.0;

		if ( bar_chart )
		{
			total += atof( point_string );

			google_chart_set_point(
				xaxis_list,
				google_datatype_name_list,
				xaxis_label,
				(char *)0 /* hhmm */,
				DATATYPE_QUANTUM_TOTAL,
				total );
		}
		piece(	xaxis_label, 
			FOLDER_DATA_DELIMITER,
			input_buffer, 
			0 );

		piece(	point_string, 
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );
#endif
	}

	pclose( input_pipe );

	return got_one;

} /* populate_point_array() */

GOOGLE_CHART *get_google_chart(
			char *application_name,
			char *where_clause,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *station_name,
			char *datatype_name,
			int date_piece,
			char *begin_date_string,
			char *end_date_string,
			char *units,
			char *units_converted,
			char *end_date,
			char accumulate_yn,
			boolean bypass_data_collection_frequency )
{
	GOOGLE_CHART *google_chart;
	char *sys_string;

if ( date_piece ){};

	google_chart =
		google_chart_new(
				 google_time_line,
				 "Date" /* xaxis_datatype_name */,
				 GOOGLE_CHART_POSITION_LEFT,
				 GOOGLE_CHART_POSITION_TOP,
				 GOOGLE_CHART_WIDTH,
				 GOOGLE_CHART_HEIGHT,
				 GOOGLE_CHART_BACKGROUND_COLOR,
				 0 /* not legend_position_bottom */,
				"annotatedtimeline" /* google_package_name */);

	sys_string = get_sys_string(	
			application_name,
			where_clause,
			aggregate_level,
			aggregate_statistic,
			station_name,
			datatype_name,
			DATE_PIECE,
			begin_date_string,
			end_date_string,
			units,
			units_converted,
			end_date,
			accumulate_yn,
			bypass_data_collection_frequency );

	list_append_pointer(	google_chart->google_datatype_name_list,
				datatype_name );

	if ( !populate_point_array(
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				sys_string ) )
	{
		return (GOOGLE_CHART *)0;;
	}

	return google_chart;

} /* get_google_chart() */

void output_measurement_googlecharts(
			char *application_name,
			char *where_clause,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *station_name,
			char *datatype_name,
			int date_piece,
			char *begin_date_string,
			char *end_date_string,
			char *units,
			char *units_converted,
			char *end_date,
			char accumulate_yn,
			boolean bypass_data_collection_frequency,
			char *yaxis_label,
			boolean bar_chart,
			char *process_name,
			char *chart_title,
			char *document_root_directory )
{
	GOOGLE_CHART *google_chart;
	char *output_filename;
	char *prompt_filename;
	FILE *output_file;

if ( bar_chart ){};

	appaserver_link_get_pid_filename(
			&output_filename,
			&prompt_filename,
			application_name,
			document_root_directory,
			getpid(),
			process_name,
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

	google_chart =
		get_google_chart(
			application_name,
			where_clause,
			aggregate_level,
			aggregate_statistic,
			station_name,
			datatype_name,
			date_piece,
			begin_date_string,
			end_date_string,
			units,
			units_converted,
			end_date,
			accumulate_yn,
			bypass_data_collection_frequency );

	google_chart_output_include( output_file );

	google_chart_output_draw_visualization_function(
				output_file,
				google_chart->google_chart_type,
				google_chart->xaxis_datatype_name,
				google_chart->xaxis_list,
				google_chart->google_datatype_name_list,
				chart_title,
				yaxis_label,
				google_chart->left,
				google_chart->top,
				google_chart->width,
				google_chart->height,
				google_chart->background_color,
				google_chart->legend_position_bottom,
				0 /* not chart_type_bar */,
				google_chart->google_package_name,
				0 /* not dont_display_range_selector */ );

	fclose( output_file );

	google_chart_output_prompt(
		application_name,
		prompt_filename,
		process_name,
		where_clause );

} /* output_measurement_googlecharts() */

