/* creel_dvr.c */
/* ----------- */

#include <stdio.h>
#include <stdlib.h>
#include "creel.h"
#include "timlib.h"
#include "piece.h"

/*
#define BEGIN_DATE	"1958-06-01"
#define END_DATE	"2014-08-31"
*/
#define BEGIN_DATE	"1992-01-18"
#define END_DATE	"1992-01-18"

#define APPLICATION	"creel"
#define FAMILY		"Sciaenidae"
#define GENUS		"Cynoscion"
#define SPECIES		"nebulosus"

void test1();
void additional_catches();

int main()
{
	/* test1(); */
	additional_catches();
	return 0;
}

void test1()
{
	CREEL *creel;

	creel =
		creel_new(
			APPLICATION,
			BEGIN_DATE,
			END_DATE,
			(char *)0 /* fishing_purpose */ );

	printf( "length creel_census_hash_table = %d\n",
		hash_table_length( creel->creel_census_hash_table ) );

	printf( "length fishing_trips_hash_table = %d\n",
		hash_table_length( creel->fishing_trips_hash_table ) );

	printf( "length catches_hash_table = %d\n",
		hash_table_length( creel->catches_hash_table ) );

}

void additional_catches()
{
	CREEL *creel;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	char input_buffer[ 1024 ];
	char fishing_purpose[ 128 ];
	char census_date[ 128 ];
	char interview_location[ 128 ];
	char interview_number[ 128 ];
	char *fishing_trips_key;
	CREEL_FISHING_TRIPS *fishing_trips;
	CREEL_CATCHES *catches;

	creel =
		creel_new(
			APPLICATION,
			BEGIN_DATE,
			END_DATE,
			"sport" );

	sprintf( sys_string,
		 "fishing_trips_for_species.sh %s %s %s %s '%s' '%s' '%s'",
		 APPLICATION,
		 BEGIN_DATE,
		 END_DATE,
		 "sport",
		 FAMILY,
		 GENUS,
		 SPECIES );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( fishing_purpose, '^', input_buffer, 0 );
		piece( census_date, '^', input_buffer, 1 );
		piece( interview_location, '^', input_buffer, 2 );
		piece( interview_number, '^', input_buffer, 3 );

		fishing_trips_key =
			creel_fishing_trips_get_key(
				fishing_purpose,
				census_date,
				interview_location,
				atoi( interview_number ) );

		if ( ! ( fishing_trips =
				hash_table_get_pointer(
					creel->fishing_trips_hash_table,
					fishing_trips_key ) ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get fishing trip for interview = %s.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 interview_number );
			pclose( input_pipe );
			exit( 1 );
		}

		if ( !list_rewind( fishing_trips->catches_list ) ) continue;

		do {
			catches =
				list_get_pointer(
					fishing_trips->catches_list );

			printf( ".family %s\n", catches->family );
			printf( ".genus %s\n", catches->genus );
			printf( ".species %s\n", catches->species );

		} while( list_next( fishing_trips->catches_list ) );
	}

	pclose( input_pipe );

}
