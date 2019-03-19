/* shef_datatype_code_dvr.c */
/* ------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "hydrology.h"
#include "shef_datatype_code.h"

#define APPLICATION_NAME	"hydrology"

int main( void )
{
	HYDROLOGY *hydrology;
	STATION *station;
	DATATYPE *datatype;
	char *datatype_units_seek_phrase;

	hydrology = hydrology_new();

#ifdef NOT_DEFINED
	SHEF_DATATYPE_CODE *c;
	c = shef_datatype_code_new( APPLICATION_NAME );

	printf( "download datatype for BA/stage should be STG is (%s)\n",
		shef_datatype_code_get_shef_download_code(
					"BA",
					"stage",
					c->shef_download_datatype_list ) );

	printf( "upload shef NE1/HH should be stage is (%s)\n",
		shef_datatype_code_get_upload_datatype( 
					(SHEF_UPLOAD_AGGREGATE_MEASUREMENT **)0,
					"hydrology",
					"NE1",
					"HH",
					(char *)0 /* measurement_date */,
					(char *)0 /* measurement_time */,
					0.0 /* measurement_value */ ) );

	station =
		hydrology_get_or_set_station(
			hydrology->input.station_list,
			APPLICATION_NAME,
			"NE1" );

	if ( !station )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot load station.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	printf( "upload shef NE1/HH should be stage is (%s)\n",
		shef_get_upload_default_datatype_name(
				"NE1",
				"HH",
				c->shef_upload_datatype_list,
				station->station_datatype_list ) );
#endif

	station =
		hydrology_get_or_set_station(
			hydrology->input.station_list,
			APPLICATION_NAME,
			"BA" );

	if ( !station )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot load station.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	datatype_units_seek_phrase = "Salinity (PSU)";

	printf( "Datatype='%s' should be salinity/ppt is (%s/%s)\n",
		datatype_units_seek_phrase,
		hydrology_datatype_name_seek_phrase(
				station->station_datatype_list,
				station->station_name,
				datatype_units_seek_phrase ),
		hydrology_units_name_seek_phrase(
				station->station_datatype_list,
				datatype_units_seek_phrase ) );

	datatype =
		hydrology_datatype_seek_phrase(
				station->station_datatype_list,
				station->station_name,
				datatype_units_seek_phrase );

	if ( !datatype->units )
	{
		printf( "Datatype='%s' returned NULL units.\n",
			datatype_units_seek_phrase );
	}
	else
	{
		printf( "Datatype='%s' should be salinity/ppt is (%s/%s)\n",
			datatype_units_seek_phrase,
			datatype->datatype_name,
			datatype->units->units_name );
	}

	return 0;
}
