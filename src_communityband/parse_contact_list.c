/* --------------------------------------------------- 	*/
/* src_capitolpops/parse_contact_list.c		       	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"
#include "list.h"
#include "musician.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */
void parse_contact_list( char *filename );

int main( int argc, char **argv )
{
	if ( argc != 2 )
	{
		fprintf( stderr,
			 "Usage: %s filename\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	parse_contact_list( argv[ 1 ] );

	exit( 0 );
} /* main() */

void parse_contact_list( char *filename )
{
	FILE *input_file;
	char buffer[ 1024 ];
	char musician_full_name[ 128 ];
	char piece_buffer[ 128 ];
	MUSICIAN *musician;
	FILE *update_pipe;
	char sys_string[ 1024 ];
	char *key = "full_name";

	if ( ! ( input_file = fopen( filename, "r" ) ) )
	{
		fprintf( stderr,
			 "Error: cannot open %s for read.\n",
			 filename );
		return;
	}

	sprintf( sys_string,
		 "update_statement table=musician key=%s carrot=y",
		 key );

	update_pipe = popen( sys_string, "w" );

	while( get_line( buffer, input_file ) )
	{
		if ( ! piece_quote_comma(
			musician_full_name,
			buffer,
			3 ) )
		{
			continue;
		}

		if ( ! ( musician = musician_fetch( musician_full_name ) ) )
		{
			continue;
		}

		/* Parse email_address */
		/* ------------------- */
		if ( ! piece_quote_comma(
			piece_buffer,
			buffer,
			1 ) )
		{
			continue;
		}

		if ( *piece_buffer )
		{
			if ( strcmp(	musician->email_address,
					piece_buffer ) != 0 )
			{
				fprintf( update_pipe,
				 	"%s^email_address^%s\n",
				 	musician_full_name,
				 	piece_buffer );
			}
		}

		/* Parse street_address */
		/* -------------------- */
		if ( ! piece_quote_comma(
			piece_buffer,
			buffer,
			4 ) )
		{
			continue;
		}

		if ( *piece_buffer )
		{
			search_replace_string(
				piece_buffer,
				",",
				"" );

			if ( strcmp(	musician->street_address,
					piece_buffer ) != 0 )
			{
				fprintf( update_pipe,
				 	"%s^street_address^%s\n",
				 	musician_full_name,
				 	piece_buffer );
			}
		}

		/* Parse city */
		/* ---------- */
		if ( ! piece_quote_comma(
			piece_buffer,
			buffer,
			6 ) )
		{
			continue;
		}

		if ( *piece_buffer )
		{
			if ( strcmp( musician->city, piece_buffer ) != 0 )
			{
				fprintf( update_pipe,
				 	"%s^city^%s\n",
				 	musician_full_name,
				 	piece_buffer );
			}
		}

		/* Parse state_code */
		/* ---------------- */
		if ( ! piece_quote_comma(
			piece_buffer,
			buffer,
			7 ) )
		{
			continue;
		}

		if ( *piece_buffer )
		{
			if ( strcmp( musician->state_code, piece_buffer ) != 0 )
			{
				fprintf( update_pipe,
				 	"%s^state_code^%s\n",
				 	musician_full_name,
				 	piece_buffer );
			}
		}

		/* Parse zip_code */
		/* -------------- */
		if ( ! piece_quote_comma(
			piece_buffer,
			buffer,
			8 ) )
		{
			continue;
		}

		if ( *piece_buffer )
		{
			if ( strcmp( musician->zip_code, piece_buffer ) != 0 )
			{
				fprintf( update_pipe,
				 	"%s^zip_code^%s\n",
				 	musician_full_name,
				 	piece_buffer );
			}
		}

		/* Parse main_phone */
		/* ---------------- */
		if ( ! piece_quote_comma(
			piece_buffer,
			buffer,
			2 ) )
		{
			continue;
		}

		if ( *piece_buffer )
		{
			if ( strcmp( musician->home_phone, piece_buffer ) != 0
			&&   strcmp( musician->cell_phone, piece_buffer ) != 0
			&&   character_count( '-', piece_buffer ) == 2 )
			{
				fprintf( update_pipe,
				 	"%s^cell_phone^%s\n",
				 	musician_full_name,
				 	piece_buffer );
			}
		}
	}

	fclose( input_file );
	pclose( update_pipe );

} /* parse_contact_list() */

