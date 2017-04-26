/* $APPASERVER_HOME/library/google_chart.h		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */


#ifndef GOOGLE_CHART_H
#define GOOGLE_CHART_H

#include <stdio.h>
#include "list.h"
#include "aggregate_level.h"

/* Constants */
/* --------- */
#define GOOGLE_CHART_POSITION_TOP		10
#define GOOGLE_CHART_POSITION_LEFT		10
#define GOOGLE_CHART_WIDTH			900
#define GOOGLE_CHART_HEIGHT			600
#define GOOGLE_CHART_BACKGROUND_COLOR		"#effdff"
#define GOOGLE_CHART_CONVERT_MONTH_EXPRESSION	atoi( month ) - 1
#define GOOGLE_ANNOTATED_TIMELINE		"annotatedtimeline"
#define GOOGLE_CORECHART			"corechart"

/* Enumerations */
/* ------------ */
enum google_chart_type{	google_column_chart,
			google_time_line,
			google_cat_whiskers };

/* Structures */
/* ---------- */
typedef struct
{
	char *stratum_name;
	double **point_array;
} GOOGLE_BARCHART;

typedef struct
{
	char *date_string;
	char *time_hhmm;
	double **point_array;
} GOOGLE_TIMELINE; 

typedef struct
{
	enum google_chart_type google_chart_type;
	int left;
	int top;
	int width;
	int height;
	char *background_color;
	boolean legend_position_bottom;
	LIST *barchart_list;
	LIST *timeline_list;
	LIST *datatype_name_list;
	char *google_package_name;
	int chart_number;
} GOOGLE_OUTPUT_CHART;

typedef struct
{
	char *date_time;
	double value;
	boolean null_value;
} GOOGLE_INPUT_VALUE;

typedef struct
{
	char *datatype_name;
	HASH_TABLE *value_hash_table;
} GOOGLE_INPUT_DATATYPE;

typedef struct
{
	char *unit;
	LIST *datatype_list;
	DICTIONARY *date_time_dictionary;
	char *xaxis_label;
	char *yaxis_label;
	boolean bar_chart;
} GOOGLE_UNIT_CHART;

typedef struct
{
	LIST *unit_chart_list;
	LIST *output_chart_list;
} GOOGLE_CHART;

/* Prototypes */
/* ---------- */
GOOGLE_INPUT_VALUE *google_input_value_new(
					void );

GOOGLE_UNIT_CHART *google_unit_chart_new(
					char *unit );

GOOGLE_INPUT_DATATYPE *google_input_datatype_new(
					void );

GOOGLE_OUTPUT_CHART *google_output_chart_new(
					enum google_chart_type,
					int left,
					int top,
					int width,
					int height,
					char *background_color,
					boolean legend_position_bottom,
					char *google_package_name );

GOOGLE_BARCHART *google_barchart_new(	char *stratum_name,
					int length_datatype_name_list );

GOOGLE_TIMELINE *google_timeline_new(	char *date_string,
					char *time_hhmm,
					int length_datatype_name_list );

GOOGLE_BARCHART *google_barchart_append(
					LIST *barchart_list,
					char *stratum_name,
					int length_datatype_name_list );

GOOGLE_TIMELINE *google_timeline_append(
					LIST *timeline_list,
					char *date_string,
					char *hhmm,
					int length_datatype_name_list );

void google_barchart_set_point(		LIST *barchart_list,
					char *stratum_name,
					LIST *datatype_name_list,
					char *datatype_name,
					double point );

void google_timeline_set_point(		LIST *timeline_list,
					LIST *datatype_name_list,
					char *date_string,
					char *time_hhmm,
					char *datatype_name,
					double point );

void google_barchart_set_point_string(	LIST *barchart_list,
					LIST *datatype_name_list,
					char *delimited_string,
					char delimiter );

void google_timeline_set_point_string(	LIST *timeline_list,
					LIST *datatype_name_list,
					char *delimited_string,
					char delimiter );

GOOGLE_BARCHART *google_barchart_get_or_set(
					LIST *barchart_list,
					char *stratum_name,
					int length_datatype_name_list );

GOOGLE_TIMELINE *google_timeline_get_or_set(
					LIST *timeline_list,
					char *date_string,
					char *time_hhmm,
					int length_datatype_name_list );

int google_chart_get_datatype_offset(
					LIST *datatype_name_list,
					char *datatype_name );

void google_timeline_display(		LIST *timeline_list,
					LIST *datatype_name_list );

void google_barchart_display(		LIST *barchar_list,
					LIST *datatype_name_list );

void google_chart_output_include(	FILE *output_file );

void google_chart_output_visualization_function(
				FILE *output_file,
				enum google_chart_type google_chart_type,
				LIST *barchart_list,
				LIST *timeline_list,
				LIST *datatype_name_list,
				char *title,
				char *yaxis_label,
		 		int width,
		 		int height,
		 		char *background_color,
				boolean legend_position_bottom,
				boolean chart_type_bar,
				char *google_package_name,
				boolean dont_display_range_selector,
				enum aggregate_level,
				int chart_number );

void google_chart_anchor_chart(	FILE *output_file,
				char *title,
				char *google_package_name,
				int left,
				int top,
				int width,
				int height,
				int chart_number );

void google_barchart_append_delimited_string(
				LIST *barchart_list,
				char *comma_delimited_string,
				int length_datatype_name_list );

void google_timeline_append_delimited_string(
				LIST *timeline_list,
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

void google_chart_output_datatype_column_heading(
				FILE *output_file,
				enum google_chart_type,
				char *first_column_datatype,
				LIST *datatype_name_list );

void google_chart_output_timeline_list(
				FILE *output_file,
				LIST *timeline_list,
				int length_datatype_name_list );

void google_chart_output_barchart_list(
				FILE *output_file,
				LIST *barchart_list,
				int length_datatype_name_list );

void google_barchart_append_datatype_name_string(
				LIST *barchart_list,
				char *datatype_name_list_string,
				int length_datatype_name_list,
				char delimiter );

void google_chart_float_chart(	FILE *output_file,
				char *title,
				int width,
				int height,
				int chart_number );

char *google_chart_get_visualization_function_name(
				int chart_number );

void google_chart_output_chart_instantiation(
				FILE *output_file,
				int chart_number );

#endif
