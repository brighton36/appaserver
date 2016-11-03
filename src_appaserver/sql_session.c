/* sql_session.c 							*/
/* -------------------------------------------------------------------- */
/* Copyright 2000-01 by Appahost					*/
/* -------------------------------------------------------------------- */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "appaserver_library.h"
#include "sql.h"

/* Prototypes */
/* ---------- */
int valid_session( char *session );

int main( int argc, char **argv )
{
	char sys_string[ 1024 ];
	char delimiter = '^';
	char *session;

	if ( argc < 2 )
	{
		fprintf( stderr, 
			 "Usage: %s session [delimiter]\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	session = argv[ 1 ];

	if ( argc == 3 ) delimiter = *argv[ 2 ];

	if ( !valid_session( session ) )
	{
		fprintf( stderr, "ERROR: %s invalid session = %s\n",
			 argv[ 0 ],
			 session );
		exit( 1 );
	}

	sprintf( sys_string, "sql.e \"%c\"", delimiter );
	system( sys_string );

	exit( 0 );
} /* main() */

int valid_session( char *session )
{
	return 1;
}
