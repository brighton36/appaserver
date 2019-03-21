/* shef_datatype_code_dvr.c */
/* ------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "hydrology.h"
#include "shef_datatype_code.h"

#define APPLICATION_NAME	"hydrology"

void test3( void );
void test4( void );

int main( void )
{
	/* test3(); */
	test4();

#ifdef NOT_DEFINED
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

	/* Test 3 */
	/* ------ */
	/* station = BK */

	datatype_units_seek_phrase = "Chlorophyll µg/L";

	if ( ! ( datatype =
			hydrology_datatype_seek_phrase(
				station->station_datatype_list,
				station->station_name,
				datatype_units_seek_phrase ) ) )
	{
		printf( "Datatype='%s' returned NULL.\n",
			datatype_units_seek_phrase );
		exit( 0 );
	}

	if ( !datatype->units )
	{
		printf( "Datatype='%s' returned NULL units.\n",
			datatype_units_seek_phrase );
		exit( 0 );
	}
	else
	{
		printf(
		"Datatype='%s' should be Chlorophyll2/ug/l is (%s/%s)\n",
			datatype_units_seek_phrase,
			datatype->datatype_name,
			datatype->units->units_name );
	}
#endif

	return 0;

} /* main() */

void test3( void )
{
	HYDROLOGY *hydrology;
	STATION *station;
	DATATYPE *datatype;
	char *datatype_units_seek_phrase;

	hydrology = hydrology_new();

	station =
		hydrology_get_or_set_station(
			hydrology->input.station_list,
			APPLICATION_NAME,
			"BK" );

	datatype_units_seek_phrase = "Chlorophyll µg/L";

	if ( ! ( datatype =
			hydrology_datatype_seek_phrase(
				station->station_datatype_list,
				station->station_name,
				datatype_units_seek_phrase ) ) )
	{
		printf( "Datatype='%s' returned NULL.\n",
			datatype_units_seek_phrase );
		return;
	}

	if ( !datatype->units )
	{
		printf( "Datatype='%s' returned NULL units.\n",
			datatype_units_seek_phrase );
		return;
	}
	else
	{
		printf(
		"Datatype='%s' should be Chlorophyll2/ug/l is (%s/%s)\n",
			datatype_units_seek_phrase,
			datatype->datatype_name,
			datatype->units->units_name );
	}

} /* test3() */

void test4( void )
{
	HYDROLOGY *hydrology;
	STATION *station;
	DATATYPE *datatype;
	char *datatype_units_seek_phrase;

	hydrology = hydrology_new();

	station =
		hydrology_get_or_set_station(
			hydrology->input.station_list,
			APPLICATION_NAME,
			"BK" );

	datatype_units_seek_phrase = "Salinity (PSU)";

	if ( ! ( datatype =
			hydrology_datatype_seek_phrase(
				station->station_datatype_list,
				station->station_name,
				datatype_units_seek_phrase ) ) )
	{
		printf( "Datatype='%s' returned NULL.\n",
			datatype_units_seek_phrase );
		return;
	}

	if ( !datatype->units )
	{
		printf( "Datatype='%s' returned NULL units.\n",
			datatype_units_seek_phrase );
		return;
	}
	else
	{
		printf(
		"Datatype='%s' should be salinity/ppt is (%s/%s)\n",
			datatype_units_seek_phrase,
			datatype->datatype_name,
			datatype->units->units_name );
	}

} /* test4() */

