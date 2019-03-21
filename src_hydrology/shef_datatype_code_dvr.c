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

/*
	SHEF_DATATYPE_CODE *c;
	c = shef_datatype_code_new( APPLICATION_NAME );

	printf( "download datatype for BA/stage should be STG is (%s)\n",
		shef_datatype_code_get_shef_download_code(
					"BA",
					"stage",
					c->shef_download_datatype_list ) );
*/

	hydrology = hydrology_new();

	/* Test 1 */
	/* ------ */
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

	/* Test 2 */
	/* ------ */
	station =
		hydrology_get_or_set_station(
			hydrology->input.station_list,
			APPLICATION_NAME,
			"BK" );

	if ( !station )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot load station.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	datatype_units_seek_phrase = "Chlorophyll [mu]g/L";

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
		printf(
		"Datatype='%s' should be Chlorophyll2/ug/l is (%s/%s)\n",
			datatype_units_seek_phrase,
			datatype->datatype_name,
			datatype->units->units_name );
	}

	return 0;
}
