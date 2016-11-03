/* get_appaserver_mount_point.c						*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "appaserver_parameter_file.h"

int main( int argc, char **argv )
{
	char *appaserver_mount_point;

	argc = 1; /* stub */

	if ( strcmp( *argv, "get_appaserver_error_directory" ) == 0
	||   strcmp( *argv, "get_appaserver_error_directory.e" ) == 0 )
	{
		appaserver_mount_point =
		appaserver_parameter_file_get_appaserver_error_directory();
	}
	else
	{
		appaserver_mount_point =
			appaserver_parameter_file_get_appaserver_mount_point();
	}


	if ( !appaserver_mount_point )
	{
		fprintf( stderr,
	 		 "%s failed. Check node in datafile.\n",
			 argv[ 0 ] );
		exit( 1 );

	}
	printf( "%s\n", appaserver_mount_point );
	return 0;
}

