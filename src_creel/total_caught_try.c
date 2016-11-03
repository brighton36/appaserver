/* ---------------------------------------------------	*/
/* src_creel/total_caught.c				*/
/* ---------------------------------------------------	*/
/* This is the header file for the Species Estimated    */
/* Total Caught Algorithm.				*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "total_caught.h"
#include "list.h"
#include "piece.h"
#include "timlib.h"
#include "appaserver_library.h"
#include "application_constants.h"
#include "or_sequence.h"

CATCH_AREA *total_caught_catch_area_new( void )
{
	CATCH_AREA *catch_area;

	if ( ! ( catch_area = calloc( 1, sizeof( CATCH_AREA ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return catch_area;
} /* total_caught_catch_area_new() */

CATCH *total_caught_catch_new( void )
{
	CATCH *catch;

	if ( ! ( catch = calloc( 1, sizeof( CATCH ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return catch;
} /* total_caught_catch_new() */

SPECIES *total_caught_species_new(
				char *family,
				char *genus,
				char *species_name )
{
	SPECIES *species;

	if ( ! ( species = calloc( 1, sizeof( SPECIES ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	species->family = family;
	species->genus = genus;
	species->species_name = species_name;
	return species;
} /* total_caught_species_new() */

FISHING_TRIP *total_caught_fishing_trip_new(
				char *fishing_purpose,
				char *census_date_string,
				char *interview_location,
				int interview_number )
{
	FISHING_TRIP *fishing_trip;

	if ( ! ( fishing_trip = calloc( 1, sizeof( FISHING_TRIP ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fishing_trip->fishing_purpose = fishing_purpose;
	fishing_trip->census_date =
		date_new_yyyy_mm_dd_date( census_date_string );
	fishing_trip->interview_location = interview_location;
	fishing_trip->interview_number = interview_number;
	fishing_trip->catch_list = list_new();
	return fishing_trip;
} /* total_caught_fishing_trip_new() */

ESTIMATED_TOTAL_CAUGHT *estimated_total_caught_new(
				int begin_month,
				int end_month,
				int year,
				char *application_name )
{
	ESTIMATED_TOTAL_CAUGHT *caught;

	if ( ! ( caught = calloc( 1, sizeof( ESTIMATED_TOTAL_CAUGHT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	caught->input =
		total_caught_input_new(
			begin_month,
			end_month,
			year,
			application_name );

	return caught;
} /* estimated_total_caught_new() */

ESTIMATED_TOTAL_CAUGHT_INPUT *total_caught_input_new(
				int begin_month,
				int end_month,
				int year,
				char *application_name )
{
	ESTIMATED_TOTAL_CAUGHT_INPUT *input;

	if ( ! ( input = calloc( 1, sizeof( ESTIMATED_TOTAL_CAUGHT_INPUT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	input->begin_month = begin_month;
	input->end_month = end_month;
	input->year = year;

	input->trailer_count_dictionary =
		total_caught_get_trailer_count_dictionary(
				application_name,
				begin_month,
				end_month,
				year );

	input->weekend_creel_census_dictionary =
		total_caught_get_weekend_creel_census_dictionary(
				application_name,
				begin_month,
				end_month,
				year );

	return input;

} /* total_caught_input_new() */

DICTIONARY *total_caught_get_weekend_creel_census_dictionary(
				char *application_name,
				int begin_month,
				int end_month,
				int year )
{
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	char where_clause[ 1024 ];
	char sys_string[ 1024 ];
	char select[ 128 ];
	char interview_location_where[ 128 ];

	sprintf(
	select,
	"concat(census_date,'%c',interview_location),dayname(census_date)",
	DICTIONARY_KEY_DELIMITER );

	total_caught_get_begin_end_date_string(
			begin_date_string,
			end_date_string,
			begin_month,
			end_month,
			year );

	sprintf( interview_location_where,
		 "interview_location in ('%s','%s')",
		 INTERVIEW_LOCATION_FLAMINGO,
		 INTERVIEW_LOCATION_EVERGLADES_CITY );

	sprintf( where_clause,
		 "census_date between '%s' and '%s'"
		 " and %s"
	  	 " and ( dayname( census_date ) = 'Saturday'"
	   	 " or   dayname( census_date ) = 'Sunday' )",
		 begin_date_string,
		 end_date_string,
		 interview_location_where );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=creel_census	"
		 "			where=\"%s\"		"
		 "			order=census_date	",
		 application_name,
		 select,
		 where_clause );

	return pipe2dictionary( sys_string, FOLDER_DATA_DELIMITER );

} /* total_caught_get_weekend_creel_census_dictionary() */

DICTIONARY *total_caught_get_trailer_count_dictionary(
				char *application_name,
				int begin_month,
				int end_month,
				int year )
{
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	char where_clause[ 128 ];
	char sys_string[ 1024 ];
	char select[ 128 ];

	sprintf( select,
		 "concat(date,'%c',parking_lot),trailer_count",
		 DICTIONARY_KEY_DELIMITER );

	total_caught_get_begin_end_date_string(
			begin_date_string,
			end_date_string,
			begin_month,
			end_month,
			year );

	sprintf( where_clause,
		 "date between '%s' and '%s'",
		 begin_date_string,
		 end_date_string );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=\"%s\"		"
		 "			folder=trailer_count	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where_clause );

	return pipe2dictionary( sys_string, FOLDER_DATA_DELIMITER );

} /* total_caught_get_trailer_count_dictionary() */

LIST *total_caught_get_species_list(
			boolean *all_species,
			char *application_name,
			char *family_list_string,
			char *genus_list_string,
			char *species_list_string )
{
	OR_SEQUENCE *or_sequence = {0};
	char *or_sequence_where_clause;
	LIST *column_name_list;
	char sys_string[ 1024 ];
	char *select;
	char input_buffer[ 512 ];
	char family[ 128 ];
	char genus[ 128 ];
	char species_name[ 128 ];
	char florida_state_code[ 128 ];
	SPECIES *species;
	FILE *input_pipe;
	LIST *species_list;

	if ( !*family_list_string
	||   strcmp( family_list_string, "family" ) == 0 )
	{
		or_sequence_where_clause = "1 = 1";
		*all_species = 1;
	}
	else
	{
		column_name_list = list_new();
		list_append_pointer( column_name_list, "family" );
		list_append_pointer( column_name_list, "genus" );
		list_append_pointer( column_name_list, "species" );
	
		or_sequence = or_sequence_new( column_name_list );
	
		or_sequence_set_data_list_string(
			or_sequence->data_list_list,
			family_list_string );
	
		or_sequence_set_data_list_string(
			or_sequence->data_list_list,
			genus_list_string );
	
		or_sequence_set_data_list_string(
			or_sequence->data_list_list,
			species_list_string );
	
		or_sequence_where_clause =
			or_sequence_get_where_clause(
					or_sequence->attribute_name_list,
				or_sequence->data_list_list,
				0 /* not with_and_prefix */ );
	}

	select = "family,genus,species,florida_state_code";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=species			"
		 "			where=\"%s\"			"
		 "			order=florida_state_code	",
		 application_name,
		 select,
		 or_sequence_where_clause );

	input_pipe = popen( sys_string, "r" );

	species_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	family,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	genus,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	species_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	florida_state_code,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		species = total_caught_species_new(
				strdup( family ),
				strdup( genus ),
				strdup( species_name ) );

		species->florida_state_code = atoi( florida_state_code );

		list_append_pointer( species_list, species );
	}
	pclose( input_pipe );
	return species_list;
} /* total_caught_get_species_list() */

HASH_TABLE *total_caught_get_fishing_trip_hash_table(
			FISHING_TRIP_LIST *fishing_trip_list_array
							[ MONTHS_PER_YEAR ]
							[ DAYS_PER_MONTH ],
			char *application_name,
			int begin_month,
			int end_month,
			int year )
{
	char sys_string[ 1024 ];
	char *select;
	char input_buffer[ 512 ];
	char fishing_purpose[ 128 ];
	char census_date_string[ 128 ];
	char interview_location[ 128 ];
	char interview_number_string[ 128 ];
	char hours_fishing_string[ 128 ];
	char fishing_area_string[ 128 ];
	FILE *input_pipe;
	FISHING_TRIP *fishing_trip;
	HASH_TABLE *fishing_trip_hash_table;
	char *fishing_trip_key;
	char where[ 256 ];
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];
	int census_date_month;
	int census_date_day;
	LIST *fishing_trip_list;

	select =
"fishing_purpose,census_date,interview_location,interview_number,hours_fishing,fishing_area";

	total_caught_get_begin_end_date_string(
			begin_date_string,
			end_date_string,
			begin_month,
			end_month,
			year );

	sprintf( where,
		 "census_date between '%s' and '%s' and			"
		 "dayname( census_date ) in ('Saturday','Sunday' ) 	",
		 begin_date_string,
		 end_date_string );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=fishing_trips	"
		 "			where=\"%s\"		"
		 "			order=census_date	",
		 application_name,
		 select,
		 where );

	input_pipe = popen( sys_string, "r" );

	fishing_trip_hash_table = hash_table_new( hash_table_large );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	fishing_purpose,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	census_date_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	interview_location,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	interview_number_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		piece(	hours_fishing_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		piece(	fishing_area_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		fishing_trip =
			total_caught_fishing_trip_new(
				strdup( fishing_purpose ),
				census_date_string,
				strdup( interview_location ),
				atoi( interview_number_string ) );

		fishing_trip->hours_fishing = atoi( hours_fishing_string );

		*(fishing_area_string + 1) = '\0';
		fishing_trip->fishing_area_integer =
				atoi( fishing_area_string );

		fishing_trip_key =
			total_caught_get_fishing_trip_key(
				fishing_purpose,
				census_date_string,
				interview_location,
				interview_number_string );

		hash_table_set_pointer(
				fishing_trip_hash_table,
				strdup( fishing_trip_key ),
				fishing_trip );

		if ( !total_caught_get_census_date_month_day(
			&census_date_month,
			&census_date_day,
			census_date_string ) )
		{
			continue;
		}

		if ( !fishing_trip_list_array	[ census_date_month - 1 ]
						[ census_date_day - 1 ] )
		{
			fishing_trip_list_array	[ census_date_month - 1 ]
						[ census_date_day - 1 ] =
				total_caught_fishing_trip_list_new();
		}

		fishing_trip_list =
			fishing_trip_list_array	[ census_date_month - 1 ]
						[ census_date_day - 1 ]->
				fishing_trip_list;

		list_append_pointer( fishing_trip_list, fishing_trip );

	}
	pclose( input_pipe );
	return fishing_trip_hash_table;
} /* total_caught_get_fishing_trip_hash_table() */

void total_caught_populate_catch_list(
				HASH_TABLE *fishing_trip_hash_table,
				LIST *species_list,
				char *application_name,
				int begin_month,
				int end_month,
				int year )
{
	char sys_string[ 1024 ];
	char *select;
	char input_buffer[ 512 ];
	char fishing_purpose[ 128 ];
	char census_date_string[ 128 ];
	char interview_location[ 128 ];
	char interview_number_string[ 128 ];
	char family[ 128 ];
	char genus[ 128 ];
	char species_name[ 128 ];
	char kept_count_string[ 128 ];
	char released_count_string[ 128 ];
	FILE *input_pipe;
	FISHING_TRIP *fishing_trip;
	CATCH *catch;
	SPECIES *species;
	char *key;
	char where[ 256 ];
	char begin_date_string[ 16 ];
	char end_date_string[ 16 ];

	select =
"fishing_purpose,census_date,interview_location,interview_number,family,genus,species,kept_count,released_count";

	total_caught_get_begin_end_date_string(
			begin_date_string,
			end_date_string,
			begin_month,
			end_month,
			year );

	sprintf( where,
		 "census_date between '%s' and '%s' and 		"
		 "dayname( census_date ) in ('Saturday','Sunday' )	",
		 begin_date_string,
		 end_date_string );

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=catches		"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

/* fprintf( stderr, "%s\n", sys_string ); */

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
/*
	select =
"fishing_purpose,census_date,interview_location,interview_number,family,genus,species,kept_count,released_count";
*/

		piece(	family,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );

		piece(	genus,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			5 );

		piece(	species_name,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			6 );

		if ( ! ( species = total_caught_seek_species(
					species_list,
					family,
					genus,
					species_name ) ) )
		{
			continue;
		}

		piece(	fishing_purpose,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	census_date_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		piece(	interview_location,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		piece(	interview_number_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );

		key = total_caught_get_fishing_trip_key(
				fishing_purpose,
				census_date_string,
				interview_location,
				interview_number_string );

		if ( ! ( fishing_trip = hash_table_fetch(
					fishing_trip_hash_table,
					key ) ) )
		{
			fprintf(stderr,
"Ignoring in %s/%s()/%d: orphaned fishing trip for primary key = (%s,%s,%s,%s).\n",
				__FILE__,
				__FUNCTION__,
				__LINE__,
				fishing_purpose,
				census_date_string,
				interview_location,
				interview_number_string );
			continue;
		}

		catch = total_caught_get_or_set_fishing_trip_catch(
					fishing_trip->catch_list,
					species->family,
					species->genus,
					species->species_name,
					species );

		piece(	kept_count_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			7 );

		piece(	released_count_string,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			8 );

		catch->kept += atoi( kept_count_string );
		catch->released += atoi( released_count_string );
	}
	pclose( input_pipe );
} /* total_caught_populate_catch_list() */

CATCH *total_caught_get_or_set_fishing_trip_catch(
					LIST *catch_list,
					char *family,
					char *genus,
					char *species_name,
					SPECIES *species )
{
	CATCH *catch;

	if ( list_rewind( catch_list ) )
	{
		do {
			catch = list_get_pointer( catch_list );

			if ( strcmp(	catch->species->family,
					family ) == 0
			&&   strcmp(	catch->species->genus,
					genus ) == 0
			&&   strcmp(	catch->species->species_name,
					species_name ) == 0 )
			{
				return catch;
			}
		} while( list_next( catch_list ) );
	}

	catch = total_caught_catch_new();
	catch->species = species;
	list_append_pointer( catch_list, catch );
	return catch;
	
} /* total_caught_get_or_set_fishing_trip_catch() */

char *total_caught_get_trailer_key(
				char *trailer_count_date_string,
				char *parking_lot )
{
	static char key[ 256 ];

	sprintf( key,
		 "%s%c%s",
		 trailer_count_date_string,
		 DICTIONARY_KEY_DELIMITER,
		 parking_lot );

	return key;
} /* total_caught_get_trailer_key() */

char *total_caught_get_fishing_trip_key(
				char *fishing_purpose,
				char *census_date_string,
				char *interview_location,
				char *interview_number_string )
{
	static char key[ 256 ];

	sprintf( key,
		 "%s%c%s%c%s%c%s",
		 fishing_purpose,
		 DICTIONARY_KEY_DELIMITER,
		 census_date_string,
		 DICTIONARY_KEY_DELIMITER,
		 interview_location,
		 DICTIONARY_KEY_DELIMITER,
		 interview_number_string );

	return key;
} /* total_caught_get_fishing_trip_key() */

char *total_caught_get_weekend_key(
				char *census_date_string,
				char *interview_location )
{
	static char key[ 256 ];

	sprintf( key,
		 "%s%c%s",
		 census_date_string,
		 DICTIONARY_KEY_DELIMITER,
		 interview_location );

	return key;
} /* total_caught_get_weekend_key() */

char *total_caught_get_species_key(
				char *family,
				char *genus,
				char *species_name )
{
	static char key[ 256 ];

	sprintf( key,
		 "%s%c%s%c%s",
		 family,
		 DICTIONARY_KEY_DELIMITER,
		 genus,
		 DICTIONARY_KEY_DELIMITER,
		 species_name );

	return key;
} /* total_caught_get_species_key() */

void total_caught_get_begin_end_date_string(
			char *begin_date_string,
			char *end_date_string,
			int begin_month,
			int end_month,
			int year )
{
	DATE *last_week;

	sprintf( begin_date_string,
		 "%d-%.2d-01",
		 year,
		 begin_month );

	last_week = date_new_yyyy_mm_dd_date( begin_date_string );
	date_decrement_days( last_week, BEGIN_DAY_GO_BACK );
	date_get_yyyy_mm_dd( begin_date_string, last_week );
	date_free( last_week );

	sprintf( end_date_string,
		 "%d-%.2d-31",
		 year,
		 end_month );

} /* total_caught_get_begin_end_date_string() */

MONTH_SHEET *total_caught_month_sheet_new( int month )
{
	MONTH_SHEET *month_sheet;

	if ( ! ( month_sheet = calloc( 1, sizeof( MONTH_SHEET ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	month_sheet->month = month;
	month_sheet->row_list = list_new();
	return month_sheet;
} /* total_caught_month_sheet_new() */

TOTAL_SHEET *total_caught_total_sheet_new( int year )
{
	TOTAL_SHEET *total_sheet;

	if ( ! ( total_sheet = calloc( 1, sizeof( TOTAL_SHEET ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	total_sheet->year = year;
	total_sheet->total_row_list = list_new();
	return total_sheet;
} /* total_caught_total_sheet_new() */

MONTH_ROW *total_caught_month_row_new( int day )
{
	MONTH_ROW *month_row;

	if ( ! ( month_row = calloc( 1, sizeof( MONTH_ROW ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	month_row->day = day;
	return month_row;
} /* total_caught_month_row_new() */

ESTIMATED_TOTAL_CAUGHT_OUTPUT *total_caught_output_new( void )
{
	ESTIMATED_TOTAL_CAUGHT_OUTPUT *output;

	if ( ! ( output = calloc( 1,
				  sizeof( ESTIMATED_TOTAL_CAUGHT_OUTPUT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	return output;
} /* total_caught_output_new() */

ESTIMATED_TOTAL_CAUGHT_OUTPUT *total_caught_get_output(
			int begin_month,
			int end_month,
			int year,
			LIST *species_list,
			DICTIONARY *trailer_count_dictionary,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			DICTIONARY *weekend_creel_census_dictionary,
			char *application_name )
{
	ESTIMATED_TOTAL_CAUGHT_OUTPUT *output;

	output = total_caught_output_new();

	output->month_sheet_list =
		total_caught_get_month_sheet_list(
			begin_month,
			end_month,
			year,
			trailer_count_dictionary,
			fishing_trip_list_array,
			species_list,
			weekend_creel_census_dictionary,
			application_name );

	output->total_sheet =
		total_caught_get_total_sheet(
			output->month_sheet_list,
			begin_month,
			year );

	total_caught_populate_grand_totals(
			&output->grand_areas_1_5_total_kept,
			&output->grand_areas_1_5_total_released,
			&output->grand_areas_1_5_total_caught,
			&output->grand_areas_1_6_total_kept,
			&output->grand_areas_1_6_total_released,
			&output->grand_areas_1_6_total_caught,
			output->total_sheet->total_row->catch_area_list );

	return output;
} /* total_caught_get_output() */

LIST *total_caught_get_month_sheet_list(
			int begin_month,
			int end_month,
			int year,
			DICTIONARY *trailer_count_dictionary,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			LIST *species_list,
			DICTIONARY *weekend_creel_census_dictionary,
			char *application_name )
{
	MONTH_SHEET *month_sheet;
	LIST *month_sheet_list = {0};
	int month;

	for( month = begin_month; month <= end_month; month++ )
	{
		month_sheet = total_caught_get_month_sheet(
				month,
				year,
				trailer_count_dictionary,
				fishing_trip_list_array,
				species_list,
				weekend_creel_census_dictionary,
				application_name );

		if ( month_sheet )
		{
			if ( !month_sheet_list )
				month_sheet_list = list_new();
			list_append_pointer( month_sheet_list, month_sheet );
		}
	}
	return month_sheet_list;
} /* total_caught_get_month_sheet_list() */

MONTH_SHEET *total_caught_get_month_sheet(
				int month,
				int year,
				DICTIONARY *trailer_count_dictionary,
				FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
				LIST *species_list,
				DICTIONARY *weekend_creel_census_dictionary,
				char *application_name )
{
	MONTH_SHEET *month_sheet;
	MONTH_ROW *month_row;
	int days_in_month;
	int day;
/*
	char search_date_string[ 16 ];
	DATE *row_search_date;
	DATE *flamingo_fishing_trip_search_date_saturday;
	DATE *flamingo_fishing_trip_search_date_sunday;
	DATE *everglades_city_fishing_trip_search_date_saturday;
	DATE *everglades_city_fishing_trip_search_date_sunday;
*/

	if ( month < 1 || month > 12 ) return (MONTH_SHEET *)0;

	month_sheet = total_caught_month_sheet_new( month );

	days_in_month = date_days_in_month( month, year );

	for( day = 1; day <= days_in_month; day++ )
	{
#ifdef NOT_DEFINED
		sprintf( search_date_string,
			 "%d-%.2d-%.2d",
			 year,
			 month,
			 day );

		row_search_date =
			date_yyyy_mm_dd_new( search_date_string );

		flamingo_fishing_trip_search_date_saturday =
			total_caught_get_fishing_trip_search_date(
				search_date_string,
				weekend_creel_census_dictionary,
				INTERVIEW_LOCATION_FLAMINGO,
				1 /* is_saturday */ );

		flamingo_fishing_trip_search_date_sunday =
			total_caught_get_fishing_trip_search_date(
				search_date_string,
				weekend_creel_census_dictionary,
				INTERVIEW_LOCATION_FLAMINGO,
				0 /* not is_saturday */ );

		everglades_city_fishing_trip_search_date_saturday =
			total_caught_get_fishing_trip_search_date(
				search_date_string,
				weekend_creel_census_dictionary,
				INTERVIEW_LOCATION_EVERGLADES_CITY,
				1 /* is_saturday */ );

		everglades_city_fishing_trip_search_date_sunday =
			total_caught_get_fishing_trip_search_date(
				search_date_string,
				weekend_creel_census_dictionary,
				INTERVIEW_LOCATION_EVERGLADES_CITY,
				0 /* not is_saturday */ );
#endif

		month_row =
			total_caught_get_month_row(
				day,
				month,
				year,
				trailer_count_dictionary,
				fishing_trip_list_array,
				species_list,
				weekend_creel_census_dictionary,
				application_name );

		list_append_pointer( month_sheet->row_list, month_row );
	}

	if ( ! ( month_sheet->total_row =
		total_caught_get_month_sheet_total_row(
			month_sheet->row_list ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty month sheet row list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return month_sheet;
} /* total_caught_get_month_sheet() */

MONTH_ROW *total_caught_get_month_row(
			int day,
			int month,
			int year,
			DICTIONARY *trailer_count_dictionary,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			LIST *species_list,
			DICTIONARY *weekend_creel_census_dictionary,
			char *application_name )
{
	MONTH_ROW *month_row;
	char *trailer_count_string;
	char *trailer_count_key;
	char buffer[ 16 ];
	char *last_weekend_string;
	double areas_1_5_coefficient;
	double areas_1_5_y_intercept;
	double area_6_coefficient;
	double area_6_y_intercept;
	char row_date_string[ 16 ];

	month_row = total_caught_month_row_new( day );

	sprintf( row_date_string,
		 "%d-%.2d-%.2d",
		 year,
		 month,
		 day );

	month_row->vessel.row_date = date_yyyy_mm_dd_new( row_date_string );

	month_row->performed_flamingo_census_weekend =
		total_caught_get_performed_census_weekend(
			&month_row->vessel.flamingo_interview_date_saturday,
			&month_row->vessel.flamingo_interview_date_sunday,
			month_row->vessel.row_date,
			weekend_creel_census_dictionary,
			INTERVIEW_LOCATION_FLAMINGO );

	month_row->performed_everglades_city_census_weekend =
		total_caught_get_performed_census_weekend(
			&month_row->
				vessel.
				everglades_city_interview_date_saturday,
			&month_row->
				vessel.
				everglades_city_interview_date_sunday,
			month_row->vessel.row_date,
			weekend_creel_census_dictionary,
			INTERVIEW_LOCATION_EVERGLADES_CITY );

#ifdef NOT_DEFINED
	month_row->vessel.flamingo_interview_date_saturday =
		total_caught_get_interview_row_date(
			row_date,
			weekend_creel_census_dictionary,
			INTERVIEW_LOCATION_FLAMINGO,
			1 /* is_saturday */ );

	month_row->vessel.flamingo_interview_date_sunday =
		total_caught_get_interview_row_date(
			row_date,
			weekend_creel_census_dictionary,
			INTERVIEW_LOCATION_FLAMINGO,
			0 /* not is_saturday */ );

	month_row->vessel.everglades_city_fishing_trip_search_date_saturday =
		total_caught_get_interview_row_date(
			row_date,
			weekend_creel_census_dictionary,
			INTERVIEW_LOCATION_EVERGLADES_CITY,
			1 /* is_saturday */ );

	month_row->vessel.everglades_city_fishing_trip_search_date_sunday =
		total_caught_get_interview_row_date(
			row_date,
			weekend_creel_census_dictionary,
			INTERVIEW_LOCATION_EVERGLADES_CITY,
			0 /* not is_saturday */ );
#endif

	/* Get florida_bay_trailer_count_saturday */
	/* -------------------------------------- */
	last_weekend_string =
		total_caught_get_last_weekend_string(
			month_row->vessel.row_date,
			1 /* is_saturday */ );

if ( day == 9 )
{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: got last_weekend_string = %s.\n",
__FILE__,
__FUNCTION__,
__LINE__,
last_weekend_string );
m2( "creel", msg );
}
	trailer_count_key =
		total_caught_get_trailer_key(
			last_weekend_string,
			PARKING_LOT_FLORIDA_BAY );

	if ( ( trailer_count_string =
			dictionary_get_pointer(
				trailer_count_dictionary,
				trailer_count_key ) ) )
	{
		month_row->vessel.florida_bay_trailer_count_saturday =
			atoi( trailer_count_string );
	}

if ( day == 9 )
{
char msg[ 65536 ];
sprintf( msg, "%s/%s()/%d: got florida_bay_trailer_count_saturday = %d.\n",
__FILE__,
__FUNCTION__,
__LINE__,
month_row->vessel.florida_bay_trailer_count_saturday );
m2( "creel", msg );
}
	/* Get florida_bay_trailer_count_sunday */
	/* ------------------------------------ */
	last_weekend_string =
		total_caught_get_last_weekend_string(
			month_row->vessel.row_date,
			0 /* not is_saturday */ );

	trailer_count_key =
		total_caught_get_trailer_key(
			last_weekend_string,
			PARKING_LOT_FLORIDA_BAY );

	if ( ( trailer_count_string =
			dictionary_get_pointer(
				trailer_count_dictionary,
				trailer_count_key ) ) )
	{
		month_row->vessel.florida_bay_trailer_count_sunday =
			atoi( trailer_count_string );
	}

	/* Get whitewater_bay_trailer_count_saturday */
	/* -------------------------------------- */
	last_weekend_string =
		total_caught_get_last_weekend_string(
			month_row->vessel.row_date,
			1 /* is_saturday */ );

	trailer_count_key =
		total_caught_get_trailer_key(
			last_weekend_string,
			PARKING_LOT_WHITEWATER_BAY );

	if ( ( trailer_count_string =
			dictionary_get_pointer(
				trailer_count_dictionary,
				trailer_count_key ) ) )
	{
		month_row->vessel.whitewater_bay_trailer_count_saturday =
			atoi( trailer_count_string );
	}

	/* Get whitewater_bay_trailer_count_sunday */
	/* --------------------------------------- */
	last_weekend_string =
		total_caught_get_last_weekend_string(
			month_row->vessel.row_date,
			0 /* not is_saturday */ );

	trailer_count_key =
		total_caught_get_trailer_key(
			last_weekend_string,
			PARKING_LOT_WHITEWATER_BAY );

	if ( ( trailer_count_string =
			dictionary_get_pointer(
				trailer_count_dictionary,
				trailer_count_key ) ) )
	{
		month_row->vessel.whitewater_bay_trailer_count_sunday =
			atoi( trailer_count_string );
	}

	/* Get florida_bay_trailer_count_day */
	/* --------------------------------- */
	trailer_count_key =
		total_caught_get_trailer_key(
	 		date_get_yyyy_mm_dd(	buffer,
						month_row->vessel.row_date ),
			PARKING_LOT_FLORIDA_BAY );

	if ( ( trailer_count_string =
			dictionary_get_pointer(
				trailer_count_dictionary,
				trailer_count_key ) ) )
	{
		month_row->vessel.florida_bay_trailer_count_day =
			atoi( trailer_count_string );
	}

	/* Get whitewater_bay_trailer_count_day */
	/* ------------------------------------ */
	trailer_count_key =
		total_caught_get_trailer_key(
	 		date_get_yyyy_mm_dd(	buffer,
						month_row->vessel.row_date ),
			PARKING_LOT_WHITEWATER_BAY );

	if ( ( trailer_count_string =
			dictionary_get_pointer(
				trailer_count_dictionary,
				trailer_count_key ) ) )
	{
		month_row->vessel.whitewater_bay_trailer_count_day =
			atoi( trailer_count_string );
	}

	/* ================== */
	/* Perform arithmetic */
	/* ================== */
	month_row->vessel.total_trailer_count_weekend =
		month_row->vessel.florida_bay_trailer_count_saturday +
		month_row->vessel.florida_bay_trailer_count_sunday +
		month_row->vessel.whitewater_bay_trailer_count_saturday +
		month_row->vessel.whitewater_bay_trailer_count_sunday;

	month_row->vessel.average_daily_trailer_count_weekend =
		(double)month_row->vessel.total_trailer_count_weekend /
		2.0;

	/* Calculate Total Trailer Count day (D) */
	/* ------------------------------------- */
	month_row->vessel.total_trailer_count_day =
		month_row->vessel.florida_bay_trailer_count_day +
		month_row->vessel.whitewater_bay_trailer_count_day;

	if ( month_row->vessel.average_daily_trailer_count_weekend )
	{
		month_row->vessel.weekday_trailer_count_ratio_day =
			(double)month_row->vessel.total_trailer_count_day /
			month_row->vessel.average_daily_trailer_count_weekend;
	}
	else
	{
		month_row->vessel.weekday_trailer_count_ratio_day = 1.0;
	}

	if ( !total_caught_get_coefficients(
				&areas_1_5_coefficient,
				&areas_1_5_y_intercept,
				&area_6_coefficient,
				&area_6_y_intercept,
				application_name ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get coefficients.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	/* Calculate (E) */
	/* ------------- */
	month_row->vessel.estimated_areas_1_5_vessels =
		areas_1_5_y_intercept +
		areas_1_5_coefficient *
		(double)month_row->
			vessel.florida_bay_trailer_count_day;

	/* Calculate (F) */
	/* ------------- */
	month_row->vessel.estimated_area_6_vessels =
		area_6_y_intercept +
		area_6_coefficient *
		(double)month_row->
			vessel.whitewater_bay_trailer_count_day;

	/* Calculate (G) */
	/* ------------- */
	month_row->vessel.estimated_areas_1_6_vessels =
		month_row->vessel.estimated_areas_1_5_vessels +
		month_row->vessel.estimated_area_6_vessels;

	total_caught_get_month_row_fishing_trip_weekend(
	  &month_row->fishing_trip.flamingo_fishing_trips_saturday,
	  &month_row->fishing_trip.flamingo_fishing_trips_sunday,
	  &month_row->fishing_trip.flamingo_pleasure_trips_saturday,
	  &month_row->fishing_trip.flamingo_pleasure_trips_sunday,
	  &month_row->fishing_trip.everglades_city_fishing_trips_saturday,
	  &month_row->fishing_trip.everglades_city_fishing_trips_sunday,
	  &month_row->fishing_trip.everglades_city_pleasure_trips_saturday,
	  &month_row->fishing_trip.everglades_city_pleasure_trips_sunday,
	  month_row->vessel.flamingo_interview_date_saturday,
	  month_row->vessel.flamingo_interview_date_sunday,
	  month_row->vessel.everglades_city_interview_date_saturday,
	  month_row->vessel.everglades_city_interview_date_sunday,
	  fishing_trip_list_array );

/*
	month_row->performed_flamingo_census_weekend =
		total_caught_get_performed_census_weekend(
			row_search_date,
			weekend_creel_census_dictionary,
			INTERVIEW_LOCATION_FLAMINGO );

	month_row->performed_everglades_city_census_weekend =
		total_caught_get_performed_census_weekend(
			row_search_date,
			weekend_creel_census_dictionary,
			INTERVIEW_LOCATION_EVERGLADES_CITY );
*/

	/* Calculate flamingo fishing trips day (H) */
	/* ---------------------------------------- */
	if (	month_row->performed_flamingo_census_weekend ==
		performed_census_saturday_only )
	{
		month_row->fishing_trip.flamingo_fishing_trips_day =
			month_row->fishing_trip.flamingo_fishing_trips_saturday;
	}
	else
	if (	month_row->performed_flamingo_census_weekend ==
		performed_census_sunday_only )
	{
		month_row->fishing_trip.flamingo_fishing_trips_day =
			month_row->fishing_trip.flamingo_fishing_trips_sunday;
	}
	else
	if (	month_row->performed_flamingo_census_weekend ==
		performed_census_saturday_and_sunday
	||  	month_row->performed_flamingo_census_weekend ==
		missed_census_weekend )
	{
		month_row->fishing_trip.flamingo_fishing_trips_weekend =
		month_row->fishing_trip.flamingo_fishing_trips_saturday +
		month_row->fishing_trip.flamingo_fishing_trips_sunday;

		month_row->fishing_trip.flamingo_fishing_trips_day =
		(double)month_row->fishing_trip.flamingo_fishing_trips_weekend /
		2.0;
	}

	/* Calculate flamingo boating trips day (I) */
	/* ---------------------------------------- */
	if (	month_row->performed_flamingo_census_weekend ==
		performed_census_saturday_only )
	{
		month_row->fishing_trip.flamingo_boating_trips_day =
		month_row->fishing_trip.flamingo_fishing_trips_saturday +
		month_row->fishing_trip.flamingo_pleasure_trips_saturday;
	}
	else
	if (	month_row->performed_flamingo_census_weekend ==
		performed_census_sunday_only )
	{
		month_row->fishing_trip.flamingo_boating_trips_day =
		month_row->fishing_trip.flamingo_fishing_trips_sunday +
		month_row->fishing_trip.flamingo_pleasure_trips_sunday;
	}
	else
	if (	month_row->performed_flamingo_census_weekend ==
		performed_census_saturday_and_sunday
	||  	month_row->performed_flamingo_census_weekend ==
		missed_census_weekend )
	{
		month_row->fishing_trip.flamingo_boating_trips_weekend =
		month_row->fishing_trip.flamingo_fishing_trips_saturday +
		month_row->fishing_trip.flamingo_pleasure_trips_saturday +
		month_row->fishing_trip.flamingo_fishing_trips_sunday +
		month_row->fishing_trip.flamingo_pleasure_trips_sunday;

		month_row->fishing_trip.flamingo_boating_trips_day =
		(double)month_row->fishing_trip.flamingo_boating_trips_weekend /
		2.0;
	}

	/* Calculate everglades city fishing trips day */
	/* ------------------------------------------- */
	if (	month_row->performed_everglades_city_census_weekend ==
		performed_census_saturday_only )
	{
		month_row->fishing_trip.everglades_city_fishing_trips_day =
		month_row->fishing_trip.everglades_city_fishing_trips_saturday;
	}
	else
	if (	month_row->performed_everglades_city_census_weekend ==
		performed_census_sunday_only )
	{
		month_row->fishing_trip.everglades_city_fishing_trips_day =
		month_row->fishing_trip.everglades_city_fishing_trips_sunday;
	}
	else
	if (	month_row->performed_everglades_city_census_weekend ==
		performed_census_saturday_and_sunday
	||  	month_row->performed_everglades_city_census_weekend ==
		missed_census_weekend )
	{
		month_row->fishing_trip.everglades_city_fishing_trips_weekend =
		month_row->fishing_trip.everglades_city_fishing_trips_saturday +
		month_row->fishing_trip.everglades_city_fishing_trips_sunday;

		month_row->fishing_trip.everglades_city_fishing_trips_day =
		(double)month_row->fishing_trip.
			everglades_city_fishing_trips_weekend /
			2.0;
	}

	/* Calculate everglades city boating trips day */
	/* ------------------------------------------- */
	if (	month_row->performed_everglades_city_census_weekend ==
		performed_census_saturday_only )
	{
		month_row->fishing_trip.everglades_city_boating_trips_day =
		month_row->fishing_trip.everglades_city_fishing_trips_saturday +
		month_row->fishing_trip.everglades_city_pleasure_trips_saturday;
	}
	else
	if (	month_row->performed_everglades_city_census_weekend ==
		performed_census_sunday_only )
	{
		month_row->fishing_trip.everglades_city_boating_trips_day =
		month_row->fishing_trip.everglades_city_fishing_trips_sunday +
		month_row->fishing_trip.everglades_city_pleasure_trips_sunday;
	}
	else
	if (	month_row->performed_everglades_city_census_weekend ==
		performed_census_saturday_and_sunday
	||  	month_row->performed_everglades_city_census_weekend ==
		missed_census_weekend )
	{
		month_row->fishing_trip.everglades_city_boating_trips_weekend =
		month_row->fishing_trip.everglades_city_fishing_trips_saturday +
		month_row->fishing_trip.
			everglades_city_pleasure_trips_saturday +
		month_row->fishing_trip.everglades_city_fishing_trips_sunday +
		month_row->fishing_trip.everglades_city_pleasure_trips_sunday;

		month_row->fishing_trip.everglades_city_boating_trips_day =
		(double)month_row->fishing_trip.
			everglades_city_boating_trips_weekend /
			2.0;
	}

	/* Calculate park fishing trips day (J) */
	/* ------------------------------------ */
	month_row->fishing_trip.park_fishing_trips_day =
		month_row->fishing_trip.flamingo_fishing_trips_day +
		month_row->fishing_trip.everglades_city_fishing_trips_day;

	/* Calculate park boating trips day (K) */
	/* ------------------------------------ */
	month_row->fishing_trip.park_boating_trips_day =
		month_row->fishing_trip.flamingo_boating_trips_day +
		month_row->fishing_trip.everglades_city_boating_trips_day;

	if ( month_row->fishing_trip.park_boating_trips_day )
	{
		month_row->fishing_trip.park_fishing_trips_ratio =
			month_row->fishing_trip.park_fishing_trips_day /
			month_row->fishing_trip.park_boating_trips_day;
	}

	/* Calculate estimated park fishing trips day (L) */
	/* ---------------------------------------------- */
	month_row->vessel.estimated_park_fishing_vessels =
		month_row->vessel.estimated_areas_1_6_vessels *
		month_row->fishing_trip.park_fishing_trips_ratio;

	if ( month_row->fishing_trip.flamingo_boating_trips_day )
	{
		month_row->vessel.estimated_areas_1_5_fishing_vessels =
		(double)month_row->vessel.estimated_areas_1_5_vessels *
		( month_row->fishing_trip.flamingo_fishing_trips_day /
		  month_row->fishing_trip.flamingo_boating_trips_day ) *
		month_row->vessel.weekday_trailer_count_ratio_day;
	}
	else
	{
		month_row->vessel.estimated_areas_1_5_fishing_vessels =
			0.0;
	}

	if ( month_row->fishing_trip.everglades_city_boating_trips_day )
	{
		month_row->vessel.estimated_area_6_fishing_vessels =
		(double)month_row->vessel.estimated_area_6_vessels *
		( month_row->fishing_trip.everglades_city_fishing_trips_day /
		  month_row->fishing_trip.everglades_city_boating_trips_day ) *
		month_row->vessel.weekday_trailer_count_ratio_day;
	}
	else
	{
		month_row->vessel.estimated_area_6_fishing_vessels = 0.0;
	}

	month_row->catch_area_list =
	   total_caught_get_catch_area_list(
		fishing_trip_list_array,
		month_row->vessel.flamingo_interview_date_saturday,
		month_row->vessel.flamingo_interview_date_sunday,
		month_row->vessel.everglades_city_interview_date_saturday,
		month_row->vessel.everglades_city_interview_date_sunday,
		species_list,
		month_row->vessel.weekday_trailer_count_ratio_day,
		month_row->fishing_trip.flamingo_fishing_trips_day,
		month_row->fishing_trip.everglades_city_fishing_trips_day,
		month_row->vessel.estimated_areas_1_5_fishing_vessels,
		month_row->vessel.estimated_area_6_fishing_vessels,
		month_row->performed_flamingo_census_weekend,
		month_row->performed_everglades_city_census_weekend );
			
	return month_row;

} /* total_caught_get_month_row() */

int total_caught_get_fishing_trips_weekend(
			DATE *search_date,
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			boolean is_fishing_trips,
			boolean is_flamingo )
{
	FISHING_TRIP *fishing_trip;
	int fishing_trips_weekend = 0;
	LIST *fishing_trip_list;

	if ( !search_date ) return 0;

	fishing_trip_list =
		total_caught_with_date_get_fishing_trip_list(
			fishing_trip_list_array,
			date_display( search_date ) );

	if ( !list_rewind( fishing_trip_list ) ) return 0;

	do {
		fishing_trip = list_get_pointer( fishing_trip_list );

		if ( is_fishing_trips && !fishing_trip->hours_fishing )
		{
			continue;
		}
		else
		if ( !is_fishing_trips && fishing_trip->hours_fishing )
		{
			continue;
		}
		else
		if ( is_flamingo
		&&   ( fishing_trip->fishing_area_integer < 1
		||     fishing_trip->fishing_area_integer > 5 ) )
		{
			continue;
		}
		else
		if ( !is_flamingo
		&&   fishing_trip->fishing_area_integer != 6 )
		{
			continue;
		}

		fishing_trips_weekend++;

	} while( list_next( fishing_trip_list ) );

	return fishing_trips_weekend;

} /* total_caught_get_fishing_trips_weekend() */

char *total_caught_get_last_weekend_string(
			DATE *search_date,
			boolean is_saturday )
{
	static char last_weekend_string[ 16 ];

	if ( is_saturday )
	{
		strcpy( last_weekend_string,
			date_last_day_of_week_yyyy_mm_dd_string(
					search_date,
					WDAY_SATURDAY ) );
	}
	else
	{
		if ( date_get_day_of_week( search_date ) == WDAY_SATURDAY )
		{
			DATE *tomorrow;
			tomorrow = date_new( 0, 0, 0 );
			date_copy( tomorrow, search_date );
			date_increment_days( tomorrow, 1.0 );

			strcpy( last_weekend_string,
				date_static_display_yyyy_mm_dd(
					tomorrow ) );
			date_free( tomorrow );
		}
		else
		{
			strcpy( last_weekend_string,
				date_last_day_of_week_yyyy_mm_dd_string(
						search_date,
						WDAY_SUNDAY ) );
		}
	}

	return last_weekend_string;

} /* total_caught_get_last_weekend_string() */

#ifdef NOT_DEFINED
char *total_caught_get_last_weekend_string(
			DATE *search_date,
			boolean is_saturday )
{
	static char last_weekend_string[ 16 ];

	if ( is_saturday )
	{
		strcpy( last_weekend_string,
			date_last_day_of_week_yyyy_mm_dd_string(
					search_date,
					WDAY_SATURDAY ) );
	}
	else
	{
		if ( date_get_day_of_week( search_date ) == WDAY_SATURDAY )
		{
			DATE *tomorrow;
			tomorrow = date_new( 0, 0, 0 );
			date_copy( tomorrow, search_date );
			date_increment_days( tomorrow, 1.0 );

			strcpy( last_weekend_string,
				date_static_display_yyyy_mm_dd(
					tomorrow ) );
			date_free( tomorrow );
		}
		else
		{
			strcpy( last_weekend_string,
				date_last_day_of_week_yyyy_mm_dd_string(
						search_date,
						WDAY_SUNDAY ) );
		}
	}

	return last_weekend_string;

} /* total_caught_get_last_weekend_string() */
#endif

LIST *total_caught_get_catch_area_list(
	FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
	DATE *flamingo_interview_date_saturday,
	DATE *flamingo_interview_date_sunday,
	DATE *everglades_city_interview_date_saturday,
	DATE *everglades_city_interview_date_sunday,
	LIST *species_list,
	double weekday_trailer_count_ratio_day,
	double flamingo_fishing_trips_day,
	double everglades_city_fishing_trips_day,
	double estimated_areas_1_5_fishing_vessels,
	double estimated_area_6_fishing_vessels,
	enum performed_census_weekend performed_flamingo_census_weekend,
	enum performed_census_weekend performed_everglades_city_census_weekend )
{
	CATCH_AREA *catch_area;
	SPECIES *species;
	LIST *catch_area_list;
	double average_per_fishing_vessel;
	LIST *flamingo_fishing_trip_list_saturday = {0};
	LIST *flamingo_fishing_trip_list_sunday = {0};
	LIST *everglades_city_fishing_trip_list_saturday = {0};
	LIST *everglades_city_fishing_trip_list_sunday = {0};

	if ( !list_rewind( species_list ) ) return (LIST *)0;

	catch_area_list = list_new();

	if ( flamingo_interview_date_saturday )
	{
		flamingo_fishing_trip_list_saturday =
			total_caught_with_date_get_fishing_trip_list(
				fishing_trip_list_array,
				date_display(
					flamingo_interview_date_saturday ) );
	}

	if ( flamingo_interview_date_sunday )
	{
		flamingo_fishing_trip_list_sunday =
			total_caught_with_date_get_fishing_trip_list(
				fishing_trip_list_array,
				date_display(
					flamingo_interview_date_sunday ) );
	}

	if ( everglades_city_interview_date_saturday )
	{
		everglades_city_fishing_trip_list_saturday =
			total_caught_with_date_get_fishing_trip_list(
				fishing_trip_list_array,
				date_display(
				everglades_city_interview_date_saturday ) );
	}

	if ( everglades_city_interview_date_sunday )
	{
		everglades_city_fishing_trip_list_sunday =
			total_caught_with_date_get_fishing_trip_list(
				fishing_trip_list_array,
				date_display(
				everglades_city_interview_date_sunday ) );
	}

	do {
		species = list_get_pointer( species_list );

		catch_area = total_caught_catch_area_new();
		catch_area->species = species;
		list_append_pointer( catch_area_list, catch_area );

		/* Areas 1-5 variables */
		/* ------------------- */
		if ( list_length( flamingo_fishing_trip_list_saturday ) )
		{
			catch_area->areas_1_5_sample_kept_saturday =
				total_caught_get_catches_weekend(
					flamingo_fishing_trip_list_saturday,
					species->family,
					species->genus,
					species->species_name,
					1 /* is_areas_1_5 */,
					1 /* is_kept */ );

			catch_area->areas_1_5_sample_released_saturday =
				total_caught_get_catches_weekend(
					flamingo_fishing_trip_list_saturday,
					species->family,
					species->genus,
					species->species_name,
					1 /* is_areas_1_5 */,
					0 /* not is_kept */ );
		}

		if ( list_length( flamingo_fishing_trip_list_sunday ) )
		{
			catch_area->areas_1_5_sample_kept_sunday =
				total_caught_get_catches_weekend(
					flamingo_fishing_trip_list_sunday,
					species->family,
					species->genus,
					species->species_name,
					1 /* is_areas_1_5 */,
					1 /* is_kept */ );

			catch_area->areas_1_5_sample_released_sunday =
				total_caught_get_catches_weekend(
					flamingo_fishing_trip_list_sunday,
					species->family,
					species->genus,
					species->species_name,
					1 /* is_areas_1_5 */,
					0 /* not is_kept */ );
		}

		/* Area 6 variables */
		/* ---------------- */
		if ( list_length( everglades_city_fishing_trip_list_saturday ) )
		{
			catch_area->area_6_sample_kept_saturday =
				total_caught_get_catches_weekend(
				everglades_city_fishing_trip_list_saturday,
				species->family,
				species->genus,
				species->species_name,
				0 /* not is_areas_1_5 */,
				1 /* is_kept */ );

			catch_area->area_6_sample_released_saturday =
				total_caught_get_catches_weekend(
				everglades_city_fishing_trip_list_saturday,
				species->family,
				species->genus,
				species->species_name,
				0 /* not is_areas_1_5 */,
				0 /* not is_kept */ );
		}

		if ( list_length( everglades_city_fishing_trip_list_sunday ) )
		{
			catch_area->area_6_sample_kept_sunday =
				total_caught_get_catches_weekend(
				everglades_city_fishing_trip_list_sunday,
				species->family,
				species->genus,
				species->species_name,
				0 /* not is_areas_1_5 */,
				1 /* is_kept */ );

			catch_area->area_6_sample_released_sunday =
				total_caught_get_catches_weekend(
				everglades_city_fishing_trip_list_sunday,
				species->family,
				species->genus,
				species->species_name,
				0 /* not is_areas_1_5 */,
				0 /* not is_kept */ );
		}

		/* Perform arithmetic areas 1-5 kept (M) */
		/* ------------------------------------- */
		if (	performed_flamingo_census_weekend ==
			performed_census_saturday_only )
		{
			catch_area->areas_1_5_sample_kept =
			(double)catch_area->areas_1_5_sample_kept_saturday *
			weekday_trailer_count_ratio_day;
		}
		else
		if (	performed_flamingo_census_weekend ==
			performed_census_sunday_only )
		{
			catch_area->areas_1_5_sample_kept =
			(double)catch_area->areas_1_5_sample_kept_sunday *
			weekday_trailer_count_ratio_day;
		}
		else
		if (	performed_flamingo_census_weekend ==
			performed_census_saturday_and_sunday
		||	performed_flamingo_census_weekend ==
			missed_census_weekend )
		{
			catch_area->areas_1_5_sample_kept_weekend =
			catch_area->areas_1_5_sample_kept_saturday +
			catch_area->areas_1_5_sample_kept_sunday;

			catch_area->areas_1_5_sample_kept =
			((double)catch_area->areas_1_5_sample_kept_weekend /
			2.0) *
			weekday_trailer_count_ratio_day;
		}

		/* Perform arithmetic areas 1-5 released (N) */
		/* ----------------------------------------- */
		if (	performed_flamingo_census_weekend ==
			performed_census_saturday_only )
		{
			catch_area->areas_1_5_sample_released =
			(double)catch_area->areas_1_5_sample_released_saturday *
			weekday_trailer_count_ratio_day;
		}
		else
		if (	performed_flamingo_census_weekend ==
			performed_census_sunday_only )
		{
			catch_area->areas_1_5_sample_released =
			(double)catch_area->areas_1_5_sample_released_sunday *
			weekday_trailer_count_ratio_day;
		}
		else
		if (	performed_flamingo_census_weekend ==
			performed_census_saturday_and_sunday
		||	performed_flamingo_census_weekend ==
			missed_census_weekend )
		{
			catch_area->areas_1_5_sample_released_weekend =
			catch_area->areas_1_5_sample_released_saturday +
			catch_area->areas_1_5_sample_released_sunday;

			catch_area->areas_1_5_sample_released =
			((double)catch_area->areas_1_5_sample_released_weekend /
			2.0) *
			weekday_trailer_count_ratio_day;
		}

		/* Perform arithmetic areas 1-5 caught (O) */
		/* --------------------------------------- */
		catch_area->areas_1_5_sample_caught =
			catch_area->areas_1_5_sample_kept +
			catch_area->areas_1_5_sample_released;

		/* Perform arithmetic areas 1-6 kept (P) */
		/* ------------------------------------- */
		if (	performed_everglades_city_census_weekend ==
			performed_census_saturday_only )
		{
			catch_area->area_6_sample_kept =
			(double)catch_area->area_6_sample_kept_saturday *
			weekday_trailer_count_ratio_day;
		}
		else
		if (	performed_everglades_city_census_weekend ==
			performed_census_sunday_only )
		{
			catch_area->area_6_sample_kept =
			(double)catch_area->area_6_sample_kept_sunday *
			weekday_trailer_count_ratio_day;
		}
		else
		if (	performed_everglades_city_census_weekend ==
			performed_census_saturday_and_sunday
		||	performed_everglades_city_census_weekend ==
			missed_census_weekend )
		{
			catch_area->area_6_sample_kept_weekend =
			catch_area->area_6_sample_kept_saturday +
			catch_area->area_6_sample_kept_sunday;

			catch_area->area_6_sample_kept =
			((double)catch_area->area_6_sample_kept_weekend /
			2.0) *
			weekday_trailer_count_ratio_day;
		}

		catch_area->areas_1_6_sample_kept =
			catch_area->areas_1_5_sample_kept +
			catch_area->area_6_sample_kept;

		/* Perform arithmetic areas 1-6 released (Q) */
		/* ----------------------------------------- */
		if (	performed_everglades_city_census_weekend ==
			performed_census_saturday_only )
		{
			catch_area->area_6_sample_released =
			(double)catch_area->area_6_sample_released_saturday *
			weekday_trailer_count_ratio_day;
		}
		else
		if (	performed_everglades_city_census_weekend ==
			performed_census_sunday_only )
		{
			catch_area->area_6_sample_released =
			(double)catch_area->area_6_sample_released_sunday *
			weekday_trailer_count_ratio_day;
		}
		else
		if (	performed_everglades_city_census_weekend ==
			performed_census_saturday_and_sunday
		||	performed_everglades_city_census_weekend ==
			missed_census_weekend )
		{
			catch_area->area_6_sample_released_weekend =
			catch_area->area_6_sample_released_saturday +
			catch_area->area_6_sample_released_sunday;

			catch_area->area_6_sample_released =
			((double)catch_area->area_6_sample_released_weekend /
			2.0) *
			weekday_trailer_count_ratio_day;
		}

		catch_area->areas_1_6_sample_released =
			catch_area->areas_1_5_sample_released +
			catch_area->area_6_sample_released;

		/* Perform arithmetic areas 1-6 caught (R) */
		/* --------------------------------------- */
		catch_area->areas_1_6_sample_caught =
			catch_area->areas_1_6_sample_kept +
			catch_area->areas_1_6_sample_released;

		/* Perform arithmetic areas 1-5 estimated total kept (S) */
		/* ----------------------------------------------------- */
		if ( flamingo_fishing_trips_day )
		{
			average_per_fishing_vessel =
				catch_area->areas_1_5_sample_kept /
				flamingo_fishing_trips_day;
		}
		else
		{
			average_per_fishing_vessel = 0.0;
		}

		catch_area->areas_1_5_estimated_total_kept =
			estimated_areas_1_5_fishing_vessels *
			average_per_fishing_vessel;

		/* Perform arithmetic areas 1-5 estimated total released (T) */
		/* --------------------------------------------------------- */
		if ( flamingo_fishing_trips_day )
		{
			average_per_fishing_vessel =
				catch_area->areas_1_5_sample_released /
				flamingo_fishing_trips_day;
		}
		else
		{
			average_per_fishing_vessel = 0.0;
		}

		catch_area->areas_1_5_estimated_total_released =
			estimated_areas_1_5_fishing_vessels *
			average_per_fishing_vessel;

		/* Perform arithmetic areas 1-5 estimated total caught (U) */
		/* ------------------------------------------------------- */
		catch_area->areas_1_5_estimated_total_caught =
			catch_area->areas_1_5_estimated_total_kept +
			catch_area->areas_1_5_estimated_total_released;

		/* Perform arithmetic area 6 estimated total kept */
		/* ---------------------------------------------- */
		if ( everglades_city_fishing_trips_day )
		{
			average_per_fishing_vessel =
				catch_area->area_6_sample_kept /
				everglades_city_fishing_trips_day;
		}
		else
		{
			average_per_fishing_vessel = 0.0;
		}

		catch_area->area_6_estimated_total_kept =
			estimated_area_6_fishing_vessels *
			average_per_fishing_vessel;

		/* Perform arithmetic areas 1-6 estimated total kept (V) */
		/* ----------------------------------------------------- */
		catch_area->areas_1_6_estimated_total_kept =
			catch_area->areas_1_5_estimated_total_kept +
			catch_area->area_6_estimated_total_kept;

		/* Perform arithmetic area 6 estimated total released */
		/* -------------------------------------------------- */
		if ( everglades_city_fishing_trips_day )
		{
			average_per_fishing_vessel =
				catch_area->area_6_sample_released /
				everglades_city_fishing_trips_day;
		}
		else
		{
			average_per_fishing_vessel = 0.0;
		}

		catch_area->area_6_estimated_total_released =
			estimated_area_6_fishing_vessels *
			average_per_fishing_vessel;

		/* Perform arithmetic areas 1-6 estimated total released (W) */
		/* --------------------------------------------------------- */
		catch_area->areas_1_6_estimated_total_released =
			catch_area->areas_1_5_estimated_total_released +
			catch_area->area_6_estimated_total_released;

		/* Perform arithmetic areas 1-6 estimated total caught (X) */
		/* ------------------------------------------------------- */
		catch_area->areas_1_6_estimated_total_caught =
			catch_area->areas_1_6_estimated_total_kept +
			catch_area->areas_1_6_estimated_total_released;

	} while( list_next( species_list ) );

	return catch_area_list;

} /* total_caught_get_catch_area_list() */

SPECIES *total_caught_seek_species(
					LIST *species_list,
					char *family,
					char *genus,
					char *species_name )
{
	SPECIES *species;

	if ( list_rewind( species_list ) )
	{
		do {
			species = list_get_pointer( species_list );

			if ( strcmp( species->family, family ) == 0
			&&   strcmp( species->genus, genus ) == 0
			&&   strcmp( species->species_name, species_name ) == 0)
			{
				return species;
			}
		} while( list_next( species_list ) );
	}

	return (SPECIES *)0;
} /* total_caught_seek_species() */

CATCH_AREA *total_caught_get_or_set_catch_area(
					LIST *catch_area_list,
					char *family,
					char *genus,
					char *species_name,
					SPECIES *species)
{
	CATCH_AREA *catch_area;

	if ( list_rewind( catch_area_list ) )
	{
		do {
			catch_area = list_get_pointer( catch_area_list );

			if ( strcmp(	catch_area->species->family,
					family ) == 0
			&&   strcmp(	catch_area->species->genus,
					genus ) == 0
			&&   strcmp(	catch_area->species->species_name,
					species_name ) == 0 )
			{
				return catch_area;
			}
		} while( list_next( catch_area_list ) );
	}

	catch_area = total_caught_catch_area_new();
	catch_area->species = species;
	list_append_pointer( catch_area_list, catch_area );
	return catch_area;
	
} /* total_caught_get_or_set_catch_area() */

MONTH_ROW *total_caught_get_month_sheet_total_row(
			LIST *month_sheet_row_list )
{
	MONTH_ROW *total_row;
	MONTH_ROW *month_row;
	CATCH_AREA *total_row_catch_area;
	CATCH_AREA *month_row_catch_area;

	if ( !list_rewind( month_sheet_row_list ) ) return (MONTH_ROW *)0;

	total_row = total_caught_month_row_new( 0 );
	total_row->catch_area_list = list_new();

	do {
		month_row = list_get_pointer( month_sheet_row_list );

		total_row->vessel.florida_bay_trailer_count_day +=
			month_row->vessel.florida_bay_trailer_count_day;

		total_row->vessel.whitewater_bay_trailer_count_day +=
			month_row->vessel.whitewater_bay_trailer_count_day;

		total_row->vessel.total_trailer_count_day +=
			month_row->vessel.total_trailer_count_day;

		total_row->vessel.estimated_areas_1_5_vessels +=
			month_row->vessel.estimated_areas_1_5_vessels;

		total_row->vessel.estimated_area_6_vessels +=
			month_row->vessel.estimated_area_6_vessels;

		total_row->vessel.estimated_areas_1_6_vessels +=
			month_row->vessel.estimated_areas_1_6_vessels;

		total_row->vessel.estimated_areas_1_5_fishing_vessels +=
		    month_row->vessel.estimated_areas_1_5_fishing_vessels;

		total_row->vessel.estimated_area_6_fishing_vessels +=
		    month_row->vessel.estimated_area_6_fishing_vessels;

		total_row->fishing_trip.flamingo_fishing_trips_day +=
			month_row->fishing_trip.flamingo_fishing_trips_day;

		total_row->fishing_trip.flamingo_boating_trips_day +=
			month_row->fishing_trip.flamingo_boating_trips_day;

		total_row->fishing_trip.park_fishing_trips_day +=
			month_row->fishing_trip.park_fishing_trips_day;

		total_row->fishing_trip.park_boating_trips_day +=
			month_row->fishing_trip.park_boating_trips_day;

		total_row->vessel.estimated_park_fishing_vessels +=
			month_row->vessel.estimated_park_fishing_vessels;

		if ( !list_rewind( month_row->catch_area_list ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: empty month row catch area list.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		do {
			month_row_catch_area =
				list_get_pointer(
					month_row->catch_area_list );

			total_row_catch_area =
				total_caught_get_or_set_catch_area(
				total_row->catch_area_list,
				month_row_catch_area->species->family,
				month_row_catch_area->species->genus,
				month_row_catch_area->species->
					species_name,
				month_row_catch_area->species );

			total_row_catch_area->
				areas_1_5_estimated_total_kept +=
				month_row_catch_area->
						areas_1_5_estimated_total_kept;

			total_row_catch_area->
				areas_1_5_estimated_total_released +=
				month_row_catch_area->
				     areas_1_5_estimated_total_released;

			total_row_catch_area->
				areas_1_5_estimated_total_caught +=
				month_row_catch_area->
				     areas_1_5_estimated_total_caught;

			total_row_catch_area->
				areas_1_6_estimated_total_kept +=
				month_row_catch_area->
					areas_1_6_estimated_total_kept;

			total_row_catch_area->
				areas_1_6_estimated_total_released +=
				month_row_catch_area->
				     areas_1_6_estimated_total_released;

			total_row_catch_area->
				areas_1_6_estimated_total_caught +=
				month_row_catch_area->
				     areas_1_6_estimated_total_caught;

		} while( list_next( month_row->catch_area_list ) );
	} while( list_next( month_sheet_row_list ) );

	return total_row;
} /* total_caught_get_month_sheet_total_row() */

void total_caught_get_month_row_fishing_trip_weekend(
	  int *flamingo_fishing_trips_saturday,
	  int *flamingo_fishing_trips_sunday,
	  int *flamingo_pleasure_trips_saturday,
	  int *flamingo_pleasure_trips_sunday,
	  int *everglades_city_fishing_trips_saturday,
	  int *everglades_city_fishing_trips_sunday,
	  int *everglades_city_pleasure_trips_saturday,
	  int *everglades_city_pleasure_trips_sunday,
	  DATE *flamingo_interview_date_saturday,
	  DATE *flamingo_interview_date_sunday,
	  DATE *everglades_city_interview_date_saturday,
	  DATE *everglades_city_interview_date_sunday,
	  FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ] )
{
	if ( flamingo_interview_date_saturday )
	{
		*flamingo_fishing_trips_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				1 /* is_flamingo */ );

		*flamingo_pleasure_trips_saturday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				1 /* is_flamingo */ );
	}

	if ( flamingo_interview_date_sunday )
	{
		*flamingo_fishing_trips_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trips */,
				1 /* is_flamingo */ );

		*flamingo_pleasure_trips_sunday =
			total_caught_get_fishing_trips_weekend(
				flamingo_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trips */,
				1 /* is_flamingo */ );
	}

	if ( everglades_city_interview_date_saturday )
	{
		*everglades_city_fishing_trips_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				1 /* is_fishing_trip */,
				0 /* not is_flamingo */ );

		*everglades_city_pleasure_trips_saturday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_saturday,
				fishing_trip_list_array,
				0 /* not is_fishing_trip */,
				0 /* not is_flamingo */ );
	}

	if ( everglades_city_interview_date_sunday )
	{
		*everglades_city_fishing_trips_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				1 /* is_fishing_trips */,
				0 /* not is_flamingo */ );

		*everglades_city_pleasure_trips_sunday =
			total_caught_get_fishing_trips_weekend(
				everglades_city_interview_date_sunday,
				fishing_trip_list_array,
				0 /* not is_fishing_trips */,
				0 /* not is_flamingo */ );
	}

} /* total_caught_get_month_row_fishing_trip_weekend() */

int total_caught_get_catches_weekend(
			LIST *fishing_trip_list,
			char *family,
			char *genus,
			char *species_name,
			boolean is_areas_1_5,
			boolean is_kept )
{
	FISHING_TRIP *fishing_trip;
	CATCH *catch;
	int catches_weekend = 0;

	if ( !list_rewind( fishing_trip_list ) )
	{
		fprintf(stderr,
			"Warning in %s/%s()/%d: empty fishing_trip_list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		return 0;
	}

	do {
		fishing_trip = list_get_pointer( fishing_trip_list );

		if ( is_areas_1_5
		&&   ( fishing_trip->fishing_area_integer < 1
		||     fishing_trip->fishing_area_integer > 5 ) )
		{
			continue;
		}

		if ( !is_areas_1_5
		&&   fishing_trip->fishing_area_integer != 6 )
		{
			continue;
		}

		if ( !list_rewind( fishing_trip->catch_list ) )
		{
			continue;
		}

		do {
			catch = list_get_pointer( fishing_trip->catch_list );

			if ( strcmp(	catch->species->family,
					family ) != 0
			||   strcmp(	catch->species->genus,
					genus ) != 0
			||   strcmp(	catch->species->species_name,
					species_name ) != 0 )
			{
				continue;
			}

			if ( is_kept )
				catches_weekend += catch->kept;
			else
				catches_weekend += catch->released;

		} while( list_next( fishing_trip->catch_list ) );

	} while( list_next( fishing_trip_list ) );

	return catches_weekend;

} /* total_caught_get_catches_weekend() */

TOTAL_SHEET *total_caught_get_total_sheet(
			LIST *month_sheet_list,
			int begin_month,
			int year )
{
	TOTAL_SHEET *total_sheet;
	MONTH_SHEET *month_sheet;
	TOTAL_ROW *total_row;
	int month_integer;

	if ( !list_rewind( month_sheet_list ) ) return (TOTAL_SHEET *)0;

	month_integer = begin_month;
	total_sheet = total_caught_total_sheet_new( year );

	do {
		month_sheet = list_get_pointer( month_sheet_list );

		total_row = total_caught_total_row_new(
				timlib_integer2full_month( month_integer ) );

		total_caught_populate_total_row(
					total_row,
					month_sheet->row_list );

		list_append_pointer( total_sheet->total_row_list, total_row );
		month_integer++;
	} while( list_next( month_sheet_list ) );

	if ( ! ( total_sheet->total_row =
		total_caught_get_total_sheet_total_row(
			total_sheet->total_row_list ) ) )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: empty total sheet row list.\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return total_sheet;
} /* total_caught_get_total_sheet() */

TOTAL_ROW *total_caught_total_row_new( char *month )
{
	TOTAL_ROW *total_row;

	if ( ! ( total_row = calloc( 1, sizeof( TOTAL_ROW ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}
	total_row->month = month;
	return total_row;

} /* total_caught_total_row_new() */

void total_caught_populate_total_row(	TOTAL_ROW *total_row,
					LIST *month_row_list )
{
	MONTH_ROW *month_row;
	CATCH_AREA *catch_area;
	CATCH_AREA *month_row_catch_area;

	total_row->catch_area_list = list_new();

	if ( !list_rewind( month_row_list ) ) return;

	do {
		month_row = list_get_pointer( month_row_list );

		total_row->florida_bay_trailer_count_day +=
		month_row->vessel.florida_bay_trailer_count_day;

		total_row->whitewater_bay_trailer_count_day +=
		month_row->vessel.whitewater_bay_trailer_count_day;

		total_row->total_trailer_count_day +=
		month_row->vessel.total_trailer_count_day;

		total_row->estimated_areas_1_5_vessels +=
		month_row->vessel.estimated_areas_1_5_vessels;

		total_row->estimated_area_6_vessels +=
		month_row->vessel.estimated_area_6_vessels;

		total_row->estimated_areas_1_6_vessels +=
		month_row->vessel.estimated_areas_1_6_vessels;

		total_row->flamingo_fishing_trips_day +=
		month_row->fishing_trip.flamingo_fishing_trips_day;

		total_row->flamingo_boating_trips_day +=
		month_row->fishing_trip.flamingo_boating_trips_day;

		total_row->park_fishing_trips_day +=
		month_row->fishing_trip.park_fishing_trips_day;

		total_row->park_boating_trips_day +=
		month_row->fishing_trip.park_boating_trips_day;

		total_row->estimated_park_fishing_vessels +=
		month_row->vessel.estimated_park_fishing_vessels;

/*
		total_row->areas_1_6_estimated_total_fishing_vessels +=
			( month_row->
				vessel.
				areas_1_5_estimated_total_fishing_vessels +
			  month_row->
				vessel.
				area_6_estimated_total_fishing_vessels );
*/

		if ( !list_rewind( month_row->catch_area_list ) )
		{
			fprintf(stderr,
	"ERROR in %s/%s()/%d: empty month row catch area list.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		do {
			month_row_catch_area =
				list_get_pointer(
					month_row->catch_area_list );

			catch_area =
			total_caught_get_or_set_catch_area(
				total_row->catch_area_list,
				month_row_catch_area->species->family,
				month_row_catch_area->species->genus,
				month_row_catch_area->species->species_name,
				month_row_catch_area->species );

			catch_area->areas_1_5_estimated_total_kept +=
			month_row_catch_area->areas_1_5_estimated_total_kept;

			catch_area->areas_1_5_estimated_total_released +=
			month_row_catch_area->
				areas_1_5_estimated_total_released;

			catch_area->areas_1_5_estimated_total_caught +=
			month_row_catch_area->areas_1_5_estimated_total_caught;

			catch_area->areas_1_6_estimated_total_kept +=
			month_row_catch_area->areas_1_6_estimated_total_kept;

			catch_area->areas_1_6_estimated_total_released +=
			month_row_catch_area->
				areas_1_6_estimated_total_released;

			catch_area->areas_1_6_estimated_total_caught +=
			month_row_catch_area->areas_1_6_estimated_total_caught;

		} while( list_next( month_row->catch_area_list ) );
	} while( list_next( month_row_list ) );

} /* total_caught_populate_total_row() */

TOTAL_ROW *total_caught_get_total_sheet_total_row(
			LIST *total_row_list )
{
	TOTAL_ROW *total_sheet_total_row;
	TOTAL_ROW *total_row;
	CATCH_AREA *total_sheet_total_row_catch_area;
	CATCH_AREA *total_row_catch_area;

	if ( !list_rewind( total_row_list ) ) return (TOTAL_ROW *)0;

	total_sheet_total_row = total_caught_total_row_new( "Total" );
	total_sheet_total_row->catch_area_list = list_new();

	do {
		total_row = list_get_pointer( total_row_list );

		total_sheet_total_row->florida_bay_trailer_count_day +=
			total_row->florida_bay_trailer_count_day;

		total_sheet_total_row->whitewater_bay_trailer_count_day +=
			total_row->whitewater_bay_trailer_count_day;

		total_sheet_total_row->total_trailer_count_day +=
			total_row->total_trailer_count_day;

		total_sheet_total_row->estimated_areas_1_5_vessels +=
			total_row->estimated_areas_1_5_vessels;

		total_sheet_total_row->estimated_area_6_vessels +=
			total_row->estimated_area_6_vessels;

		total_sheet_total_row->estimated_areas_1_6_vessels +=
			total_row->estimated_areas_1_6_vessels;

		total_sheet_total_row->flamingo_fishing_trips_day +=
			total_row->flamingo_fishing_trips_day;

		total_sheet_total_row->flamingo_boating_trips_day +=
			total_row->flamingo_boating_trips_day;

		total_sheet_total_row->park_fishing_trips_day +=
			total_row->park_fishing_trips_day;

		total_sheet_total_row->park_boating_trips_day +=
			total_row->park_boating_trips_day;

		total_sheet_total_row->estimated_park_fishing_vessels +=
		total_row->estimated_park_fishing_vessels;

/*
		total_sheet_total_row->
			areas_1_6_estimated_total_fishing_vessels +=
			total_row->
				areas_1_6_estimated_total_fishing_vessels;
*/

		if ( !list_rewind( total_row->catch_area_list ) )
		{
			fprintf(stderr,
		"ERROR in %s/%s()/%d: empty total row catch area list.\n",
				__FILE__,
				__FUNCTION__,
				__LINE__ );
			exit( 1 );
		}

		do {
			total_row_catch_area =
				list_get_pointer(
					total_row->catch_area_list );

			total_sheet_total_row_catch_area =
				total_caught_get_or_set_catch_area(
				total_sheet_total_row->catch_area_list,
				total_row_catch_area->species->family,
				total_row_catch_area->species->genus,
				total_row_catch_area->species->
					species_name,
				total_row_catch_area->species );

			total_sheet_total_row_catch_area->
				areas_1_5_estimated_total_kept +=
				total_row_catch_area->
						areas_1_5_estimated_total_kept;

			total_sheet_total_row_catch_area->
				areas_1_5_estimated_total_released +=
				total_row_catch_area->
				     areas_1_5_estimated_total_released;

			total_sheet_total_row_catch_area->
				areas_1_5_estimated_total_caught +=
				total_row_catch_area->
				     areas_1_5_estimated_total_caught;

			total_sheet_total_row_catch_area->
				areas_1_6_estimated_total_kept +=
				total_row_catch_area->
					areas_1_6_estimated_total_kept;

			total_sheet_total_row_catch_area->
				areas_1_6_estimated_total_released +=
				total_row_catch_area->
				     areas_1_6_estimated_total_released;

			total_sheet_total_row_catch_area->
				areas_1_6_estimated_total_caught +=
				total_row_catch_area->
				     areas_1_6_estimated_total_caught;

		} while( list_next( total_row->catch_area_list ) );
	} while( list_next( total_row_list ) );

	return total_sheet_total_row;
} /* total_caught_get_total_sheet_total_row() */

void total_caught_populate_grand_totals(
			double *grand_areas_1_5_total_kept,
			double *grand_areas_1_5_total_released,
			double *grand_areas_1_5_total_caught,
			double *grand_areas_1_6_total_kept,
			double *grand_areas_1_6_total_released,
			double *grand_areas_1_6_total_caught,
			LIST *catch_area_list )
{
	CATCH_AREA *catch_area;

	if ( !list_rewind( catch_area_list ) ) return;

	do {
		catch_area = list_get_pointer( catch_area_list );

		*grand_areas_1_5_total_kept +=
			catch_area->areas_1_5_estimated_total_kept;

		*grand_areas_1_5_total_released +=
			catch_area->areas_1_5_estimated_total_released;

		*grand_areas_1_5_total_caught +=
			catch_area->areas_1_5_estimated_total_caught;

		*grand_areas_1_6_total_kept +=
			catch_area->areas_1_6_estimated_total_kept;

		*grand_areas_1_6_total_released +=
			catch_area->areas_1_6_estimated_total_released;

		*grand_areas_1_6_total_caught +=
			catch_area->areas_1_6_estimated_total_caught;

	} while( list_next( catch_area_list ) );

} /* total_caught_populate_grand_totals() */

enum performed_census_weekend total_caught_get_enum_performed_census_weekend(
				DATE *row_date,
				DICTIONARY *weekend_creel_census_dictionary,
				char *interview_location )
{
	char *last_weekend_string;
	boolean exists_saturday;
	boolean exists_sunday;
	char *key;

	last_weekend_string =
		total_caught_get_last_weekend_string(
			row_date,
			1 /* is_saturday */ );

	key = total_caught_get_weekend_key(
			last_weekend_string,
			interview_location );

	exists_saturday =
		(boolean)dictionary_fetch(
			weekend_creel_census_dictionary,
			key );

	last_weekend_string =
		total_caught_get_last_weekend_string(
			row_date,
			0 /* not is_saturday */ );

	key = total_caught_get_weekend_key(
			last_weekend_string,
			interview_location );

	exists_sunday =
		(boolean)dictionary_fetch(
			weekend_creel_census_dictionary,
			key );

	if ( exists_saturday && exists_sunday )
	{
		return performed_census_saturday_and_sunday;
	}
	else
	if ( exists_saturday )
	{
		return performed_census_saturday_only;
	}
	else
	if ( exists_sunday )
	{
		return performed_census_sunday_only;
	}
	else
	{
		return missed_census_weekend;
	}

} /* total_caught_get_enum_performed_census_weekend() */

enum performed_census_weekend total_caught_get_performed_census_weekend(
			DATE **interview_date_saturday,
			DATE **interview_date_sunday,
			DATE *row_date,
			DICTIONARY *weekend_creel_census_dictionary,
			char *interview_location )
{
	enum performed_census_weekend performed_census_weekend;

	performed_census_weekend =
		total_caught_get_enum_performed_census_weekend(
			row_date,
			weekend_creel_census_dictionary,
			interview_location );

	if ( performed_census_weekend != performed_census_sunday_only )
	{
		*interview_date_saturday =
			total_caught_get_interview_weekend_date(
				row_date,
				weekend_creel_census_dictionary,
				interview_location,
				1 /* is_saturday */ );
	}

	if ( performed_census_weekend != performed_census_saturday_only )
	{
		*interview_date_sunday =
			total_caught_get_interview_weekend_date(
				row_date,
				weekend_creel_census_dictionary,
				interview_location,
				0 /* not is_saturday */ );
	}

	return performed_census_weekend;

} /* total_caught_get_performed_census_weekend() */

DATE *total_caught_get_interview_weekend_date(
				DATE *row_date,
				DICTIONARY *weekend_creel_census_dictionary,
				char *interview_location,
				boolean is_saturday )
{
	enum performed_census_weekend performed_census_weekend;
	int i;
	char *last_weekend_string = {0};
	DATE *interview_weekend_date = {0};

	interview_weekend_date = date_new( 0, 0, 0 );
	date_copy( interview_weekend_date, row_date );

	for( i = 0; i < WEEKENDS_TO_GO_BACK; i++ )
	{
		performed_census_weekend =
			total_caught_get_enum_performed_census_weekend(
				interview_weekend_date,
				weekend_creel_census_dictionary,
				interview_location );

		if ( performed_census_weekend != missed_census_weekend )
		{
			break;
		}
		date_decrement_days( interview_weekend_date, 7.0 );
	}

	last_weekend_string =
		total_caught_get_last_weekend_string(
			row_date,
			is_saturday );

	if ( last_weekend_string )
	{
		interview_weekend_date =
			date_yyyy_mm_dd_new( last_weekend_string );
	}

	return interview_weekend_date;

} /* total_caught_get_interview_weekend_date() */

FISHING_TRIP_LIST *total_caught_fishing_trip_list_new( void )
{
	FISHING_TRIP_LIST *fishing_trip_list;

	if ( ! ( fishing_trip_list =
			calloc( 1, sizeof( FISHING_TRIP_LIST ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	fishing_trip_list->fishing_trip_list = list_new();
	return fishing_trip_list;
} /* total_caught_fishing_trip_list_new() */

boolean	total_caught_get_census_date_month_day(
			int *census_date_month,
			int *census_date_day,
			char *census_date_string )
{
	char piece_buffer[ 128 ];

	if ( character_count( '-', census_date_string ) != 2 )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: invalid census_date_string = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 census_date_string );
		return 0;
	}

	/* Get census date month */
	/* --------------------- */
	piece( piece_buffer, '-', census_date_string, 1 );
	*census_date_month = atoi( piece_buffer );

	if ( *census_date_month < 1
	||   *census_date_month > MONTHS_PER_YEAR )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: invalid census_date_string = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 census_date_string );
		return 0;
	}

	/* Get census date day */
	/* ------------------- */
	piece( piece_buffer, '-', census_date_string, 2 );
	*census_date_day = atoi( piece_buffer );

	if ( *census_date_day < 1
	||   *census_date_day > DAYS_PER_MONTH )
	{
		fprintf( stderr,
		"Error in %s/%s()/%d: invalid census_date = (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 census_date_string );
		return 0;
	}

	return 1;

} /* total_caught_get_census_date_month_day() */

LIST *total_caught_with_date_get_fishing_trip_list(
			FISHING_TRIP_LIST *fishing_trip_list_array
						[ MONTHS_PER_YEAR ]
						[ DAYS_PER_MONTH ],
			char *date_string )
{
	int census_date_month;
	int census_date_day;

	if ( !total_caught_get_census_date_month_day(
		&census_date_month,
		&census_date_day,
		date_string ) )
	{
		return (LIST *)0;
	}

	if ( !fishing_trip_list_array	[ census_date_month - 1 ]
					[ census_date_day - 1 ] )
	{
		return (LIST *)0;
	}

	return fishing_trip_list_array	[ census_date_month - 1 ]
					[ census_date_day - 1 ]->
			fishing_trip_list;

} /* total_caught_with_date_get_fishing_trip_list() */

boolean total_caught_get_coefficients(
				double *areas_1_5_coefficient,
				double *areas_1_5_y_intercept,
				double *area_6_coefficient,
				double *area_6_y_intercept,
				char *application_name )
{
	APPLICATION_CONSTANTS *application_constants;
	char *value_string;

	application_constants = application_constants_new();

	application_constants->dictionary =
			application_constants_get_dictionary(
				application_name );

	if ( ! ( value_string =
			application_constants_fetch(
				application_constants->dictionary,
				"areas_1_5_coefficient" ) ) )
	{
		return 0;
	}
	*areas_1_5_coefficient = atof( value_string );

	if ( ! ( value_string =
			application_constants_fetch(
				application_constants->dictionary,
				"areas_1_5_y_intercept" ) ) )
	{
		return 0;
	}
	*areas_1_5_y_intercept = atof( value_string );

	if ( ! ( value_string =
			application_constants_fetch(
				application_constants->dictionary,
				"area_6_coefficient" ) ) )
	{
		return 0;
	}
	*area_6_coefficient = atof( value_string );

	if ( ! ( value_string =
			application_constants_fetch(
				application_constants->dictionary,
				"area_6_y_intercept" ) ) )
	{
		return 0;
	}
	*area_6_y_intercept = atof( value_string );

	application_constants_free( application_constants );
	return 1;

} /* total_caught_get_coefficients() */

