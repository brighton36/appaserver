/* mysql.c		 						*/
/* -------------------------------------------------------------------- */
/* This is a wrapper to the regular mysql executable. It first removes	*/
/* the password from the command line, so ps won't report it.		*/
/* -------------------------------------------------------------------- */
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "column.h"

int main( int argc, char **argv )
{
	char *flags;
	char flag1[ 16 ] = {0};
	char flag2[ 16 ] = {0};
	char password[ 256 ];
	char *user;
	char *quick_flag;
	char *database_name;
	int flags_count;

	if ( argc != 6 )
	{
		fprintf( stderr,
"Usage: %s flags password user quick_flag database_name\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	flags = argv[ 1 ];

	flags_count = count_columns( flags );

	if ( flags_count > 2 )
	{
		fprintf(	stderr,
				"Error in %s: can only have 2 mysql flags.\n",
				argv[ 0 ] );
		exit( 1 );
	}
	else
	if ( flags_count == 2 )
	{
		column(	flag1, 0, flags );
		column(	flag2, 1, flags );
	}
	else
	if ( flags_count == 1 )
	{
		column(	flag1, 0, flags );
	}


	strcpy( password, argv[ 2 ] );
	argv[ 2 ] = "-psecret";

	user = argv[ 3 ];
	quick_flag = argv[ 4 ];
	database_name = argv[ 5 ];

	if ( *flag1 )
	{
		if ( *quick_flag )
		{
			execlp(	"mysql",
				"mysql",
				flag1,
				password,
				user,
				quick_flag,
				database_name,
				(char *)0 );
		}
		else
		{
			execlp(	"mysql",
				"mysql",
				flag1,
				password,
				user,
				database_name,
				(char *)0 );
		}
	}
	else
	{
		if ( *quick_flag )
		{
			execlp(	"mysql",
				"mysql",
				password,
				user,
				quick_flag,
				database_name,
				(char *)0 );
		}
		else
		{
			execlp(	"mysql",
				"mysql",
				password,
				user,
				database_name,
				(char *)0 );
		}
	}

	fprintf( stderr, "Error in %s: could not execute mysql.\n", argv[ 0 ] );
	return 1;
} /* main() */

