/* ---------------------------------------------------	*/
/* $APPASERVER_HOME/src_creel/additional_species.c	*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "creel.h"
#include "timlib.h"
#include "piece.h"
#include "boolean.h"

#define DELIMITER		'^'

boolean additional_catches(	char *application_name,
				char *fishing_purpose,
				char *begin_date_string,
				char *end_date_string,
				char *family_string,
				char *genus_string,
				char *species_string );

int main( int argc, char **argv )
{
	char *application_name;
	char *fishing_purpose;
	char *begin_date_string;
	char *end_date_string;
	char *family_string;
	char *genus_string;
	char *species_string;

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s application fishing_purpose begin_date end_date family genus species \n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	fishing_purpose = argv[ 2 ];
	begin_date_string = argv[ 3 ];
	end_date_string = argv[ 4 ];
	family_string = argv[ 5 ];
	genus_string = argv[ 6 ];
	species_string = argv[ 7 ];

	if ( !additional_catches(
		application_name,
		fishing_purpose,
		begin_date_string,
		end_date_string,
		family_string,
		genus_string,
		species_string ) )
	{
		return 1;
	}
	else
	{
		return -1;
	}

} /* main() */

boolean additional_catches(	char *application_name,
				char *fishing_purpose,
				char *begin_date_string,
				char *end_date_string,
				char *family_string,
				char *genus_string,
				char *species_string )
{
	CREEL *creel;
	char sys_string[ 1024 ];
	FILE *input_pipe;
	FILE *output_pipe;
	char input_buffer[ 1024 ];
	char census_date[ 128 ];
	char interview_location[ 128 ];
	char interview_number[ 128 ];
	char *fishing_trips_key;
	CREEL_FISHING_TRIPS *fishing_trips;
	CREEL_CATCHES *catches;
	char species_key[ 128 ];
	char species_compare_key[ 128 ];

	strcpy( sys_string, "sort | group.e | sort -n -r" );

	output_pipe = popen( sys_string, "w" );

	sprintf(	species_key,
			"%s%c%s%c%s\n",
			species_string,
			DELIMITER,
			genus_string,
			DELIMITER,
			family_string );

	creel =
		creel_new(
			application_name,
			begin_date_string,
			end_date_string,
			fishing_purpose );

	sprintf( sys_string,
		 "fishing_trips_for_species.sh %s %s %s %s '%s' '%s' '%s'",
		 application_name,
		 begin_date_string,
		 end_date_string,
		 fishing_purpose,
		 family_string,
		 genus_string,
		 species_string );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		if ( character_count( DELIMITER, input_buffer ) != 3 )
		{
			fprintf( stderr,
			"ERROR in %s/%s()/%d: not 3 delimiters in (%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			pclose( input_pipe );
			pclose( output_pipe );
			return 0;
		}

		/* piece( fishing_purpose, DELIMITER, input_buffer, 0 ); */
		piece( census_date, DELIMITER, input_buffer, 1 );
		piece( interview_location, DELIMITER, input_buffer, 2 );
		piece( interview_number, DELIMITER, input_buffer, 3 );

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
			pclose( output_pipe );
			return 0;
		}

		if ( !list_rewind( fishing_trips->catches_list ) )
		{
			fprintf( stderr,
"ERROR in %s/%s()/%d: no catches for interview = %s.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 interview_number );

			pclose( input_pipe );
			pclose( output_pipe );
			return 0;
		}

		do {
			catches =
				list_get_pointer(
					fishing_trips->catches_list );

			sprintf(	species_compare_key,
					"%s%c%s%c%s\n",
					catches->species,
					DELIMITER,
					catches->genus,
					DELIMITER,
					catches->family );

			if ( timlib_strcmp(	species_key,
						species_compare_key ) != 0 )
			{
				fprintf(output_pipe,
					"%s^%s^%s\n",
					catches->family,
					catches->genus,
					catches->species );
			}

		} while( list_next( fishing_trips->catches_list ) );
	}

	pclose( input_pipe );
	pclose( output_pipe );

	return 1;

} /* additional_catches() */

