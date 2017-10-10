/* library/date_convert.c */
/* ---------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "piece.h"
#include "timlib.h"
#include "julian.h"
#include "folder.h"
#include "date.h"
#include "date_convert.h"

enum date_convert_format date_convert_get_database_date_format(
					char *application_name )
{
	char sys_string[ 1024 ];
	char where_string[ 128 ];
	char *results;
	static enum date_convert_format date_format = date_convert_unknown;

	if ( date_format != date_convert_unknown ) return date_format;

	sprintf( where_string, "application = '%s'", application_name );
	
	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=database_date_format	"
		 "			folder=application		"
		 "			where=\"%s\"			",
		 application_name,
		 where_string );
	
	results = pipe2string( sys_string );
	if ( !results || !*results ) return date_convert_unknown;
	
	date_format = date_convert_get_date_convert_format( results );
	return date_format;
} /* date_convert_get_database_date_format() */

enum date_convert_format date_convert_get_user_date_format(
					char *application_name,
					char *login_name )
{
	static enum date_convert_format user_date_format = date_convert_unknown;
	char sys_string[ 1024 ];
	char where_string[ 128 ];
	char *results;

	if ( user_date_format != date_convert_unknown ) return user_date_format;

	/* Get from APPASERVER_USER */
	/* ------------------------ */
	sprintf( where_string, "login_name = '%s'", login_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=user_date_format		"
		 "			folder=appaserver_user		"
		 "			where=\"%s\"			",
		 application_name,
		 where_string );

	results = pipe2string( sys_string );

	user_date_format = date_convert_get_date_convert_format( results );

	if ( user_date_format == date_convert_unknown )
	{
		/* Get from APPLICATION */
		/* -------------------- */
		sprintf( where_string, "application = '%s'", application_name );
	
		sprintf( sys_string,
			 "get_folder_data	application=%s		"
			 "			select=user_date_format	"
		 	"			folder=application	"
			 "			where=\"%s\"		",
			 application_name,
			 where_string );
		results = pipe2string( sys_string );
		if ( !results || !*results ) return date_convert_unknown;
	
		user_date_format =
			date_convert_get_date_convert_format(
				results );
	}

	return user_date_format;
} /* date_convert_get_user_date_format() */

DATE_CONVERT *date_convert_new_user_format_date_convert(
				char *application_name,
				char *login_name,
				char *date_string )
{
	enum date_convert_format user_date_format;

	user_date_format =
		date_convert_get_user_date_format(
					application_name,
					login_name );

	return date_convert_new_date_convert(
				user_date_format,
				date_string );
} /* date_convert_new_user_format_date_convert() */

DATE_CONVERT *date_convert_new_date_convert(
				enum date_convert_format destination_format,
				char *date_string )
{
	DATE_CONVERT *d = (DATE_CONVERT *)calloc( 1, sizeof( DATE_CONVERT ) );

	if ( !d )
	{
		fprintf(stderr,
			"ERROR in %s/%s(): cannot allocate memory.\n",
			__FILE__,
			__FUNCTION__ );
		exit( 1 );
	}

	if ( strcasecmp( date_string, "today" ) == 0 )
		date_string = date_get_today_yyyy_mm_dd( HOURS_WEST_GMT );
	else
	if ( strcasecmp( date_string, "yesterday" ) == 0 )
		date_string =
			date_get_yesterday_yyyy_mm_dd(
				HOURS_WEST_GMT );
	else
	if ( strcasecmp( date_string, "monday" ) == 0 )
		date_string =
			date_get_day_of_week_yyyy_mm_dd(
				WDAY_MONDAY,
				HOURS_WEST_GMT );
	else
	if ( strcasecmp( date_string, "tuesday" ) == 0 )
		date_string =
			date_get_day_of_week_yyyy_mm_dd(
				WDAY_TUESDAY,
				HOURS_WEST_GMT );
	else
	if ( strcasecmp( date_string, "wednesday" ) == 0 )
		date_string =
			date_get_day_of_week_yyyy_mm_dd(
				WDAY_WEDNESDAY,
				HOURS_WEST_GMT );
	else
	if ( strcasecmp( date_string, "thursday" ) == 0 )
		date_string =
			date_get_day_of_week_yyyy_mm_dd(
				WDAY_THURSDAY,
				HOURS_WEST_GMT );
	else
	if ( strcasecmp( date_string, "friday" ) == 0 )
		date_string =
			date_get_day_of_week_yyyy_mm_dd(
				WDAY_FRIDAY,
				HOURS_WEST_GMT );
	else
	if ( strcasecmp( date_string, "saturday" ) == 0 )
		date_string =
			date_get_day_of_week_yyyy_mm_dd(
				WDAY_SATURDAY,
				HOURS_WEST_GMT );
	else
	if ( strcasecmp( date_string, "sunday" ) == 0 )
		date_string =
			date_get_day_of_week_yyyy_mm_dd(
				WDAY_SUNDAY,
				HOURS_WEST_GMT );

	d->source_format = date_convert_date_get_format( date_string );
	d->destination_format = destination_format;

	if ( d->source_format == date_convert_unknown
	||   d->destination_format == date_convert_unknown )
	{
		strcpy( d->return_date, date_string );
		return d;
	}

	d->source_format = date_convert_populate_return_date(
				d->return_date,
				d->source_format,
				d->destination_format,
				date_string );

	return d;
} /* date_convert_new_date_convert() */

DATE_CONVERT *date_convert_new_database_format_date_convert(
				char *application_name,
				char *date_string )
{
	enum date_convert_format database_date_format;

	database_date_format =
		date_convert_get_database_date_format(
					application_name );

	return date_convert_new_date_convert(
				database_date_format,
				date_string );
} /* date_convert_new_user_format_date_convert() */

enum date_convert_format date_convert_populate_return_date(
		char *return_date,
		enum date_convert_format source_format,
		enum date_convert_format destination_format,
		char *date_string )
{
	if ( source_format == date_convert_unknown )
	{
		source_format = date_convert_date_get_format( date_string );
	}

	if ( source_format == american )
	{
		if ( !date_convert_source_american(
			return_date,
			destination_format,
			date_string ) )
		{
			return 0;
		}
	}
	else
	if ( source_format == international )
	{
		date_convert_source_international(
			return_date,
			destination_format,
			date_string );
	}
	else
	if ( source_format == military )
	{
		date_convert_source_military(
			return_date,
			destination_format,
			date_string );
	}
	else
	{
		*return_date = '\0';
		source_format = date_convert_unknown;
	}

	return source_format;

} /* date_convert_populate_return_date() */

enum date_convert_format date_convert_date_get_format( char *date_string )
{
	char delimiter;
	char year_string[ 16 ];

	if ( !timlib_exists_numeric( date_string ) )
		return date_convert_unknown;

	if ( strlen( date_string ) >= 19 )
		return date_convert_unknown;

	if ( timlib_exists_alpha( date_string ) )
	{
		if ( date_convert_is_valid_military( date_string ) )
			return military;
		else
			return date_convert_unknown;
	}

	delimiter = date_convert_get_delimiter( date_string );
	if ( !delimiter ) return date_convert_unknown;

	if ( character_count( delimiter, date_string ) != 2 )
		return date_convert_unknown;

	if ( delimiter == '/' )
	{
		piece( year_string, delimiter, date_string, 2 );

		if ( strlen( year_string ) == 2 )
		{
			strcpy( year_string,
				julian_make_y2k_year( year_string ) );
		}

		if ( strlen( year_string ) == 4
		&& ( atoi( year_string ) > 1700 ) )
		{
			return american;
		}
		else
		{
			return date_convert_unknown;
		}
	}
	else
	if ( delimiter == '-' )
	{
		piece( year_string, delimiter, date_string, 0 );
		if ( ( strcmp( year_string, "0000" ) == 0 )
		||   ( atoi( year_string ) > 1700 ) )
		{
			return international;
		}
		else
		{
			return date_convert_unknown;
		}
	}
	else
	{
		return date_convert_unknown;
	}

} /* date_convert_date_get_format() */

void date_convert_free(	DATE_CONVERT *date_convert )
{
	free( date_convert );
}

boolean date_convert_source_unknown(
		char *return_date,
		enum date_convert_format destination_format,
		char *date_string )
{
	enum date_convert_format source_format;

	source_format = date_convert_date_get_format( date_string );

	if ( source_format == american )
	{
		if ( !date_convert_source_american(
			return_date,
			destination_format,
			date_string ) )
		{
			return 0;
		}
	}
	else
	if ( source_format == international )
	{
		return date_convert_source_international(
			return_date,
			destination_format,
			date_string );
	}
	else
	if ( source_format == military )
	{
		date_convert_source_military(
			return_date,
			destination_format,
			date_string );
	}
	else
	{
		*return_date = '\0';
		return 0;
	}

	return 1;

} /* date_convert_source_unknown() */

boolean date_convert_source_american(
				char *return_date,
				enum date_convert_format destination_format,
				char *date_string )
{
	if ( !date_string ) return 0;

	if ( destination_format == military )
	{
		char international[ 16 ];

		strcpy( international,
			date_convert_american2international(
				date_string ) );

		strcpy(	return_date,
			timlib_yyyymmdd_to_oracle_format(
				international ) );
	}
	else
	if ( destination_format == international )
	{
		strcpy( return_date,
			date_convert_american2international(
				date_string ) );

		return date_convert_is_valid_international( return_date );
	}
	else
	{
		char day[ 16 ];
		char month[ 16 ];
		char year[ 16 ];

		piece( month, '/', date_string, 0 );
		piece( day, '/', date_string, 1 );
		piece( year, '/', date_string, 2 );

		if ( strlen( year ) == 2 )
			strcpy( year, julian_make_y2k_year( year ) );

		sprintf(	return_date,
				"%s/%s/%s",
				month,
				day,
				year );
	}

	return 1;

} /* date_convert_source_american() */

void date_convert_source_military(
			char *return_date,
			enum date_convert_format destination_format,
			char *date_string )
{
	if ( !date_string ) return;

	if ( destination_format == american )
	{
		char international[ 16 ];

		strcpy( international,
			timlib_oracle_date2mysql_date_string(
				date_string ) );
		strcpy(	return_date,
			date_convert_international2american(
				international ) );
	}
	else
	if ( destination_format == international )
	{
		strcpy( return_date,
			timlib_oracle_date2mysql_date_string(
				date_string ) );
	}
	else
	{
		strcpy( return_date, date_string );
	}

} /* date_convert_source_military() */

boolean date_convert_source_international(
			char *return_date,
			enum date_convert_format destination_format,
			char *date_string )
{
	if ( !date_string ) return 0;

	if ( destination_format == american )
	{
		strcpy(	return_date,
			date_convert_international2american(
				date_string ) );
	}
	else
	if ( destination_format == military )
	{
		strcpy( return_date,
			timlib_yyyymmdd_to_oracle_format(
				date_string ) );
	}
	else
	{
		char delimiter_string[ 2 ];

		*delimiter_string = date_convert_get_delimiter( date_string );
		*(delimiter_string + 1) = '\0';

		strcpy( return_date, date_string );

		if ( !timlib_exists_character( date_string, '-' ) )
		{
			search_replace_string(
				return_date, delimiter_string, "-" );
		}
	}

	return date_convert_international_correct_format( return_date );

} /* date_convert_source_international() */

char *date_convert_international2american( char *international )
{
	char day[ 16 ];
	char month[ 16 ];
	char year[ 16 ];
	static char american[ 16 ];

	if ( character_count( '-', international ) != 2 )
		return international;

	piece( year, '-', international, 0 );

	if ( strlen( year ) == 2 )
		strcpy( year, julian_make_y2k_year( year ) );

	piece( month, '-', international, 1 );
	piece( day, '-', international, 2 );
	sprintf(american,
		"%.2d/%.2d/%s",
		atoi( month ),
		atoi( day ),
		year );
	return american;
} /* date_convert_international2american() */

char *date_convert_american2international( char *american )
{
	char day[ 16 ];
	char month[ 16 ];
	char year[ 16 ];
	static char international[ 256 ];
	char delimiter;

	delimiter = date_convert_get_delimiter( american );

	if ( !delimiter
	||   character_count( delimiter, american ) != 2 )
	{
		sprintf( international, "invalid: %s", american );
		return international;
	}

	piece( month, delimiter, american, 0 );
	piece( day, delimiter, american, 1 );
	piece( year, delimiter, american, 2 );

	if ( strlen( year ) == 2 )
		strcpy( year, julian_make_y2k_year( year ) );

	sprintf(international,
		"%s-%.2d-%.2d",
		year,
		atoi( month ),
		atoi( day ) );
	return international;
} /* date_convert_american2international() */

enum date_convert_format date_convert_get_date_convert_format(
					char *format_string )
{
	if ( strcmp( format_string, "american" ) == 0 )
		return american;
	else
	if ( strcmp( format_string, "international" ) == 0 )
		return international;
	else
	if ( strcmp( format_string, "military" ) == 0 )
		return military;
	else
		return date_convert_unknown;
} /* date_convert_get_date_convert_format() */

char *date_convert_get_date_format_string(
				enum date_convert_format date_convert_format )
{
	if ( date_convert_format == american )
		return "american";
	else
	if ( date_convert_format == international )
		return "international";
	else
	if ( date_convert_format == military )
		return "military";
	else
		return "unknown";
} /* date_convert_get_date_format_string() */

char date_convert_get_delimiter( char *date_string )
{
	if ( timlib_exists_character( date_string, '/' ) )
		return '/';
	else
	if ( timlib_exists_character( date_string, '.' ) )
		return '.';
	else
	if ( timlib_exists_character( date_string, '-' ) )
		return '-';
	else
		return 0;

} /* date_convert_get_delimiter() */

boolean date_convert_international_correct_format( char *date_string )
{
	char day[ 16 ];
	char month[ 16 ];
	char year[ 16 ];

	if ( !date_convert_is_valid_international( date_string ) )
	{
		return 0;
	}

	if ( strlen( date_string ) == 10 ) return 1;
	if ( strlen( date_string ) == 0 ) return 0;

	piece( year, '-', date_string, 0 );
	piece( month, '-', date_string, 1 );
	piece( day, '-', date_string, 2 );

	if ( strlen( year ) == 2 )
		strcpy( year, julian_make_y2k_year( year ) );

	sprintf(date_string,
		"%s-%.2d-%.2d",
		year,
		atoi( month ),
		atoi( day ) );

	return 1;

} /* date_convert_international_correct_format() */

boolean date_convert_is_valid_military(
				char *date_string )
{
	char piece_string[ 128 ];
	int str_len = strlen( date_string );
	int day;
	int month;

	if ( character_count( '-', date_string ) != 2 ) return 0;

	if ( str_len > 11 || str_len < 9 ) return 0;

	piece( piece_string, '-', date_string, 0 );
	day = atoi( piece_string );
	if ( day < 1 || day > 31 ) return 0;

	piece( piece_string, '-', date_string, 1 );
	month = timlib_ora_month2integer( piece_string );
	if ( month < 1 ) return 0;

	return 1;

} /* date_convert_is_valid_military() */

boolean date_convert_is_valid_american(
				char *date_string )
{
	char day[ 128 ];
	char month[ 128 ];
	char year[ 128 ];
	int day_integer;
	int month_integer;
	int year_integer;

	piece( month, '/', date_string, 0 );
	if ( !piece( day, '/', date_string, 1 ) ) return 0;
	if ( !piece( year, '/', date_string, 2 ) ) return 0;
	month_integer = atoi( month );
	day_integer = atoi( day );
	year_integer = atoi( year );

	return date_convert_is_valid_integers(
			year_integer,
			month_integer,
			day_integer );

} /* date_convert_is_valid_american() */

boolean date_convert_is_valid_international(
				char *date_string )
{
	char day[ 128 ];
	char month[ 128 ];
	char year[ 128 ];
	int day_integer;
	int month_integer;
	int year_integer;

	if ( strlen( date_string ) != 10 )
	{
		return 0;
	}

	piece( year, '-', date_string, 0 );
	if ( !piece( month, '-', date_string, 1 ) ) return 0;
	if ( !piece( day, '-', date_string, 2 ) ) return 0;

	year_integer = atoi( year );
	month_integer = atoi( month );
	day_integer = atoi( day );

	return date_convert_is_valid_integers(
			year_integer,
			month_integer,
			day_integer );

} /* date_convert_is_valid_international() */

boolean date_convert_is_valid_integers(
				int year_integer,
				int month_integer,
				int day_integer )
{
	if ( day_integer < 1 || day_integer > 31 ) return 0;

	if ( month_integer < 1 || month_integer > 12 ) return 0;

	if ( month_integer == 2 && day_integer > 29 ) return 0;

	if ( ( month_integer == 4
	||     month_integer == 6
	||     month_integer == 9
	||     month_integer == 11 ) && day_integer > 30 ) return 0;

	if ( month_integer == 2
	&&   ( year_integer % 4 )
	&&   day_integer == 29 )
	{
		return 0;
	}

	if ( year_integer < 1492 || year_integer > 3000 ) return 0;

	return 1;

} /* date_convert_is_valid_integers() */

char *date_convert_american_sans_slashes(
				char *compressed_date_string )
{
	static char return_date[ 16 ];

	if ( strlen( compressed_date_string ) == 5 )
	{
		*(return_date + 0) = '0';
	}
	else
	if ( strlen( compressed_date_string ) == 6 )
	{
		*(return_date + 0) = *compressed_date_string++;
	}
	else
	{
		return (char *)0;
	}

	*(return_date + 1) = *compressed_date_string++;
	*(return_date + 2) = '/';
	*(return_date + 3) = *compressed_date_string++;
	*(return_date + 4) = *compressed_date_string++;
	*(return_date + 5) = '/';

	if ( *compressed_date_string <= '3' )
	{
		*(return_date + 6) = '2';
		*(return_date + 7) = '0';
	}
	else
	{
		*(return_date + 6) = '1';
		*(return_date + 7) = '9';
	}
	*(return_date + 8) = *compressed_date_string++;
	*(return_date + 9) = *compressed_date_string++;

	return return_date;

} /* date_convert_american_sans_slashes() */

