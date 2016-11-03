/* google_earth_station.c				*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "application_constants.h"
#include "appaserver_library.h"
#include "environ.h"
#include "station.h"
#include "google_map.h"
#include "google_earth.h"
#include "google_earth_station.h"

void google_earth_station_populate_placemark_list(
			LIST *placemark_list,
			char *station_name,
			char *application_name,
			char *document_root_directory )
{
	char sys_string[ 1024 ];
	char input_buffer[ 128 ];
	char *placemark_description;
	char where[ 256 ];
	char input_station[ 16 ];
	char latitude[ 128 ];
	char longitude[ 128 ];
	FILE *input_pipe;
	char *select = "station,lat_nad83,long_nad83";

	if ( station_name && *station_name )
	{
		sprintf( where,
			 "station = '%s' and lat_nad83 is not null",
			 station_name );
	}
	else
	{
		strcpy( where, "lat_nad83 is not null" );
	}

	sprintf(sys_string,
		"get_folder_data	application=%s		"
		"			select=%s		"
		"			folder=station		"
		"			where=\"%s\"		",
		application_name,
		select,
		where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	input_station,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	latitude,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	longitude,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		if ( !google_earth_station_jpg_exists(
				application_name,
				input_station ) )
		{
			placemark_description =
			google_earth_station_get_untagged_placemark_description(
					application_name,
					strdup( input_station ) );
		}
		else
		{
			placemark_description =
			google_earth_station_get_tagged_placemark_description(
					application_name,
					strdup( input_station ),
					latitude,
					longitude,
					document_root_directory );
		}

		if ( placemark_description )
		{
			google_earth_set_placemark(
				placemark_list,
				strdup( input_station ),
				strdup( placemark_description ),
				atof( latitude ),
				atof( longitude ) );
		}
	}

	pclose( input_pipe );

} /* google_earth_station_populate_placemark_list() */

char *google_earth_station_get_tagged_placemark_description(
					char *application_name,
					char *station,
					char *latitude,
					char *longitude,
					char *document_root_directory )
{
	static char placemark_description[ 4096 ];
	char *uname;
	char *image_tag;
	char *pdf_tag;
	char *jpg_tag;

	uname = pipe2string( "uname -n" );

	image_tag = google_earth_station_get_image_tag(
				application_name,
				station,
				uname,
				document_root_directory );

	pdf_tag = google_earth_station_get_pdf_tag(
				application_name,
				station,
				uname,
				document_root_directory );

	jpg_tag = google_earth_station_get_jpg_tag(
				application_name,
				station,
				uname,
				document_root_directory );

	sprintf( placemark_description,
"<table align=center border=0 cellpadding=0 cellspacing=0>\n"
"<tr><td align=center><big><big><big><bold>%s</bold></big></big></big>\n"
"%s\n"
"<tr><td align=center>"
"<a %s>\n"
"<img border=2 src=%s width=400></a>\n"
"<tr><table align=center border=0 cellpadding=0 cellspacing=0>\n"
"<tr><td>Station Located at:\n"
"<tr><td>Latitude<td>%s\n"
"<tr><td>Longitude<td>%s\n"
"</table>\n"
"<tr><td><a href=\"mailto:EVER_data_request@nps.gov?subject=GoogleEarth Everglades data request for %s\">DataForEVER data request</a>\n"
"</table>\n",
		 station,
		 image_tag,
		 pdf_tag,
		 jpg_tag,
		 latitude,
		 longitude,
		 station );

	return placemark_description;

} /* google_earth_station_get_tagged_placemark_description() */

char *google_earth_station_get_untagged_placemark_description(
					char *application_name,
					char *station_name )
{
	static char placemark_description[ 4096 ];
	DICTIONARY *dictionary;
	char *balloon;

	if ( ! ( dictionary = station_load_dictionary(
			application_name,
			station_name ) ) )
	{
		fprintf( stderr,
			 "WARNING in %s/%s()/%d: cannot load station = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 station_name );
		return (char *)0;
	}

	balloon =
		google_map_get_balloon(
			application_name,
			"station" /* folder_name */,
			(char *)0 /* role_name */,
			dictionary,
			(char *)0 /* balloon_attribute_name_list_string */ );

	if ( !balloon ) return (char *)0;

	sprintf( placemark_description,
"<table align=center border=0 cellpadding=0 cellspacing=0>\n"
"<tr><td align=center><big><big><big><bold>%s</bold></big></big></big>\n"
"<tr><td align=center>"
"<tr><table align=center border=0 cellpadding=0 cellspacing=0>\n"
"<tr><td>%s\n"
"</table>\n"
"<tr><td><a href=\"mailto:EVER_data_request@nps.gov?subject=GoogleEarth Everglades data request for %s\">DataForEVER data request</a>\n"
"</table>\n",
		 station_name,
		 balloon,
		 station_name );

	return placemark_description;

} /* google_earth_station_get_untagged_placemark_description() */

char *google_earth_station_get_image_filename(
			char *application_name,
			char *station,
			char *document_root_directory )
{
	static char filename[ 256 ];

	sprintf( filename,
		 "%s/%s/%s/%s.png",
		 document_root_directory,
		 application_name,
		 STATION_IMAGE_SUBDIRECTORY,
		 station );

	return filename;
}

char *google_earth_station_get_image_tag(
			char *application_name,
			char *station,
			char *uname,
			char *document_root_directory )
{
	static char image_tag[ 256 ];

	*image_tag = '\0';

	if ( google_earth_station_image_exists(
				application_name,
				station ) )
	{
		if ( document_root_directory )
		{
			sprintf( image_tag,
"<tr><td align=center><img border=2 src=%s/%s/%s/%s.png width=200>",
				document_root_directory + 1,
				application_name,
				STATION_IMAGE_SUBDIRECTORY,
		 		station );
		}
		else
		{
			sprintf( image_tag,
"<tr><td align=center><img border=2 src=http://%s/%s/%s/%s.png width=200>",
		 		uname,
				application_name,
				STATION_IMAGE_SUBDIRECTORY,
		 		station );
		}
	}

	return image_tag;
}

boolean google_earth_station_image_exists(
				char *application_name,
				char *station )
{
	char filename[ 256 ];
	
	sprintf( filename,
		 "%s/%s/%s/%s.png",
		 DOCUMENT_ROOT,
		 application_name,
		 STATION_IMAGE_SUBDIRECTORY,
		 station );
	return timlib_file_exists( filename );
}

boolean google_earth_station_pdf_exists(
				char *application_name,
				char *station )
{
	char filename[ 256 ];
	
	sprintf( filename,
		 "%s/%s/%s/%s.pdf",
		 DOCUMENT_ROOT,
		 application_name,
		 STATION_PDF_SUBDIRECTORY,
		 station );

	return timlib_file_exists( filename );
}

boolean google_earth_station_jpg_exists(
				char *application_name,
				char *station )
{
	char filename[ 256 ];
	
	sprintf( filename,
		 "%s/%s/%s/%s.jpg",
		 DOCUMENT_ROOT,
		 application_name,
		 STATION_CHART_SUBDIRECTORY,
		 station );

	return timlib_file_exists( filename );
}

char *google_earth_station_get_pdf_filename(
			char *application_name,
			char *station,
			char *document_root_directory )
{
	static char filename[ 256 ];

	sprintf(filename,
		"%s/%s/%s/%s.pdf",
		document_root_directory,
		application_name,
		STATION_PDF_SUBDIRECTORY,
		station );

	return filename;
}

char *google_earth_station_get_pdf_tag(
			char *application_name,
			char *station,
			char *uname,
			char *document_root_directory )
{
	static char pdf_tag[ 256 ];

	*pdf_tag = '\0';

	if ( google_earth_station_pdf_exists(
				application_name,
				station ) )
	{
		if ( document_root_directory )
		{
			sprintf(pdf_tag,
				"href=%s/%s/%s/%s.pdf",
		 		document_root_directory + 1,
				application_name,
				STATION_PDF_SUBDIRECTORY,
		 		station );
		}
		else
		{
			sprintf(pdf_tag,
				"href=http://%s/%s/%s/%s.pdf",
		 		uname,
				application_name,
				STATION_PDF_SUBDIRECTORY,
		 		station );
		}
	}

	return pdf_tag;
}

char *google_earth_station_get_jpg_tag(
			char *application_name,
			char *station,
			char *uname,
			char *document_root_directory )
{
	static char jpg_tag[ 256 ];

	*jpg_tag = '\0';

	if ( google_earth_station_jpg_exists(
				application_name,
				station ) )
	{
		if ( document_root_directory )
		{
			sprintf(jpg_tag,
				"%s/%s/%s/%s.jpg",
		 		document_root_directory + 1,
				application_name,
				STATION_CHART_SUBDIRECTORY,
		 		station );
		}
		else
		{
			sprintf(jpg_tag,
				"http://%s/%s/%s/%s.jpg",
		 		uname,
				application_name,
				STATION_CHART_SUBDIRECTORY,
		 		station );
		}
	}

	return jpg_tag;
}

