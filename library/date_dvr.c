/* date_dvr.c */
/* ---------- */

#include <stdio.h>
#include <stdlib.h>
#include "date.h"
#include "timlib.h"

void test_subtract_second( void );
void test_grandfather_father_son();
void test_round();
void test1();
void test2();
void test3();
void test4();
void test5();
void test6();
void test_week_of_year();
void test_1962();
void test_richie();
void test_11pm();
void test_1am();
void test_cross_dst();
void test_days_in_month();
void test_minutes_between( void );
void test_years_between( void );
void test_2015_10_26( void );

int main()
{
	test_2015_10_26();
	/*
	test_subtract_second();
	test_round();
	test_week_of_year();
	test1();
	test2();
	test3();
	test4();
	test5();
	test6();
	test_week_of_year();
	test_richie();
	printf( "Got appaserver.err = (%s)\n",
		APPASERVER_ERROR_FILE );
	test_11pm();
	test_1am();
	test_1962();
	test_cross_dst();
	test_days_in_month();
	test_minutes_between();
	test_years_between();
	test_grandfather_father_son();
	*/

	return 0;
}

void test_round()
{
	DATE *date;
	int hour,minutes;
	char date_buffer[ 128 ];
	char time_buffer[ 128 ];

	for( hour = 0; hour < 24; hour++ )
	{
		for( minutes = 0; minutes < 60; minutes++ )
		{
			date = date_today_new();
			date_set_time( date, hour, minutes );
			date_round2five_minutes( date );
			printf( "hour = %d minutes = %d round = %s date = %s\n",
				hour, minutes,
				date_get_hhmm( time_buffer, date ),
				date_get_yyyy_mm_dd( date_buffer, date ) );
			date_free( date );
		}
	}
}

void test4()
{
	DATE *d;
	char *date_string = "1999-12-31";
	char *time_string = "2320";

	d = date_yyyy_mm_dd_hhmm_new( date_string, time_string );

	printf( "should be 1999-12-31 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );

	printf( "should be 2320 is (%s)\n", 
		date_display_hhmm( d ) );

	date_increment_hours( d, 1 );

	printf( "should be 2000-01-01 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );

	printf( "should be 0020 is (%s)\n", 
		date_display_hhmm( d ) );
}

void test_week_of_year()
{
	DATE *d;

	d = date_populate_week_of_year_dictionary( "1999" );

	printf( "should be week 0 is: %d\n",
		date_get_week_of_year( d, "1999-01-01" ) );
	printf( "should be week 0 is: %d\n",
		date_get_week_of_year( d, "1999-01-02" ) );
	printf( "should be week 1 is: %d\n",
		date_get_week_of_year( d, "1999-01-03" ) );
	printf( "should be week 1 is: %d\n",
		date_get_week_of_year( d, "1999-01-04" ) );
	printf( "should be week 1 is: %d\n",
		date_get_week_of_year( d, "1999-01-09" ) );
	printf( "should be week 2 is: %d\n",
		date_get_week_of_year( d, "1999-01-10" ) );
	printf( "should be week 52 is: %d\n",
		date_get_week_of_year( d, "1999-12-26" ) );
	printf( "should be week 52 is: %d\n",
		date_get_week_of_year( d, "1999-12-31" ) );
}

void test2()
{
	DATE *d;

	d = date_yyyy_mm_dd_new( "1999-12-31" );
	printf( "should be 1999-12-31 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );
	date_increment_days( d, 1 );
	printf( "should be 2000-01-01 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );

	d = date_yyyy_mm_dd_new( "2000-1-1" );
	printf( "should be 2000-01-01 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );
	date_increment_days( d, 1 );
	printf( "should be 2000-01-02 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );

}

void test1()
{
	DATE *d = date_new( 1997, 1, 4 );
	int month, day;

	month = get_month( d );
	day = get_day_of_month( d );
	printf( "before increment week: %2d %2d\n", month, day );

	increment_week( d );
	month = get_month( d );
	day = get_day_of_month( d );
	printf( "after increment week: %2d %2d\n", month, day );
}

void test3()
{
	DATE *d = date_get_today_new();
	printf( "got today = (%s)\n",
		date_display_yyyy_mm_dd( d ) );
}

void test5()
{
	DATE *d = date_get_today_new();
	char buffer[ 128 ];

	date_increment_days( d, -1.0 );
	printf( "yesterday: %s\n", date_get_yyyy_mm_dd( buffer, d ) );
}

void test6()
{
	char buffer[ 128 ];
	DATE *d;

	d = date_new( 2020, 1, 1 );
	printf( "should be 2020-01-01: %s\n",
		date_get_yyyy_mm_dd( buffer, d ) );
	date_free( d );
	d = date_new( 2130, 1, 1 );
	printf( "should be 2130-01-01: %s\n",
		date_get_yyyy_mm_dd( buffer, d ) );
	d = date_new( 1969, 12, 31 );
	printf( "should be 1969-12-31: %s\n",
		date_get_yyyy_mm_dd( buffer, d ) );
	date_free( d );
	d = date_new( 1970, 1, 1 );
	printf( "should be 1970-01-01: %s\n",
		date_get_yyyy_mm_dd( buffer, d ) );
	date_free( d );

	d = date_new( 1968, 12, 31 );
	printf( "should be 1968-12-31: %s\n",
		date_get_yyyy_mm_dd( buffer, d ) );

	date_decrement_days( d, 1.0 );
	printf( "should be 1968-12-30: %s\n",
		date_get_yyyy_mm_dd( buffer, d ) );
	date_free( d );

}

void test_1am()
{
	char buffer[ 128 ];
	DATE *d;

	d = date_time_new( 2008, 3, 13, 1, 00 );

	printf( "should be 2008-03-13:0100, is %s:%s\n",
		date_get_yyyy_mm_dd( buffer, d ),
		date_display_hhmm( d ) );
	d->current += 600;
	date_set_tm_structures( d, d->current );
	printf( "should be 2008-03-13:0110, is %s:%s\n",
		date_get_yyyy_mm_dd( buffer, d ),
		date_display_hhmm( d ) );
	date_free( d );
}

void test_11pm()
{
	char buffer[ 128 ];
	DATE *d;

	d = date_time_new( 2008, 3, 21, 23, 00 );

	printf( "should be 2008-03-21:2300, is %s:%s\n",
		date_get_yyyy_mm_dd( buffer, d ),
		date_display_hhmm( d ) );
	date_free( d );
}

void test_1962()
{
	DATE *d;

	d = date_yyyy_mm_dd_hhmm_new( "1962-01-01", "0000" );
	printf( "should be 1962-01-01 is (%s)\n", 
		date_display_yyyy_mm_dd( d ) );

	d = date_yyyy_mm_dd_hhmm_new( "1970-01-01", "0000" );
	printf( "should be 1970-01-01:0000 is (%s)\n", 
		date_display_yyyy_mm_dd_hhmm( d ) );

	d = date_yyyy_mm_dd_hhmm_new( "1969-12-31", "2359" );
	printf( "should be 1969-12-31:2359 is (%s)\n", 
		date_display_yyyy_mm_dd_hhmm( d ) );

	d = date_yyyy_mm_dd_hhmm_new( "1969-12-31", "2358" );
	printf( "should be 1969-12-31:2358 is (%s)\n", 
		date_display_yyyy_mm_dd_hhmm( d ) );

	d = date_yyyy_mm_dd_hhmm_new( "1969-12-30", "2358" );
	printf( "should be 1969-12-30:2358 is (%s)\n",
		date_display_yyyy_mm_dd_hhmm( d ) );
	date_decrement_hour( d );
	printf( "should be 1969-12-30:2258 is (%s)\n",
		date_display_yyyy_mm_dd_hhmm( d ) );
}

void test_richie()
{
	DATE *d;

	d = date_new_date();
	d->current = -893400000;

	date_set_tm_structures( d, d->current );
	printf( "Ritchie's birth time is (%s)\n",
		date_display_yyyy_mm_dd_hhmm( d ) );
}

void test_cross_dst()
{
	DATE *d;

	d = date_time_new( 2009, 3, 8, 1, 0 );

	printf( "should be 2009-03-08:0100 is (%s:%s)\n", 
		date_display_yyyy_mm_dd( d ),
		date_display_hhmm( d ) );

	date_increment_hours( d, 1 );

	printf( "should be 2009-03-08:0300 is (%s:%s)\n", 
		date_display_yyyy_mm_dd( d ),
		date_display_hhmm( d ) );

	date_increment_hours( d, 1 );

	printf( "should be 2009-03-08:0400 is (%s:%s)\n", 
		date_display_yyyy_mm_dd( d ),
		date_display_hhmm( d ) );
}

void test_days_in_month()
{
	printf( "should be 31 is = (%d)\n",
		date_days_in_month( 1, 2010 ) );
	printf( "should be 28 is = (%d)\n",
		date_days_in_month( 2, 2010 ) );
	printf( "should be 29 is = (%d)\n",
		date_days_in_month( 2, 2012 ) );
}

void test_minutes_between( void )
{
	printf( "Should be 6: is %d\n",
		date_minutes_between(
				"2011-07-20",
				"1530",
				"2011-07-20",
	       			"1535",
				1 /* add_one */ ) );

}

void test_years_between( void )
{
	printf( "Should be 50: is %d\n",
		date_years_between(
				"1960-10-06",
				"2011-08-04" ) );

}

void test_grandfather_father_son()
{
	DATE *d;

	d = date_new_yyyy_mm_dd( "2010-01-01" );
	printf( "Should be greatgreatgrandfather: is %d\n",
		date_is_greatgreatgrandfather( d ) );

	d = date_new_yyyy_mm_dd( "2013-01-01" );
	printf( "Should be greatgrandfather: is %d\n",
		date_is_greatgrandfather( d ) );

	d = date_new_yyyy_mm_dd( "2013-02-01" );
	printf( "Should be grandfather: is %d\n",
		date_is_grandfather( d ) );

	d = date_new_yyyy_mm_dd( "2013-04-14" );
	printf( "Should be father: is %d\n",
		date_is_father( d ) );

}

void test_subtract_second( void )
{
	int seconds_offset;
	DATE *transaction_date;

	transaction_date =
		date_yyyy_mm_dd_hms_new(
			"2016-08-31 23:59:59" );

	printf( "got tm_isdst = %d.\n",
		transaction_date->tm->tm_isdst );

	printf( "Initial = %s\n",
		date_display_yyyy_mm_dd_colon_hms(
			transaction_date ) );

	for(	seconds_offset = 0;
		seconds_offset < 20;
		seconds_offset++ )
	{
		date_increment_seconds( transaction_date, -1 );

		printf( "%d seconds ago = %s\n",
			seconds_offset,
			date_display_yyyy_mm_dd_colon_hms(
				transaction_date ) );
	}
} /* test_subtract_second() */

void test_2015_10_26()
{
	DATE *date;
	char *s;

	date = date_new( 2015, 10, 26 );
	date = date_new_now();

	s = "2015-10-26^null";

	date_set_yyyy_mm_dd_hhmm_delimited(
			date,
			s,
			0 /* date_piece */,
			1 /* time_piece */,
			'^' );

	printf( "expecting (2015-10-26), got (%s)\n",
		date_display( date ) );
}
