/* parse_report_dvr.c					*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "timlib.h"
#include "list.h"
#include "piece.h"
#include "column.h"
#include "boolean.h"
#include "parse_report.h"

int main( int argc, char **argv )
{
	char buffer[ 65536 ];
	PARSE_REPORT *parse_report;
	char delimiter;

	if ( argc != 2 )
	{
		fprintf( stderr, "Usage: %s delimiter\n", argv[ 0 ] );
		exit( 1 );
	}

	delimiter = *argv[ 1 ];

	parse_report = parse_report_new_parse_report();

	while( get_line( buffer, stdin ) )
	{
		if ( isspace( *buffer ) )
		{
			parse_report->heading_string_list =
				parse_report_load_heading_string_list(
					buffer );

			/* buffer now contains the line string */
			/* ----------------------------------- */
			parse_report->cell_list =
				parse_report_get_cell_list(
					parse_report->heading_string_list,
					buffer );
		}
		else
		{
			parse_report_populate_cell_data(
					parse_report->cell_list,
					buffer );
			parse_report_display_cell_list(
					parse_report->cell_list,
					delimiter );
		}
	}
	return 0;
} /* main() */

