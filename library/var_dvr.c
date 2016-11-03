/* invest2:$root/library/tim/var_dvr.c */
/* ----------------------------------- */
/* Test drive program for variable ADT */
/* ----------------------------------- */

#include <stdio.h>
#include "variable.h"

main()
{
	/* test1(); */
	test2();
	/* test_to_memory(); */
}

test_to_memory()
{
	char *key[ 2 ];
	char *other_data[ 2 ];
	VARIABLE *a;
	HASH_TABLE *h = hash_table_init();
	int duplicate_indicator;

	VARIABLE *v = variable_init();

	set_variable( v, "a.x", "5" );
	printf( "got a.x = %s\n", get_value( v, "a.x" ) );

	variable_to_memory( key, other_data, v );

	printf( "got key = (%s)\n", key[ 0 ] );
	printf( "got other_data = (%s)\n", other_data[ 0 ] );

	if ( !key[ 1 ] )
	{
		printf( "Good, null in place for key.\n" );
	}

	if ( !other_data[ 1 ] )
	{
		printf( "Good, null in place for other_data.\n" );
	}

	hash_table_free( h );

} /* test_to_memory() */

test2()
{
	VARIABLE *v = variable_init();
	char buffer[ 128 ];

	set_variable( v, "sponsor_key", "KM" );

	printf( "%s\n", 
		replace_all_variables(	buffer, 
					"got sponsor_key = :sponsor_key", 
					v ) );
/*
	printf( "%s\n", 
		replace_all_variables(	buffer, 
					"got not found = :not_found", 
					v ) );
*/

	display_variable_table( v, "stdout" );
	variable_free( v );
}

test1()
{
	VARIABLE *v = variable_init();

	set_variable( v, "sponsor_key", "KM" );
	printf( "got sponsor_key = %s\n", get_value( v, "sponsor_key" ) );

	set_variable( v, "sponsor_key", "OP" );
	printf( "got sponsor_key = %s\n", get_value( v, "sponsor_key" ) );

	set_variable( v, "sponsor_name", "Oppenheimer" );
	printf( "got sponsor_name = %s\n", get_value( v, "sponsor_name" ) );

	variable_free( v );
}
