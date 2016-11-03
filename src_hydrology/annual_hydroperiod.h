/* annual_hydroperiod.h					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef ANNUAL_HYDROPERIOD_H
#define ANNUAL_HYDROPERIOD_H

#include "list.h"

#define ANNUAL_HYDROPERIOD_ROWS_BETWEEN_HEADING 20
#define GRACE_TICKLABEL_ANGLE			90
#define GRACE_DATATYPE_ENTITY_PIECE		0
#define GRACE_DATATYPE_PIECE			1
#define GRACE_DATE_PIECE			2
#define GRACE_TIME_PIECE			-1
#define GRACE_VALUE_PIECE			3

typedef struct
{
	char *station_name;
	char *stage_datatype_name;
	LIST *year_list;
	double threshold;
	double elevation;
} ANNUAL_HYDROPERIOD_STATION;

typedef struct
{
	double elevation_offset;
	LIST *station_list;
} ANNUAL_HYDROPERIOD;

typedef struct
{
	LIST *wetting_event_list;
	int year;
	int above_threshold_count;
} ANNUAL_HYDROPERIOD_YEAR;

typedef struct
{
	char *begin_date;
	char *end_date;
	int days_count;
	LIST *data_list;
} ANNUAL_HYDROPERIOD_WETTING_EVENT;

typedef struct
{
	char *measurement_date;
	double measurement_value;
} ANNUAL_HYDROPERIOD_DATA;

int annual_hydroperiod_get_minimum_days_count(
				       	LIST *wetting_event_list );

int annual_hydroperiod_get_maximum_days_count(
				       	LIST *wetting_event_list );

ANNUAL_HYDROPERIOD_WETTING_EVENT *annual_hydroperiod_wetting_event_new(
					char *begin_date_string,
					boolean detail );

LIST *annual_hydroperiod_get_wetting_event_list(
					int *above_threshold_count,
					FILE *input_pipe,
					double threshold,
					boolean detail );

char *annual_hydroperiod_get_where_clause(
					char *station_name,
					char *stage_datatype_name,
					int year );

LIST *annual_hydroperiod_get_year_list(
					char *application_name,
					char *station_name,
					char *stage_datatype_name,
					double threshold,
					int begin_year,
					int end_year,
					boolean detail );

void annual_hydroperiod_display(	LIST *station_list );

LIST *annual_hydroperiod_get_station_list(
					char **station_elevation_null,
					char *application_name,
					char *station_list_string,
					char *stage_datatype_list_string,
					double elevation_offset,
					int begin_year,
					int end_year,
					boolean detail );

LIST *annual_hydroperiod_get_alt_station_list(
					char **station_elevation_null,
					char *application_name,
					LIST *model_list,
					LIST *alternative_list,
					LIST *station_name_list,
					LIST *datatype_name_list,
					double elevation_offset,
					int begin_year,
					int end_year,
					boolean detail );

ANNUAL_HYDROPERIOD_DATA *annual_hydroperiod_data_new(
					char *measurement_date,
					double measurement_value );

ANNUAL_HYDROPERIOD_YEAR *annual_hydroperiod_year_new(
					int year );

ANNUAL_HYDROPERIOD_STATION *annual_hydroperiod_station_new(
					char *station_name,
					char *stage_datatype_name,
					char *model_name,
					char *alternative_name );

ANNUAL_HYDROPERIOD *annual_hydroperiod_new(
					double elevation_offset );

char *annual_hydroperiod_get_alt_where_clause(
					char *model_name,
					char *alternative_name,
					char *station_name,
					char *stage_datatype_name,
					int year );

LIST *annual_hydroperiod_get_alt_year_list(
					char *application_name,
					char *model_name,
					char *alternative_name,
					char *station_name,
					char *stage_datatype_name,
					double threshold,
					int begin_year,
					int end_year,
					boolean detail );

void annual_hydroperiod_output_text_file_summary(
				FILE *output_pipe,
				char *title,
				ANNUAL_HYDROPERIOD *annual_hydroperiod,
				char *application_name );

boolean annual_hydroperiod_output_chart_discontinuous(
				char *report_title,
				int begin_year,
				int end_year,
				ANNUAL_HYDROPERIOD *annual_hydroperiod,
				char *argv_0,
				char *application_name,
				char *appaserver_mount_point,
				double elevation_offset );

void annual_hydroperiod_output_transmit_detail(
				FILE *output_pipe,
				char *title,
				ANNUAL_HYDROPERIOD *annual_hydroperiod,
				boolean omit_elevation,
				char *application_name );

void annual_hydroperiod_output_table_summary(
				char *report_title,
				int begin_year,
				int end_year,
				ANNUAL_HYDROPERIOD *annual_hydroperiod,
				boolean omit_elevation,
				char *application_name );

void annual_hydroperiod_output_table_detail(
				char *report_title,
				int begin_year,
				int end_year,
				ANNUAL_HYDROPERIOD *annual_hydroperiod,
				boolean omit_elevation,
				char *application_name );

void annual_hydroperiod_get_report_title(
				char *title,
				char *report_title,
				int begin_year,
				int end_year );

int annual_hydroperiod_get_full_period_of_record_year(
				char *application_name,
				LIST *station_list,
				LIST *stage_datatype_list,
				char *aggregation_select,
				char *min_max_date_string,
				char *folder_name,
				char *date_attribute_name );

#endif
