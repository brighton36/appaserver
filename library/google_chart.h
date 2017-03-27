/* $APPASERVER_HOME/library/google_chart.h */
/* --------------------------------------- */

#ifndef GOOGLE_CHART_H
#define GOOGLE_CHART_H

#include <stdio.h>
#include "list.h"

/* Constants */
/* --------- */
#define GOOGLE_CHART_POSITION_TOP		10
#define GOOGLE_CHART_POSITION_LEFT		10
#define GOOGLE_CHART_WIDTH			800
#define GOOGLE_CHART_HEIGHT			500
#define GOOGLE_CHART_BACKGROUND_COLOR		"#effdff"
#define GOOGLE_CHART_CONVERT_MONTH_EXPRESSION	atoi( month ) - 1

/* Enumerations */
/* ------------ */
enum google_chart_type{	google_column_chart,
			google_time_line,
			google_cat_whiskers };

/* Structures */
/* ---------- */
typedef struct
{
	char *xaxis_label;
	char *hhmm;
	double **point_array;
} GOOGLE_CHART_XAXIS;

typedef struct
{
	enum google_chart_type google_chart_type;
	char *xaxis_datatype_name;
	int left;
	int top;
	int width;
	int height;
	char *background_color;
	boolean legend_position_bottom;
	LIST *xaxis_list;
	LIST *google_datatype_name_list;
	char *google_package_name;
} GOOGLE_CHART;

/* Prototypes */
/* ---------- */
GOOGLE_CHART *google_chart_new(
					enum google_chart_type,
					char *xaxis_datatype_name,
					int left,
					int top,
					int width,
					int height,
					char *background_color,
					boolean legend_position_bottom,
					char *google_package_name );

GOOGLE_CHART_XAXIS *google_chart_xaxis_new(
					char *xaxis_label,
					char *hhmm,
					int length_datatype_name_list );

GOOGLE_CHART_XAXIS *google_chart_append_xaxis(
					LIST *xaxis_list,
					char *xaxis_label,
					char *hhmm,
					int length_datatype_name_list );

void google_chart_set_point(		LIST *xaxis_list,
					LIST *google_datatype_name_list,
					char *xaxis_label,
					char *hhmm,
					char *datatype_name,
					double point );

void google_chart_set_point_string(	LIST *xaxis_list,
					LIST *google_datatype_name_list,
					char *delimited_string,
					char delimiter );

GOOGLE_CHART_XAXIS *google_chart_get_or_set_xaxis(
					LIST *xaxis_list,
					char *xaxis_label,
					char *hhmm,
					int length_datatype_name_list );

int google_chart_get_datatype_offset(
					LIST *google_datatype_name_list,
					char *datatype_name );

void google_chart_display(		char *xaxis_datatype_name,
					LIST *xaxis_list,
					LIST *google_datatype_name_list );

void google_chart_output_include(	FILE *output_file );

void google_chart_output_draw_visualization_function(
				FILE *output_file,
				enum google_chart_type google_chart_type,
				char *xaxis_datatype_name,
				LIST *xaxis_list,
				LIST *google_datatype_name_list,
				char *title,
				char *yaxis_label,
				int left,
				int top,
		 		int width,
		 		int height,
		 		char *background_color,
				boolean legend_position_bottom,
				boolean chart_type_bar,
				char *google_package_name,
				boolean dont_display_range_selector );

void google_chart_append_delimited_string_xaxis(
				LIST *xaxis_list,
				char *comma_delimited_string,
				int length_datatype_name_list );

char *google_chart_convert_date(char *destination,
				char *yyyy_mm_dd,
				char *hhmm );

void google_chart_output_prompt(
				char *application_name,
				char *prompt_filename,
				char *process_name,
				char *where_clause );

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
				char *legend_position_bottom_string );

#endif
