/* ---------------------------------------------------------	*/
/* src_hydrology/measurement_adjust_time_to_sequence.c		*/
/* ---------------------------------------------------------	*/
/* Freely available software: see Appaserver.org		*/
/* ---------------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "query.h"
#include "date.h"
#include "basename.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "document.h"
#include "timlib.h"
#include "column.h"
#include "piece.h"
#include "list.h"
#include "environ.h"
#include "julian.h"
#include "boolean.h"
#include "process.h"
#include "dictionary.h"
#include "session.h"
#include "application.h"
#include "hydrology_library.h"
#include "station.h"
#include "datatype.h"
#include "units.h"
#include "hydrology.h"
#include "name_arg.h"

/* Structures */
/* ---------- */

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void execute(		char *date_piece,
			char *delimiter );

void setup_arg(		NAME_ARG *arg, int argc, char **argv );

void fetch_parameters(	char **date_piece,
			char **delimiter,
			NAME_ARG *arg );

int main( int argc, char **argv )
{
	char *date_piece;
	char *delimiter;
	NAME_ARG *arg;

	arg = name_arg_new( argv[ 0 ] );

	setup_arg( arg, argc, argv );

	fetch_parameters(
			&date_piece,
			&delimiter,
			arg );

	execute( date_piece, delimiter );

	return 0;

} /* main() */

void execute(		char *date_piece,
			char *delimiter )
{
	char input_buffer[ 1024 ];
	MEASUREMENT *measurement;

	while ( timlib_get_line( input_buffer, stdin, 1024 ) )
	{
		if ( ! ( measurement = measurement_parse(
						input_buffer,
						delimiter ) ) )
		{
			fprintf( stderr,
				 "Error in %s/%s()/%d: cannot parse [%s].\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );

			pclose( input_pipe );
			exit( 1 );
		}

		if ( !date_set_yyyy_mm_dd(
				measurement_date_time,
				measurement_date_string ) )
		{
			continue;
		}

		if ( !date_set_time_hhmm(
				measurement_date_time,
				measurement_time_string ) )
		{
			continue;
		}

		julian_set_yyyy_mm_dd_hhmm(
			measurement_date_time_julian,
			date_display_yyyy_mm_dd( measurement_date_time ),
			date_display_hhmm( measurement_date_time ) );

		measurement_date_time_julian =
			measurement_adjust_time_to_sequence(
				measurement_date_time_julian,
				VALID_FREQUENCY_TIME_SEQUENCE );

		measurement_text_output( measurement, *delimiter );

	}

	return measurement_list;

} /* execute() */

void fetch_parameters(	char **date_piece,
			char **delimiter,
			NAME_ARG *arg )
{
	*date_piece = fetch_arg( arg, "date_piece" );
	*delimiter = fetch_arg( arg, "delimiter" );

} /* fetch_parameters() */

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "date_piece" );

        ticket = add_valid_option( arg, "delimiter" );
        set_default_value( arg, ticket, "^" );

        ins_all( arg, argc, argv );

} /* setup_arg() */

