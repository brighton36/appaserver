/* $APPASERVER_HOME/utility/hashsearch.c	 */
/* --------------------------------------------- */
/* Given:					 */
/* 1) input file containing key|data1[|data2...] */
/* 2) stream of key				 */
/* Output corresponding:			 */
/* data1[|data2...]				 */
/* --------------------------------------------- */
/* Freely available software: see Appaserver.org */
/* ----------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "piece.h"
#include "dictionary.h"

void hashsearch(char *argv_0,
		char *dictionary_filename,
		char delimiter );

DICTIONARY *get_dictionary(
		char *argv_0,
		char *dictionary_filename,
		char delimiter );

DICTIONARY *get_dictionary(	char *argv_0,
				char *dictionary_filename,
				char delimiter )
{
	FILE *dictionary_file;
	char input_buffer[ 4096 ];
	char key[ 1024 ];
	char data[ 4096 ];
	DICTIONARY *dictionary;

	dictionary = dictionary_large_new();

	dictionary_file = open_file( dictionary_filename, "r" );

	while( get_line( input_buffer, dictionary_file ) )
	{
		if ( !character_count( delimiter, input_buffer ) )
		{
			fprintf( stderr,
				 "ERROR in %s: no delimiter (%c) found.\n",
				 argv_0,
				 delimiter );
			fclose( dictionary_file );
			exit(1);
		}

		piece( key, delimiter, input_buffer, 0 );
		strcpy( data, input_buffer );
		piece_inverse( data, delimiter, 0 );

		dictionary_set_pointer(	dictionary,
					strdup( key ),
					strdup( data ) );
	}

	fclose( dictionary_file );
	return dictionary;

} /* get_dictionary() */

void hashsearch(char *argv_0,
		char *dictionary_filename,
		char delimiter )
{
	char input_buffer[ 4096 ];
	DICTIONARY *dictionary;
	char *data;

	dictionary =
		get_dictionary(
			argv_0,
			dictionary_filename,
			delimiter );

	while( get_line( input_buffer, stdin ) )
	{
		if ( ( data =
			dictionary_get_pointer(
				dictionary, input_buffer ) ) )
		{
			printf(	"%s\n", data );
		}
		else
		{
			printf(	"%s not found\n",
				input_buffer );
		}

	}

} /* hashsearch() */

int main( int argc, char **argv )
{
	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s hashfile delimiter\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	hashsearch( argv[ 0 ], argv[ 1 ], *argv[ 2 ] ); 
	return 0;

} /* main() */

