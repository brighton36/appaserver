/* src_hydrology/kml_extract.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "google_earth.h"
#include "google_earth_station.h"
#include "piece.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "hydrology_library.h"

#define APPLICATION			"hydrology"

void kml_extract(	void );

int main( void )
{
	kml_extract();
	return 0;
}

void kml_extract( void )
{
	GOOGLE_EARTH *google_earth;

	google_earth =
		google_earth_new(
		"Everglades National Park Hydrographic Stations",
		"Hydro Stations",
		"Locations and current conditions at Hydro Stations located in and around Everglades National Park",
		GOOGLE_EARTH_CENTER_LATITUDE,
		GOOGLE_EARTH_CENTER_LONGITUDE );

	google_earth_output_heading(
				stdout,
				google_earth->document_name,
				google_earth->folder_name,
				google_earth->folder_description,
				google_earth->center_latitude,
				google_earth->center_longitude );

	google_earth_station_populate_placemark_list(
			google_earth->placemark_list,
			APPLICATION,
			(char *)0 /* station */,
			(char *)0 /* appaserver_mount_point */ );

	google_earth_output_placemark_list(
			stdout,
			google_earth->placemark_list );

	google_earth_output_closing( stdout );

} /* kml_extract() */

