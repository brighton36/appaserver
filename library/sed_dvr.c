#include <string.h>
#include <stdio.h>
#include "sed.h"

void test1( void );
void test2( void );

int main( void )
{
	/* test1(); */
	test2();
	return 0;
}

void test1( void )
{
	char *search  = "\\$_SESSION\\['\\$dbuser'\\]";
	char *replace = "$_SESSION['dbuser']";
	char buffer[ 512 ];
	SED *sed;

	strcpy( buffer,
		"a this is a test $_SESSION['$dbuser'] for this string" );

	sed = new_sed( search, replace );
	if ( sed_will_replace( buffer, sed ) )
	{
		printf( "before: %s\n", buffer );
		sed_search_replace( buffer, sed );
		printf( "after:  %s\n", buffer );
	}
}

void test2( void )
{
	char *regular_expression  =
		"[12][0-9][0-9][0-9]-[01][0-9]-[0123][0-9]";

	char *replace = "01-JUN-1999";
	char buffer[ 512 ];
	SED *sed;

	strcpy( buffer,
		"a this is a test (1999-06-01) for this string" );

	sed = new_sed( regular_expression, (char *)0 );
	if ( sed_will_replace( buffer, sed ) )
	{
		sed->replace = replace;
		printf( "before: %s\n", buffer );
		sed_search_replace( buffer, sed );
		printf( "after:  %s\n", buffer );
	}
}

