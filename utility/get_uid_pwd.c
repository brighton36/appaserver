/* get_uid_pwd.c */
/* ------------- */

#define UID_PWD_FILE	"/home/riley/utility/get_uid_pwd.dat"

#include <stdio.h>
#include "timlib.h"

main( int argc, char **argv )
{
	char *uid_pwd, *get_uid_pwd();

	if ( argc != 2 )
	{
		/* Exit silently */
		/* ------------- */
		exit( 1 );
	}

	/* If exists a slash */
	/* ----------------- */
	if ( instr( "/", argv[ 1 ], 1 ) != -1 )
	{
		/* Then just output and exit */
		/* ------------------------- */
		printf( "%s\n", argv[ 1 ] );
		return 0;
	}

	uid_pwd = get_uid_pwd( argv[ 1 ] );

	if ( !uid_pwd )
	{
		/* Exit silently */
		/* ------------- */
		exit( 1 );
	}
	printf( "%s\n", uid_pwd );
	return 0;

} /* main() */


char *get_uid_pwd( char *schema )
{
	FILE *p;
	static char buffer[ 256 ];

	sprintf( buffer, 
		 "cat %s 					|"
		 "crypt 					|"
		 "grep \"^%s:\" 				|"
		 "trim_until_character.e ':'			 ", 
		 UID_PWD_FILE,
		schema );

	p = popen( buffer, "r" );

	if ( !fgets( buffer, 255, p ) )
	{
		pclose( p );
		return (char *)0;
	}
	else
	{
		/* Trim off trailing <CR> */
		/* ---------------------- */
		buffer[ strlen( buffer ) - 1 ] = '\0';

		pclose( p );
		return buffer;
	}

} /* get_uid_pwd() */


