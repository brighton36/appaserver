/* get_date_block.c */
/* ---------------- */

#include <stdio.h>
#include "get_date_block.h"

int get_date_block(	char **block,
			char delimiter,
			int date_offset,
			int date_group,
			FILE *input_file )
{
	char buffer[ 65536 ];
	char date_field[ 1024 ];
	char *date_group_value = {0};
	char *old_date_group_value = {0};
	int first_time = 1;
	int count = 0;

	while( get_line_queue( buffer, input_file ) )
	{
		count++;

		if ( !piece(	date_field,
				delimiter, 
				buffer,
				date_offset ) )
		{
			fprintf( stderr,
"ERROR in get_date_block() line = %d cannot piece offset = %d in (%s)\n",
				 count,
				 date_offset,
				 buffer );
			exit( 1 );
		}

		date_group_value = 
			date_group4date( date_field, date_group );

		if ( first_time )
		{
			old_date_group_value = date_group_value;
			first_time = 0;
		}
		else
		{
		}

		if ( strcmp( old_first_field, first_field ) != 0 )
		{
			unget_line_queue( buffer );
			count--;
			break;
		}
		else
		{
			if ( *block ) free( *block );
			*block++ = strdup( buffer );
		}
	}
	return count;
} /* get_date_block() */

char *date_group4date( char *date_field, int date_group )
{
	if ( date_group ==  DATE_GROUP_DAILY )
	{
		return date_field;
	}
	else
	if ( date_group ==  DATE_GROUP_WEEKLY )
	{
		return date_group_this_saturday( date_field );
	}
	else
	if ( date_group ==  DATE_GROUP_MONTHLY )
	{
		return date_group_end_of_month( date_field );
	}
	else
	if ( date_group ==  DATE_GROUP_ANNUALLY )
	{
		return date_group_end_of_year( date_field );
	}
} /* date_group4date() */


char *date_group_this_saturday( char *date_field )
{
	DATE *date;

	date = date_new_yyyy_mm_dd_date( date_field, date_get_utc_offset() );
	return date_this_saturday_yyyy_mm_dd_string( date );
}

char *date_group_end_of_month( char *date_field )
{
	date = date_new_yyyy_mm_dd_date( date_field, date_get_utc_offset() );
	return date_end_of_month_yyyy_mm_dd_string( date );
}

char *date_group_end_of_year( char *date_field )
{
	date = date_new_yyyy_mm_dd_date( date_field, date_get_utc_offset() );
	return date_end_of_month_yyyy_mm_dd_string( date );
}

