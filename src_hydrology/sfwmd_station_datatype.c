/* sfwmd_station_datatype.c */
/* ------------------------ */
#include <string.h>
#include <stdlib.h>
#include "sfwmd_station_datatype.h"
#include "hash_table.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"

HASH_TABLE *load_sfwmd_station_datatype_hash_table(
			char *application_name )
{
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char *select = "db_key,station,datatype,conversion_factor";
	char input_buffer[ 1024 ];
	char piece_buffer[ 1024 ];
	HASH_TABLE *sfwmd_station_datatype_hash_table;
	SFWMD_STATION_DATATYPE *sfwmd_station_datatype;
	char *where;

	where =
"station is not null and station <> '' and datatype is not null and datatype <> ''";

	sfwmd_station_datatype_hash_table =
		hash_table_new_hash_table(
			hash_table_medium );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			folder=%s			"
		 "			select=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 SFWMD_STATION_DATATYPE_FOLDER,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		sfwmd_station_datatype = new_sfwmd_station_datatype();
		sfwmd_station_datatype->db_key = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		sfwmd_station_datatype->station = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		sfwmd_station_datatype->datatype = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		sfwmd_station_datatype->conversion_factor =
			atof( piece_buffer );

		hash_table_insert(
				sfwmd_station_datatype_hash_table,
				sfwmd_station_datatype->db_key,
				sfwmd_station_datatype );
	}

	pclose( input_pipe );
	return sfwmd_station_datatype_hash_table;

} /* load_sfwmd_station_datatype_hash_table() */

SFWMD_STATION_DATATYPE *new_sfwmd_station_datatype( void )
{
	return (SFWMD_STATION_DATATYPE *)
			calloc( 1, sizeof( SFWMD_STATION_DATATYPE ) );
} /* new_sfwmd_station_datatype() */

