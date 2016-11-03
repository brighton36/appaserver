/* ---------------------------------------------------	*/
/* src_benthic/environment.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include "environment.h"
#include "hash_table.h"

ENVIRONMENT *environment_new(
				char *anchor_date,
				char *anchor_time,
				char *actual_latitude,
				char *actual_longitude )
{
	ENVIRONMENT *environment;

	if ( ! ( environment =
			calloc( 1, sizeof( ENVIRONMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	environment->anchor_date = anchor_date;
	environment->anchor_time = anchor_time;
	environment->actual_latitude = actual_latitude;
	environment->actual_longitude = actual_longitude;
	return environment;
} /* environment_new() */

#ifdef NOT_DEFINED
HASH_TABLE *environment_get_datatype_hash_table(
				char *appliction_name,
				char *begin_date_string,
				char *end_date_string,
				char *datatype )
{
	char sys_string[ 1024 ];
	char *select;
	char *folder;
	char where[ 128 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char piece_buffer[ 128 ];
	HASH_TABLE *hash_table;
	ENVIRONMENT *environment;

	select =
"anchor_date,anchor_time,actual_latitude,actual_longitude,value";

	folder = "abiotic_measurement";

	sprintf(where,
		"anchor_date between '%s' and '%s' and datatype = '%s'",
		begin_date_string,
		end_date_string,
		datatype );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
ENVIRONMENT *environment_new(
				char *anchor_date,
				char *anchor_time,
				char *actual_latitude,
				char *actual_longitude )
	}

	pclose( input_pipe );

	return hash_table;

} /* environment_get_datatype_hash_table() */
#endif
