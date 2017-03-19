/* src_creel/creel_load_library.c */
/* ------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "timlib.h"
#include "piece.h"
#include "dictionary.h"
#include "date.h"
#include "folder.h"
#include "creel_load_library.h"

boolean creel_load_library_common_get_species(
				char *family,
				char *genus,
				char *species,
				char *common_name,
				char *application_name )
{
	static DICTIONARY *common_name_dictionary = {0};
	char *results;

	if ( !common_name_dictionary )
	{
		common_name_dictionary =
			creel_load_library_get_species_common_name_dictionary(
					application_name );
	}

	if ( ! (results =
		dictionary_get_pointer( common_name_dictionary,
					common_name ) ) )
	{
		return 0;
	}

	piece( family, '/', results, 0 );
	piece( genus, '/', results, 1 );
	piece( species, '/', results, 2 );
	return 1;
} /* creel_load_library_common_get_species() */

boolean creel_load_library_get_family_genus_species(
				char *family,
				char *genus,
				char *species,
				char *florida_code_string,
				char *application_name )
{
	static DICTIONARY *florida_code_dictionary = {0};
	char *results;
	char florida_code_integer_string[ 16 ];
	int florida_code_integer;

	if ( !florida_code_dictionary )
	{
		florida_code_dictionary =
			creel_load_library_get_species_florida_code_dictionary(
					application_name );
	}

	florida_code_integer = atoi( florida_code_string );
	sprintf( florida_code_integer_string, "%d", florida_code_integer );

	if ( ! (results =
		dictionary_get_pointer( florida_code_dictionary,
					florida_code_integer_string ) ) )
	{
		return 0;
	}

	piece( family, '/', results, 0 );
	piece( genus, '/', results, 1 );
	piece( species, '/', results, 2 );
	return 1;
} /* creel_load_library_get_family_genus_species() */

boolean creel_load_library_get_family(
			char *family,
			char *florida_code_string,
			char *application_name )
{
	static DICTIONARY *florida_code_dictionary = {0};
	char *results;

	if ( !florida_code_dictionary )
	{
		florida_code_dictionary =
			creel_load_library_get_family_florida_code_dictionary(
					application_name );
	}

	if ( ! (results =
		dictionary_get_pointer( florida_code_dictionary,
					florida_code_string ) ) )
	{
		return 0;
	}

	strcpy( family, results );
	return 1;
} /* creel_load_library_get_family() */

DICTIONARY *creel_load_library_get_species_common_name_dictionary(
			char *application_name )
{
	char sys_string[ 1024 ];
	char *select = "common_name,family,genus,species";

	sprintf(sys_string,
		"get_folder_data	application=%s		 "
		"			select=%s		 "
		"			folder=species		|"
		"sed 's|\\^|/|g'				|"
		"sed 's|/|\\^|1'				 ",
		application_name,
		select );

	return pipe2dictionary(	sys_string, FOLDER_DATA_DELIMITER );
} /* creel_load_library_get_species_common_name_dictionary() */

DICTIONARY *creel_load_library_get_species_florida_code_dictionary(
			char *application_name )
{
	char sys_string[ 1024 ];
	char *select = "florida_state_code,family,genus,species";

	sprintf(sys_string,
		"get_folder_data	application=%s		 "
		"			select=%s		 "
		"			folder=species		|"
		"sed 's|\\^|/|g'				|"
		"sed 's|/|\\^|1'				 ",
		application_name,
		select );

	return pipe2dictionary(	sys_string, FOLDER_DATA_DELIMITER );
} /* creel_load_library_get_species_florida_code_dictionary() */

DICTIONARY *creel_load_library_get_family_florida_code_dictionary(
			char *application_name )
{
	char sys_string[ 1024 ];
	char *select = "florida_state_code,family";

	sprintf(sys_string,
		"get_folder_data	application=%s		 "
		"			select=%s		 "
		"			folder=fish_families	 ",
		application_name,
		select );

	return pipe2dictionary(	sys_string, FOLDER_DATA_DELIMITER );
} /* creel_load_library_get_family_florida_code_dictionary() */

boolean creel_load_library_populate_census_date_mdyy(
				char *census_date_mdyy,
				char *census_date_input )
{
	if ( strlen( census_date_input ) == 6 )
	{
		*census_date_mdyy++ = *census_date_input++;
		*census_date_mdyy++ = *census_date_input++;
		*census_date_mdyy++ = '/';
		*census_date_mdyy++ = *census_date_input++;
		*census_date_mdyy++ = *census_date_input++;
		*census_date_mdyy++ = '/';
		*census_date_mdyy++ = *census_date_input++;
		*census_date_mdyy++ = *census_date_input++;
		*census_date_mdyy = '\0';
	}
	else
	if ( strlen( census_date_input ) == 5 )
	{
		*census_date_mdyy++ = '0';
		*census_date_mdyy++ = *census_date_input++;
		*census_date_mdyy++ = '/';
		*census_date_mdyy++ = *census_date_input++;
		*census_date_mdyy++ = *census_date_input++;
		*census_date_mdyy++ = '/';
		*census_date_mdyy++ = *census_date_input++;
		*census_date_mdyy++ = *census_date_input++;
		*census_date_mdyy = '\0';
	}
	else
	{
		return 0;
	}

	return 1;
} /* creel_load_library_populate_census_date_mdyy() */

void creel_load_library_pad_time_four(
				char *interview_time_string )
{
	int str_len;

	if ( !*interview_time_string )
		strcpy( interview_time_string, "0000" );

	str_len = strlen( interview_time_string );

	if ( str_len > 4 || str_len == 3 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid interview time = (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			interview_time_string );
		return;
	}

	if ( str_len == 1 ) strcat( interview_time_string, "000" );
	if ( str_len == 2 ) strcat( interview_time_string, "00" );

} /* creel_load_library_pad_time_four() */

char *creel_load_library_get_guide_angler_name(
				char *first_name,
				char *last_name,
				char *application_name,
				char *permit_code )
{
	static char guide_angler_name[ 256 ];
	static LIST *permits_list = {0};

	/* If need the permit holder name from permit_code */
	/* ----------------------------------------------- */
	if ( application_name && permit_code )
	{
		char *record;
		int str_len;

		str_len = strlen( permit_code );

		if ( !permits_list )
		{
			char sys_string[ 1024 ];
			char *select =
				"permit_code,guide_angler_name";

			sprintf(
			sys_string,
		"get_folder_data	application=%s			"
		"			select=%s			"
		"			folder=permits			",
			application_name,
			select );

			permits_list = pipe2list( sys_string );
		}

		if ( !list_rewind( permits_list ) )
		{
			fprintf( stderr,
		"Error in %s/%s()/%d: empty permits_list.\n",
			 	__FILE__,
			 	__FUNCTION__,
			 	__LINE__ );
			exit( 1 );
		}

		do {
			record = list_get_pointer( permits_list );

			if ( strncmp(	permit_code,
					record,
					str_len ) == 0 )
			{
				piece(	guide_angler_name,
					FOLDER_DATA_DELIMITER,
					record,
					1 );
				return guide_angler_name;
			}
		} while( list_next( permits_list ) );

		return (char *)0;
	}
	else
	{
	/* -------------------------------------------------------- */
	/* If need the permit holder name from first and last names */
	/* -------------------------------------------------------- */
		sprintf(guide_angler_name,
		 	"%s, %s",
		 	last_name,
		 	first_name );

		escape_single_quotes( guide_angler_name );

		return guide_angler_name;
	}
} /* creel_load_library_get_guide_angler_name() */

