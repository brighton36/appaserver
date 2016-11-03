/* google_earth_station.h */
/* ---------------------- */

#ifndef GOOGLE_EARTH_STATION_H
#define GOOGLE_EARTH_STATION_H

#include <stdio.h>
#include "list.h"

/* Constants */
/* --------- */
#define DOCUMENT_ROOT			"/var/www"
#define STATION_IMAGE_SUBDIRECTORY	"images/station_images_medium"
#define STATION_CHART_SUBDIRECTORY	"images/station_plots"
#define STATION_PDF_SUBDIRECTORY	"dailyplots"


/* Prototypes */
/* ---------- */
char *google_earth_station_get_pdf_filename(
			char *application_name,
			char *station,
			char *document_root_directory );

char *google_earth_station_get_image_filename(
			char *application_name,
			char *station,
			char *document_root_directory );

boolean google_earth_station_jpg_exists(
			char *application_name,
			char *station );

boolean google_earth_station_pdf_exists(
			char *application_name,
			char *station );

boolean google_earth_station_image_exists(
			char *application_name,
			char *station );

char *google_earth_station_get_image_tag(
			char *application_name,
			char *station,
			char *uname,
			char *document_root_directory );

char *google_earth_station_get_pdf_tag(
			char *application_name,
			char *station,
			char *uname,
			char *document_root_directory );

char *google_earth_station_get_jpg_tag(
			char *application_name,
			char *station,
			char *uname,
			char *document_root_directory );

char *google_earth_station_get_tagged_placemark_description(
			char *application_name,
			char *station,
			char *latitude,
			char *longitude,
			char *document_root_directory );

char *google_earth_station_get_untagged_placemark_description(
			char *application_name,
			char *station_name );

void google_earth_station_populate_placemark_list(
			LIST *placemark_list,
			char *station,
			char *application_name,
			char *document_root_directory );

#endif
