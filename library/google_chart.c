/* $APPASERVER_HOME/library/google_chart.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "date.h"
#include "google_chart.h"
#include "application.h"
#include "appaserver_library.h"
#include "appaserver_link_file.h"

GOOGLE_CHART *google_chart_new(
				enum google_chart_type google_chart_type,
				int left,
				int top,
				int width,
				int height,
				char *background_color,
				boolean legend_position_bottom,
				char *google_package_name )
{
	GOOGLE_CHART *google_chart;
	static int chart_number = 0;

	google_chart =
		(GOOGLE_CHART *)
			calloc( 1, sizeof( GOOGLE_CHART ) );

	google_chart->google_chart_type = google_chart_type;
	google_chart->left = left;
	google_chart->top = top;
	google_chart->width = width;
	google_chart->height = height;
	google_chart->background_color = background_color;
	google_chart->legend_position_bottom = legend_position_bottom;
	google_chart->barchart_list = list_new();
	google_chart->timeline_list = list_new();
	google_chart->datatype_name_list = list_new();
	google_chart->google_package_name = google_package_name;
	google_chart->chart_number = ++chart_number;

	return google_chart;

} /* google_chart_new() */

GOOGLE_BARCHART *google_barchart_new(
				char *datatype_name,
				int length_datatype_name_list )
{
	GOOGLE_BARCHART *google_barchart;

	google_barchart =
		(GOOGLE_BARCHART *)
			calloc( 1, sizeof( GOOGLE_BARCHART ) );

	if ( !google_barchart )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	google_barchart->datatype_name = datatype_name;

	google_barchart->point_array =
		calloc( length_datatype_name_list, sizeof( double * ) );

	return google_barchart;

} /* google_barchart_new() */

GOOGLE_TIMELINE *google_timeline_new(
				char *date_string,
				char *time_hhmm,
				int length_datatype_name_list )
{
	GOOGLE_TIMELINE *google_timeline;

	google_timeline =
		(GOOGLE_TIMELINE *)
			calloc( 1, sizeof( GOOGLE_TIMELINE ) );

	if ( !google_timeline )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	google_timeline->date_string = date_string;
	google_timeline->time_hhmm = time_hhmm;

	google_timeline->point_array =
		calloc( length_datatype_name_list, sizeof( double * ) );

	return google_timeline;

} /* google_timeline_new() */

GOOGLE_BARCHART *google_barchart_append(
				LIST *barchart_list,
				char *datatype_name,
				int length_datatype_name_list )
{
	GOOGLE_BARCHART *barchart;

	barchart = google_barchart_new(
				datatype_name,
				length_datatype_name_list );

	list_append_pointer( barchart_list, barchart );

	return barchart;

} /* google_barchart_append() */

GOOGLE_TIMELINE *google_timeline_append(
				LIST *timeline_list,
				char *date_string,
				char *time_hhmm,
				int length_datatype_name_list )
{
	GOOGLE_TIMELINE *timeline;

	timeline = google_timeline_new(
				date_string,
				time_hhmm,
				length_datatype_name_list );

	list_append_pointer( timeline_list, timeline );

	return timeline;

} /* google_timeline_append() */

int google_chart_get_datatype_offset(
				LIST *datatype_name_list,
				char *datatype_name )
{
	int offset = 0;

	if ( !list_rewind( datatype_name_list ) ) return -1;

	do {
		if ( timlib_strcmp(
				list_get_pointer( datatype_name_list ),
				datatype_name ) == 0 )
		{
			return offset;
		}
		offset++;
	} while( list_next( datatype_name_list ) );

	return -1;

} /* google_chart_get_datatype_offset() */

GOOGLE_BARCHART *google_barchart_get_or_set(
				LIST *barchart_list,
				char *datatype_name,
				int length_datatype_name_list )
{
	GOOGLE_BARCHART *barchart;

	if ( list_rewind( barchart_list ) )
	{
		do {
			barchart = list_get_pointer( barchart_list );

			if ( timlib_strcmp(	barchart->datatype_name,
						datatype_name ) == 0 )
			{
				return barchart;
			}

		} while( list_next( barchart_list ) );
	}

	return google_barchart_append(
			barchart_list,
			strdup( datatype_name ),
			length_datatype_name_list );

} /* google_barchart_get_or_set() */

GOOGLE_TIMELINE *google_timeline_get_or_set(
				LIST *timeline_list,
				char *date_string,
				char *time_hhmm,
				int length_datatype_name_list )
{
	GOOGLE_TIMELINE *timeline;

	if ( list_rewind( timeline_list ) )
	{
		do {
			timeline = list_get_pointer( timeline_list );

			if ( timlib_strcmp(	timeline->date_string,
						date_string ) == 0 )
			{
				if ( !time_hhmm || !*time_hhmm )
					return timeline;

				if ( timlib_strcmp(
					timeline->time_hhmm,
					time_hhmm ) == 0 )
				{
					return timeline;
				}
			}

		} while( list_next( timeline_list ) );
	}

	return google_timeline_append(
			timeline_list,
			strdup( date_string ),
			(time_hhmm) ? strdup( time_hhmm ) : (char *)0,
			length_datatype_name_list );

} /* google_timeline_get_or_set() */

void google_chart_set_point_string(	LIST *xaxis_list,
					LIST *google_datatype_name_list,
					char *delimited_string,
					char delimiter )
{
	char xaxis_label[ 128 ];
	char datatype_name[ 128 ];
	char point_string[ 128 ];
	char hhmm[ 128 ];
	char buffer[ 128 ];

	if ( character_count( delimiter, delimited_string ) < 2 )
	{
		fprintf( stderr,
"warning in %s/%s()/%d: not 2 delimiters (%c) in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 delimiter,
			 delimited_string );
		return;
	}

	piece( xaxis_label, delimiter, delimited_string, 0 );

	if ( character_count( ':', xaxis_label ) == 1 )
	{
		piece( hhmm, ':', xaxis_label, 1 );

		if ( strlen( hhmm ) != 4 )
		{
			*hhmm = '\0';
		}
		else
		{
			timlib_strcpy( buffer, xaxis_label, 128 );
			piece( xaxis_label, ':', buffer, 0 );
		}
	}
	else
	{
		*hhmm = '\0';
	}

	piece( datatype_name, delimiter, delimited_string, 1 );
	piece( point_string, delimiter, delimited_string, 2 );

	google_chart_set_point(	xaxis_list,
				google_datatype_name_list,
				xaxis_label,
				hhmm,
				datatype_name,
				atof( point_string ) );

} /* google_chart_set_point_string() */

void google_chart_set_point(	LIST *xaxis_list,
				LIST *google_datatype_name_list,
				char *xaxis_label,
				char *hhmm,
				char *datatype_name,
				double point )
{
	GOOGLE_CHART_XAXIS *xaxis;
	int offset;

	xaxis = google_chart_get_or_set_xaxis(
			xaxis_list,
			xaxis_label,
			hhmm,
			list_length( google_datatype_name_list ) );

	if ( ( offset = google_chart_get_datatype_offset(
			google_datatype_name_list,
			datatype_name ) ) < 0 )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get datatype_name = %s.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			datatype_name );
		exit( 1 );
	}

	xaxis->point_array[ offset ] = calloc( 1, sizeof( double ) );
	*xaxis->point_array[ offset ] = point;

} /* google_chart_set_point() */

void google_chart_display(	LIST *xaxis_list,
				LIST *google_datatype_name_list )
{
	GOOGLE_CHART_XAXIS *xaxis;
	int offset;
	int length_datatype_name_list;

	length_datatype_name_list = list_length( google_datatype_name_list );

	fprintf(stderr,
		"google_datatype_name_list = %s\n",
		list_display( google_datatype_name_list ) );

	if ( !list_rewind( xaxis_list ) ) return;

	do {
		xaxis = list_get_pointer( xaxis_list );
		fprintf( stderr, "xaxis_label = %s", xaxis->xaxis_label );
		fprintf( stderr, ",hhmm = %s", xaxis->hhmm );

		for( offset = 0; offset < length_datatype_name_list; offset++ )
		{
			if ( xaxis->point_array[ offset ] )
			{
				fprintf(stderr,
					", *point_array[%d] = %.3lf",
					offset,
					*xaxis->point_array[ offset ] );
			}
			else
			{
				fprintf(stderr,
					", *point_array[%d] is empty",
					offset );
			}
		}
		fprintf( stderr, "\n" );

	} while( list_next( xaxis_list ) );

} /* google_chart_display() */

void google_chart_output_include( FILE *output_file )
{
	fprintf( output_file,
"<script type=\"text/javascript\" src=\"https://www.google.com/jsapi\"></script>\n" );
}

void google_chart_append_delimited_string_xaxis(
			LIST *xaxis_list,
			char *comma_delimited_string,
			int length_datatype_name_list )
{
	char xaxis_label[ 128 ];
	int i = 0;

	while( piece( xaxis_label, ',', comma_delimited_string, i++ ) )
	{
		google_chart_append_xaxis(
			xaxis_list,
			strdup( xaxis_label ),
			(char *)0 /* hhmm */,
			length_datatype_name_list );
	}

} /* google_chart_append_delimited_string_xaxis() */

char *google_chart_convert_date(	char *destination,
					char *yyyy_mm_dd,
					char *hhmm )
{
	char year[ 16 ];
	char month[ 16 ];
	char day[ 16 ];
	int hours = 0;
	int minutes = 0;

	if ( character_count( '-', yyyy_mm_dd ) != 2 )
	{
		strcpy( destination, yyyy_mm_dd );
		return destination;
	}

	piece( year, '-', yyyy_mm_dd, 0 );
	piece( month, '-', yyyy_mm_dd, 1 );
	piece( day, '-', yyyy_mm_dd, 2 );

	date_time_parse(	&hours,
				&minutes,
				hhmm );

	sprintf(	destination,
			"new Date(%s,%d,%s,%d,%d,0)",
			year,
			GOOGLE_CHART_CONVERT_MONTH_EXPRESSION,
			day,
			hours,
			minutes );

	return destination;

} /* google_chart_convert_date() */

void google_chart_output_visualization_function(
				FILE *output_file,
				enum google_chart_type google_chart_type,
				LIST *xaxis_list,
				LIST *google_datatype_name_list,
				char *title,
				char *yaxis_label,
		 		int width,
		 		int height,
		 		char *background_color,
				boolean legend_position_bottom,
				boolean chart_type_bar,
				char *google_package_name,
				boolean dont_display_range_selector,
				enum aggregate_level aggregate_level,
				int chart_number )
{
	GOOGLE_CHART_XAXIS *xaxis;
	int offset;
	int length_datatype_name_list;
	char *datatype_name;
	char buffer[ 1024 ];
	char *chart_type_string;
	char *legend_position_bottom_string;
	char *google_chart_instantiation;
	char *first_column_datatype;
	char draw_visualization_function_name[ 128 ];

	sprintf( draw_visualization_function_name,
		 "drawVisualization%d",
		 chart_number );

	if ( ! ( length_datatype_name_list =
			list_length( google_datatype_name_list ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty google_datatype_name_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( !google_package_name )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: google_package_name is null.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	if ( timlib_strcmp(	google_package_name,
				GOOGLE_ANNOTATED_TIMELINE ) == 0 )
	{
		if ( aggregate_level == aggregate_level_none
		||   aggregate_level == real_time
		||   aggregate_level == half_hour
		||   aggregate_level == hourly )
		{
			first_column_datatype = "datetime";
		}
		else
		{
			first_column_datatype = "date";
		}
	}
	else
	{
		first_column_datatype = "string";
	}

	fprintf( output_file,
"<script type=\"text/javascript\">\n"
"google.load('visualization', '1', {packages: ['%s']});\n",
		 google_package_name );

	fprintf( output_file,
"function %s()\n"
"{\n"
"	var data = new google.visualization.DataTable();\n"
"\n"
"	data.addColumn('%s', '');\n",
		draw_visualization_function_name,
		first_column_datatype );

	offset = 0;
	list_rewind( google_datatype_name_list );

	do {
		datatype_name = list_get_pointer( google_datatype_name_list );

		fprintf( output_file,
"	data.addColumn('number', '%s');\n",
		 	format_initial_capital( buffer, datatype_name ) );

		if ( google_chart_type == google_cat_whiskers
		&&   ( offset % 3 ) == 1 )
		{
			fprintf(
			output_file,
			"	data.addColumn('number', '');\n" );
		}

		offset++;

	} while( list_next( google_datatype_name_list ) );

	if ( !list_rewind( xaxis_list ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: empty xaxis_list.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fprintf( output_file,
"	data.addRows([\n" );

	do {
		xaxis = list_get_pointer( xaxis_list );

		if ( strcmp(	google_package_name,
				GOOGLE_ANNOTATED_TIMELINE ) == 0 )
		{
			fprintf(output_file,
			 	"\t\t[%s",
			 	google_chart_convert_date(
					buffer,
					xaxis->xaxis_label,
					xaxis->hhmm ) );
		}
		else
		{
			fprintf(output_file,
			 	"\t\t['%s'",
			 	format_initial_capital(
					buffer,
					xaxis->xaxis_label ) );
		}

		for( offset = 0; offset < length_datatype_name_list; offset++ )
		{
			if ( xaxis->point_array[ offset ] )
			{
				fprintf( output_file,
				 	",%.3lf",
				 	*xaxis->point_array[ offset ] );
			}
			else
			{
				fprintf( output_file,
				 	",undefined" );
			}

			if ( google_chart_type == google_cat_whiskers
			&&   ( offset % 3 ) == 1 )
			{
				if ( xaxis->point_array[ offset ] )
				{
					fprintf( output_file,
			 			",%.3lf",
			 			*xaxis->point_array[
							offset ] );
				}
				else
				{
					fprintf( output_file,
			 			",undefined" );
				}
			}
		}

		fprintf( output_file, "]" );

		if ( !list_at_end( xaxis_list ) )
			fprintf( output_file, ",\n" );
		else
			fprintf( output_file, "\n" );

	} while( list_next( xaxis_list ) );

	fprintf( output_file,
"	]);\n\n" );

	if ( chart_type_bar )
		chart_type_string = "seriesType: \"bars\"";
	else
		chart_type_string = "type: \"line\"";

	if ( legend_position_bottom )
		legend_position_bottom_string = "bottom";
	else
		legend_position_bottom_string = "";

	google_chart_output_options(
		output_file,
		title,
		google_chart_type,
		chart_type_string,
		dont_display_range_selector,
		yaxis_label,
		width,
		height,
		background_color,
		legend_position_bottom_string );

	if ( google_chart_type == google_column_chart )
		google_chart_instantiation = "ColumnChart";
	else
	if ( google_chart_type == google_time_line )
		google_chart_instantiation = "AnnotatedTimeLine";
	else
	if ( google_chart_type == google_cat_whiskers )
		google_chart_instantiation = "CandlestickChart";
	else
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: unknown google_chart_type = %d.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 google_chart_type );
		exit( 1 );
	}

	fprintf( output_file,
"	var chart = new google.visualization.%s(\n"
"		document.getElementById('chart_div%d'));\n"
"	chart.draw(data, options);\n"
"}\n",
		google_chart_instantiation,
		chart_number );

	fprintf( output_file,
"google.setOnLoadCallback(%s);\n"
"</script>\n",
		 draw_visualization_function_name );

/*
	if ( title
	&&   *title
	&&   strcmp(	google_package_name,
			GOOGLE_ANNOTATED_TIMELINE ) == 0 )
	{
		fprintf( output_file,
"<div style=\"	position: absolute;	\n"
"		left: %dpx;		\n"
"		top: %dpx;		\n"
"		font-size: 16px\">	\n"
"%s</div>\n",
			 left,
			 top,
			 title );
		top += 20;
	}

	fprintf( output_file,
"<div style=\"position: absolute;\n"
"	left: %dpx;\n"
"	top: %dpx;\n"
"	width: %dpx;\n"
"	border-width: thin;\n"
"	border-style: solid;\n"
"	border-color: teal\" >\n"
"	<div id=\"chart_div%d\" style=\"width: %dpx; height: %dpx\"></div>\n"
"</div>\n",
		 left,
		 top,
		 width,
		 name_key,
		 width,
		 height );
*/

} /* google_chart_output_visualization_function() */

void google_chart_output_body(	FILE *output_file,
				char *title,
				char *google_package_name,
				int left,
				int top,
				int width,
				int height,
				int chart_number )
{
	if ( title
	&&   *title
	&&   strcmp(	google_package_name,
			GOOGLE_ANNOTATED_TIMELINE ) == 0 )
	{
		fprintf( output_file,
"<div style=\"	position: absolute;	\n"
"		left: %dpx;		\n"
"		top: %dpx;		\n"
"		font-size: 16px\">	\n"
"%s</div>\n",
			 left,
			 top,
			 title );
		top += 20;
	}

	fprintf( output_file,
"<div style=\"position: absolute;\n"
"	left: %dpx;\n"
"	top: %dpx;\n"
"	width: %dpx;\n"
"	border-width: thin;\n"
"	border-style: solid;\n"
"	border-color: teal\" >\n"
"	<div id=\"chart_div%d\" style=\"width: %dpx; height: %dpx\"></div>\n"
"</div>\n",
		 left,
		 top,
		 width,
		 chart_number,
		 width,
		 height );

} /* google_chart_output_body() */

void google_chart_output_prompt(
			char *application_name,
			char *prompt_filename,
			char *process_name,
			char *where_clause )
{
	if ( application_name )
	{
		printf(
"<body bgcolor=\"%s\" onload=\"window.open('%s','%s');\">\n",
			application_get_background_color( application_name ),
			prompt_filename,
			process_name );
	}

	printf( "<h1>Google Chart Viewer " );
	fflush( stdout );
	system( "date '+%x %H:%M'" );
	printf( "</h1>\n" );

	if ( where_clause && *where_clause )
		printf( "<br>Search criteria: %s\n", where_clause );

	printf( "<br><hr><a href=\"%s\" target=%s>Press to view chart.</a>\n",
		prompt_filename,
		process_name );

} /* google_chart_output_prompt() */

void google_chart_output_options(
			FILE *output_file,
			char *title,
			enum google_chart_type google_chart_type,
			char *chart_type_string,
			boolean dont_display_range_selector,
			char *yaxis_label,
			int width,
			int height,
			char *background_color,
			char *legend_position_bottom_string )
{
	boolean got_one = 0;

	fprintf( output_file,
"	var options = {\n" );

	if ( chart_type_string && *chart_type_string )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		%s",
			 chart_type_string );

		got_one = 1;
	}

	if ( dont_display_range_selector )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		displayRangeSelector: false,\n"
"		displayZoomButtons: false" );

		got_one = 1;
	}

	if ( yaxis_label && *yaxis_label )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		vAxis: {title: \"%s\"}",
			 yaxis_label );

		got_one = 1;

	}

	if ( title && *title )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		title: \"%s\"",
			 title );

		got_one = 1;
	}

	if ( width )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		width: %d",
			 width );

		got_one = 1;
	}

	if ( height )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		height: %d",
			 height );

		got_one = 1;
	}

	if ( background_color && *background_color )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		backgroundColor: \"%s\"",
			 background_color );

		got_one = 1;
	}

	if ( legend_position_bottom_string && *legend_position_bottom_string )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		legend: { position: \"%s\" }",
			 legend_position_bottom_string );

		got_one = 1;
	}

	if ( google_chart_type == google_cat_whiskers )
	{
		if ( got_one ) fprintf( output_file, ",\n" );

		fprintf( output_file,
"		series: [\n"
"		{color: 'blue',	visibleInLegend: true},\n"
"		{color: 'red',	visibleInLegend: true}\n"
"			]" );

		got_one = 1;
	}

	fprintf( output_file, "\n}\n" );

} /* google_chart_output_options() */

