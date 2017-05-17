/* $APPASERVER_HOME/library/date.c			*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "piece.h"
#include "column.h"
#include "timlib.h"
#include "julian.h"
#include "date.h"

DATE *date_new_date( void )
{
	DATE *d = (DATE *)calloc( 1, sizeof( DATE ) );
	if ( !d )
	{
		fprintf(stderr,
			"ERROR: memory allocation failed in %s/%s()/%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	d->tm = ( struct tm *)calloc( 1, sizeof( struct tm ) );
	if ( !d->tm )
	{
		fprintf(stderr,
			"ERROR: memory allocation failed in %s/%s()/%d\n",
			__FILE__,
			__FUNCTION__,
			__LINE__ );
		exit( 1 );
	}

	return d;
} /* date_new_date() */

DATE *date_current_new( time_t current )
{
	DATE *date;

	date = date_new_date();

	date_set_tm_structures( date, current );
	date->current = current;

	return date;

} /* date_current_new() */

time_t date_yyyy_mm_dd_time_hhmm_to_time_t(
			char *date_string,
			char *time_string )
{
	DATE *date;
	time_t current;

	date = date_yyyy_mm_dd_hhmm_new( date_string, time_string );
	current = date->current;
	date_free( date );
	return current;
} /* date_yyyy_mm_dd_time_hhmm_to_time_t() */

DATE *date_time_new( int year, int month, int day, int hour, int minute )
{
	return date_new_date_time(
			year,
			month,
			day,
			hour,
			minute,
			0 /* seconds */ );
}

DATE *date_new_date_time(
			int year,
			int month,
			int day,
			int hour,
			int minutes,
			int seconds )
{
	DATE *d;
	struct tm tm;

	d = date_new_date();
	d->tm->tm_year = year - 1900;
	d->tm->tm_mon = month - 1;
	d->tm->tm_mday = day;
	d->tm->tm_hour = hour;
	d->tm->tm_min = minutes;
	d->tm->tm_sec = seconds;

	d->current = date_tm_to_current( d->tm );

	memcpy( &tm, localtime( &d->current ), sizeof( struct tm ) );
	date_set_tm_structures( d, d->current );

/*
	if ( d->tm->tm_isdst ) date_increment_hours( d, -1.0 );
*/

	return d;
} /* date_new_date_time() */

DATE *date_new( int year, int month, int day )
{
	return date_new_date_time(
			year,
			month,
			day,
			0 /*  hour */,
			0 /*  minutes */,
			0 /* seconds */ );
} /* date_new() */

time_t date_tm_to_current( struct tm *tm )
{
	tm->tm_isdst = 0;

	if ( tm->tm_year < 70 )
	{
		return date_tm_to_current_pre_1970( tm );
	}
	else
	{
		return date_mktime( tm );
	}
} /* date_tm_to_current() */

time_t date_tm_to_current_pre_1970( struct tm *tm )
{
	struct tm future_tm = {0};
	time_t future_time_t;
	struct tm current_tm = {0};

	future_tm.tm_year = 2026 - 1900;
	future_tm.tm_mon = 0;
	future_tm.tm_mday = 0;
	future_tm.tm_hour = 0;
	future_tm.tm_min = 0;
	future_tm.tm_sec = 0;

	future_time_t = date_mktime( &future_tm );

	current_tm.tm_year = tm->tm_year + 56;
	current_tm.tm_mon = tm->tm_mon;
	current_tm.tm_mday = tm->tm_mday;
	current_tm.tm_hour = tm->tm_hour;
	current_tm.tm_min = tm->tm_min;

	return date_mktime( &current_tm ) - future_time_t;

} /* date_tm_to_current_pre_1970() */

void date_set_date_time_integers(
				DATE *date,
				int year,
				int month,
				int day,
				int hours,
				int minutes,
				int seconds )
{
	date->tm->tm_year = year - 1900;
	date->tm->tm_mon = month - 1;
	date->tm->tm_mday = day;
	date->tm->tm_hour = hours;
	date->tm->tm_min = minutes;
	date->tm->tm_sec = seconds;

	date->current = date_tm_to_current( date->tm );
	date_set_tm_structures( date, date->current );

} /* date_set_date_time_integers() */

void date_set_time_integers(	DATE *date,
				int hour,
				int minute,
				int seconds )
{
	date->tm->tm_hour = hour;
	date->tm->tm_min = minute;
	date->tm->tm_sec = seconds;

	date->current = date_tm_to_current( date->tm );
	date_set_tm_structures( date, date->current );

} /* date_set_time_integers() */

void date_set_date_integers(	DATE *date,
				int year,
				int month,
				int day )
{
	date->tm->tm_year = year - 1900;
	date->tm->tm_mon = month - 1;
	date->tm->tm_mday = day;

	date->current = date_tm_to_current( date->tm );
	date_set_tm_structures( date, date->current );

} /* date_set_date_integers() */

#ifdef NOT_DEFINED
int date_set_yyyy_mm_dd_hhmm(	DATE *date,
				char *yyyy_mm_dd_hhmm,
				char delimiter )
{
	char yyyy_mm_dd[ 16 ];
	char hhmm[ 16 ];

	piece( yyyy_mm_dd, delimiter, yyyy_mm_dd_hhmm, 0 );

	if ( !date_set_yyyy_mm_dd( date, yyyy_mm_dd ) ) return 0;

	piece( hhmm, delimiter, yyyy_mm_dd_hhmm, 1 );

	if ( *hhmm && strcasecmp( hhmm, "null" ) != 0 )
	{
		return date_set_time_hhmm( date, hhmm );
	}
	else
	{
		return 1;
	}

} /* date_set_yyyy_mm_dd_hhmm() */
#endif

int date_set_yyyy_mm_dd_hhmm_delimited(
				DATE *date,
				char *yyyy_mm_dd_hhmm,
				int date_piece,
				int time_piece,
				char delimiter )
{
	char yyyy_mm_dd[ 128 ];
	char hhmm[ 128 ];

	if ( !piece( yyyy_mm_dd, delimiter, yyyy_mm_dd_hhmm, date_piece ) )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: cannot piece(%d) in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 date_piece,
			 yyyy_mm_dd );
		return 0;
	}

	if ( time_piece != -1
	&&   !piece( hhmm, delimiter, yyyy_mm_dd_hhmm, time_piece ) )
	{
		fprintf( stderr,
"Warning in %s/%s()/%d: cannot piece(%d) in (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 time_piece,
			 yyyy_mm_dd );
		return 0;
	}
	else
	{
		*hhmm = '\0';
	}

	if ( !*hhmm || strcasecmp( hhmm, "null" ) == 0 )
		return date_set_yyyy_mm_dd( date, yyyy_mm_dd );

	return date_set_yyyy_mm_dd_hhmm(
		date,
		yyyy_mm_dd,
		hhmm );

} /* date_set_yyyy_mm_dd_hhmm_delimited() */

int date_set_yyyy_mm_dd_hhmm(	DATE *date,
				char *yyyy_mm_dd,
				char *hhmm )
{
	char year_string[ 128 ];
	char month_string[ 128 ];
	char day_string[ 128 ];
	char delimiter;
	int hours, minutes;
	char buffer[ 3 ];

	if ( count_character( '-', yyyy_mm_dd ) == 2 )
		delimiter = '-';
	else
	if ( count_character( '.', yyyy_mm_dd ) == 2 )
		delimiter = '.';
	else
		return 0;

	piece( year_string, delimiter, yyyy_mm_dd, 0 );
	piece( month_string, delimiter, yyyy_mm_dd, 1 );
	piece( day_string, delimiter, yyyy_mm_dd, 2 );

	if ( strlen( hhmm ) != 4 ) return 0;

	*(buffer + 2) = '\0';
	*buffer = *hhmm;
	*(buffer + 1) = *(hhmm + 1);
	hours = atoi( buffer );

	*buffer = *(hhmm + 2);
	*(buffer + 1) = *(hhmm + 3);
	minutes = atoi( buffer );

	date_set_date_time_integers(
				date,
				atoi( year_string ),
				atoi( month_string ),
				atoi( day_string ),
				hours,
				minutes,
				0 /* seconds */ );

	return 1;

} /* date_set_yyyy_mm_dd_hhmm() */

int date_set_yyyy_mm_dd( DATE *date, char *yyyy_mm_dd )
{
	char year_string[ 128 ];
	char month_string[ 128 ];
	char day_string[ 128 ];
	char delimiter;

	if ( count_character( '-', yyyy_mm_dd ) == 2 )
		delimiter = '-';
	else
	if ( count_character( '.', yyyy_mm_dd ) == 2 )
		delimiter = '.';
	else
		return 0;

	piece( year_string, delimiter, yyyy_mm_dd, 0 );
	piece( month_string, delimiter, yyyy_mm_dd, 1 );
	piece( day_string, delimiter, yyyy_mm_dd, 2 );

	date_set_date_integers(	date,
				atoi( year_string ),
				atoi( month_string ),
				atoi( day_string ) );
	return 1;
} /* date_set_yyyy_mm_dd() */

void date_set_time( DATE *date, int hour, int minutes )
{
	date_set_time_integers( date, hour, minutes, 0 );
}

void date_free( DATE *d )
{
	free( d->tm );
	free( d );
}

void date_increment( DATE *d )
{
	date_increment_day( d );
}

void date_decrement_minute( DATE *d )
{
	date_increment_minutes( d, -1 );
}

void date_decrement_hour( DATE *d )
{
	date_increment_hours( d, -1.0 );
}

void date_increment_hour( DATE *d )
{
	date_increment_hours( d, 1.0 );
}

void date_increment_minute( DATE *d )
{
	date_increment_minutes( d, 1 );
}

void date_decrement_day( DATE *d )
{
	return date_decrement_days( d, 1.0 );
}

void date_decrement_days( DATE *d, float days )
{
	float minus_days = -days;
	date_increment_days( d, minus_days );
}

void date_increment_months( DATE *d, int months )
{
	int year;
	int month;
	int day;

	year = date_get_year( d );
	month = date_get_month( d );
	day = date_get_day( d );
	month += months;
	date_set_date_integers(	d, year, month, day );

} /* date_increment_months() */

void date_decrement_years( DATE *d, int years )
{
	int year;
	int month;
	int day;

	year = date_get_year( d );
	month = date_get_month( d );
	day = date_get_day( d );
	year -= years;
	date_set_date_integers(	d, year, month, day );

} /* date_decrement_years() */

void date_increment_day( DATE *d )
{
	d->current += SECONDS_IN_DAY;
	date_set_tm_structures( d, d->current );
}

void increment_week( DATE *d )
{
	d->current += SECONDS_IN_WEEK;
	date_set_tm_structures( d, d->current );
}

int get_month( DATE *d )
{
	return date_get_month( d );
}

int date_get_month( DATE *d )
{
	return d->tm->tm_mon + 1;
}

int get_day_of_month( DATE *d )
{
	return date_get_day_of_month( d );
}

int date_get_day_of_month( DATE *d )
{
	return d->tm->tm_mday;
}

int date_get_day_of_week( DATE *d )
{
	return d->tm->tm_wday;
}

char *date_get_day_of_week_string( DATE *d )
{
	if ( d->tm->tm_wday == 0 )
		return "Sunday";
	else
	if ( d->tm->tm_wday == 1 )
		return "Monday";
	else
	if ( d->tm->tm_wday == 2 )
		return "Tuesday";
	else
	if ( d->tm->tm_wday == 3 )
		return "Wednesday";
	else
	if ( d->tm->tm_wday == 4 )
		return "Thursday";
	else
	if ( d->tm->tm_wday == 5 )
		return "Friday";
	else
	if ( d->tm->tm_wday == 6 )
		return "Saturday";
	else
		return "Unknown";
} /* date_get_day_of_week_string() */

int get_year( DATE *d )
{
	return date_get_year( d );
}

int date_get_year( DATE *d )
{
	return d->tm->tm_year + 1900;
}

int get_hour( DATE *d )
{
	return date_get_hour( d );
}

int date_get_hour( DATE *d )
{
	return d->tm->tm_hour;
}

int get_minutes( DATE *d )
{
	return date_get_minutes( d );
}

int get_seconds( DATE *d )
{
	return date_get_seconds( d );
}

int date_get_minutes( DATE *d )
{
	return d->tm->tm_min;
}

int date_get_seconds( DATE *d )
{
	return d->tm->tm_sec;
}

/* ------------------------------------------------------------- */
/* Sample input: from_date = "2017-03-01" to_date = "2017-04-16" */
/* ------------------------------------------------------------- */
int date_days_between(	char *from_date_string,
			char *to_date_string )
{
	DATE *from_date;
	DATE *to_date;
	time_t difference;

	if ( ! ( from_date = date_yyyy_mm_dd_new( from_date_string ) ) )
	{
		return 0;
	}

	if ( ! ( to_date = date_yyyy_mm_dd_new( to_date_string ) ) )
	{
		return 0;
	}

	difference = to_date->current - from_date->current;

	date_free( from_date );
	date_free( to_date );

	return (int) (difference / SECONDS_IN_DAY);

} /* date_days_between() */

/* --------------------------------------------------------- */
/* Sample input: from_date = "10/06/60" to_date = "08/04/11" */
/* --------------------------------------------------------- */
int date_years_between( char *from_date, char *to_date )
{
	int from_year, from_month, from_day;
	int to_year, to_month, to_day;
	int diff_year, diff_month, diff_day;

	if ( !date_parse(	&from_year,
				&from_month,
				&from_day,
				from_date ) )
	{
		return -1;
	}

	if ( !date_parse(	&to_year,
				&to_month,
				&to_day,
				to_date ) )
	{
		return -1;
	}

	diff_year = to_year - from_year;
	diff_month = to_month - from_month;
	diff_day = to_day - from_day;

	if ( diff_day < 0 ) diff_month--;
	if ( diff_month < 0 ) diff_year--;

	if ( diff_year < 0 ) diff_year = 0;

	return diff_year;

} /* date_years_between() */

void date_time_parse(		int *hours,
				int *minutes,
				char *hhmm )
{
	char buffer[ 3 ];
	char *ptr;

	*hours = 0;
	*minutes = 0;

	if ( timlib_strlen( hhmm ) != 4 ) return;

	ptr = buffer;
	*ptr++ = *hhmm++;
	*ptr++ = *hhmm++;
	*ptr = '\0';

	*hours = atoi( buffer );

	ptr = buffer;
	*ptr++ = *hhmm++;
	*ptr++ = *hhmm++;
	*ptr = '\0';

	*minutes = atoi( buffer );

} /* date_time_parse() */

boolean date_parse(	int *year,
			int *month,
			int *day,
			char *date_string )
{
	char piece_buffer[ 128 ];

	*month = 0;
	*day = 0;
	*year = 0;

	if ( number_occurrences_char( '/', date_string ) == 2 )
	{
		*month = atoi( piece( piece_buffer, '/', date_string, 0 ) );
		*day = atoi( piece( piece_buffer, '/', date_string, 1 ) );
		piece( piece_buffer, '/', date_string, 2 );

		timlib_strcpy(	piece_buffer,
				julian_make_y2k_year( piece_buffer ),
				128 );

		*year = atoi( piece_buffer );
		return 1;
	}
	else
	if ( number_occurrences_char( '-', date_string ) == 2 )
	{
		*month = atoi( piece( piece_buffer, '-', date_string, 1 ) );
		*day = atoi( piece( piece_buffer, '-', date_string, 2 ) );
		piece( piece_buffer, '-', date_string, 0 );
	
		timlib_strcpy(	piece_buffer,
				julian_make_y2k_year( piece_buffer ),
				128 );

		*year = atoi( piece_buffer );
		return 1;
	}
	return 0;
} /* date_parse() */

int date_minutes_between(	char *from_date_string,
				char *from_time_string,
				char *to_date_string,
	       			char *to_time_string,
				boolean add_one )
{
	DATE *from_date;
	DATE *to_date;
	time_t difference;

	if ( strcmp( from_time_string, "null" ) == 0
	||   strcmp( to_time_string, "null" ) == 0 )
	{
		return 24 * 60;
	}

	from_date = date_yyyy_mm_dd_new( from_date_string );
	date_set_time_hhmm( from_date, from_time_string );

	to_date = date_yyyy_mm_dd_new( to_date_string );
	date_set_time_hhmm( to_date, to_time_string );

	difference = to_date->current - from_date->current;

	date_free( from_date );
	date_free( to_date );

	if ( add_one )
		return ( difference / SECONDS_IN_MINUTE ) + 1;
	else
		return difference / SECONDS_IN_MINUTE;

} /* date_minutes_between() */

int age( char *birth_date )
{
	char today[ 64 ];
	char with_slashes[ 64 ];
	FILE *p;

	p = popen( "date '+%m/%d/%y'", "r" );
	if ( !p )
	{
		fprintf(stderr,
			"ERROR: age() cannot open pipe to get today's date\n" );
		exit( 1 );
	}

	get_line( today, p );
	pclose( p );

	add_slashes_maybe( with_slashes, birth_date );

	return date_years_between( with_slashes, today );

} /* age() */

void add_slashes_maybe( char *d, char *s )
{
	char buf1[ 32 ], buf2[ 32 ], buf3[ 32 ];

	if ( strlen( s ) != 6 )
	{
		strcpy( d, s );
		return;
	}

	sprintf( d, 
		 "%s/%s/%s",
		 midstr( buf1, s, 0, 2 ),
		 midstr( buf2, s, 2, 2 ),
		 midstr( buf3, s, 4, 2 ) );

} /* add_slashes_maybe() */

DATE *date_yyyy_mm_dd_hms_new( char *date_time_string )
{
	char year_string[ 16 ];
	char month_string[ 16 ];
	char day_string[ 16 ];
	char hour_string[ 16 ];
	char minute_string[ 16 ];
	char second_string[ 16 ];
	char date_half[ 32 ];
	char time_half[ 32 ];
	DATE *date;

	if ( count_character( '-', date_time_string ) != 2 )
		return (DATE *)0;

	if ( count_character( ' ', date_time_string ) != 1 )
		return (DATE *)0;

	if ( count_character( ':', date_time_string ) != 2 )
		return (DATE *)0;

	column( date_half, 0, date_time_string );
	column( time_half, 1, date_time_string );

	piece( year_string, '-', date_half, 0 );
	piece( month_string, '-', date_half, 1 );
	piece( day_string, '-', date_half, 2 );

	piece( hour_string, ':', time_half, 0 );
	piece( minute_string, ':', time_half, 1 );
	piece( second_string, ':', time_half, 2 );

	date = date_new_date_time(
			atoi( year_string ),
			atoi( month_string ),
			atoi( day_string ),
			atoi( hour_string ),
			atoi( minute_string ),
			atoi( second_string ) );


	return date;

} /* date_yyyy_mm_dd_hms_new() */

DATE *date_yyyy_mm_dd_new( char *date_string )
{
	char year_string[ 128 ];
	char month_string[ 128 ];
	char day_string[ 128 ];
	DATE *date;
	char delimiter;

	if ( count_character( '-', date_string ) == 2 )
		delimiter = '-';
	else
	if ( count_character( '.', date_string ) == 2 )
		delimiter = '.';
	else
		return (DATE *)0;

	piece( year_string, delimiter, date_string, 0 );
	piece( month_string, delimiter, date_string, 1 );
	piece( day_string, delimiter, date_string, 2 );

	date = date_new( 	atoi( year_string ),
				atoi( month_string ),
				atoi( day_string ) );
	return date;

} /* date_yyyy_mm_dd_new() */

void date_increment_days( DATE *d, float days )
{
	d->current += (long)((float)SECONDS_IN_DAY * days);
	date_set_tm_structures( d, d->current );
}

char *date_yyyy_mm_dd( DATE *date )
{
	static char destination[ 16 ];
	return date_get_yyyy_mm_dd( destination, date );
}

char *date_get_yyyy_mm_dd( char *destination, DATE *date )
{
	sprintf( 	destination, 
			"%d-%02d-%02d",
			date_get_year( date ),
			date_get_month( date ),
			date_get_day_of_month( date ) );
	return destination;
}

char *date_get_hhmm( char *destination, DATE *date )
{
	sprintf( 	destination, 
			"%02d%02d",
			date_get_hour( date ),
			date_get_minutes( date ) );
	return destination;
}

char *date_get_colon_hhmmss( char *destination, DATE *date )
{
	sprintf( 	destination, 
			"%02d:%02d:%02d",
			date_get_hour( date ),
			date_get_minutes( date ),
			date_get_seconds( date ) );
	return destination;
}

char *date_get_hhmmss( char *destination, DATE *date )
{
	sprintf( 	destination, 
			"%02d%02d:%02d",
			date_get_hour( date ),
			date_get_minutes( date ),
			date_get_seconds( date ) );
	return destination;
}

char *date_display( DATE *date )
{
	return date_display_yyyy_mm_dd( date );
}

char *date_static_display_yyyy_mm_dd( DATE *date )
{
	static char buffer[ 16 ];

	if ( !date ) return "";

	sprintf( 	buffer, 
			"%d-%02d-%02d",
			date_get_year( date ),
			date_get_month( date ),
			date_get_day_of_month( date ) );

	return buffer;
} /* date_static_display_yyyy_mm_dd() */

char *date_static_display( DATE *date )
{
	return date_static_display_yyyy_mm_dd( date );
}

char *date_display_yyyy_mm_dd_colon_hms( DATE *date )
{
	char buffer[ 128 ];
	
	sprintf( buffer,
		 "%s ",
		 date_static_display_yyyy_mm_dd( date ) );

	date_get_colon_hhmmss(
		 buffer + strlen( buffer ), date );

	return strdup( buffer );

} /* date_display_yyyy_mm_dd_colon_hms() */

char *date_display_yyyy_mm_dd( DATE *date )
{
	char buffer[ 128 ];

	if ( !date ) return "";

	sprintf( 	buffer, 
			"%d-%02d-%02d",
			date_get_year( date ),
			date_get_month( date ),
			date_get_day_of_month( date ) );

	return strdup( buffer );
} /* date_display_yyyy_mm_dd() */

char *date_display_hhmm( DATE *date )
{
	char buffer[ 128 ];

	if ( !date ) return "";

	date_get_hhmm( buffer, date );
	return strdup( buffer );
} /* date_display_hhmm() */

char *date_display_hhmmss( DATE *date )
{
	char buffer[ 128 ];

	if ( !date ) return "";

	date_get_hhmmss( buffer, date );
	return strdup( buffer );
} /* date_display_hhmmss() */

char *date_get_yyyy_mm_dd_string( DATE *date )
{
	return date_display_yyyy_mm_dd( date );
}


char *date_get_day_of_week_yyyy_mm_dd( int wday_of_week )
{
	char *date_string;
	DATE *date = date_get_today_new();

	while( date->tm->tm_wday != wday_of_week )
		date_increment_days( date, -1.0 );
	date_string = date_get_yyyy_mm_dd_string( date );
	date_free( date );
	return date_string;
}

char *date_get_yesterday_yyyy_mm_dd()
{
	return date_get_yesterday_yyyy_mm_dd_string();
}

char *date_get_yesterday_yyyy_mm_dd_string()
{
	DATE *date;

	date = date_get_today_new();
	date_increment_days( date, -1.0 );
	return date_get_yyyy_mm_dd_string( date );
} /* date_get_yesterday_yyyy_mm_dd_string() */


DATE *date_now_new( void )
{
	return date_get_today_new();
}

DATE *date_new_now( void )
{
	return date_get_today_new();
}

DATE *date_today_new( void )
{
	return date_get_today_new();
}

DATE *date_get_today_new( void )
{
	time_t now;
	struct tm *tm;
	DATE *return_date;

	now = time( (time_t *)0 );
	tm = localtime( &now );
	return_date =
		date_new_date_time(
			tm->tm_year + 1900,
			tm->tm_mon + 1,
			tm->tm_mday,
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec );
	return return_date;
} /* date_get_today_new() */

char *date_get_now_hhmm()
{
	return date_get_now_date_hhmm();
}

char *date_get_now_yyyy_mm_dd()
{
	return date_get_now_date_yyyy_mm_dd();
}

char *date_get_today_yyyy_mm_dd()
{
	return date_get_now_date_yyyy_mm_dd();
}

char *date_get_current_hhmm()
{
	return date_get_now_date_hhmm();
}

char *date_get_current_yyyy_mm_dd()
{
	return date_get_now_date_yyyy_mm_dd();
}

char *date_get_now_date_hhmm()
{
	char *date_time_string;
	static char hhmm[ 8 ];

/*
	DATE *date = date_get_today_new();
	return date_display_hhmm( date );
*/
	date_time_string = date_get_unix_now_string();

	if ( !piece( hhmm, ':', date_time_string, 1 ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get time using (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 date_time_string );
		exit( 1 );
	}

	return hhmm;
} /* date_get_now_date_hhmm() */

char *date_get_now_date_oracle_format()
{
	return timlib_yyyymmdd_to_oracle_format(
		date_get_now_date_yyyy_mm_dd() );
}

char *date_get_now_date_yyyy_mm_dd()
{
/*
	DATE *date = date_get_today_new();
	return date_display_yyyy_mm_dd( date );
*/
	char *date_time_string;
	static char yyyy_mm_dd[ 16 ];

	date_time_string = date_get_unix_now_string();

	piece( yyyy_mm_dd, ':', date_time_string, 0 );
	return yyyy_mm_dd;

} /* date_get_now_date_yyyy_mm_dd() */

char *date_get_now_time_hhmm( void )
{
	char buffer[ 128 ];
	time_t now;
	struct tm *tm;

	now = time( (time_t *)0 );
	tm = localtime( &now );
	sprintf( buffer, "%02d%02d", tm->tm_hour, tm->tm_min );
	return strdup( buffer );
} /* date_get_now_time_hhmm() */

char *date_get_now_hhmm_colon_ss( void )
{
	return date_get_now_time_hhmm_colon_ss();
}

char *date_get_now_time_second( void )
{
	return date_get_now_hh_colon_mm_colon_ss();
}

char *date_get_yyyy_mm_dd_hh_mm_ss(
				DATE *date_time )
{
	char buffer[ 32 ];

	sprintf( 	buffer, 
			"%d-%02d-%02d %02d:%02d:%02d",
			date_get_year( date_time ),
			date_get_month( date_time ),
			date_get_day_of_month( date_time ),
			date_get_hour( date_time ),
			date_get_minutes( date_time ),
			date_get_seconds( date_time ) );

	return strdup( buffer );

} /* date_get_yyyy_mm_dd_hh_mm_ss() */

char *date_get_now_hh_colon_mm_colon_ss( void )
{
	char buffer[ 128 ];
	time_t now;
	struct tm *tm;

	now = time( (time_t *)0 );
	tm = localtime( &now );
	sprintf(	buffer,
			"%02d:%02d:%02d",
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec );

	return strdup( buffer );

} /* date_get_now_hh_colon_mm_colon_ss() */

char *date_get_now_time_hhmm_colon_ss( void )
{
	char buffer[ 128 ];
	time_t now;
	struct tm *tm;

	now = time( (time_t *)0 );
	tm = localtime( &now );
	sprintf( buffer, "%02d%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec );

	return strdup( buffer );

} /* date_get_now_time_hhmm_colon_ss() */

void date_set_tm_structures( DATE *d, time_t current )
{
	if ( current < 0 )
	{
		date_set_tm_structures_pre_1970(
				d,
				current );
	}
	else
	{
		memcpy( d->tm, localtime( &current ), sizeof( struct tm ) );
	}

} /* date_set_tm_structures() */

void date_set_tm_structures_pre_1970(	DATE *d,
					time_t current )
{
	struct tm future_tm = {0};
	time_t future_time_t;
	time_t new_current;

	future_tm.tm_year = 2026 - 1900;
	future_tm.tm_mon = 0;
	future_tm.tm_mday = 0;
	future_tm.tm_hour = 0;
	future_tm.tm_min = 0;
	future_tm.tm_sec = 0;

	future_time_t = date_mktime( &future_tm );

	new_current = future_time_t + current;

	memcpy( d->tm, localtime( &new_current ), sizeof( struct tm ) );
	d->tm->tm_year -= 56;

} /* date_set_tm_structures_pre_1970() */

time_t date_mktime( struct tm *tm )
{
	time_t return_value;
	struct tm local_tm;

	memcpy( &local_tm, tm, sizeof (struct tm ) );
	return_value = mktime( &local_tm );

	return return_value;

} /* date_mktime() */

char *date_hhmm( DATE *d )
{
	static char destination[ 16 ];
	char *tmp;

	tmp = date_get_hhmm_string( d );
	strcpy( destination, tmp );
	free( tmp );
	return destination;
}

char *date_get_hhmm_string( DATE *d )
{
	char buffer[ 128 ];
	sprintf( buffer, "%02d%02d", d->tm->tm_hour, d->tm->tm_min );
	return strdup( buffer );
} /* date_get_hhmm_string() */

char *date_new_get_yyyy_mm_dd( time_t current )
{
	DATE *d = date_new_date();

	date_set_tm_structures( d, current );
	d->current = date_tm_to_current( d->tm );

	return date_get_yyyy_mm_dd_string( d );
}

DATE *date_new_yyyy_mm_dd_hhmm(	char *date_string,
				char *time_string )
{
	return date_yyyy_mm_dd_hhmm_new( date_string, time_string );
}

DATE *date_new_yyyy_mm_dd( char *date_string )
{
	return date_yyyy_mm_dd_new( date_string );
}

DATE *date_new_yyyy_mm_dd_date( char *date_string )
{
	return date_yyyy_mm_dd_new( date_string );
}

boolean date_is_day_of_week(	DATE *d,
				int day_of_week )
{
	int wday;
	struct tm *tm = localtime( &d->current );
	
	wday = tm->tm_wday;
	return( wday == day_of_week );

} /* date_is_day_of_week() */

boolean date_is_sunday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_SUNDAY );
}

boolean date_is_monday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_MONDAY );
}

boolean date_is_tuesday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_TUESDAY );
}

boolean date_is_wednesday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_WEDNESDAY );
}

boolean date_is_thursday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_THURSDAY );
}

boolean date_is_friday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_FRIDAY );
}

boolean date_is_saturday( DATE *d )
{
	return date_is_day_of_week( d, WDAY_SATURDAY );
}

DATE *date_forward_to_first_month( DATE *d )
{
	DATE *return_date;

	return_date = date_back_to_first_month( d );
	date_increment_months( return_date, 1 );

	return return_date;

} /* date_forward_to_first_month() */

DATE *date_back_to_first_month( DATE *d )
{
	int mday;
	time_t current;
	struct tm *tm = localtime( &d->current );
	
	mday = tm->tm_mday;
	current = d->current;

	while( mday > 1 )
	{
		mday--;
		current -= SECONDS_IN_DAY;
	}
	return date_current_new( current );

} /* date_back_to_first_month() */

char *date_this_saturday_yyyy_mm_dd_string( DATE *d )
{
	int wday;
	time_t current;
	struct tm *tm = localtime( &d->current );
	
	wday = tm->tm_wday;
	current = d->current;

	while( wday != WDAY_SATURDAY )
	{
		wday++;
		current += SECONDS_IN_DAY;
		if ( wday == 7 ) wday = 0;
	}

	return date_new_get_yyyy_mm_dd( current );

} /* date_this_saturday_yyyy_mm_dd_string() */

DATE *date_get_prior_day(	DATE *date,
				int wday_parameter )
{
	int wday;
	time_t current;
	struct tm *tm = localtime( &date->current );
	
	wday = tm->tm_wday;
	current = date->current;

	wday--;
	current -= SECONDS_IN_DAY;

	if ( wday == -1 ) wday = WDAY_SATURDAY;

	while( wday != wday_parameter )
	{
		wday--;
		current -= SECONDS_IN_DAY;
		if ( wday == -1 ) wday = WDAY_SATURDAY;
	}

	return date_current_new( current );

} /* date_get_prior_day() */

DATE *date_get_prior_saturday( DATE *date )
{
	return date_get_prior_day( date, WDAY_SATURDAY );
}

DATE *date_get_prior_sunday( DATE *date )
{
	return date_get_prior_day( date, WDAY_SUNDAY );
}

DATE *date_populate_week_of_year_dictionary( char *year )
{
	DATE *date;
	char *date_string;
	int wday;
	int week;
	char week_string[ 64 ];

	date = date_new( atoi( year ), 1, 1 );
	date->week_of_year_dictionary = dictionary_new();

	date_set_tm_structures( date, date->current );

	wday = date->tm->tm_wday;

	week = 0;
	sprintf( week_string, "%d", week );

	while( 1 )
	{
		date_string = date_get_yyyy_mm_dd_string( date );

		if( wday != ( WDAY_SATURDAY + 1 ) )
		{
			wday++;
		}
		else
		{
			wday = 1;
			week++;
			sprintf( week_string, "%d", week );
		}
		dictionary_set_string(	date->week_of_year_dictionary,
					date_string,
					week_string );

		/* If at end of year */
		/* ----------------- */
		if ( strcmp( date_string + 5, "12-31" ) == 0 ) break;

		date_increment_day( date );
	}
	return date;
} /* date_populate_week_of_year_dictionary() */

int date_get_week_of_year( DATE *date, char *yyyy_mm_dd )
{
	char *week_string = 
		dictionary_get( date->week_of_year_dictionary, yyyy_mm_dd );
	return atoi( week_string );
}

DATE *date_yyyy_mm_dd_hhmm_new( char *date_string, char *time_string )
{
	DATE *date;
	int year, month, day, hour, minutes;
	char buffer[ 3 ];

	year = atoi( date_string );
	month = atoi( date_string + 5 );
	day = atoi( date_string + 8 );

	*buffer = *time_string;
	*(buffer + 1) = *(time_string + 1);
	*(buffer + 2) = '\0';

	hour = atoi( buffer );

	*buffer = *(time_string + 2);
	*(buffer + 1) = *(time_string + 3);

	minutes = atoi( buffer );

	date = date_new( year, month, day );
	date_set_time( date, hour, minutes );
	return date;
} /* date_yyyy_mm_dd_hhmm_new() */

int date_set_time_hhmm( DATE *date, char *hhmm )
{
	int hours, minutes;
	char buffer[ 3 ];

	if ( strlen( hhmm ) != 4 ) return 0;

	*(buffer + 2) = '\0';
	*buffer = *hhmm;
	*(buffer + 1) = *(hhmm + 1);
	hours = atoi( buffer );

	*buffer = *(hhmm + 2);
	*(buffer + 1) = *(hhmm + 3);
	minutes = atoi( buffer );

	date_set_time( date, hours, minutes );
	return 1;
} /* date_set_time_hhmm() */

void date_increment_hours( DATE *d, float hours )
{
	d->current += (long)((float)SECONDS_IN_HOUR * hours);
	date_set_tm_structures( d, d->current );
}

void date_add_minutes( DATE *d, int minutes )
{
	date_increment_minutes( d, minutes );
}

void date_increment_minutes( DATE *d, int minutes )
{
	d->current += (long)(SECONDS_IN_MINUTE * minutes);
	date_set_tm_structures( d, d->current );
}

void date_increment_seconds( DATE *d, int seconds )
{
	d->current += (long)seconds;
	date_set_tm_structures( d, d->current );
}

int date_subtract_days( DATE *later_date, DATE *earlier_date )
{
	time_t difference;
	int results;

	difference = later_date->current - earlier_date->current;
	results = (int)((double)difference / (double)SECONDS_IN_DAY);
	return results;
} /* date_subtract_days() */

int date_get_month_day_number( DATE *date )
{
	return ( date->tm->tm_mday );
}

int date_get_day( DATE *date )
{
	return date_get_day_number( date );
}

int date_get_day_number( DATE *date )
{
	return date->tm->tm_mday;
}

int date_get_week_number( DATE *date )
{
	return ( date->tm->tm_yday / 7 ) + 1;
}

int date_get_month_number( DATE *date )
{
	return date->tm->tm_mon + 1;
}

int date_get_year_number( DATE *date )
{
	return date->tm->tm_year + 1900;
}

int date_get_hour_number( DATE *date )
{
	return date->tm->tm_hour;
}

int date_get_minutes_number( DATE *date )
{
	return date->tm->tm_min;
}

void date_round2five_minutes( DATE *date )
{
	int hour;
	int minutes;
	int remainder;

	hour = date_get_hour_number( date );
	minutes = date_get_minutes_number( date );
	remainder = minutes % 5;

	if ( !remainder ) return;

	if ( remainder <= 2 )
		minutes = minutes - remainder;
	else
	{
		minutes = (5 - remainder) + minutes;
		if ( minutes == 60 )
		{
			minutes = 0;
			hour++;

			if ( hour == 24 )
			{
				hour = 0;
				date_increment_day( date );
			}
		}
	}
	
	date_set_time( date, hour, minutes );
} /* date_round2five_minutes() */

double date_yyyy_mm_dd_hhmm_to_julian( char *yyyy_mm_dd, char *hhmm )
{
	return julian_yyyy_mm_dd_time_hhmm_to_julian( yyyy_mm_dd, hhmm );
}

double date_yyyy_mm_dd_to_julian( char *yyyy_mm_dd )
{
	return julian_yyyy_mm_dd_time_hhmm_to_julian( yyyy_mm_dd, "0000" );
}

boolean date_copy( DATE *d1, DATE *d2 )
{
	if ( !d2 ) return 0;

	d1->current = d2->current;
	d1->format_yyyy_mm_dd = d2->format_yyyy_mm_dd;
	d1->week_of_year_dictionary = d2->week_of_year_dictionary;
	memcpy( d1->tm, d2->tm, sizeof( struct tm ) );
	return 1;
}

char *date_display_mdy( DATE *date )
{
	char buffer[ 32 ];

	sprintf( 	buffer, 
			"%02d/%02d/%04d",
			date_get_month( date ),
			date_get_day_of_month( date ),
			date_get_year( date ) );

	return strdup( buffer );
} /* date_display_mdy() */

char *date_get_yyyy_mm_dd_hhmm( DATE *date )
{
	return date_display_yyyy_mm_dd_hhmm( date );
}

char *date_display_yyyy_mm_dd_hhmm( DATE *date )
{
	char buffer[ 32 ];

	sprintf( 	buffer, 
			"%d-%02d-%02d:%02d%02d",
			date_get_year( date ),
			date_get_month( date ),
			date_get_day_of_month( date ),
			date_get_hour( date ),
			date_get_minutes( date ) );

	return strdup( buffer );

} /* date_display_yyyy_mm_dd_hhmm() */

void date_increment_weekly_ceiling( DATE *date )
{
	while( date_get_day_of_week( date ) != DATE_END_OF_WEEK_INDICATOR )
		date_increment_day( date );
} /* date_increment_weekly_ceiling() */

boolean date_compare(		DATE *date1,
				DATE *date2 )
{
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	if ( !date1 || !date2 ) return 0;

	return ( strcmp(
			date_get_yyyy_mm_dd( buffer1, date1 ),
			date_get_yyyy_mm_dd( buffer2, date2 ) ) );
} /* date_compare() */

boolean date_same_day(		DATE *old_date,
				DATE *new_date )
{
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	if ( !old_date || !new_date ) return 0;

	return ( strcmp(
			date_get_yyyy_mm_dd( buffer1, old_date ),
			date_get_yyyy_mm_dd( buffer2, new_date ) ) == 0 );
} /* date_same_day() */

boolean date_tomorrow( 		DATE *old_date, 
				DATE *new_date )
{
	int return_value;
	char buffer1[ 128 ];
	char buffer2[ 128 ];
	DATE *tomorrow_date;

	tomorrow_date = date_new_date();
	date_copy( tomorrow_date, old_date );
	date_increment_day( tomorrow_date );
	return_value =
		( strcmp(
			date_get_yyyy_mm_dd( buffer1, new_date ),
			date_get_yyyy_mm_dd( buffer2, tomorrow_date ) ) == 0 );
	date_free( tomorrow_date );
	return return_value;
} /* date_tomorrow() */

boolean date_year_in_future(	DATE *date,
				int year )
{
	return ( year > date_get_year_number( date ) );
}

void date_output_hour_error(	char *function_name,
				int hour )
{
	fprintf(stderr,
		"ERROR in %s(): daylight savings time hour problem = %d.\n",
		function_name,
		hour );
}

int date_days_in_month(	int month,
			int year )
{
	DATE *begin_date;
	char date_string[ 16 ];
	int days_in_month;

	sprintf( date_string, "%d-%.2d-01", year, month );
	begin_date = date_yyyy_mm_dd_new( date_string );
	days_in_month = 0;
	do {
		days_in_month++;
		date_increment( begin_date );
	} while( date_get_month( begin_date ) == month );
	return days_in_month;
} /* date_days_in_month() */

char *date_current_to_static_yyyy_mm_dd_string( time_t current )
{
	DATE *d = date_new_date();
	static char yyyy_mm_dd_string[ 16 ];

	date_set_tm_structures( d, current );
	d->current = date_tm_to_current( d->tm );

	strcpy( yyyy_mm_dd_string, date_static_display_yyyy_mm_dd( d ) );
	date_free( d );
	return yyyy_mm_dd_string;
} /* date_current_to_static_yyyy_mm_dd_string() */

char *date_prior_day_of_week_yyyy_mm_dd_string(	DATE *d,
						int day_of_week )
{
	int wday;
	time_t current;
	struct tm *tm = localtime( &d->current );
	
	wday = tm->tm_wday;
	current = d->current;

	while( wday != day_of_week )
	{
		wday--;
		current -= SECONDS_IN_DAY;
		if ( wday == -1 ) wday = 6;
	}

	return date_current_to_static_yyyy_mm_dd_string( current );

} /* date_prior_day_of_week_yyyy_mm_dd_string() */

char *date_get_unix_now_string(	void )
{
	char sys_string[ 128 ];

	sprintf( sys_string,
		 "date +'%cF:%cH%cM'",
		 '%', '%', '%' );
	return pipe2string( sys_string );
} /* date_get_unix_now_string() */

boolean date_is_greatgreatgrandfather( DATE *d )
{
	int month;
	int day;

	month = date_get_month( d );
	day = date_get_day( d );

	if ( month == 1 && day == 1 )
		return 1;
	else
		return 0;

} /* date_is_greatgreatgrandfather() */

boolean date_is_greatgrandfather( DATE *d )
{
	int month;
	int day;

	month = date_get_month( d );
	day = date_get_day( d );

	if ( month == 1 && day == 1 )
		return 1;
	else
	if ( month == 7 && day == 1 )
		return 1;
	else
		return 0;

} /* date_is_greatgrandfather() */

boolean date_is_grandfather( DATE *d )
{
	int day;

	day = date_get_day( d );

	if ( day == 1 )
		return 1;
	else
		return 0;

} /* date_is_grandfather() */

boolean date_is_father( DATE *d )
{
	return date_is_sunday( d );

} /* date_is_father() */

char *date_get_colon_now_time( void )
{
	char *sys_string;

	sys_string = "now.sh hhmmss | sed 's/\\([0-9][0-9]$\\)/:&/'";
	return pipe2string( sys_string );

} /* date_get_colon_now_time() */

void date_remove_colon_in_time( char *time_string )
{
	char hour[ 3 ];
	char minute[ 3 ];

	if ( timlib_strlen( time_string ) != 5 ) return;

	strncpy( hour, time_string, 2 );
	*(hour + 2) = '\0';

	strncpy( minute, time_string + 3, 2 );
	*(minute + 2) = '\0';

	sprintf( time_string, "%s%s", hour, minute );

} /* date_remove_colon_in_time() */

void date_place_colon_in_time( char *time_string )
{
	char hour[ 3 ];
	char minute[ 3 ];

	if ( timlib_strlen( time_string ) != 4 ) return;
	if ( timlib_strcmp( time_string, "null" ) == 0 ) return;

	strncpy( hour, time_string, 2 );
	*(hour + 2) = '\0';

	strncpy( minute, time_string + 2, 2 );
	*(minute + 2) = '\0';

	sprintf( time_string, "%s:%s", hour, minute );

} /* date_place_colon_in_time() */

char *date_append_hhmmss( char *date_string )
{
	static char date_hhmmss[ 32 ];
	char *hhmmss;

	if ( !date_string || !*date_string ) return "";

	hhmmss = date_get_now_hh_colon_mm_colon_ss();

	sprintf( date_hhmmss,
		 "%s %s",
		 date_string,
		 hhmmss );

	free( hhmmss );
	return date_hhmmss;

} /* date_append_hhmmss() */

char *date_subtract_colon_from_time( char *time_string )
{
	static char buffer[ 128 ];

	int str_len = strlen( time_string );

	*buffer = '\0';

	if ( timlib_strcmp( time_string, "null" ) == 0 ) return time_string;

	if ( !character_exists( time_string, ':' ) ) return time_string;

	if ( str_len == 4 )
	{
		*buffer = '0';
		strncpy( buffer + 1, time_string, 1 );
		strncpy( buffer + 2, time_string + 2, 2 );
		*(buffer + 4) = '\0';
		return buffer;
	}
	else
	if ( str_len == 5 )
	{
		strncpy( buffer, time_string, 2 );
		strncpy( buffer + 2, time_string + 3, 2 );
		*(buffer + 4) = '\0';
		return buffer;
	}
	else
	{
		return (char *)0;
	}

} /* date_subtract_colon_from_time() */

