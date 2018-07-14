/* $APPASERVER_HOME/library/timlib.c			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include "timlib.h"
#include "julian.h"
#include "column.h"
#include "piece.h"
#include "date.h"

int timlib_strlen( char *s )
{
	if ( !s ) return 0;
	return strlen( s );
}

boolean timlib_strcmp( char *s1, char *s2 )
{
	if ( !s1 && !s2 ) return 0;
	if ( !s2 ) return -1;
	if ( !s1 ) return 1;

	return strcasecmp( s1, s2 );

} /* timlib_strcmp() */

boolean timlib_strcpy( char *d, char *s, int buffer_size )
{
	int str_len;

	if ( d && !s )
	{
		*d = '\0';
		return 0;
	}

	if ( !d || !s ) return 0;
	if ( d == s ) return 1;

	*d = '\0';
	str_len = strlen( s );

	if ( buffer_size && str_len >= buffer_size ) return 0;

	while( *s ) *d++ = *s++;
	*d = '\0';

	return 1;

} /* timlib_strcpy() */

char *timlib_strcat( char *d, char *s )
{
	char *end;

	if ( !d ) return "";
	if ( !s ) return d;

	end = d + strlen( d );
	while( *s ) *end++ = *s++;
	*end = '\0';
	return d;

} /* timlib_strcat() */

int timlib_get_index( char *s )
{
	return get_index( s );
}

int get_index_from_string( char *s )
{
	return get_index( s );
}

/* This function expects source to have beginning and ending delimiters */
/* -------------------------------------------------------------------- */
void extract_delimited( char *destination, char *source )
{
	char *ptr = destination;
	char destination_delimiter;

	*ptr = '\0';
	destination_delimiter = *source++;
	if ( destination_delimiter == '<' ) destination_delimiter = '>';

	while(	*source && *source != destination_delimiter )
	{
		*ptr++ = *source++;
	}
	*ptr = '\0';
} /* extract_delimited() */

void mail_tim( char *message )
{
	char buffer [ 1024 ];

	sprintf( buffer, 
		 "echo \"%s\" 				|"
		 "mail timriley@appahost.com		",
		 message );
	system( buffer );
} /* mail_tim() */

/* Sample: attribute_name = "station_1" */
/* ------------------------------------ */
int get_index( char *attribute_name )
{
	char *end_ptr;

 	end_ptr = attribute_name + strlen( attribute_name ) - 1;
	while( end_ptr != attribute_name )
	{
		if ( isdigit( *end_ptr ) )
		{
			end_ptr--;
			continue;
		}
		if ( *end_ptr == '_' )
		{
			end_ptr++;
			break;
		}
		else
			break;
	}
	return atoi( end_ptr );
} /* get_index() */


char *timlib_trim_index( char *destination, char *attribute_name )
{
	return trim_index( destination, attribute_name );
}

/* Sample: attribute_name = "station_1" */
/* ------------------------------------ */
char *trim_index( char *destination, char *attribute_name )
{
	char *end_ptr;

	strcpy( destination, attribute_name );
 	end_ptr = destination + strlen( destination ) - 1;
	while( end_ptr != destination )
	{
		if ( isdigit( *end_ptr ) )
		{
			end_ptr--;
			continue;
		}
		if ( *end_ptr == '_' )
		{
			*end_ptr = '\0';
			break;
		}
		else
		{
			break;
		}
	}
	return destination;
} /* trim_index() */

/* Sample: attribute_name = "station_0" */
/* ------------------------------------ */
char *trim_index_zero( char *destination, char *attribute_name )
{
	char *end_ptr;

	strcpy( destination, attribute_name );
 	end_ptr = destination + strlen( destination ) - 1;

	if ( *end_ptr != '0' ) return destination;
	end_ptr--;
	if ( *end_ptr == '_' ) *end_ptr = '\0';
	return destination;
} /* trim_index_zero() */

boolean exists_index( char *attribute_name )
{
	char *end_ptr;

 	end_ptr = attribute_name + strlen( attribute_name ) - 1;
	while( end_ptr != attribute_name )
	{
		if ( !isdigit( *end_ptr ) && *end_ptr != '_' )
		{
			return 0;
		}
		else
		if ( isdigit( *end_ptr ) )
		{
			end_ptr--;
		}
		else
		if ( *end_ptr == '_' )
		{
			return 1;
		}
	}
	return 0;
} /* exists_index() */

char *mnemonic2upper_case( char *d, char *s )
{
	return format_initial_capital( d, s );
}

char *format_plural( char *destination, char *string )
{
	char *end_pointer;

	strcpy( destination, string );
	end_pointer = string + strlen( string ) - 1;
	if ( *end_pointer != 's' ) strcat( destination, "s" );
	return destination;
}

char *format_mnemonic( char *mnemonic, char *string )
{
	char *mnemonic_anchor = mnemonic;

	int beginning = 1;

	if ( !string ) return "";

	while( *string )
	{
		if ( *string == '\\' )
		{
			string++;
		}

		if ( beginning )
		{
			if ( isspace( *string )
			||   ispunct( *string ) )
			{
				beginning = 1;
			}
			else
			{
				beginning = 0;
				*mnemonic++ = tolower( *string++ );
			}
		}
		else
		if ( isspace( *string ) )
		{
			beginning = 1;
			*mnemonic++ = '_';
			string++;
		}
		else
		if ( ispunct( *string ) )
		{
			beginning = 1;
			string++;
		}
		else
		{
			*mnemonic++ = tolower( *string++ );
		}
	}
	*mnemonic = '\0';

	return mnemonic_anchor;
} /* format_mnemonic() */

char *format_first_initial_capital( char *destination, char *string )
{
	strcpy( destination, string );
	*destination = toupper( *destination );
	search_replace_character( destination, '_', ' ' );

	return destination;
}

char *format_initial_capital_cr( char *destination, char *string )
{
	int beginning = 1;
	char *destination_ptr = destination;

	if ( !string ) return "";

	while( *string )
	{
		if ( *string == '\\' )
		{
			*destination_ptr++ = *string++;
			beginning = 0;
		}
		else
		if ( beginning )
		{
			if ( isspace( *string )
			|| ( ispunct( *string ) && *string != '\'' ) )
			{
				beginning = 1;
			}
			else
			{
				beginning = 0;
			}
			*destination_ptr++ = toupper( *string++ );
		}
		else
		if ( *string == '_' )
		{
			beginning = 1;
			destination_ptr += sprintf( destination_ptr, "<br>" );
			/* *destination_ptr++ = delimiter; */
			string++;
		}
		else
		if ( *string == '|' )
		{
			beginning = 1;
			*destination_ptr++ = *string++;
		}
		else
		if ( isspace( *string )
		|| ( ispunct( *string ) && *string != '\'' ) )
		{
			beginning = 1;
			*destination_ptr++ = *string++;
		}
		else
		{
			*destination_ptr++ = *string++;
		}
	}
	*destination_ptr = '\0';

	return destination;

} /* format_initial_capital_cr() */

char *format_initial_capital( char *destination, char *string )
{
	return format_initial_capital_delimiter(
			destination,
			string, ' ',
			1 /* with_inside_parens */ );
}

char *format_initial_capital_not_parens( char *destination, char *string )
{
	return format_initial_capital_delimiter(
			destination,
			string, ' ',
			0 /* not with_inside_parens */ );
}

char *format_initial_capital_delimiter(
				char *destination,
				char *string,
				char delimiter,
				boolean with_inside_parens )
{
	boolean beginning = 1;
	char *destination_ptr = destination;

	if ( !string ) return "";

	while( *string )
	{
		if ( with_inside_parens && *string == '(' )
		{
			*destination_ptr++ = *string++;
			beginning = 1;
			while( *string )
			{
				*destination_ptr++ = *string++;
				if ( *string == ')' ) break;
			}
		}

		if ( *string == '\\' )
		{
			*destination_ptr++ = *string++;
			beginning = 0;
		}
		else
		if ( beginning )
		{
			if ( isspace( *string )
			|| ( ispunct( *string ) && *string != '\'' ) )
			{
				beginning = 1;
			}
			else
			{
				beginning = 0;
			}
			*destination_ptr++ = toupper( *string++ );
		}
		else
		if ( *string == '_' )
		{
			beginning = 1;
			*destination_ptr++ = delimiter;
			string++;
		}
		else
		if ( *string == '|' )
		{
			beginning = 1;
			*destination_ptr++ = *string++;
		}
		else
		if ( isspace( *string )
		|| ( ispunct( *string ) && *string != '\'' ) )
		{
			beginning = 1;
			*destination_ptr++ = *string++;
		}
		else
		{
			*destination_ptr++ = *string++;
		}
	}
	*destination_ptr = '\0';

	/* Keep booleans at lower case */
	/* --------------------------- */
	search_replace_string( destination, " Yn", " yn" );

	if ( strcmp( destination, "Y" ) == 0 ) *destination = 'y';
	if ( strcmp( destination, "N" ) == 0 ) *destination = 'n';

	return destination;

} /* format_initial_capital_delimiter() */

void output_starting_argv_stderr( int argc, char **argv )
{

	fprintf( stderr, "%s %s: %s",
		 date_get_now_yyyy_mm_dd( date_get_utc_offset() ),
		 date_get_now_hhmm( date_get_utc_offset() ),
		 argv[ 0 ] );
	while( --argc ) fprintf( stderr, " %s", *++argv );
	fprintf( stderr, "\n" );
	fflush( stderr );
}

char *pipe_multiple_lines2string(	char *sys_string,
					char delimiter )
{
	char buffer[ 65536 ];
	char input_buffer[ 1024 ];
	FILE *p;
	boolean null_input = 1;
	char *ptr = buffer;
	boolean first_time = 1;

	p = popen( sys_string, "r" );
	while ( get_line( input_buffer, p ) )
	{
		if ( first_time )
		{
			ptr += sprintf( ptr, "%s", input_buffer );
			first_time = 0;
			null_input = 0;
		}
		else
		{
			ptr += sprintf( ptr, "%c%s", delimiter, input_buffer );
		}
	}
	pclose( p );
	if ( null_input )
		return (char *)0;
	else
		return strdup( buffer );
} /* pipe_multiple_lines2string() */

char *pipe2string( char *sys_string )
{
	char buffer[ 65536 ];
	FILE *p;
	int null_input = 0;

	*buffer = '\0';
	p = popen( sys_string, "r" );

	timlib_reset_get_line_check_utf_16();
	if ( !timlib_get_line( buffer, p, 65536 ) ) null_input = 1;
	timlib_reset_get_line_check_utf_16();

	pclose( p );

	if ( null_input )
		return (char *)0;
	else
		return strdup( buffer );

} /* pipe2string() */

char *get_line_system( char *sys_string )
{
	char buffer[ 4096 ];
	FILE *p;
	int return_value;

	p = popen( sys_string, "r" );
	return_value = get_line( buffer, p );
	pclose( p );
	if ( !return_value )
		return (char *)0;
	else
		return strdup( buffer );
} /* get_line_system() */


char *trim_after_character(	char *destination,
				char c,
				char *source,
				int num2trim )
{
	char *anchor = source;

	*destination = '\0';
	if ( num2trim <= 0 ) num2trim = 1;

	while ( *source )
	{
		if ( *source == c )
		{
			if ( --num2trim == 0 )
			{
				*destination = '\0';
				return destination;
			}
		}
		*destination++ = *source++;
	}
	return anchor;
} /* trim_after_character() */

char *trim_until_character(	char *destination,
				char c,
				char *source,
				int num2trim )
{
	char *anchor = source;

	*destination = '\0';
	if ( num2trim <= 0 ) num2trim = 1;

	while ( *source )
	{
		if ( *source == c )
		{
			if ( --num2trim == 0 )
			{
				strcpy( destination, source + 1 );
				return destination;
			}
		}
		source++;
	}
	return anchor;
} /* trim_until_character() */

char *trim_character( char *destination, char c, char *source )
{
	char *anchor = destination;

	while ( *source )
	{
		if ( *source != c )
		{
			*destination = *source;
			destination++;
		}
		source++;
	}

	*destination = '\0';
	return anchor;

} /* trim_character() */

int timlib_character_exists( char *buffer, int c )
{
	return exists_character( buffer, c );
}

int character_exists( char *buffer, int c )
{
	return exists_character( buffer, c );
}

boolean timlib_exists_alpha( char *s )
{
	while( *s )
	{
		if ( isalpha( *s++ ) ) return 1;
	}
	return 0;

} /* timlib_exists_alpha() */

boolean timlib_exists_numeric( char *s )
{
	while( *s )
	{
		if ( isdigit( *s++ ) ) return 1;
	}
	return 0;

} /* timlib_exists_numeric() */

boolean timlib_exists_string(	 	char *string,
					char *substring )
{
	return ( instr( substring, string, 1 ) > -1 );

} /* timlib_exists_string() */

boolean timlib_begins_string(	 	char *string,
					char *substring )
{
	return ( instr( substring, string, 1 ) == 0 );

} /* timlib_begins_string() */

boolean timlib_ends_string(	 	char *string,
					char *substring )
{
	char *string_end;
	char *substring_end;

	string_end = string + strlen( string );
	substring_end = substring + strlen( substring );

	while( 1 )
	{
		if ( *string_end != *substring_end ) return 0;

		string_end--;
		substring_end--;

		if ( substring_end < substring ) return 1;
		if ( string_end < string ) return 0;
	}

} /* timlib_ends_string() */

int exists_character( char *buffer, int c )
{
	int position = 1;

	if ( !buffer ) return 0;

	while( *buffer )
	{
		if ( *buffer++ == c ) return position;
		position++;
	}
	return 0;
} /* exists_character() */

int character_position( char *buffer, int c )
{
	int position = -1;

	while( *buffer )
	{
		if ( *buffer++ == c )
		{
			position++;
			break;
		}
		position++;
	}
	return position;
} /* character_position() */

char *reformat_double_return_string( double d, char *format_string )
{
	char return_buffer[ 128 ];

	sprintf( return_buffer, format_string, d );
	return strdup( return_buffer );

} /* reformat_float() */

char *trim_right( char *buffer, int length )
{
	int str_len;

	str_len = strlen( buffer );

	if ( str_len < length ) return buffer;

	buffer[ str_len - length ] = '\0';
	return buffer;
}

char *right_trim( char *buffer )
{
	return rtrim( buffer );
}

char *right_trim_session( char *buffer )
{
        char *buf_ptr;

        /* If buffer is empty then return */
        /* ------------------------------ */
        if ( !buffer || !*buffer ) return buffer;

        buf_ptr = buffer + strlen( buffer ) - 1;

        while ( *buf_ptr && isdigit( *buf_ptr ) )
        	buf_ptr--;

	if ( *buf_ptr == '_' )
	{
        	*(buf_ptr) = '\0';
	}

        return buffer;

} /* right_trim_session() */

char *rtrim( char *buffer )
{
        char *buf_ptr;

        /* If buffer is empty then return */
        /* ------------------------------ */
        if ( !buffer || !*buffer ) return buffer;

        buf_ptr = buffer + strlen( buffer ) - 1;

        while ( *buf_ptr && isspace( *buf_ptr ) )
        	buf_ptr--;

        *(buf_ptr + 1) = '\0';

        return buffer;

} /* rtrim() */

char *trim_length(	char *buffer,
			int length )
{
	if ( strlen( buffer ) <= length ) return buffer;

	buffer[ length ] = '\0';
	return buffer;

} /* trim_length() */

char *trim( char *buffer )
{
        char *buf_ptr = buffer;

        /* If buffer is empty then return */
        /* ------------------------------ */
        if ( !buffer || !*buffer ) return buffer;

        /* First trim leading spaces */
        /* ------------------------- */
        while( *buf_ptr && isspace( *buf_ptr ) )
        	buf_ptr++;

        /* If *buf_ptr is NULL then buffer was just spaces */
        /* ----------------------------------------------- */
        if ( !*buf_ptr )
        {
        	*buffer = '\0';
        	return buffer;
        }

        /* Copy the buffer */
        /* --------------- */
        if ( buffer != buf_ptr )
		timlib_strcpy( buffer, buf_ptr, 65536 );

        /* Trim trailing spaces */
        /* -------------------- */
        buf_ptr = buffer + strlen( buffer ) - 1;

        while (	*buf_ptr &&
		( isspace( *buf_ptr ) ||
		  *buf_ptr > 127 ||
		  *buf_ptr < 0 ) )
	{
        	buf_ptr--;
	}

        *(buf_ptr + 1) = '\0';

        return buffer;

} /* trim() */

char *up_string_to_delimiter ( char *s, char delimiter )
{
        char *hold = s;
        while ( *s )
        {
		if ( *s == delimiter ) break;
                *s = toupper( *s );
                s++;
        }
        return hold;

} /* up_string_to_delimiter() */

char *up_string ( char *s )
{
        char *hold = s;
        while ( *s )
        {
                *s = toupper( *s );
                s++;
        }
        return hold;

} /* up_string() */

char *low_string ( char *s )
{
        char *hold = s;
        while ( *s )
        {
                *s = tolower( *s );
                s++;
        }
        return hold;

} /* low_string() */

static boolean get_line_check_utf_16 = 1;
static boolean is_utf_16 = 0;
static boolean utf_16_toggle = 1;

void timlib_reset_get_line_check_utf_16( void )
{
	get_line_check_utf_16 = 1;
	is_utf_16 = 0;
	utf_16_toggle = 1;
}

int timlib_get_line(	char *in_line,
			FILE *infile,
			int buffer_size )
{
	int in_char;
	int size = 0;

	*in_line = '\0';

	/* Exit in middle. */
	/* --------------- */
	while ( 1 )
	{
		in_char = fgetc( infile );

		if ( get_line_check_utf_16 )
		{
			get_line_check_utf_16 = 0;

			if ( in_char == 255 )
			{
				in_char = fgetc( infile );

				if ( in_char == 254 )
				{
					is_utf_16 = 1;
					continue;
				}
			}
		}

		if ( is_utf_16 )
		{
			utf_16_toggle = 1 - utf_16_toggle;

			if ( utf_16_toggle )
			{
				continue;
			}
		}

		/* Why are there zeros? */
		/* -------------------- */
		if ( !in_char ) continue;

		if ( in_char == CR ) continue;

		if ( in_char == EOF )
		{
#ifdef NOT_DEFINED
	char *anchor = in_line;
			/* --------------------------------------- */
			/* If last line in file doesn't have a CR, */
			/* then call this function one more time.  */
			/* --------------------------------------- */
			if ( in_line != anchor )
			{
				*in_line = '\0';
				return 1;
			}
			else
			{
				timlib_reset_get_line_check_utf_16();
				return 0;
			}
#endif
			*in_line = '\0';
			timlib_reset_get_line_check_utf_16();
			return 0;
		}

		if ( in_char == LF )
		{
			*in_line = '\0';
			return 1;
		}

		/* If '\' then get the next character */
		/* ---------------------------------- */
		if ( in_char == '\\' )
		{
			in_char = fgetc( infile );

			if ( in_char == CR ) continue;

			/* Can't escape the LF */
			/* ------------------- */
			if ( in_char == LF )
			{
				*in_line = '\0';
				return 1;
			}

			*in_line++ = '\\';
			size++;
		}

		if ( buffer_size && ( size++ >= buffer_size ) )
		{
			fprintf( stderr,
		"Error in %s/%s()/%d: exceeded buffer size of %d.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 buffer_size );
			*in_line = '\0';
			return 1;
		}

		*in_line++ = in_char;

	} /* while( 1 ) */

} /* timlib_get_line() */

int get_line( char *in_line, FILE *infile )
{
	return timlib_get_line( in_line, infile, 0 );

} /* get_line */

void skip_line( FILE *infile )
{
	int in_char;

	while ( 1 )
	{
		in_char = fgetc(infile);
		if (in_char == EOF || in_char == LF) return;
	}
} /* skip_line */

char queue_static_buffer[ QUEUE_STATIC_BUFFER_SIZE ] = {0};
int queue_toggle = 0;

void timlib_reset_line_queue( void )
{
	*queue_static_buffer = '\0';
	queue_toggle = 0;
}

void timlib_unget_line( char *in_line )
{
	unget_line_queue( in_line );
}

void unget_line_queue( char *in_line )
{
	if ( queue_toggle )
	{
		fprintf( stderr,
			 "Warning in %s/%s()/%d: queue full.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		return;
	}

	strcpy( queue_static_buffer, in_line );
	queue_toggle = 1;
}

int get_line_queue( char *in_line, FILE *infile )
{
	if ( queue_toggle )
	{
		strcpy( in_line, queue_static_buffer );
		queue_toggle = 0;
		return 1;
	}
	else
	{
		return get_line( in_line, infile );
	}
} /* get_line_queue() */

int timlib_get_block_delimiter(
				char **block,
				int delimiter,
				int columns_to_block,
				FILE *input_file,
				int max_lines )
{
	char buffer[ 65536 ];
	char first_field[ 1024 ];
	char old_first_field[ 1024 ];
	int count = 0;

	*old_first_field = '\0';

	while( get_line_queue( buffer, input_file ) )
	{
		if ( count == max_lines )
		{
			fprintf( stderr, 
		"ERROR in get_block_delimiter: max lines of %d exceeded\n",
				 max_lines );
			exit( 1 );
		}

		count++;

		piece_multiple(	first_field, 
				delimiter, 
				buffer, 
				columns_to_block );

		if ( !*old_first_field )
		{
			strcpy( old_first_field, first_field );
			*block++ = strdup( buffer );
		}
		else
		{
			if ( strcmp( old_first_field, first_field ) != 0 )
			{
				unget_line_queue( buffer );
				return count - 1;
			}
			else
			{
				*block++ = strdup( buffer );
			}
		}
	}

	return count;

} /* timlib_get_block_delimiter() */

void free_array_string_with_count( 	char **block, 
					int block_count )
{
	while( block_count-- ) free( *block++ );
}

char *midstr( char *d, char *s, int start, int how_many )
{
	strncpy( d, s + start, how_many );
	*( d + how_many ) = '\0';
	return d;

} /* midstr() */


boolean instr_exclude_double_quoted(
			char *substr, char *string, int occurrence )
{
        int x,found;
	int inside_double_quotes = 0;
        int str_len_str = strlen(string);
        int str_len_sub = strlen(substr);

        for(x = 0,found = 0;x < str_len_str;x++)
	{
		if ( *( string + x ) == '"' )
			inside_double_quotes = 1 - inside_double_quotes;

		if ( !inside_double_quotes
                &&   strncmp(&string[x],substr,str_len_sub) == 0 )
                        	if ( ++found == occurrence )
                                	return x;
	}
        return -1;

} /* instr_exclude_double_quoted() */

int instr_character( char delimiter, char *string )
{
        int x;

	for(x = 0; *string; x++)
		if ( *string++ == delimiter )
			return x;
	return -1;
} /* instr_character() */

int instr( char *substr, char *string, int occurrence )
{
        int x,found;
        int str_len_str;
        int str_len_sub;

	if ( !substr || !*substr ) return -1;
	if ( !string || !*string ) return -1;

        str_len_str = strlen(string);
        str_len_sub = strlen(substr);

        for(x = 0,found = 0;x < str_len_str;x++)
                if (strncasecmp(&string[x],substr,str_len_sub) == 0)
                        if (++found == occurrence)
                                return x;
        return -1;

} /* instr() */


char *delete_str( char *string, int start, int num_chars )
{
        int far_str;

        for(far_str = start + num_chars;
                far_str <= strlen(string);far_str++,start++)
                        string[start] = string[far_str];

        return string;

} /* delete_str() */

char *insert_string( char *source_destination, char *substring, int pos )
{
	return insert_str( substring, source_destination, pos );
}

char *insert_str( char *sub, char *string, int pos )
{
        char *temp = malloc(strlen(string) + strlen(sub) + 2);

        temp[0] = '\0';

        /* copy first part over */
        strncpy(temp,string,pos);
        temp[pos] = '\0';

        /* place sub in middle */
        strcat(temp,sub);

        /* copy rest of string */
        strcat(temp,&string[pos]);

        /* copy entire string back to original */
        strcpy(string,temp);

        free(temp);
        return string;

} /* insert_str() */


char *search_replace_word( 	char *source_destination,
				char *search_str, 
				char *replace_str )
{
        int here, len_search = strlen( search_str );

	if ( (here = instr_exclude_double_quoted(
			search_str,
			source_destination, 1) ) == -1 )
	{
		return source_destination;
	}

	delete_str( source_destination, here, len_search );
	insert_str( replace_str, source_destination, here );
	return source_destination;
} /* search_replace_word */

char *search_replace_single_quoted_string(
			char *source_destination,
			char *search_string,
			char *replace_string )
{
	char buffer[ 1024 ];
	sprintf( buffer, "'%s'", replace_string );
	strcpy( replace_string, buffer );
	return search_replace_string(
			source_destination,
			search_string,
			replace_string );
}

char *timlib_search_replace_character_limited(
					char *source_destination,
					char search_character,
					char replace_character,
					int how_many )
{
	char *ptr = source_destination;

	while( *ptr )
	{
		if ( *ptr == search_character )
		{
			*ptr = replace_character;
			if ( !--how_many ) break;
		}
		ptr++;
	}

	return source_destination;
} /* timlib_search_replace_character_limited() */

char *search_replace_character(
			char *source_destination,
			char search_character, 
			char replace_character )
{
	char *ptr = source_destination;

	while( *ptr )
	{
		if ( *ptr == search_character ) *ptr = replace_character;
		ptr++;
	}

	return source_destination;
} /* search_replace_character() */

char *search_replace_string(
			char *source_destination,
			char *search_string,
			char *replace_string )
{
	return search_replace( 	search_string, 
				replace_string, 
				source_destination );
} /* search_replace_string() */

char *search_replace_once(
			char *source_destination,
			char *search_string,
			char *replace_string )
{
        int here,len_search = strlen(search_string);

        if ((here = instr(search_string, source_destination, 1)) == -1)
	{
                return source_destination;
	}

        delete_str( source_destination, here, len_search );
        insert_str( replace_string, source_destination, here );
        return source_destination;

} /* search_replace_once() */

char *timlib_remove_character( char *source_destination, char character )
{
	return remove_character( source_destination, character );
}

char *remove_character( char *source_destination, char character )
{
	char *anchor = source_destination;

	if ( source_destination )
	{
		while( *source_destination )
		{
			if ( *source_destination == character )
				timlib_strcpy(	source_destination,
						source_destination + 1,
						0 );
			else
				source_destination++;
		}
	}

	return anchor;

} /* remove_character() */

char *search_replace( 	char *search_str, 
			char *replace_str, 
			char *source_destination )
{
        int here,len_search = strlen(search_str);
	char *return_pointer;
	int str_len = strlen( replace_str );

	return_pointer = source_destination;
	if ( strcmp( search_str, replace_str ) == 0 )
		return return_pointer;

        while( 1 )
	{
                if ((here = instr(search_str, source_destination, 1)) == -1)
		{
                        return return_pointer;
		}

                delete_str( source_destination, here, len_search );
                insert_str( replace_str, source_destination, here );
		source_destination += (here + str_len );
        }

} /* search_replace */

char *skip_words( char *source, int number_words )
{

	/* Skip leading spaces */
	/* ------------------- */
	while ( *source && isspace( *source ) ) source++;

	while( number_words-- )
	{
		/* Skip word */
		/* --------- */
		while ( *source && !isspace( *source ) ) source++;

		/* Skip spaces */
		/* ----------- */
		while ( *source && isspace( *source ) ) source++;
	}
	return source;
}

void remove_end_characters( char *d, char *s, char end_character );

char *trim_quotes( char *d, char *s )
{
	return remove_quotes( d, s );
}

char *remove_quotes( char *d, char *s )
{
	char buffer[ 1024 ];

	remove_end_characters( buffer, s, '\'' );
	remove_end_characters( d, buffer, '\"' );
	return d;
}

void remove_end_characters( char *d, char *s, char end_character )
{
	if ( *s == end_character )
		strcpy( d, s + 1 );
	else
		strcpy( d, s );
		
	if ( * ( d + strlen( d ) - 1 ) == end_character )
		*( d + strlen( d ) - 1 ) = '\0';
}

/* ------------------------------------ */
/* Sample source: "age(birth_date)"	*/
/* ------------------------------------ */
int get_parameter(	char *destination, 
			char *source, 
			int parameter_index 	/* zero based */  )
{
	char without_parans[ 128 ];

	remove_parans( without_parans, source );
	if ( !piece( destination, ',', without_parans, parameter_index ) )
	{
		return 0;
	}

	/* remove_quotes( destination, destination ); */
	return 1;

} /* get_parameter() */


void remove_parans( char *without_parans, char *source )
{
	char tmp_buffer[ 128 ];

	/* Note: parans are optional */
	/* ------------------------- */
	if ( !piece( tmp_buffer, '(', source, 1 ) )
	{
		strcpy( without_parans, skip_words( source, 1 ) );
		return;
	}
	piece( without_parans, ')', tmp_buffer, 0 );

} /* remove_parans() */

int count_character( int char_to_count, char *source )
{
	return number_occurrences_char( char_to_count, source );
}

int count_characters( int char_to_count, char *source )
{
	return number_occurrences_char( char_to_count, source );
}

int timlib_count_delimiters( int char_to_count, char *source )
{
	return number_occurrences_char( char_to_count, source );
}

int timlib_character_count( char char_to_count, char *source )
{
	return number_occurrences_char( char_to_count, source );
}

int character_count( char char_to_count, char *source )
{
	return number_occurrences_char( char_to_count, source );
}

int number_occurrences_char( int char_to_count, char *source )
{
	int count = 0;

	if ( source )
	{
		while( *source )
		{
			if ( *source == char_to_count ) count++;
			source++;
		}
	}

	return count;

} /* number_occurrences_char() */

char *place_commas_in_unsigned_long_long( unsigned long long n )
{
        char s[ 64 ];

        sprintf( s, "%llu", n );
	return place_commas_in_number_string( s );

} /* place_commas_in_unsigned_long_long() */
 
char *place_commas_in_number_string( char *s )
{
	static char return_string[ 64 ];
	char *r_ptr;
	char *s_ptr;
	int c = 0;
 
	if ( !timlib_strlen( s ) ) return "";

	r_ptr = &return_string[ 63 ];
	*r_ptr-- = '\0';
 
	s_ptr = s + strlen( s ) - 1;

	if ( *s_ptr != '.' && !isdigit( *s_ptr ) ) return s;

	if ( instr( ".", s, 1 ) != -1 )
	{
		while( s_ptr >= s )
		{
			if ( *s_ptr == '.' )
			{
				*r_ptr-- = *s_ptr--;
				break;
			}
			else
			{
				*r_ptr-- = *s_ptr--;
			}
		}
	}

	while( s_ptr >= s )
	{
		*r_ptr-- = *s_ptr--;

		if ( *s_ptr == '-' )
		{
			*r_ptr-- = *s_ptr--;
			continue;
		}

		if ( ++c == 3 )
		{
			if ( s_ptr != s - 1 ) *r_ptr-- = ',';
			c = 0;
		}
	}

	return r_ptr + 1;

} /* place_commas_in_number_string()  */

char *place_commas_in_integer( int n )
{
	char s[ 20 ];

	sprintf( s, "%d", n );
	return place_commas_in_number_string( s );

} /* place_commas_in_integer()  */

char *place_commas_in_unsigned_int( unsigned int n )
{
	char s[ 20 ];

	sprintf( s, "%u", n );
	return place_commas_in_number_string( s );

} /* place_commas_in_unsigned_int()  */

char *place_commas_in_unsigned_long( unsigned long n )
{
	char s[ 64 ];

	sprintf( s, "%lu", n );
	return place_commas_in_number_string( s );

} /* place_commas_in_unsigned_long() */
 
char *place_commas_in_long( long n )
{
	char s[ 64 ];

	sprintf( s, "%ld", n );
	return place_commas_in_number_string( s );

} /* place_commas_in_long() */

char *center( char *string, int screen_len )
{
        static char line[ 512 ];
        int x;
        int str_len_div_two = strlen( string ) / 2;
        int scr_len_div_two = screen_len / 2;

        /* fill lead with blanks */
        /* --------------------- */
        for (x = 0;x < scr_len_div_two - str_len_div_two; x++)
                line[x] = ' ';

        /* place string into line */
        /* ---------------------- */
        line[x] = '\0';
        strcat(line,string);

        return line;

} /* center() */

char *center_rpad( char *string, int screen_len )
{
        static char line[ 512 ];
        int x;
        int str_len_div_two = strlen( string ) / 2;
        int scr_len_div_two = screen_len / 2;
	char *ptr;

	strcpy( line, center( string, screen_len ) );

	/* pad with trailing spaces */
	/* ------------------------ */
	ptr = line + strlen( line );
        for (x = 0;x < scr_len_div_two - str_len_div_two; x++)
                *ptr++ = ' ';
	*ptr = '\0';

        return line;

} /* center_rpad() */

char *right( char *string, char *substr, int width )
{
	static char line[ 1024 ];
	int here;

	strcpy( line, string );
	
	here = width - strlen( substr );

	/* fill middle with blanks */
	/* ---------------------- */
	memset( line + strlen( string ), ' ', here );

	strcpy( line + here, substr );
	return line;

} /* right() */

char *timlib_right_string( char *string, int width )
{
	static char return_string[ 1024 ];
	char *return_ptr;
	char *source_ptr;

	if ( width > timlib_strlen( string ) ) return (char *)0;

	return_ptr = return_string;
	source_ptr = string + width;

	while( *source_ptr )
	{
		*return_ptr++ = *source_ptr++;
	}

	*return_ptr = '\0';
	return return_string;

} /* timlib_right_string() */


char *get_date( char *d )
{
	FILE *p = popen( "date.sh", "r" );
	get_line( d, p );
	pclose( p );
	return d;
} /* get_date() */

char *get_system( char *sys_str )
{
	char buffer[ 1024 ];
	FILE *p;

	p = popen( sys_str, "r" );
	if ( !p )
	{
		fprintf(stderr,
			"ERROR: get_system(%s) cannot open pipe\n",
			sys_str );
		exit( 1 );
	}

	*buffer = '\0';
	get_line( buffer, p );
	pclose( p );
	return strdup( buffer );
	
} /* get_system() */

char *skip_spaces( char *s )
{
	while ( *s && isspace( *s ) ) s++;
	return s;
} /* skip_spaces() */

FILE *open_file( char *s, char *mode )
{
	FILE *f = fopen( s, mode );
	if ( !f )
	{
		fprintf( stderr, "ERROR: cannot open file %s\n", s );
		exit( 1 );
	}
	return f;
} /* open_file() */

char **pipe_list2list( char *pipe_list, int max_items )
{
	char **return_list = (char **)calloc( max_items + 1, sizeof( char * ) );
	char **return_list_ptr = return_list;
	char buffer[ 512 ];
	char i;

	for( i = 0; piece( buffer, '|', pipe_list, i ); i++ )
		*return_list_ptr++ = strdup( buffer );
	return return_list;
} /* pipe_list2list() */

char *double_quotes_around( char *destination, char *s )
{
	return quotes_around( destination, s, '\"' );
}

char *single_quotes_around( char *destination, char *s )
{
	return quotes_around( destination, s, '\'' );
}

char *quotes_around( char *destination, char *s, int c )
{
	if ( !s )
	{
		sprintf( destination, "%c%c", c, c );
	}
	else
	{
		sprintf( destination, "%c%s%c", c, s, c );
	}
	return destination;
}

char *timlib_pad_spaces_to_justify_left( char *d, char *s, int size )
{
	sprintf( d, "%-*s", size, s );

/*
	register int spaces_remaining;
	int str_len;
	char *end_ptr;

	strcpy( d, s );
	str_len = strlen( d );
	end_ptr = d + str_len;
	spaces_remaining = size - str_len;

	while( spaces_remaining-- > 0 ) *end_ptr++ = ' ';

	*end_ptr = '\0';
*/
	return d;
} /* timlib_pad_spaces_to_justify_left() */

char *timlib_pad_spaces_to_justify_right( char *d, char *s, int size )
{
	sprintf( d, "%*s", size, s );
	return d;
} /* timlib_pad_spaces_to_justify_right() */

double *linear_interpolate( 	double *buffer,
				int x1,
				int x2 )
{
	double slope;
	double y1, y2;
	register int i;
	double new_value;
	double *anchor = buffer;

	y1 = buffer[ x1 ];
	y2 = buffer[ x2 ];

	slope =	( y2 - y1 ) / (double)( x2 - x1 );

	new_value = y1 + slope;
	buffer += (x1 + 1);

	for( i = x1 + 1; i < x2; i++ )
	{
		*buffer++ = new_value;
		new_value += slope;
	}

	return anchor;

} /* linear_interpolate() */

void timlib_atof_array( double *f_array, char *pipe_delimited_string )
{
	int pipe_count, i;
	char buffer[ 128 ];

	pipe_count = count_characters( '|', pipe_delimited_string );

	if ( !pipe_count )
	{
		fprintf( stderr, 
			 "Error: %s() needs a piped delimited string\n",
			 __FUNCTION__ );
		exit( 1 );
	}

	for ( i = 0; i <= pipe_count; i++ )
	{
		piece( buffer, '|', pipe_delimited_string, i );
		f_array[ i ] = atof( buffer );
	}
} /* timlib_atof_array() */

double get_percent_of_day( char *time_string )
{
	int hour, minute;
	char temp_buffer[ 3 ];

	if ( strlen( time_string ) != 4 )
	{
		fprintf( stderr,
			 "WARNING: get_percent_of_day(%s) must be length 4\n",
			 time_string );
		return 0.0;
	}

	*(temp_buffer + 2) = '\0';

	strncpy( temp_buffer, time_string, 2 );
	hour = atoi( temp_buffer );

	strncpy( temp_buffer, time_string + 2, 2 );
	minute = atoi( temp_buffer );
	return time2day_percentage( hour, minute );

} /* get_percent_of_day() */


double time2day_percentage( int hour, int minute )
{
	double hour_percentage;
	double minute_percentage;

	hour_percentage = (double)hour / 24.0;
	minute_percentage = (double)minute / 1440.0;

	return hour_percentage + minute_percentage;
} /* time2day_percentage() */

double float_abs( double f )
{
	return abs_float( f );
}

double abs_float( double f )
{
	if ( f < 0.0 )
		return -f;
	else
		return f;
}

void increment_time_one_hour( char *time_string )
{
	char hour[ 3 ], minute[ 3 ];
	int new_hour;

	if ( strlen( time_string ) != 4 )
	{
		fprintf( stderr,
	"Warning in increment_time_one_hour(%s) time must be of length 4\n",
			 time_string );
		exit( 1 );
	}

	*hour = *time_string;
	*(hour + 1) = *(time_string + 1);
	*(hour + 2) = '\0';

	strcpy( minute, time_string + 2 );
	new_hour = atoi( hour ) + 1;
	sprintf( time_string, "%.2d%s", new_hour, minute );

} /* increment_time_one_hour() */

int zap_file( char *filename )
{
	FILE *f;

	f = fopen( filename, "w" );
	if ( !f )
	{
		return 0;
	}
	fclose( f );
	return 1;
} /* zap_file() */

boolean timlib_dollar_virtually_same( double d1, double d2 )
{
	return dollar_virtually_same( d1, d2 );
}

boolean dollar_virtually_same( double d1, double d2 )
{
	double difference = d1 - d2;
	boolean results;
	
	results = ( abs_float( difference ) < 0.005 );

/*
fprintf( stderr, "%s/%s(): d1 = %lf and d2 = %lf, virtually same? = %d\n",
__FILE__,
__FUNCTION__,
d1, d2, results );
*/

	return results;
}

boolean timlib_double_virtually_same( double d1, double d2 )
{
	return double_virtually_same( d1, d2 );
}

boolean double_virtually_same( double d1, double d2 )
{
	double difference = d1 - d2;
	boolean results;
	
	results = ( abs_float( difference ) < 0.000005 );

/*
fprintf( stderr, "%s/%s(): d1 = %lf and d2 = %lf, virtually same? = %d\n",
__FILE__,
__FUNCTION__,
d1, d2, results );
*/

	return results;
}

boolean timlib_double_virtually_same_places(
		double d1, double d2, int places )
{
	char d1_string[ 32 ];
	char d2_string[ 32 ];

	sprintf( d1_string, "%.*lf", places, d1 );
	sprintf( d2_string, "%.*lf", places, d2 );

	return ( strcmp( d1_string, d2_string )  == 0 );
}

double floor( double d )
{
	if ( d >=0 )
		return (double)(int)d;
	else
		return -ceiling( -d );
}

double ceiling( double d )
{
	if ( d >= 0 )
		return (double)(int)(d + 0.99);
	else
		return -floor( -d );
}

double timlib_round_money( double d )
{
	char string[ 32 ];

	if ( timlib_double_virtually_same( d, 0.0 ) ) return 0.0;

	sprintf( string, "%.2lf", d );
	return atof( string );

} /* timlib_round_money() */

double round_double( double d )
{
	if ( d >= 0.0 )
		return (double)(int)(d + 0.5);
	else
		return -floor( -d );
}

double floor_double( double d )
{
	if ( d >= 0.0 )
	{
		if ( d > 1.0 )
			return (double)(int)d;
		else
		if ( d > 0.5 )
			return 0.5;
		else
		if ( d > 0.2 )
			return 0.2;
		else
		if ( d > 0.1 )
			return 0.1;
		else
		if ( d > 0.05 )
			return 0.05;
		else
			return 0.0;
	}
	else
	{
		return -ceiling_double( -d );
	}
} /* floor_double() */

double ceiling_double( double d )
{
	if ( d >= 0.0 )
	{
		if ( d < 0.05 )
			return 0.05;
		else
		if ( d < 0.1 )
			return 0.1;
		else
		if ( d < 0.2 )
			return 0.2;
		else
		if ( d < 0.5 )
			return 0.5;
		else
			return (double)(int)(d + 0.99);
	}
	else
	{
		return -floor_double( -d );
	}
} /* ceiling_double() */

char *get_node_name()
{
	struct utsname name;
	uname( &name );
	return strdup( name.nodename );
}

char *escape_string( char *source_destination )
{
	return escape_field( source_destination );
}

char *timlib_escape_field( char *source_destination )
{
	return escape_field( source_destination );
}

char *timlib_escape_character_string(
				char *source_destination,
				char *character_string )
{
	char buffer[ 65536 ];

	strcpy( buffer, source_destination );

	return timlib_escape_character_array(
			source_destination /* destination */,
			buffer /* source */,
			character_string /* character_array */ );

}

char *escape_field( char *source_destination )
{
	char character_string[ 16 ];

	sprintf( character_string,
		 "',$()%c",
		 '%' );

	return timlib_escape_character_string(
				source_destination,
				character_string );

/*
	char buffer[ 65536 ];

	strcpy( buffer, source_destination );
	escape_character( source_destination, buffer, '\'' );

	strcpy( buffer, source_destination );
	escape_character( source_destination, buffer, ',' );

	strcpy( buffer, source_destination );
	escape_character( source_destination, buffer, '%' );

	strcpy( buffer, source_destination );
	escape_character( source_destination, buffer, '$' );

	strcpy( buffer, source_destination );
	escape_character( source_destination, buffer, '(' );

	strcpy( buffer, source_destination );
	escape_character( source_destination, buffer, ')' );

#ifdef NOT_DEFINED
	strcpy( buffer, source_destination );
	escape_character( source_destination, buffer, '&' );
#endif

	return source_destination;
*/

} /* escape_field() */

char *timlib_escape_single_quotes( char *destination, char *source )
{
	escape_character( destination, source, '\'' );
	return destination;
}

char *escape_single_quotes( char *data )
{
	char buffer[ 65536 ];
	strcpy( data, escape_character( buffer, data, '\'' ) );
	return data;
}

char *escape_dollar_signs( char *data )
{
	return escape_dollar_sign( data );
}

char *escape_dollar_sign( char *data )
{
	char buffer[ 65536 ];

	strcpy( data, escape_character( buffer, data, '$' ) );
	return data;
}

char *timlib_trim_money_characters( char *amount )
{
	static char destination[ 128 ];
	char *ptr = destination;

	while( *amount )
	{
		if ( *amount != '$' && *amount != ',' )
		{
			*ptr++ = *amount;
		}
		amount++;
	}

	*ptr = '\0';
	return destination;
} /* timlib_trim_money_characters() */

char *escape_single_quote( char *data )
{
	return escape_single_quotes( data );
}

char *timlib_escape_character_array(
				char *destination,
				char *source,
				char *character_array )
{
	char *character_array_anchor = character_array;
	char *destination_anchor = destination;

	while ( *source )
	{
		character_array = character_array_anchor;

		while( *character_array )
		{
			if ( *source == *character_array )
			{
				*destination++ = '\\';
				break;
			}
			character_array++;
		}
		*destination++ = *source++;

	}
	*destination = '\0';

	return destination_anchor;

} /* timlib_escape_character_array() */

char *escape_character( char *destination,
			char *data,
			int character_to_escape )
{
	char *anchor = destination;

	if ( !data )
	{
		*destination = '\0';
		return destination;
	}

	while ( *data )
	{
		if ( *data == character_to_escape )
			*destination++ = '\\';
		*destination++ = *data++;
	}
	*destination = '\0';
	return anchor;
}

char *unescape_character(	char *destination,
				char *data,
				int character_to_unescape )
{
	char *anchor = destination;

	if ( !data )
	{
		*destination = '\0';
		return destination;
	}

	while ( *data )
	{
		if ( *data == '\\' )
		{
			if ( *(data + 1) == character_to_unescape )
			{
				data++;
			}
		}
		*destination++ = *data++;
	}
	*destination = '\0';
	return anchor;
}

char *timlib_string_array_display( char **string_array )
{
	char buffer[ 65536 ];
	char *ptr = buffer;
	static boolean first_time = 1;

	while( *string_array )
	{
		if ( first_time )
			first_time = 0;
		else
			ptr += sprintf( ptr, "," );

		ptr += sprintf( ptr, "%s", *string_array++ );
	}
	return strdup( buffer );
} /* timlib_string_array_display() */

int string_array_search_offset( char **string_array, char *search_string )
{
	int offset = 0;

	while( *string_array )
	{
		if ( strcmp( search_string, *string_array ) == 0 )
			return offset;
		offset++;
		string_array++;
	}
	return -1;
} /* string_array_search_offset() */

char *last_word_string( char *s )
{
	char *end = s + strlen( s );

	/* Skip trailing spaces */
	/* -------------------- */
	while( *end && isspace( *end ) ) end--;

	while ( end > s )
	{
		if ( isspace( *end ) )
		{
			end++;
			break;
		}
		end--;
	}
	return end;
} /* last_word_string() */

int is_mnemonic( char *s )
{
	while ( *s )
		if ( *s++ == '_' )
			return 1;
	return 0;
}

void unescape_string( char *s )
{
	search_replace_string( s, "\\", "" );
}

unsigned long get_file_size( char *filename )
{
	char sys_string[ 1024 ];
	char *results_string;

	sprintf( sys_string,
		 "echo '%s'				|"
		 "stat.e				|"
		 "grep \"\\.size \"			|"
		 "column.e 1				 ",
		 filename );

	results_string = pipe2string( sys_string );
	if ( !results_string )
		return 0;
	else
		return atol( results_string );
}

boolean timlib_exists_special_character( char *data )
{
	while( *data )
	{
		if ( *data != '_' && !isalnum( *data ) )
			return 1;
		data++;
	}
	return 0;
}

void search_replace_special_characters( char *buffer )
{
	search_replace_string( buffer, "`", "'" );
	search_replace_string( buffer, "\\", "" );
	search_replace_string( buffer, "\"", "'" );
/*
	search_replace_string( buffer, "^", "+" );
*/

} /* search_replace_special_characters() */

void escape_special_characters( char *data )
{
	char buffer[ 4096 ];

	if ( !*data ) return;

	strcpy( buffer, data );
	escape_character( data, buffer, '=' );

	strcpy( buffer, data );
	escape_character( data, buffer, '\'' );

	strcpy( buffer, data );
	escape_character( data, buffer, '$' );

	strcpy( buffer, data );
	escape_character( data, buffer, '&' );

	strcpy( buffer, data );
	escape_character( data, buffer, '`' );

} /* escape_special_characters() */

char *mysql2american_date( char *date_buffer, char *mysql_date )
{
	char piece_buffer[ 128 ];

	*date_buffer = '\0';

	if ( !piece( piece_buffer, '-', mysql_date, 1 ) ) return "";
	strcpy( date_buffer, piece_buffer );

	if ( !piece( piece_buffer, '-', mysql_date, 2 ) ) return "";
	strcat( date_buffer, "/" );
	strcat( date_buffer, piece_buffer );

	if ( !piece( piece_buffer, '-', mysql_date, 0 ) ) return "";
	strcat( date_buffer, "/" );
	strcat( date_buffer, piece_buffer );

	return date_buffer;

} /* mysql2american_date() */

char *place_commas_in_double( double d )
{
	static char destination[ 64 ];

	commas_in_double(	destination, 
				d );
	return destination;

} /* place_commas_in_double() */
 
char *timlib_place_commas_in_dollars( double d )
{
	char s[ 20 ];

	sprintf( s, "%.0lf", round_double( d ) );
	return place_commas_in_number_string( s );

} /* timlib_place_commas_in_dollars()  */

char *timlib_commas_in_dollars( double d )
{
	return place_commas_in_money( d );
}

char *timlib_place_commas_in_money( double d )
{
	return place_commas_in_money( d );
}

char *place_commas_in_money( double d )
{
	char *results;

	/* Returns static memory of 3 decimal places */
	/* ----------------------------------------- */
	results = place_commas_in_double( d );
	*( results + strlen( results ) - 1 ) = '\0';
	return results;

} /* place_commas_in_money() */

char *commas_in_double(		char *destination, 
				double d )
{
	char integer_part[ 64 ];
	char buffer[ 64 ];
	char decimal_part[ 8 ];
	char d_string[ 128 ];
	char reversed_integer[ 64 ];
	char *reversed_integer_pointer = reversed_integer;
	char *integer_part_pointer;
	register int counter = 0;

	/* Sometimes, d is -0.0 */
	/* -------------------- */
	if ( timlib_double_virtually_same( d, 0.0 ) ) d = 0.0;

	sprintf( d_string, "%.3lf", d );

	piece( integer_part, '.', d_string, 0 );
	piece( decimal_part, '.', d_string, 1 );
 
	integer_part_pointer = integer_part + strlen( integer_part ) - 1;

	while( integer_part_pointer >= integer_part )
	{
		*reversed_integer_pointer++ = *integer_part_pointer;
		if ( integer_part_pointer > integer_part
		&&   *(integer_part_pointer - 1) != '-' )
		{
			if ( !(++counter % 3 ) )
			{
				if ( integer_part_pointer != integer_part )
				{
					*reversed_integer_pointer++ = ',';
				}
			}
		}
		integer_part_pointer--;
	}
	*reversed_integer_pointer = '\0';
	reverse_string( buffer, reversed_integer );
	sprintf( destination, "%s.%s", buffer, decimal_part );
	return destination;
} /* commas_in_double() */

char *reverse_string(			char *destination,
					char *source )
{
	char *marker = destination;
	char *ptr = source + strlen( source ) - 1;

	while( ptr >= source )
	{
		*destination++ = *ptr--;
	}
	*destination = '\0';
	return marker;
} /* reverse_string() */

void make_single_quotes_double_single_quotes( char *d )
{
	char tmp[ 65536 ];
	char *tmp_ptr = tmp;

	strcpy( tmp, d );

	while ( *tmp_ptr )
	{
		if ( *tmp_ptr == '\'' )
		{
			/* If at beginning */
			/* --------------- */
			if ( tmp_ptr == tmp )
			{
				if ( *(tmp_ptr + 1) && *(tmp_ptr + 1) != '\'' )
				{
					*d++ = '\'';
					*d++ = *tmp_ptr++;
				}
				else
				{
					*d++ = *tmp_ptr++;
				}
			}
			else
			/* ------------ */
			/* If at ending */
			/* ------------ */
			if ( !*(tmp_ptr + 1 ) )
			{
				if ( *(tmp_ptr - 1) && *(tmp_ptr - 1) != '\'' )
				{
					*d++ = '\'';
					*d++ = *tmp_ptr++;
				}
				else
				{
					*d++ = *tmp_ptr++;
				}
			}
			else
			/* ----------------- */
			/* Must be in middle */
			/* ----------------- */
			if ( *(tmp_ptr + 1) && *(tmp_ptr + 1) != '\''
			&&   *(tmp_ptr - 1) && *(tmp_ptr - 1) != '\'' )
			{
				*d++ = '\'';
				*d++ = *tmp_ptr++;
			}
			else
			{
				*d++ = *tmp_ptr++;
			}
		}
		else
		{
			*d++ = *tmp_ptr++;
		}
	}

	*d = '\0';

} /* make_single_quotes_double_single_quotes() */

boolean timlib_file_populated( char *filename )
{
	char sys_string[ 1024 ];
	char *return_string;

	sprintf( sys_string,
		 "stat.e %s | grep '.size' | column.e 1",
		 filename );
	return_string = pipe2string( sys_string );

	if ( !return_string || !*return_string )
		return 0;
	else
		return atoi( return_string );

} /* timlib_file_populated() */

char *timlib_mysql_date2ddmmyyyy(	char *destination,
					char *source )
{
	return mysql2american_date( destination, source );
}

char *timlib_oracle_date2mysql( char *s )
{
	return timlib_ora_date_to_yyyy_mm_dd( s );
}

char *timlib_yyyy_mm_dd_to_julian( char *yyyy_mm_dd )
{
	static char julian_string[ 16 ];

	sprintf(julian_string,
		"%.1lf",
		julian_yyyy_mm_dd_to_julian( yyyy_mm_dd ) );
	return julian_string;
}

char *timlib_oracle_date2mysql_date_string( char *s )
{
	return timlib_ora_date_to_yyyy_mm_dd( s );
}

char *timlib_ora_date_to_yyyy_mm_dd( char *s )
{
	static char ret_buffer[ 128 ];
	int mm_int;
	char yyyy[ 128 ], mon[ 128 ], dd[ 128 ];

	strcpy( ret_buffer, s );

	if ( ! piece( dd, '-', s, 0 ) ) return ret_buffer;
	if ( ! piece( mon, '-', s, 1 ) ) return ret_buffer;
	if ( ! piece( yyyy, '-', s, 2 ) ) return ret_buffer;

	if ( strlen( yyyy ) == 2 )
		strcpy( yyyy, julian_make_y2k_year( yyyy ) );

	mm_int = timlib_ora_month2integer( mon );
	if ( mm_int < 1 ) return ret_buffer;

	sprintf( ret_buffer, 
		 "%s-%02d-%02d",
		 yyyy,
		 mm_int,
		 atoi( dd ) );

	return ret_buffer;

} /* timlib_ora_date_to_yyyy_mm_dd() */

char *timlib_month_array[] = { 	"JAN",
				"FEB",
				"MAR",
				"APR",
				"MAY",
				"JUN",
				"JUL",
				"AUG",
				"SEP",
				"OCT",
				"NOV",
				"DEC",
				(char *)0 };

char *timlib_get_three_character_month_string( int month_offset )
{
	if ( month_offset < 0 || month_offset >= 12 )
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: invalid month_offset = (%d)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			month_offset );
		exit( 1 );
	}
	else
	{
		return timlib_month_array[ month_offset ];
	}

} /* timlib_get_three_character_month_string() */

char *timlib_full_month_array[] = {
				"January",
				"February",
				"March",
				"April",
				"May",
				"June",
				"July",
				"August",
				"September",
				"October",
				"November",
				"December",
				(char *)0 };

int timlib_full_month2integer( char *full_month )
{
	char **array_ptr = timlib_full_month_array;
	register int i = 0;

	while( *array_ptr )
	{
		if ( strcasecmp( full_month, *array_ptr ) == 0 )
		{
			return i;
		}
		else
		{
			i++;
			array_ptr++;
		}
	}
	return -1;
} /* timlib_full_month2integer() */

char *timlib_integer2full_month( int month_integer )
{
	if ( month_integer < 1 || month_integer > 12 )
		return "unknown";
	else
		return timlib_full_month_array[ month_integer - 1 ];
}

int timlib_ora_month2integer( char *ora_month )
{
	char **array_ptr = timlib_month_array;
	register int i = 1;

	while( *array_ptr )
	{
		if ( strcasecmp( ora_month, *array_ptr ) == 0 )
		{
			return i;
		}
		else
		{
			i++;
			array_ptr++;
		}
	}
	return 0;
} /* timlib_ora_month2integer() */

char *timlib_yyyymmdd_to_oracle_format(	char *s )
{
	return timlib_yyyymmdd_to_ora_date( s );
}

char *timlib_yyyymmdd_to_ora_date( char *s )
{
	static char ret_buffer[ 128 ];
	int mm_int;
	char yy[ 16 ], mm[ 16 ], dd[ 16 ];

	strcpy( ret_buffer, s );

	if ( ! piece( yy, '-', s, 0 ) ) return ret_buffer;
	if ( ! piece( mm, '-', s, 1 ) ) return ret_buffer;
	if ( ! piece( dd, '-', s, 2 ) ) return ret_buffer;

	mm_int = atoi( mm );
	if ( mm_int < 1 || mm_int > 12 ) return "00-000-0000";

	sprintf( ret_buffer, 
		 "%s-%s-%s",
		 dd,
		 timlib_month_array[ mm_int - 1 ],
		 yy );

	return ret_buffer;

} /* timlib_yyyymmdd_to_ora_date() */

boolean timlib_exists_character( 	char *s,
					char ch )
{
	while( *s )
	{
		if ( *s == ch )
			return 1;
		s++;
	}
	return 0;
}

long timlib_random( long up_to )
{
/*
	double random_double;

	random_double = (double)random() / (double)RAND_MAX;
	return (long)((double)up_to * random_double) + 1;
	double r_num;
	long time();
	double drand48();

	r_num = drand48() * (double)time(0);
	r_num = r_num - (int)r_num;
	return ((long)(r_num * up_to) + 1);
*/
	return (long)( (drand48() * up_to) + 1 );
}

char *timlib_trim_double_quotes( char *s )
{
	int str_len;

	if ( !s ) return (char *)0;

	if ( *s == '"' ) timlib_strcpy( s, s + 1, 65536 );

	str_len = strlen( s );

	if ( str_len
	&&  *( s + str_len - 1 ) == '"' )
	{
		*(s + str_len - 1) = '\0';
	}

	return s;

} /* timlib_trim_double_quotes() */

boolean timlib_parse_database_string(	char **database_string,
					char *application_name )
{
	int position;

	*database_string = '\0';

	if ( ( position = instr( ":", application_name, 1 ) ) != -1 )
	{
		*database_string = application_name + position + 1;
		*(application_name + position ) = '\0';
		return 1;
	}
	return 0;
} /* timlib_parse_database_string() */

#ifdef NOT_DEFINED
char *timlib_get_parameter_application_name(
					char *application_name,
					char *database_string )
{

	if ( database_string && *database_string )
	{
		char return_string[ 128 ];

		sprintf( return_string,
			 "%s:%s",
			 application_name,
			 database_string );
		return strdup( return_string );
	}
	else
	{
		return application_name;
	}

} /* timlib_get_parameter_application_name() */
#endif

int timlib_get_x_increment_seconds(
				int expected_count_per_day,
				enum aggregate_level aggregate_level )
{
	int x_increment;

	if ( aggregate_level == real_time && expected_count_per_day )
		x_increment = SECONDS_IN_DAY / expected_count_per_day;
	else
	if ( aggregate_level == half_hour )
		x_increment = SECONDS_IN_HALF_HOUR;
	else
	if ( aggregate_level == hourly )
		x_increment = SECONDS_IN_HOUR;
	else
	if ( aggregate_level == daily )
		x_increment = SECONDS_IN_DAY;
	else
		x_increment = 0.0;

	return x_increment;
} /* timlib_get_x_increment_seconds() */

double timlib_get_x_increment_days(
				int expected_count_per_day,
				enum aggregate_level aggregate_level )
{
	double x_increment = 0.0;

	if ( aggregate_level == real_time && expected_count_per_day )
		x_increment = 1.0 / (double)expected_count_per_day;
	else
	if ( aggregate_level == half_hour )
		x_increment = 1.0 / 48.0;
	else
	if ( aggregate_level == hourly )
		x_increment = 1.0 / 24.0;
	else
	if ( aggregate_level == daily )
		x_increment = 1.0;
	else
		x_increment = 0.0;

	return x_increment;
} /* timlib_get_x_increment_days() */

double timlib_get_x_increment(	int expected_count_per_day,
				enum aggregate_level aggregate_level )
{
	double x_increment;

	if ( aggregate_level == real_time && expected_count_per_day )
		x_increment = 1.0 / (double)expected_count_per_day;
	else
	if ( aggregate_level == half_hour )
		x_increment = 1.0 / 48.0;
	else
	if ( aggregate_level == hourly )
		x_increment = 1.0 / 24.0;
	else
	if ( aggregate_level == daily )
		x_increment = 1.0;
	else
		x_increment = 0.0;

	return x_increment;
} /* timlib_get_x_increment() */

/* From "The C Programming Language" by Kernighan and Ritchie */
/* ---------------------------------------------------------- */
unsigned getbits( unsigned integer, unsigned position, unsigned n )
{
	return ( integer >> ( position + 1 - n ) ) & ~( ~0 << n );
}

char *timlib_integer2binary( unsigned integer )
{
#define NUMBER_BINARY_DIGITS	16
	static char binary[ NUMBER_BINARY_DIGITS + 1 ] = {0};
	char *pointer;
	int i;

	pointer = binary;
	for ( i = 0; i < NUMBER_BINARY_DIGITS; i++, pointer++ )
		*pointer = '0';

	pointer = binary + NUMBER_BINARY_DIGITS - 1;

	for( i = 0; i < NUMBER_BINARY_DIGITS; i++, pointer-- )
	{
		if ( getbits( integer, i, 1 ) ) *pointer = '1';
		
	}

	return binary;
} /* timlib_integer2binary() */

boolean timlib_string_until_character_matches(
					char *source,
					char *string_compare,
					char delimiter )
{
	while( *string_compare )
	{
		if ( *source++ != *string_compare++ ) return 0;
		if ( *source == delimiter ) return 1;
		if ( !*source ) return 0;
	}
	return 0;
} /* timlib_string_until_character_matches() */

unsigned int timlib_factoral( int n )
{
	unsigned int factoral = 1;

	while( n ) factoral *= n--;
	return factoral;
} /* timlib_factoral() */

char timlib_get_delimiter( char *string )
{
	if ( timlib_exists_character( string, '|' ) )
		return '|';
	else
	if ( timlib_exists_character( string, '^' ) )
		return '^';
	else
	if ( timlib_exists_character( string, ',' ) )
		return ',';
	else
	if ( timlib_exists_character( string, ';' ) )
		return ';';
	else
	if ( timlib_exists_character( string, ':' ) )
		return ':';
	else
		return 0;

} /* timlib_get_delimiter() */

int timlib_delimiter_independent_strcmp(
			char *string1,
			char *string2 )
{
	char string1_delimiter;
	char string2_delimiter;

	string1_delimiter = timlib_get_delimiter( string1 );
	string2_delimiter = timlib_get_delimiter( string2 );

	while( *string1 )
	{
		if ( *string1 == string1_delimiter
		&&   *string2 == string2_delimiter )
		{
			string1++;
			string2++;
			continue;
		}

		if ( *string1 < *string2 ) return -1;
		if ( *string1 > *string2 ) return 1;
		string1++;
		string2++;
	}

	if ( !*string2 )
		return 0;
	else
		return 1;
} /* timlib_delimiter_independent_strcmp() */

char *timlib_with_list_get_in_clause( LIST *data_list )
{
	char in_clause[ 131072 ];
	char *ptr = in_clause;
	char *data;
	boolean first_time = 1;

	if ( !list_rewind( data_list ) ) strdup( "" );

	*in_clause = '\0';
	do {
		data = list_get_pointer( data_list );

		if ( first_time )
			first_time = 0;
		else
		{
			ptr += sprintf( ptr, "," );
		}

		ptr += sprintf( ptr, "'%s'", data );

	} while( list_next( data_list ) );

	return strdup( in_clause );

} /* timlib_with_list_get_in_clause() */

char *timlib_get_in_clause( char *data_list_string )
{
	char *in_clause;
	LIST *data_list;

	data_list = list_string2list( data_list_string, ',' );
	in_clause = timlib_with_list_get_in_clause( data_list );
	list_free( data_list );

	return in_clause;

} /* timlib_get_in_clause() */

boolean timlib_file_exists( char *filename )
{
	struct stat s;

	return ( stat( filename, &s ) != -1 );
}

double timlib_latitude_longitude_degrees_minutes_to_decimal(
					char *latitude_longitude )
{
	char degrees[ 16 ];
	char decimal[ 16 ];

	if ( !*latitude_longitude ) return 0.0;

	column( degrees, 0, latitude_longitude );
	column( decimal, 1, latitude_longitude );

	if ( !*decimal ) return 0.0;

	if ( *degrees == '-' )
	{
		return ( atof( degrees ) - (atof( decimal ) / 60.0 ) );
	}
	else
	{
		return ( atof( degrees ) + (atof( decimal ) / 60.0 ) );
	}
} /* timlib_latitude_longitude_degrees_minutes_to_decimal() */

char *timlib_latitude_longitude_degrees_decimal_to_minutes(
					char *latitude_longitude_decimal )
{
	static char degrees_minutes[ 32 ];
	char degrees[ 16 ];
	char decimal_integer_string[ 16 ];
	char decimal_float_string[ 16 ];
	double decimal_double;
	
	piece( degrees, '.', latitude_longitude_decimal, 0 );

	if ( !piece(	decimal_integer_string,
			'.',
			latitude_longitude_decimal,
			1 ) )
	{
		*decimal_integer_string = '0';
	}

	sprintf( decimal_float_string, "0.%s", decimal_integer_string );

	decimal_double = atof( decimal_float_string ) * 60.0;

	sprintf( degrees_minutes, "%s %.5lf", degrees, decimal_double );
	return degrees_minutes;

} /* timlib_latitude_longitude_degrees_decimal_to_minutes() */

void **timlib_allocate_pointer_array(
				int array_pointer_size,
				int item_pointer_size,
				int array_length )
{
	void **pointer_array;
	int x;

	if ( array_length < 1 )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: invalid array_length = %d.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 array_length );
		exit( 1 );
	}

	pointer_array = calloc( array_length, array_pointer_size );

	for( x = 0; x < array_length; x++ )
	{
		pointer_array[ x ] = calloc( 1, item_pointer_size );
	}
	return pointer_array;
} /* timlib_allocate_pointer_array() */

int timlib_strncmp( char *s1, char *s2 )
{
	int str_len2;

	if ( !s1 || !s2 ) return -1;
	if ( !*s1 && !*s2 ) return 0;
	if ( !*s1 || !*s2 ) return -1;

	str_len2 = strlen( s2 );
	return strncasecmp( s1, s2, str_len2 );

} /* timlib_strncmp() */

/* Returns 1 for yes, did some padding or 0 for no, didn't pad any. */
/* ---------------------------------------------------------------- */
boolean timlib_pad_time_four( char *time_string )
{
	int str_len = strlen( time_string );
	char buffer[ 5 ];

	if ( str_len == 4 ) return 0;

	if ( str_len == 0 || str_len > 4 ) return 0;

	if ( str_len == 1 ) sprintf( buffer, "000%s", time_string );
	if ( str_len == 2 ) sprintf( buffer, "00%s", time_string );
	if ( str_len == 3 ) sprintf( buffer, "0%s", time_string );

	strcpy( time_string, buffer );
	return 1;
} /* timlib_pad_time_four() */

int timlib_sum_delimited_string(char *string,
				char delimiter,
				int starting_piece,
				int ending_piece )
{
	int p;
	int sum = 0;
	char buffer[ 128 ];

	for (	p = starting_piece;
		p <= ending_piece;
		p++ )
	{
		if ( !piece( buffer, delimiter, string, p ) ) return -1;

		if ( *buffer && isdigit( *buffer ) )
		{
			sum += atoi( buffer );
		}
	}

	return sum;
} /* timlib_sum_delimited_string() */

char *timlib_character_pad(		char delimiter,
					int column_count )
{
	static char buffer[ 1024 ];
	char *ptr = buffer;

	if ( column_count >= 1024 ) return "";

	while( column_count-- ) *ptr++ = delimiter;
	*ptr = '\0';
	return buffer;

} /* timlib_character_pad() */

int timlib_get_seed( void )
{
	int seed;

	seed = atoi( pipe2string( "now.sh hhmmss" ) );
	return seed;
} /* timlib_get_seed() */

char *timlib_strncpy( char *s1, char *s2, int count )
{
	char *results;

	results = strncpy( s1, s2, count );
	*(s1 + count) = '\0';
	return results;
} /* timlib_strncpy() */

char *timlib_sql_injection_escape( char *source )
{
	char destination[ 65536 ];

	timlib_escape_character_array(
		destination,
		source,
		"`'$;%" );

	return strdup( destination );

} /* timlib_sql_injection_escape() */

char *timlib_delete_character(
			char *source_destination,
			char character )
{
	char buffer[ 4096 ];
	char *source_ptr = source_destination;
	char *destination_ptr = buffer;

	while( *source_ptr )
	{
		if ( *source_ptr != character )
			*destination_ptr++ = *source_ptr;

		source_ptr++;
	}

	*destination_ptr = '\0';
	strcpy( source_destination, buffer );

	return source_destination;

} /* timlib_delete_character() */

void timlib_display_error_file( char *error_filename )
{
	if ( timlib_file_populated( error_filename ) )
	{
		char sys_string[ 1024 ];

		sprintf( sys_string,
"cat %s | queue_top_bottom_lines.e 50 | html_table.e 'Load Errors' '' ''",
			 error_filename );
		fflush( stdout );
		system( sys_string );
	}
} /* timlib_display_error_file() */

boolean timlib_is_valid_time( char *time_string )
{
	int str_len = strlen( time_string );

	if ( str_len != 4
	&&   str_len != 6 )
	{
		return 0;
	}

	if ( timlib_strcmp( time_string, NULL_STRING ) == 0 ) return 1;

	while( *time_string )
	{
		if ( !isdigit( *time_string ) )
			return 0;
		time_string++;
	}
	return 1;
} /* timlib_is_valid_time() */

char *timlib_get_now_date_time( void )
{
	char sys_string[ 256 ];
	char *now_date_string;
	char *now_time_string;
	static char now_date_time[ 32 ];

	strcpy( sys_string, "now.sh ymd" );
	now_date_string = pipe2string( sys_string );

	strcpy( sys_string,
"now.sh hhmmss | sed 's/\\([0-9][0-9]\\)$/:&/' | sed 's/\\([0-9][0-9]:\\)/:&/'" );

	now_time_string = pipe2string( sys_string );

	sprintf( now_date_time,
		 "%s %s",
		 now_date_string,
		 now_time_string );

	free( now_date_string );
	free( now_time_string );

	return now_date_time;

} /* timlib_get_now_date_time() */

boolean timlib_string_empty(	char *s,
				char *empty_label )
{
	if ( !s )
		return 1;
	else
	if ( !*s )
		return 1;
	else
	if ( strcmp( s, empty_label ) == 0 )
		return 1;
	else
		return 0;

} /* timlib_string_empty() */

char *timlib_directory_filesystem(	long int *available_megabytes,
					char *directory )
{
	int max_length = 0;
	char input_buffer[ 512 ];
	FILE *input_pipe;
	char mount_point[ 128 ];
	char available_megabytes_string[ 128 ];
	int str_len;
	static char filesystem[ 128 ] = {0};

	*available_megabytes = 0;
	input_pipe = popen( "df -BM 2>/dev/null", "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		column( mount_point, 5, input_buffer );

		/* A long mount point will span two lines. */
		/* --------------------------------------- */
		if ( !*mount_point )
		{
			get_line(input_buffer + strlen( input_buffer ),
				 input_pipe );

			column( mount_point, 5, input_buffer );
		}

		if ( timlib_begins_string( directory, mount_point ) )
		{
			str_len = strlen( mount_point );

			if ( str_len > max_length )
			{
				max_length = str_len;

				strcpy( filesystem, mount_point );

				column(	available_megabytes_string,
					3,
					input_buffer );

				*available_megabytes =
					strtol( available_megabytes_string,
						(char **)0,
						10 );
			}
		}
	}

	pclose( input_pipe );

	return filesystem;

} /* timlib_directory_filesystem() */

char *timlib_get_latest_filename_datestamp( char *directory )
{
	char sys_string[ 1024 ];
	char *filename;
	int underbar_count;
	static char datestamp[ 128 ];

	sprintf( sys_string,
		 "ls -1t %s 2>/dev/null | head -1",
		 directory );

	if ( ! ( filename = pipe2string( sys_string ) ) )
		return (char *)0;

	underbar_count = count_character( '_', filename );

	if ( !underbar_count ) return (char *)0;

	piece( datestamp, '_', filename, underbar_count );

	if ( strlen( datestamp ) < 8 ) return (char *)0;

	*(datestamp + 8) = '\0';
	return datestamp;

} /* timlib_get_latest_filename_datestamp() */

boolean timlib_login_name_email_address(
				char *login_name )
{
	return ( character_exists( login_name, '@' ) );
}

char *timlib_generate_password( void )
{
	return pipe2string( "generate_password.sh" );

}

boolean timlib_double_is_integer( double d )
{
	int i;

	i = (int)d;

	return timlib_double_virtually_same( (double)i, d );
}

void *timlib_memcpy( void *object, int sizeof_object )
{
	void *new_object;

	if ( ! ( new_object = calloc( 1, sizeof_object ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	memcpy(	new_object,
		object,
		sizeof_object );

	return new_object;

} /* timlib_memcpy() */

int timlib_get_line_escape_CR(	char *in_line,
				FILE *infile,
				int buffer_size )
{
	int in_char;
	char *anchor = in_line;
	int size = 0;

	*in_line = '\0';

	while ( 1 )
	{
		in_char = fgetc( infile );

		if ( in_char == CR ) continue;

		if ( in_char == EOF )
		{
			/* If last line in file doesn't have a CR */
			/* -------------------------------------- */
			if ( in_line != anchor )
			{
				*in_line = '\0';
				return 1;
			}
			else
			{
				return 0;
			}
		}

		if ( in_char == LF )
		{
			*in_line = '\0';
			return 1;
		}

		/* If '\' then get the next character */
		/* ---------------------------------- */
		if ( in_char == '\\' )
		{
			in_char = fgetc( infile );

			/* If escaping the <CR> */
			/* -------------------- */
			if ( in_char == LF || in_char == CR )
			{
				if ( buffer_size && ( size++ == buffer_size ) )
				{
					fprintf( stderr,
			"Error in %s/%s()/%d: exceeded buffer size of %d.\n",
						 __FILE__,
						 __FUNCTION__,
						 __LINE__,
						 buffer_size );
					*in_line = '\0';
					return 1;
				}

				*in_line++ = ' ';
				continue;
			}
		}

		if ( buffer_size && ( size++ == buffer_size ) )
		{
			fprintf( stderr,
		"Error in %s/%s()/%d: exceeded buffer size of %d.\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 buffer_size );
			*in_line = '\0';
			return 1;
		}

		*in_line++ = in_char;
	}
} /* timlib_get_line_escape_CR() */

int timlib_atoi( char *s )
{
	char buffer[ 32 ];

	trim_character( buffer, ',', s );

	return atoi( buffer );

} /* timlib_atoi() */

