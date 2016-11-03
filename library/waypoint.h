/* library/waypoint.h					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef WAYPOINT_H
#define WAYPOINT_H

#include "boolean.h"
#include "list.h"

/* Constants */
/* --------- */

#define WAYPOINT_MAX_ARRAY	7

/* Structures */
/* ---------- */
typedef struct
{
	char *latitude_degrees_float;
	char *longitude_degrees_float;
	double latitude;
	double longitude;
	char *latitude_string;
	char *longitude_string;
	int utm_easting;
	int utm_northing;
} WAYPOINT;

typedef struct
{
	WAYPOINT *starting_waypoint;
	LIST *visit_waypoint_list;
	WAYPOINT *visit_waypoint_array[ WAYPOINT_MAX_ARRAY ];
	WAYPOINT *arranged_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ];
	WAYPOINT *optimal_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ];
	int optimal_distance;
	int array_length;
	FILE *arrangement_input_pipe;
} WAYPOINT_TRIP;

/* Prototypes */
/* ---------- */
boolean waypoint_convert_degrees_float(
			double *latitude_longitude,
			char *latitude_longitude_degrees_float );

boolean waypoint_convert_degrees_float_latitude_longitude(
			double *latitude,
			double *longitude,
			char *latitude_degrees_float,
			char *longitude_degrees_float );

boolean waypoint_set_visit(
			LIST *visit_waypoint_list,
			WAYPOINT *waypoint );

WAYPOINT *waypoint_new(	void );

WAYPOINT_TRIP *waypoint_trip_new(
			WAYPOINT *starting_waypoint );

boolean waypoint_convert_to_utm(
			WAYPOINT *starting_waypoint,
			LIST *visit_waypoint_list );

char *waypoint_get_temp_filename(
			void );

boolean waypoint_convert_to_utm_write_temp_file(
			char *temp_filename,
			WAYPOINT *starting_waypoint,
			LIST *waypoint_list );

boolean waypoint_convert_to_utm_read_temp_file(
			WAYPOINT *starting_waypoint,
			LIST *waypoint_list,
			char *temp_filename );

LIST *waypoint_get_utm_distance_list(
			WAYPOINT *starting_waypoint,
			WAYPOINT *visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
			int array_length );

int waypoint_get_total_distance(
			LIST *distance_list );

/* Returns array_length */
/* -------------------- */
int waypoint_set_visit_array(
		WAYPOINT *visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		LIST *visit_waypoint_list );

void waypoint_free_distance_list(
			LIST *distance_list );

boolean waypoint_set_optimal_visit_waypoint_array(
		int *optimal_distance,
		FILE **arrangement_input_pipe,
		WAYPOINT *optimal_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		WAYPOINT *visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		WAYPOINT *starting_waypoint,
		int array_length );

boolean waypoint_get_arrangement_array(
		FILE **input_pipe,
		int arrangement_array[ WAYPOINT_MAX_ARRAY ],
		int array_length );

void waypoint_set_arranged_visit_waypoint_array(
		WAYPOINT *arranged_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		WAYPOINT *visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		int arrangement_array[ WAYPOINT_MAX_ARRAY ],
		int array_length );

void waypoint_set_optimal_visit_waypoint_array_maybe(
		int *optimal_distance,
		WAYPOINT *optimal_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		WAYPOINT *arranged_visit_waypoint_array[ WAYPOINT_MAX_ARRAY ],
		WAYPOINT *starting_waypoint,
		int array_length );

WAYPOINT *waypoint_new_degrees_float(
		char *latitude_degrees_float,
		char *longitude_degrees_float );

char *waypoint_display(
		WAYPOINT *waypoint );

char *waypoint_array_display(
		WAYPOINT *waypoint_array[ WAYPOINT_MAX_ARRAY ],
		int array_length );

void waypoint_array_copy(
		WAYPOINT **destination_waypoint_array,
		WAYPOINT **source_waypoint_array,
		int array_length );

boolean waypoint_list_convert_to_utm(
		LIST *waypoint_list );

boolean waypoint_list_convert_to_utm_write_temp_file(
		char *temp_filename,
		LIST *waypoint_list );

boolean waypoint_list_convert_to_utm_read_temp_file(
		LIST *waypoint_list,
		char *temp_filename );

LIST *waypoint_list_get_utm_distance_list(
		LIST *waypoint1_list,
		LIST *waypoint2_list );

void waypoint_list_convert_degrees_float(
		LIST *waypoint_list );

void waypoint_list_remove_temp_file(
		char *temp_filename );

#endif
