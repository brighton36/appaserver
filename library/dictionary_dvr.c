/* dictionary_dvr.c			 */
/* ------------------------------------- */
/* Test drive program for dictionary ADT */
/* ------------------------------------- */
/* Tim Riley				 */
/* ------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "dictionary.h"

void test_where_to_parameter_dictionary();

int main()
{
	/* test_1(); */
	/* test_2(); */
	test_where_to_parameter_dictionary();
	return 0;

} /* main() */

#ifdef NOT_DEFINED
test_1()
{
	DICTIONARY *d = dictionary_new();
	char *return_data;
	char *data;
	int size;

	data = "data for one";
	size = strlen( data ) + 1;
	dictionary_set( d, "one", data, size );

	data = "data for two";
	size = strlen( data ) + 1;
	dictionary_set( d, "two", data, size );

	data = "data for two (changed)";
	size = strlen( data ) + 1;
	dictionary_set( d, "two", data, size );

	data = "data for three";
	size = strlen( data ) + 1;
	dictionary_set( d, "three", data, size );

	return_data = dictionary_get( d, "two" );
	printf( "should be (data for two (changed)) is (%s)\n", return_data );

	return_data = dictionary_get( d, "three" );
	printf( "should be (data for three) is (%s)\n", return_data );

	dictionary_free( d );

} /* test_1() */

void test_2()
{
	DICTIONARY *d = dictionary_new();
	char *return_data;
	char *data;
	int size;

	data = "data for one";
	size = strlen( data ) + 1;
	dictionary_set( d, "one", data, size );

	data = "data for two";
	size = strlen( data ) + 1;
	dictionary_set( d, "two", data, size );

	return_data = dictionary_get( d, "two" );
	printf( "should be (data for two) is (%s)\n", return_data );

	return_data = dictionary_get( d, "one" );
	printf( "should be (data for one) is (%s)\n", return_data );

	dictionary_free( d );

} /* test_2() */
#endif

void test_where_to_parameter_dictionary()
{
#ifdef NOT_DEFINED
/*
	char *where_clause = "1 = 1 and station = 'BA' and datatype = 'stage'";
*/
	char *where_clause = "1 = 1 and station = 'BA' and measurement_date between '2000-10-01' and '2000-10-02'";

	DICTIONARY *d;

	d = where_clause_to_dictionary( where_clause );
	printf( "got dictionary = (%s)\n",
		 dictionary_display( d ) );
#endif
}
