/* parse_report.c 					*/
/* ---------------------------------------------------- */
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parse_report.h"
#include "timlib.h"
#include "column.h"
#include "list.h"

void parse_report_populate_cell_data(
				LIST *cell_list,
				char *buffer )
{
	CELL *cell;
	char data[ 512 ];

	if ( !list_rewind( cell_list ) )
	{
		fprintf( stderr,
			 "Warning: %s/%s(): empty cell list\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}

	do {
		cell = list_get_pointer( cell_list );
		column( data, 0, buffer + cell->beginning_offset );
		cell->data = strdup( data );
	} while( list_next( cell_list ) );
	
} /* parse_report_populate_cell_data() */

LIST *parse_report_get_cell_list(
				LIST *heading_string_list,
				char *buffer )
{
	LIST *cell_list = list_new_list();
	CELL *cell;
	int beginning_offset = 0;

	while( *buffer )
	{
		cell = parse_report_new_cell();
		cell->beginning_offset = beginning_offset;
		list_append_pointer( cell_list, cell );

		while( *buffer && !isspace( *buffer ) )
		{
			buffer++;
			beginning_offset++;
		}

		while( *buffer && isspace( *buffer ) )
		{
			buffer++;
			beginning_offset++;
		}
	}
	parse_report_populate_heading( cell_list, heading_string_list );
	return cell_list;
} /* parse_report_get_cell_list() */

void parse_report_populate_heading(	LIST *cell_list,
					LIST *heading_string_list )
{
	char *heading_string;
	boolean with_append = 0;

	if ( !list_rewind( heading_string_list ) ) return;

	do {
		heading_string = list_get_pointer( heading_string_list );
		parse_report_populate_heading_row(
					cell_list,
					heading_string,
					with_append );
		with_append = 1;
	} while( list_next( heading_string_list ) );

} /* parse_report_populate_heading() */

void parse_report_populate_heading_row(	LIST *cell_list,
					char *heading_string,
					boolean with_append )
{
	CELL *cell;
	char heading[ 512 ];

	if ( !list_rewind( cell_list ) ) return;

	do {
		cell = list_get_pointer( cell_list );

		column(	heading,
			0,
			heading_string + cell->beginning_offset );

		if ( with_append )
		{
			sprintf( cell->heading +
				 strlen( cell->heading ),
				 " %s",
				 heading );
		}
		else
		{
			strcpy( cell->heading, heading );
		}
	} while( list_next( cell_list ) );
	
} /* parse_report_populate_heading_row() */

LIST *parse_report_load_heading_string_list(
					char *buffer )
{
	LIST *heading_string_list = list_new_list();

	while( get_line( buffer, stdin ) )
	{
		if ( *buffer == '-' ) return heading_string_list;

		if ( isalpha( *buffer ) )
		{
			list_append_pointer(
				heading_string_list,
				strdup( buffer ) );
		}
	}
	return heading_string_list;
} /* parse_report_load_heading_string_list() */


void parse_report_display_cell_list(	LIST *cell_list,
					char delimiter )
{
	CELL *cell;

	if ( !list_rewind( cell_list ) ) return;

	do {
		cell = list_get_pointer( cell_list );
		printf( "%s%c%s\n",
			 cell->heading,
			 delimiter,
			 cell->data );
	} while( list_next( cell_list ) );
} /* parse_report_display_cell_list() */

CELL *parse_report_new_cell(	void )
{
	CELL *cell = (CELL *)calloc( 1, sizeof( CELL ) );
	return cell;
} /* parse_report_new_cell() */

PARSE_REPORT *parse_report_new_parse_report( void )
{
	return (PARSE_REPORT *)calloc( 1, sizeof( PARSE_REPORT ) );
} /* parse_report_new_parse_report() */

