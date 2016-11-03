/* library/function.c */
/* ------------------ */

#include <stdio.h>
#include <stdlib.h>
#include "function.h"
#include "piece.h"

FUNCTION_ADT *function_init()
{
	FUNCTION_ADT *f;

	f = (FUNCTION_ADT *)calloc( 1, sizeof( FUNCTION_ADT ) );
	if ( !f )
	{
		fprintf( stderr, 
			 "function_init(): Memory allocation error.\n" );
		exit( 1 );
	}

	return f;

} /* function_init() */


function_free( f )
FUNCTION_ADT *f;
{
	free( f );
	return 1;
}


char *replace_all_functions( char *destination, char *source, FUNCTION_ADT *f )
{
	char *replace_string;
	char *expression_ptr, *replace_ptr;
	char *ptr;
	char function_name[ 128 ];
	char function_expression[ 128 ];
	char function_results[ 128 ];
	int more_to_do = 1;

	strcpy( destination, source );

	while( more_to_do )
	{
		more_to_do = 0;
		ptr = destination;

		while( *ptr )
		{
			/* If a colon followed by a letter */
			/* ------------------------------- */
			if ( *ptr == ':' 
			&&   isalpha( *(ptr + 1 ) ) )
			{
				if ( !is_function( ptr ) )
				{
					ptr++;
					continue;
				}

				expression_ptr = function_expression;

				while( *ptr )
				{
					if ( isspace( *ptr ) )
					{
						break;
					}
					else
					{
						*expression_ptr++ = *ptr++;
					}
				}
				*expression_ptr = '\0';

				piece(	function_name, 
					'(', 
					function_expression, 
					0 );

				if ( !execute_function_adt(
					function_results,
					function_name + 1,	/* skip ':' */
					f,
					function_expression ) )
				{
					fprintf(stderr,
	"replace_all_functions() could not execute: (%s) on statement: (%s)\n",
					function_name + 1,
					function_expression );
					exit( 1 );
				}

				search_replace(	function_expression, 
						function_results, 
						destination );

				more_to_do = 1;
				break;
			}
			ptr++;
		}
	}

	return destination;

} /* replace_all_functions() */


/* Sample input = ":age(10/6/60)" */
/* ------------------------------ */
is_function( char *s )
{
	while( *s )
	{
		if ( isspace( *s ) ) return 0;
		if ( *s == '(' ) return 1;
		s++;
	}
	return 0;

} /* is_function() */



void set_function( FUNCTION_ADT *f, char *name, int (*entry)() )
{
	int i;

	for ( i = 0; i < MAX_FUNCTION_ADT; i++ )
	{
		if ( !f->function[ i ].name )
		{
			f->function[ i ].name = name;
			f->function[ i ].entry = entry;
			return;
		}
	}

	fprintf(stderr, 
		"set_function(): max functions of %d exceeded with (%s)\n",
		MAX_FUNCTION_ADT,
		name );
	exit( 1 );

} /* set_function() */


int execute_function_adt( 	char *function_results,
				char *function_name,
				FUNCTION_ADT *f,
				char *entire_function )
{
	int i;

	for( i = 0; f->function[ i ].name; i++ )
	{
		if ( strcmp( f->function[ i ].name, function_name ) == 0 )
		{
			return f->function[ i ].entry(	function_results,
							entire_function );
		}
	}
	return 0;

} /* execute_function_adt() */


