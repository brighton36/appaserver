/* ---------------------------------------------------	*/
/* update_transecf_basin.c				*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "dictionary.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "process.h"
#include "html_table.h"
#include "appaserver_parameter_file.h"
#include "application_constants.h"
#include "environ.h"
#include "application.h"
#include "alligator.h"

/* Enumerated Types */
/* ---------------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char nest_number[ 16 ];
	char utm_easting[ 16 ];
	char utm_northing[ 16 ];
	char *transect_number_string;
	int cell_number = 0;
	char *basin;
	char in_transect_boundary_yn;
	double distance = 0.0;
	char sys_string[ 1024 ];
	char *select;
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char *nest_table_name;

	if ( argc != 2 )
	{
		fprintf( stderr, 
			 "Usage: %s application\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	appaserver_error_starting_argv_append_file(
				argc,
				argv,
				application_name );

	nest_table_name = get_table_name( application_name, "nest" );
	sprintf( sys_string,
		 "update_statement.e table=%s key=nest_number carrot=y",
		 nest_table_name);
	output_pipe = popen( sys_string, "w" );

	select = "nest_number,utm_easting,utm_northing";
	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=nest			",
		 application_name,
		 select );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( nest_number, '^', input_buffer, 0 );
		piece( utm_easting, '^', input_buffer, 1 );
		piece( utm_northing, '^', input_buffer, 2 );

		if ( ! ( transect_number_string =
			alligator_get_transect_number_string(
					application_name,
					atoi( utm_northing ) ) ) )
		{
			fprintf( stderr,
	"Warning: for nest = %s, cannot get transect for %s\n",
				 nest_number,
				 utm_northing );
			continue;
		}

		if ( ! ( basin = alligator_get_basin_should_be(
						&cell_number,
						application_name,
						atof( utm_northing ),
						atof( utm_easting ) ) ) )
		{
			fprintf( stderr,
"Warning: for nest = %s, cannot get basin for %s/%s\n",
				  nest_number,
				  utm_northing,
				  utm_easting );
			continue;
		}

		in_transect_boundary_yn =
			alligator_get_in_transect_boundary_should_be_yn(
					&distance,
					application_name,
					atoi( transect_number_string ),
					atoi( utm_northing ),
					atoi( utm_easting ) );

		fprintf( output_pipe,
			 "%s^transect_number^%s\n",
			 nest_number,
			 transect_number_string );

		fprintf( output_pipe,
			 "%s^in_transect_boundary_yn^%c\n",
			 nest_number,
			 in_transect_boundary_yn );

		fprintf( output_pipe,
			 "%s^basin^%s\n",
			 nest_number,
			 basin );
	}

	pclose( input_pipe );
	pclose( output_pipe );
	return 0;
} /* main() */

