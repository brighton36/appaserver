/* src_everglades/hydrology_library_dvr.e */
/* -------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "hydrology_library.h"
#include "expected_count.h"
#include "list.h"

int main( void )
{
	LIST *expected_count_list;
	int return_value;
	char *begin_date = "2000-01-29";
	char *begin_time = "0000";
	char *end_date = "2000-03-03";
	char *end_time = "2200";

	expected_count_list =
		expected_count_get_expected_count_list(
				"everglades",
				"1AAA",
				"stage" );

	if ( expected_count_invalid_collection_frequency_date(
			expected_count_list,
			begin_date ) )
	{
		fprintf( stderr,
			 "ERROR: data collection frequency starts after %s\n",
			 begin_date );
		exit( 1 );
	}

	return_value = hydrology_library_insert_null_measurements(
					"1AAA",
					"stage",
					begin_date,
					begin_time,
					end_date,
					end_time,
					(char *)0 /* reason_value_missing */,
					0 /* minutes_offset */,
					'n' /* really_yn */,
					(char *)0 /* process_title */,
					0 /* not with_table_output */,
					"everglades" );
	if ( !return_value ) fprintf( stderr, "FAILED\n" );
	return 0;
}

