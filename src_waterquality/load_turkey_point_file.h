/* src_waterquality/load_turkey_point_file.h */
/* ----------------------------------------- */

#ifndef LOAD_TURKEY_POINT_FILE_H
#define LOAD_TURKEY_POINT_FILE_H

/* Includes */
/* -------- */
#include "water_quality.h"

/* Constants */
/* --------- */
#define STDERR_COUNT				1000

#define STATION1_HEADING			"SITENAME"
#define STATION2_HEADING			"SITE"
#define COLLECTION_DATE_HEADING			"DATE"
#define COLLECTION_TIME_HEADING			"TIME"
#define COLLECTION_DEPTH_METERS_HEADING		"DEPTH"
#define STATION_LATITUDE_HEADING		"LATDEC"
#define STATION_LONGITUDE_HEADING		"LONDEC"

#define INSERT_RESULTS 			"station,collection_date,collection_time,parameter,units,concentration"

#define DISPLAY_RESULTS 			"station,collection_date,collection_time,parameter,units,concentration,exception"
#define INSERT_COLLECTION		"station,collection_date,collection_time,collection_depth_meters,meta_data_file"
#define INSERT_STATION 			"station"
#define INSERT_STATION_PARAMETER	"station,parameter,units"
#define INSERT_WATER_PROJECT_STATION 	"project_name,station"

/* Prototypes */
/* ---------- */
boolean extract_station_collection_attributes(
				char **error_message,
				char *station,
				char *collection_date,
				char *collection_time,
				char *depth_meters,
				char *longitude,
				char *latitude,
				char *input_string,
				LIST *load_column_list,
				char *application_name );

void close_pipes(
		FILE *results_insert_pipe,
		FILE *station_parameter_insert_pipe,
		FILE *station_insert_pipe,
		FILE *water_project_station_insert_pipe,
		FILE *collection_insert_pipe,
		FILE *table_output_pipe );

void insert_waterquality_parameters(
				char *application_name,
				char *input_filename );

void delete_waterquality(
				char *application_name,
				char *input_filename,
				WATER_QUALITY *water_quality );

int load_concentration_file(	char *application_name,
				char *input_filename,
				WATER_QUALITY *water_quality,
				boolean execute,
				char *project_name,
				char *heading_error_message );

char *subtract_colon_from_hrmi(	 char *hrmi );

RESULTS *extract_results(
				char *input_string,
				LIST *load_column_list,
				LIST *exception_list );

#endif
