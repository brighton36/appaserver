/* shef_datatype_code_dvr.c */
/* ------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "shef_datatype_code.h"

int main( void )
{
	SHEF_DATATYPE_CODE *c;

	c = new_shef_datatype_code( "hydrology" );

	printf( "download datatype for BA/STG should be stage is (%s)\n",
		shef_datatype_code_get_shef_download_code(
					"BA",
					"STG",
					c->shef_download_datatype_list ) );

	printf( "upload shef NE1/HH should be stage is (%s)\n",
		shef_datatype_code_get_upload_datatype( 
					(SHEF_UPLOAD_AGGREGATE_MEASUREMENT **)0,
					"NE1",
					"HH",
					c->shef_upload_datatype_list,
					c->station_datatype_list->
						station_datatype_list,
					(char *)0 /* measurement_date */,
					(char *)0 /* measurement_time */,
					0.0 /* measurement_value */ ) );

	return 0;
}
