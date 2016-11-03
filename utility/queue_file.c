/* queue_file.c */
/* ------------ */
/* Input:	1) path/filename 2) Number_to_queue			*/
/* Process:	This program:	1) appends a number to the filename to	*/
/*				   make it unique.			*/
/*				2) creates the new file with [mode]	*/
/*				3) removes all but number_to_queue 	*/
/*				   files with the same name.		*/
/* Tim Riley								*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"

/* Prototypes */
/* ---------- */
char *create_new_file( char *path_filename, char *mode );
void remove_files_maybe( char *path_filename, int number_to_queue );
int get_next_sequence( char *path_filename );
int create_file( char *filename, char *mode );

/* Constants */
/* --------- */
#define START_SEQUENCE	100000

int main( int argc, char **argv )
{
	char *path_filename, *new;
	char *mode = (char *)0;
	int number_to_queue = 0;
	int exit_usage = 0;

	if ( argc < 3 )
	{
		exit_usage = 1;
	}
	else
	if ( ! ( number_to_queue = atoi( argv[ 2 ] ) ) )
	{
		exit_usage = 1;
	}

	if ( exit_usage )
	{
		fprintf( stderr,
			"Usage: %s /path/filename number_to_queue [mode]\n",
			argv[ 0 ] );
		fprintf( stderr, "Note: mode must be octal\n" );
		exit( 1 );
	}

	path_filename = argv[ 1 ];
	if ( argc == 4 ) mode = argv[ 3 ];

	new = create_new_file( path_filename, mode );

	if ( new )
	{
		printf( "%s\n", new );
		remove_files_maybe( path_filename, number_to_queue );
		return 0;
	}
	else
	{
		fprintf( stderr, 
			 "ERROR: %s cannot create next file for (%s).\n",
			 argv[ 0 ], path_filename );

		exit( 1 );
	}
	return 0;
} /* main() */


char *create_new_file( char *path_filename, char *mode )
{
	static char new[ 1024 ];
	int next_sequence;

	next_sequence = get_next_sequence( path_filename );

	sprintf( new, "%s_#%d", path_filename, next_sequence );

	if ( !create_file( new, mode ) )
		return (char *)0;
	else
		return new;

} /* char *create_new_file() */

int create_file( char *filename, char *mode )
{
	FILE *f;
	char buffer[ 128 ];

	if ( ! ( f = fopen( filename, "w" ) ) ) return 0;
	fclose( f );

	if ( mode )
	{
		sprintf( buffer, "chmod %s %s", mode, filename );
		system( buffer );
	}

	return 1;

} /* create_file() */


int get_next_sequence( char *path_filename )
{
	FILE *p;
	char buffer[ 1024 ];
	char current_sequence[ 16 ];
	int next_sequence;

	sprintf( 
	buffer, 
"ls -dt %s* | egrep \"#[0-9][0-9]*\" | head -1 | sed 's/^.*#//' 2>/dev/null",
	 path_filename );

	p = popen( buffer, "r" );
	if ( fgets( current_sequence, 15, p ) )
		next_sequence = atoi( current_sequence ) + 1;
	else
		next_sequence = START_SEQUENCE;

	pclose( p );
	return next_sequence;

} /* get_next_sequence() */


void remove_files_maybe( char *path_filename, int number_to_queue )
{
	FILE *p;
	char buffer[ 1024 ];
	char filename[ 1024 ];

	sprintf( 
	buffer, 
"ls -dt %s* | egrep \"#[0-9][0-9]*\" | sed '1,%dd' 2>/dev/null",
	 path_filename, number_to_queue );

	p = popen( buffer, "r" );

	while( get_line( filename, p ) )
	{
		sprintf( buffer, "rm -f %s 2>/dev/null", filename );
		system( buffer );
	}

	pclose( p );

} /* remove_files_maybe() */

