/* ---------------------------------------------------	*/
/* output_measurement_easycharts_timeline.c		*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

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
#include "easycharts.h"
#include "appaserver_parameter_file.h"
#include "environ.h"
#include "statistics_weighted.h"

/* Constants */
/* --------- */
#define PROCESS_NAME				"easychart_measurements"
#define SOURCE_FOLDER				"measurement"
#define DATE_PIECE				0
#define TIME_PIECE				3
#define SELECT_LIST				"measurement_date,measurement_time,measurement_value"

/* Prototypes */
/* ---------- */
char *get_sys_string(	char *application_name,
			char *where_clause,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *datatype_name,
			int date_piece,
			int time_piece,
			char *appaserver_mount_point,
			char *end_date );

int main( int argc, char **argv )
{
	char *person, *session, *application_name;
	char *begin_date, *end_date;
	char *aggregate_level_string;
	char *validation_level_string;
	char *aggregate_statistic_string;
	char *station_name;
	char *datatype_name;
	char where_clause[ 4096 ];
	char *sys_string;
	DOCUMENT *document;
	APPASERVER_PARAMETER_FILE *appaserver_parameter_file;
	enum aggregate_level aggregate_level;
	enum validation_level validation_level;
	enum aggregate_statistic aggregate_statistic;
	EASYCHARTS *easycharts;
	FILE *input_pipe;
	char *chart_filename;
	char *prompt_filename;
	FILE *chart_file;
	char applet_library_archive[ 128 ];
	EASYCHARTS_OUTPUT_DATASET *output_dataset;
	char input_buffer[ 128 ];
	char date_string[ 32 ];
	char time_string[ 32 ];
	char value_string[ 64 ];
	char x_label[ 64 ];
	int first_time = 1;
	char *lowest_date_string = {0};
	char ddmmyyyy_date_string[ 32 ];
	char *database_string = {0};
				
	if ( argc != 11 )
	{
		fprintf( stderr, 
	"Usage: %s person session application station datatype begin_date end_date aggregate_level validation_level aggregate_statistic\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	person = argv[ 1 ];
	session = argv[ 2 ];
	application_name = argv[ 3 ];
	station_name = argv[ 4 ];
	datatype_name = argv[ 5 ];
	begin_date = argv[ 6 ];
	end_date = argv[ 7 ];
	aggregate_level_string = argv[ 8 ];
	validation_level_string = argv[ 9 ];
	aggregate_statistic_string = argv[ 10 ];

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

	hydrology_library_get_clean_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					station_name,
					datatype_name,
					appaserver_parameter_file->
						database_management_system );

	if ( !appaserver_library_validate_begin_end_date(
					&begin_date,
					&end_date,
					application_name,
					appaserver_parameter_file->
						database_management_system,
					(PROCESS_GENERIC_OUTPUT *)0,
					(DICTIONARY *)0 /* post_dictionary */,
					appaserver_parameter_file->
						appaserver_mount_point ) )
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
		aggregate_level_get_aggregate_level( aggregate_level_string );

	validation_level =
		validation_level_get_validation_level( validation_level_string);

	aggregate_statistic =
		aggregate_statistic_get_aggregate_statistic(
				aggregate_statistic_string );

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

	strcat( where_clause, " and measurement_value is not null" );

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

	easycharts_get_chart_filename(
			&chart_filename,
			&prompt_filename,
			application_name,
			appaserver_parameter_file->
				appaserver_mount_point,
			getpid(),
			( application_get_prepend_http_protocol_yn(
				application_name ) == 'y' ) );

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

	easycharts = easycharts_new_easycharts( xy );

	easycharts->applet_library_code =
		"com.objectplanet.chart.ext.TimeLineChartApplet";

	sprintf(applet_library_archive,
		"/%s/%s",
		application_name,
		"chart.ext.jar" );

	easycharts->applet_library_archive = applet_library_archive;

	easycharts->title = "Measurements";
	easycharts->legend_on = 0;
	easycharts->bold_labels = 0;
	easycharts->bold_legends = 0;
	easycharts->set_y_lower_range = 1;
	easycharts->sample_scroller_on = 1;
	easycharts->range_scroller_on = 1;
	easycharts->label_parameter_name = "timePlots";

	easycharts->output_dataset_list = list_new_list();
	output_dataset = easycharts_new_output_dataset( datatype_name );
	list_append_pointer( easycharts->output_dataset_list, output_dataset );

	sys_string = get_sys_string(	
			application_name,
			where_clause,
			aggregate_level,
			aggregate_statistic,
			datatype_name,
			DATE_PIECE,
			TIME_PIECE,
			appaserver_parameter_file->appaserver_mount_point,
			end_date );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( date_string, '^', input_buffer, 0 );

		timlib_mysql_date2ddmmyyyy(
			ddmmyyyy_date_string,
			date_string );

		if ( aggregate_level == real_time
		||   aggregate_level == half_hour
		||   aggregate_level == hourly )
		{
			piece( time_string, '^', input_buffer, 1 );
			sprintf(ddmmyyyy_date_string +
				strlen( ddmmyyyy_date_string ),
				":%c%c:%c%c",
				*time_string,
				*(time_string + 1),
				*(time_string + 2),
				*(time_string + 3 ) );
		}

		piece( value_string, '^', input_buffer, 2 );
		sprintf( x_label, "%s|%s", ddmmyyyy_date_string, value_string );
		easycharts_set_point(
				output_dataset->point_list,
				strdup( x_label ),
				0.0,
				atof( value_string ) );

		if ( first_time )
		{
			lowest_date_string =
				strdup( ddmmyyyy_date_string );
			first_time = 0;
		}

	}

	pclose( input_pipe );

	if ( aggregate_level == real_time
	||   aggregate_level == half_hour
	||   aggregate_level == hourly )
	{
		easycharts->xy->timeline_info =
			easycharts_new_timeline_info(
				"MM/dd/yyyy:HH:mm",
				"yyyy-MM-dd:HHmm",
				lowest_date_string,
				ddmmyyyy_date_string );
	}
	else
	{
		easycharts->xy->timeline_info =
			easycharts_new_timeline_info(
				"MM/dd/yyyy",
				"yyyy-MM-dd",
				lowest_date_string,
				ddmmyyyy_date_string );
	}

	easycharts_output(
			chart_file,
			easycharts->output_dataset_list,
			easycharts->xy->highlight_on,
			easycharts->xy->highlight_style,
			easycharts->xy->point_highlight_size,
			easycharts->xy->series_labels,
			easycharts->xy->series_line_off,
			easycharts->applet_library_code,
			easycharts->applet_library_archive,
			easycharts->width,
			easycharts->height,
			easycharts->title,
			easycharts->xy->x_axis_label,
			easycharts->xy->y_axis_label,
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
			easycharts->bar_labels_on,
			easycharts->label_parameter_name,
			easycharts->xy->timeline_info,
			0 /* dont include_sample_series_output */,
			1 /* output_multiple_labels */ );

	easycharts_output_html( chart_file );

	fclose( chart_file );

	printf(
"<body bgcolor=\"%s\" onload=\"window.open('%s','easychart_measurements');\">\n",
		application_get_background_color( application_name ),
		prompt_filename );

	printf( "<h1>Chart Viewer " );
	fflush( stdout );
	system( "date '+%x %H:%M'" );
	printf( "</h1>\n" );

	document_close();
	process_increment_execution_count(
				application_name,
				PROCESS_NAME,
				appaserver_parameter_file_get_dbms() );
	return 0;
} /* main() */

char *get_sys_string(	char *application_name,
			char *where_clause,
			enum aggregate_level aggregate_level,
			enum aggregate_statistic aggregate_statistic,
			char *datatype_name,
			int date_piece,
			int time_piece,
			char *appaserver_mount_point,
			char *end_date )
{
	char sys_string[ 4096 ];
	char intermediate_process[ 1024 ];
	char trim_time_process[ 1024 ];
	char *select_list_string;

	select_list_string = SELECT_LIST;

	if ( aggregate_statistic == none )
	{
		aggregate_statistic =
			based_on_datatype_get_aggregate_statistic(
				application_name,
				datatype_name,
				aggregate_statistic );
	}

	if ( aggregate_level == real_time )
	{
		strcpy( intermediate_process, "cat" );
	}
	else
	{
		sprintf( intermediate_process, 
			 "real_time2aggregate_value.e %s %d %d %d '^' %s n %s",
			 aggregate_statistic_get_string( aggregate_statistic ),
			 date_piece,
			 date_piece + 1,
			 date_piece + 2,
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

	sprintf( sys_string,
		 "get_folder_data	application=%s			  "
		 "			folder=%s			  "
		 "			select='%s'			  "
		 "			where=\"%s\"			 |"
		 "%s							 |"
		 "cat							  ",
		 application_name,
		 SOURCE_FOLDER,
		 select_list_string,
		 where_clause,
		 intermediate_process );

	return strdup( sys_string );

} /* get_sys_string() */

