/* library/google_map.h */
/* -------------------- */

#ifndef GOOGLE_MAP_H
#define GOOGLE_MAP_H

#include <stdio.h>
#include "list.h"
#include "dictionary.h"

/* Constants */
/* --------- */
#define GOOGLE_MAP_FILENAME_STEM	"google_map"
#define GOOGLE_MAP_CENTER_LATITUDE	"25.68821"
#define GOOGLE_MAP_CENTER_LONGITUDE	"-80.84817"
#define GOOGLE_MAP_STARTING_ZOOM	9
/*
#define GOOGLE_MAP_URL_TEMPLATE		"%s/%s/google_map_%s.html"
#define GOOGLE_MAP_PROMPT_TEMPLATE	"/%s/google_map_%s.html"
#define GOOGLE_MAP_HTTP_PROMPT_TEMPLATE	"http://%s/%s/google_map_%s.html"
*/
#define GOOGLE_MAP_WIDTH		750
#define GOOGLE_MAP_HEIGHT		550
#define GOOGLE_MAP_CANVAS_ID		"map_canvas"

/* Structures */
/* ---------- */
typedef struct 
{
	char *click_message;
	double latitude_nad83;
	double longitude_nad83;
	int utm_easting;
	int utm_northing;
} GOOGLE_MAP_POINT;

typedef struct
{
	LIST *point_list;
} GOOGLE_MAP;

/* Prototypes */
/* ---------- */
double google_map_convert_base_60_with_float(
					char *latitude_longitude );

void google_map_convert_to_latitude_longitude(
					LIST *point_list );

GOOGLE_MAP *google_map_new(		void );

void google_map_set_point(		LIST *point_list,
					char *click_message,
					double latitude_nad83,
					double longitude_nad83,
					int utm_easting,
					int utm_northing );

void google_map_output_heading(		FILE *output_file,
					char *title,
					char *google_map_key_data,
					char *balloon_click_parameter,
					char *balloon_click_function,
					char *application_name,
					int absolute_position_top,
					int absolute_position_left,
					char *google_map_center_latitude,
					char *google_map_center_longitude,
					int google_map_starting_zoom,
					int google_map_width,
					int google_map_height );

void google_map_output_map_thumbtack(	FILE *output_file,
					char *click_message,
					double latitude,
					double longitude );

void google_map_output_selectable_map_thumbtack(
					FILE *output_file,
					char *click_message,
					double latitude,
					double longitude );

void google_map_output_heading_close(	FILE *output_file );

void google_map_output_body(		FILE *output_file,
					boolean with_table,
					char *additional_javascript );

char *google_map_get_map_key(
					char *application_name );

void google_map_output_selectable_point_list(
					FILE *output_file,
					LIST *point_list );

void google_map_output_point_list(	FILE *output_file,
					LIST *point_list );

double google_map_convert_base_60(	char *latitude_longitude );

double google_map_convert_coordinate(	char *latitude_longitude );

void google_map_output_rectangle(
					FILE *output_file,
					char *southwest_latitude,
					char *southwest_longitude,
					char *northeast_latitude,
					char *northeast_longitude );

void google_map_output_rectangle_bounds_changed(
					FILE *output_file );

void google_map_output_rectangle_copy_to_form(
					FILE *output_file );

char *google_map_get_balloon(	char *application_name,
				char *folder_name,
				char *role_name,
				DICTIONARY *dictionary,
				char *balloon_attribute_name_list_string );

LIST *google_map_get_no_display_attribute_name_list(
					DICTIONARY *dictionary );

#endif
