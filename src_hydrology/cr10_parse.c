/* -------------------------------------------------------	*/
/* $APPASERVER_HOME/src_hydrology/cr10_parse.c			*/
/* -------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* -------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "appaserver_parameter_file.h"
#include "document.h"
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "date.h"
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "column.h"
#include "application.h"
#include "basename.h"
#include "date_convert.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */
#define YEAR_PIECE			1 
#define DAYOFYEAR_PIECE			2 
#define TIME_PIECE			2

/* Prototypes */
/* ---------- */
void cr10_parse(			char *application_name,
					char *filename,
					char *station );

void cr10_parse_date_range(		char *filename );

int main( int argc, char **argv )
{
	char *application_name;
	char *filename;
	char *station;
	boolean date_range_only;

	/* Exits if failure. */
	/* ----------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 4 )
	{
		fprintf( stderr, 
			 "Usage: %s filename station date_range_only_yn\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	filename = argv[ 1 ];
	station = argv[ 2 ];
	date_range_only = (*argv[ 3 ] == 'y');

	if ( date_range_only )
	{
		cr10_parse_date_range( filename );
	}
	else
	{
		cr10_parse(
			application_name,
			filename,
			station );
	}

	return 0;

} /* main() */

void cr10_parse(	char *application_name,
			char *filename,
			char *station )
{
	char replace_cr10_error_with_null_process[ 1024 ];
	char sys_string[ 4096 ];

	sprintf(	replace_cr10_error_with_null_process,
			"%s",
			"sed 's/,6999\\.0*$/,/' | sed 's/,-6999\\.0*$/,/'" );

	sprintf( sys_string,
	"cat %s						      	     |"
	"grep -v '^$'						     |"
	"dayofyear2date.e ',' %d %d y 			  	     |"
	"piece_inverse.e 1 ','					     |"
	"prepend_zeros_to_piece.e ',' %d 4			     |"
	"apply_position_translation_to_record %s %s \',\' 	     |"
	"validate_midnight 2 3 ','				     |"
	"%s							     |"
	"cat							      ",
			 filename,
			 YEAR_PIECE,
			 DAYOFYEAR_PIECE,
			 TIME_PIECE,
			 application_name,
			 station,
			 replace_cr10_error_with_null_process );

	if ( system( sys_string ) ){};

} /* cr10_parse() */

void cr10_parse_date_range( char *filename )
{
	char sys_string[ 1024 ];

	sprintf( sys_string,
	"head -1 %s				|"
	"dayofyear2date.e ',' %d %d y 		|"
	"piece.e ',' 2 				|"
	"cat					 ",
			 filename,
			 YEAR_PIECE,
			 DAYOFYEAR_PIECE );

	if ( system( sys_string ) ){};

	sprintf( sys_string,
	"tail -1 %s				|"
	"dayofyear2date.e ',' %d %d y 		|"
	"piece.e ',' 2 				|"
	"cat					 ",
			 filename,
			 YEAR_PIECE,
			 DAYOFYEAR_PIECE );

	if ( system( sys_string ) ){};

} /* cr10_parse_date_range() */

