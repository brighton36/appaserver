/* ---------------------------------------------------------------	*/
/* decode_html_post.c							*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "timlib.h"
#include "decode_html_post.h"

char *decode_html_post( char *destination, char *source )
{
	char *source_ptr, *destination_ptr;
	char hex[ 3 ];
	int c;

	source_ptr = source;
	destination_ptr = destination;
	*(hex + 2) = '\0';

	while( *source_ptr )
	{
#ifdef NOT_DEFINED
		/* Convert plus to space */
		/* --------------------- */
		if ( *source_ptr == '+' )
		{
			*destination_ptr++ = ' ';
			source_ptr++;
			continue;
		}

		/* Convert carrot to two stars. */
		/* ---------------------------- */
		if ( *source_ptr == '^' )
		{
			*destination_ptr++ = '*';
			*destination_ptr++ = '*';
			source_ptr++;
			continue;
		}
#endif

		if ( *source_ptr == '%' )
		{
			/* Sometimes, there's a preceeding "25" */
			/* ------------------------------------ */
			if ( strncmp( source_ptr + 1, "25", 2 ) == 0 )
			{
				strcpy( ( source_ptr + 1 ),
					( source_ptr + 3 ) );
			}

			/* If 10 */
			/* ------ */
			if ( strncmp( source_ptr + 1, "0A", 2 ) == 0 )
			{
				*destination_ptr++ = *source_ptr++;
				*destination_ptr++ = *source_ptr++;
				*destination_ptr++ = *source_ptr++;
				continue;
			}

			/* ----- */
			/* If 13 */
			/* ----- */
			if ( strncmp( source_ptr + 1, "0D", 2 ) == 0 )
			{
				*destination_ptr++ = *source_ptr++;
				*destination_ptr++ = *source_ptr++;
				*destination_ptr++ = *source_ptr++;
				continue;
			}

			if ( *(source_ptr + 1)
			&&     isalnum( *(source_ptr + 1) ) )
			{
				*hex = *(source_ptr + 1);
				*(hex + 1) = *(source_ptr + 2);
				sscanf( hex, "%x", &c );

				/* Make some conversions */
				/* --------------------- */
				/* if ( c == '+' ) c = ' '; */
				if ( c == '"' ) c = '\'';

				if ( c == '=' )
					*destination_ptr++ = '\\';
				if ( c == '&' )
					*destination_ptr++ = '\\';

				*destination_ptr++ = c;
				source_ptr += 3;
				continue;
			}
		}

		if ( *source_ptr == '\\' )
		{
			if ( *( source_ptr + 1 ) == ',' )
			{
				source_ptr++;
				*destination_ptr++ = *source_ptr++;
				continue;
			}
		}

		*destination_ptr++ = *source_ptr++;
	}
	*destination_ptr = '\0';
	return destination;
} /* decode_html_post() */

char *string_decode_cr( char *d, char *s )
{
	char *tmp = d;

	while( *s )
	{
		if ( strncmp( s, "%0D", 3 ) == 0 )
		{
/*
			*tmp++ = 13;
*/
			s += 3;
		}
		else
		if ( strncmp( s, "%0A", 3 ) == 0 )
		{
			*tmp++ = 10;
			s += 3;
		}
		else
			*tmp++ = *s++;
	}
	*tmp = '\0';

	return trim( d );

} /* string_decode_cr() */

