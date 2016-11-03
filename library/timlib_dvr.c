#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

void test_last_word( void );
void test_get_line_queue( void );
void test_commas_in_double( void );
void test_commas_in_integer( void );
void test_string_until_character_matches( void );
void test_factoral( void );
void test_strcpy( void );
void test_latitude_longitude( void );
void test_search_replace( void );
void test_pad_time( void );
void test_directory_filesystem( void );

int main()
{
	/* test_midstr(); */
	/* test_time2day_percentage(); */
	/* test_increment_time_one_hour(); */
	/* test_low_string(); */
	/* test_floor_ceiling(); */
	/* test_last_word(); */
	/* test_get_line_queue(); */
	/* test_commas_in_double(); */
	/* test_string_until_character_matches(); */
	/* test_factoral(); */
	/* test_strcpy(); */
	/* test_latitude_longitude(); */
	/* test_search_replace(); */
	/* test_pad_time(); */
	/* test_commas_in_integer(); */
	test_directory_filesystem();
	return 0;
}

void test_strcpy()
{
	char whatever[ 1024 ];
	char overflow[ 8 ];
	char righton[ 8 ];
	int return_value;

	return_value = timlib_strcpy( whatever, "whatever", 1024 );
	if ( return_value )
		printf( "Good: should be \"whatever\", is \"%s\".\n",
			whatever );
	else
		printf( "buffer overflow.\n" );

	return_value = timlib_strcpy( overflow, "overflow", 8 );
	if ( !return_value )
		printf( "Good: buffer overflow.\n" );
	else
		printf( "should not happen.\n" );

	return_value = timlib_strcpy( righton, "righton", 8 );
	if ( return_value )
		printf( "Good: should be \"righton\", is \"%s\".\n",
			righton );
	else
		printf( "should not happen.\n" );

}

void test_factoral( void )
{
	printf( "should be 362880: is = %d\n",
		timlib_factoral( 9 ) );
}

void test_string_until_character_matches( void )
{
	char *source = "quick_ratio^whatever";
	char *compare = "quick_ratio";

	printf( "should be 1: is = %d\n",
		timlib_string_until_character_matches(
			source,
			compare,
			'^' ) );
}

void test_commas_in_integer( void )
{
	unsigned int i;

	i = 116;
	printf( "Should be 116 is: %s\n",
		place_commas_in_unsigned_int( i ) );

	i = 7116;
	printf( "Should be 7,116 is: %s\n",
		place_commas_in_unsigned_int( i ) );

	i = 87116;
	printf( "Should be 87,116 is: %s\n",
		place_commas_in_unsigned_int( i ) );

	i = 987116;
	printf( "Should be 987,116 is: %s\n",
		place_commas_in_unsigned_int( i ) );

	i = 2987116;
	printf( "Should be 2,987,116 is: %s\n",
		place_commas_in_unsigned_int( i ) );

}

void test_commas_in_double( void )
{
	double d;
	char buffer[ 512 ];

	for( d = 1000000000000.0; d > 1.0; d /= 1000.0 )
	{
		printf( "for d = %.2lf: %s\n",
			d,
			commas_in_double( buffer, d ) );
	}
}

void test_get_line_queue( void )
{
	char buffer[ 1024 ];
	FILE *p;
	int i = 0;

	p = popen( "(echo one;echo two;echo three;echo four)", "r" );
	while( get_line_queue( buffer, p ) )
	{
		if ( ++i == 1 )
		{
			printf( "%s\n", buffer );
			unget_line_queue( buffer );
		}
		else
		{
			printf( "%s\n", buffer );
		}
	}
	pclose( p );
}

/*
void test_floor_ceiling()
{
	printf( "should be 5.0 is %lf\n", floor( 5.9 ) );
	printf( "should be 5.0 is %lf\n", floor( 5.1 ) );
	printf( "should be 5.0 is %lf\n", ceiling( 4.1 ) );
	printf( "should be 5.0 is %lf\n", ceiling( 4.9 ) );
}
*/
	
/*
test_low_string()
{
	char buffer[ 128 ];

	printf( "should be lower is = (%s)\n",
		low_string( strdup( "LOW_STRING" ) ) );
}

test_increment_time_one_hour()
{
	char time[ 5 ];

	strcpy( time, "1420" );
	increment_time_one_hour( time );
	printf( "should be 1520 is %s\n", time );

	strcpy( time, "0820" );
	increment_time_one_hour( time );
	printf( "should be 0920 is %s\n", time );

}

test_time2day_percentage()
{
	printf( "0000 = %.4lf\n", time2day_percentage( 0, 0 ) );
	printf( "0030 = %.4lf\n", time2day_percentage( 0, 30 ) );
	printf( "0100 = %.4lf\n", time2day_percentage( 1, 0 ) );
	printf( "1100 = %.4lf\n", time2day_percentage( 11, 0 ) );
	printf( "1130 = %.4lf\n", time2day_percentage( 11, 30 ) );
	printf( "1200 = %.4lf\n", time2day_percentage( 12, 0 ) );
	printf( "1230 = %.4lf\n", time2day_percentage( 12, 30 ) );
	printf( "1300 = %.4lf\n", time2day_percentage( 13, 0 ) );
	printf( "2359 = %.4lf\n", time2day_percentage( 23, 59 ) );
	printf( "2400 = %.4lf\n", time2day_percentage( 24, 0 ) );
}

test_midstr()
{
	char buffer[ 128 ];

	printf( "midstr( \"hello there\", 0, 2 ) = %s\n",
		midstr( buffer, "hello there", 0, 2 ) );
}
*/

void test_last_word()
{
	char *s = "DATE         G1111";

	printf( "test_last_word should b G1111 = (%s)\n",
		last_word_string( s ) );
}

void test_latitude_longitude( void )
{
	printf( "Latitude to decimal: should be 25.55233 is %.5lf\n",
		timlib_latitude_longitude_degrees_minutes_to_decimal(
					"25 33.14" ) );

	printf( "Longitude to decimal: should be -81.21017 is %.5lf\n",
		timlib_latitude_longitude_degrees_minutes_to_decimal(
					"-81 12.61" ) );

	printf( "Latitude to minutes: should be 25 33.14 is %s\n",
		timlib_latitude_longitude_degrees_decimal_to_minutes(
					"25.55233" ) );

	printf( "Latitude to minutes: should be -81 12.61 is %s\n",
		timlib_latitude_longitude_degrees_decimal_to_minutes(
					"-81.21017" ) );

}

void test_search_replace( void )
{
	char source_destination[ 1024 ];

	strcpy( source_destination, "stage|this|that" );
	printf( "should be (stage2|this|that): is = (%s)\n",
		search_replace_string(
			source_destination,
			"stage",
			"stage2" ) );

	strcpy( source_destination, "this|that|stage" );
	printf( "should be (this|that|stage2): is = (%s)\n",
		search_replace_string(
			source_destination,
			"stage",
			"stage2" ) );

	strcpy( source_destination, "this|that|stage2" );
	printf( "should be (this|that|stage2): is = (%s)\n",
		search_replace_string(
			source_destination,
			"stage",
			"stage" ) );

	strcpy( source_destination, "this|that|stage2" );
	printf( "should be (this|that|stage2): is = (%s)\n",
		search_replace_string(
			source_destination,
			"stage2",
			"stage2" ) );

}

void test_pad_time( void )
{
	char time[ 5 ];

	sprintf( time, "0" );
	timlib_pad_time_four( time );
	printf( "should be 0000: is %s\n", time );

	sprintf( time, "000" );
	timlib_pad_time_four( time );
	printf( "should be 0000: is %s\n", time );
}

void test_directory_filesystem( void )
{
	char *directory = "/var/appaserver/utility";
	int available_megabytes = 0;

	printf( "Filesystem should be (/var)\n" );
	printf( "Filesystem is be (%s)\n",
		timlib_directory_filesystem(
			&available_megabytes,
			directory ) );

	printf( "Available meg = %d\n", available_megabytes );
}
