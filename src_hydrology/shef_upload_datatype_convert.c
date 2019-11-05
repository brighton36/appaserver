/* ------------------------------------------------------------- 	*/
/* $APPASERVER_HOME/src_hydrology/shef_upload_datatype_convert.c      	*/
/* ------------------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org			*/
/* ------------------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "hydrology.h"
#include "environ.h"
#include "appaserver_error.h"
#include "shef_datatype_code.h"

void shef_upload_datatype_convert_stdin(
				char *application_name,
				int station_piece,
				int shef_code_piece,
				char delimiter );

void shef_upload_datatype_convert_one_time(
				char *application_name,
				char *station_name,
				char *shef_upload_code );

int main( int argc, char **argv )
{
	char *application_name;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc == 3 )
	{
		char *station_name;
		char *shef_upload_code;

		station_name = argv[ 1 ];
		shef_upload_code = argv[ 2 ];

		shef_upload_datatype_convert_one_time(
			application_name,
			station_name,
			shef_upload_code );
	}
	else
	if ( argc == 4 )
	{
		int station_piece;
		int shef_code_piece;
		char delimiter;

		station_piece = atoi( argv[ 1 ] );
		shef_code_piece = atoi( argv[ 2 ] );
		delimiter = *argv[ 3 ];

		shef_upload_datatype_convert_stdin(
			application_name,
			station_piece,
			shef_code_piece,
			delimiter );
	}
	else
	{
		fprintf( stderr,
"Usage: %s [station shef_upload_code] | [station_piece shef_code_piece delimiter]\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	return 0;

} /* main() */

void shef_upload_datatype_convert_one_time(
				char *application_name,
				char *station_name,
				char *shef_upload_code )
{
	HYDROLOGY *hydrology;
	STATION *station;
	char *datatype_name;

	hydrology = hydrology_new();

	station =
		station_get_or_set_station(
			hydrology->input.station_list,
			application_name,
			station_name );

	if ( !station )
	{
		fprintf( stderr,
			 "Error in %s/%s()/%d: cannot load station.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	datatype_name =
		datatype_name_seek_phrase(
			station->station_datatype_list,
			station->station_name,
			shef_upload_code
				/* datatype_units_seek_phrase */ );

	if ( datatype_name && *datatype_name )
	{
		printf( "%s\n", datatype_name );
	}

} /* shef_upload_datatype_convert_one_time() */

void shef_upload_datatype_convert_stdin(
				char *application_name,
				int station_piece,
				int shef_code_piece,
				char delimiter )
{
	char input_buffer[ 256 ];
	char station_name[ 128 ];
	char shef_upload_code[ 128 ];
	char *datatype_name;
	HYDROLOGY *hydrology;
	STATION *station;

	hydrology = hydrology_new();

	while ( get_line( input_buffer, stdin ) )
	{
		if ( !piece(	station_name,
				delimiter,
				input_buffer,
				station_piece ) )
		{
			continue;
		}

		if ( !piece(	shef_upload_code,
				delimiter,
				input_buffer,
				shef_code_piece ) )
		{
			continue;
		}

		station =
			station_get_or_set_station(
				hydrology->input.station_list,
				application_name,
				station_name );

		if ( !station )
		{
			fprintf( stderr,
			 "Error in %s/%s()/%d: cannot load station.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
			exit( 1 );
		}

		datatype_name =
			datatype_name_seek_phrase(
				station->station_datatype_list,
				station->station_name,
				shef_upload_code
					/* datatype_units_seek_phrase */ );

		if ( datatype_name && *datatype_name )
		{
			printf( "%s%c%s\n",
				input_buffer,
				delimiter,
				datatype_name );
		}
	}

} /* shef_upload_datatype_convert_stdin() */

