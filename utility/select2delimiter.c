/* utility/select_to_delimiter.c				 */
/* ------------------------------------------------------------- */
/* This program converts a standard ORACLE select statement to	 */
/* one that will separate each field with a delimiter.		 */
/*								 */
/* Freely available software: see Appaserver.org		 */
/* ------------------------------------------------------------- */
/* Note: need to enhance to use "set colsep '^'"		 */
/* ------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "token.h"
#include "timlib.h"

/* Function prototypes */
/* ------------------- */
int build_sql_statement(	char *sql_stmt, 
				char *tok_ptr,
				char delimiter );

/* Global variables */
/* ---------------- */
char from_table[ 128 ];

int main( int argc, char **argv )
{
	char buffer[ 2048 ];
	char sql_stmt[ 4096 ];
	char delimiter = '^';
	char *buf_ptr;
	char *tok_ptr;		/* Note: tok_ptr is the 4K static buffer */
				/*       inside of get_token().		 */

	if ( argc == 2 ) delimiter = *argv[ 1 ];

	*sql_stmt = '\0';
	printf( "set linesize 9999;\n" );

	while( get_line( buffer, stdin ) )
	{
		buf_ptr = buffer;

        	while ( 1 )
        	{
                	tok_ptr = get_token( &buf_ptr );
                	if ( !*tok_ptr ) break;

			if ( strcasecmp( tok_ptr, "rem" ) == 0 ) break;

			if ( !build_sql_statement(	sql_stmt,
							tok_ptr,
							delimiter ) )
			{
				break;
			}
		}
		printf( "%s\n", sql_stmt );
		*sql_stmt = '\0';
	}

	return 0;

} /* main() */


#define beginning		0
#define select_state		1
#define from_state		2
#define ok_to_end		3
#define literal			4

int build_sql_statement(	char *d, 
				char *tok_ptr,
				char delimiter )
{
	static int state = beginning;
	static int before_literal_state;

	if ( *tok_ptr == ';' )
	{
		strcat( d, ";" );
		state = beginning;
		return 0;
	}

	if ( state == literal )
	{
		sprintf( d + strlen( d ), "%s ", tok_ptr );
		state = before_literal_state;
		return 1;
	}

	if ( *tok_ptr == '\\' )
	{
		before_literal_state = state;
		state = literal;
		return 1;
	}

	if ( state == beginning && strcasecmp( tok_ptr, "select" ) != 0 )
	{
		state = ok_to_end;
	}
	else
	if ( state == beginning && strcasecmp( tok_ptr, "select" ) == 0 )
	{
		strcpy( tok_ptr, "select '^' ||" );
		sprintf( d + strlen( d ), "%s ", tok_ptr );
		state = select_state;
		return 1;
	}
	else
	if ( strcasecmp( tok_ptr, "from" ) == 0 )
		state = from_state;
	else
	if ( strcasecmp( tok_ptr, "where" ) == 0 )
		state = ok_to_end;

	/* This must be first to capture the table */
	/* --------------------------------------- */
	if ( state == from_state )
	{
		strcpy( from_table, tok_ptr );
	}

	if ( state == select_state )
	{
		if ( *tok_ptr == ',' )
		{
			sprintf( tok_ptr, "||'%c'||", delimiter );
			sprintf( d + strlen( d ), "%s ", tok_ptr );
		}
		else
		{
			sprintf( d + strlen( d ), "%s ", tok_ptr );
			/* sprintf( d + strlen( d ), "rtrim(%s) ", tok_ptr ); */
		}
	}
	else
	{
		sprintf( d + strlen( d ), "%s ", tok_ptr );
	}
	return 1;

} /* build_sql_statement() */

