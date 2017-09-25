/* $APPASERVER_HOME/utility/payroll_period.c		*/
/* ---------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------- 	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "name_arg.h"
#include "date.h"

/* Constants */
/* --------- */
#define PAYROLL_PERIOD_NUMBER_LABEL	".payroll_period_number"
#define PAYROLL_BEGIN_DATE_LABEL	".payroll_begin_date"
#define PAYROLL_END_DATE_LABEL		".payroll_end_date"

/* Prototypes */
/* ---------- */
void payroll_period_biweekly_period(
				int year,
				int period_number );

void payroll_period_biweekly_date(char *date_string );

void payroll_period_weekly_period(
				int year,
				int period_number );

void payroll_period_weekly_date(char *date_string );

void payroll_period(		char *argv_0,
				char *period,
				char *date_string,
				int period_number,
				int year );

void setup_arg(			NAME_ARG *arg,
				int argc,
				char **argv );

void fetch_parameters(		char **period, 
				char **date_string,
				char **period_number_string,
				char **year_string,
				NAME_ARG *arg );

int main( int argc, char **argv )
{
        NAME_ARG *arg;
	char *period = {0};
	char *date_string = "";
	char *period_number_string = "";
	char *year_string = "";

	arg = init_arg( argv[ 0 ] );

	setup_arg( arg, argc, argv );

	fetch_parameters(	&period,
				&date_string,
				&period_number_string,
				&year_string,
				arg );

	payroll_period(	argv[ 0 ],
			period,
			date_string,
			atoi( period_number_string ),
			atoi( year_string ) );

	return 0;

} /* main() */

void setup_arg( NAME_ARG *arg, int argc, char **argv )
{
        int ticket;

        ticket = add_valid_option( arg, "period" );
	add_possible_value( arg, ticket, "weekly" );
	add_possible_value( arg, ticket, "biweekly" );
	add_possible_value( arg, ticket, "semimonthly" );
	add_possible_value( arg, ticket, "monthly" );

        ticket = add_valid_option( arg, "date" );
        set_default_value( arg, ticket, "" );

        ticket = add_valid_option( arg, "number" );
        set_default_value( arg, ticket, "" );

        ticket = add_valid_option( arg, "year" );
        set_default_value( arg, ticket, "" );

        name_arg_insert( arg, argc, argv );

} /* setup_arg() */

void fetch_parameters(	char **period, 
			char **date_string,
			char **period_number_string,
			char **year_string,
			NAME_ARG *arg )
{
	*period = fetch_arg( arg, "period" );
	*date_string = fetch_arg( arg, "date" );
	*period_number_string = fetch_arg( arg, "number" );
	*year_string = fetch_arg( arg, "year" );
}

void payroll_period(	char *argv_0,
			char *period,
			char *date_string,
			int period_number,
			int year )
{
	boolean output_error = 0;

	if ( timlib_strcmp( period, "weekly" ) == 0 )
	{
		if ( *date_string )
		{
			payroll_period_weekly_date( date_string );
		}
		else
		if ( period_number )
		{
			payroll_period_weekly_period(
				year,
				period_number );
		}
		else
		{
			output_error = 1;		
		}
	}
	else
	if ( timlib_strcmp( period, "biweekly" ) == 0 )
	{
		if ( *date_string )
		{
			payroll_period_biweekly_date( date_string );
		}
		else
		if ( period_number )
		{
			payroll_period_biweekly_period(
				year,
				period_number );
		}
		else
		{
			output_error = 1;		
		}
	}

	if ( output_error )
	{
		fprintf( stderr,
			 "Error in %s: invalid combination of parameters.\n",
			 argv_0 );
	}

} /* payroll_period() */

void payroll_period_weekly_date( char *date_string )
{
	DATE *d;

	d = date_yyyy_mm_dd_new( date_string );

	printf( "%s %d\n",
		PAYROLL_PERIOD_NUMBER_LABEL,
		date_get_week_of_year( d ) );

} /* payroll_period_weekly_date() */

void payroll_period_weekly_period(
				int year,
				int period_number )
{
	DATE *d;
	char date_string[ 16 ];
	int week_of_year;
	boolean all_done = 0;
	boolean first_time = 1;

	if ( period_number == 1 )
	{
		sprintf( date_string, "%d-12-25", year - 1 );
	}
	else
	if ( period_number == 52 )
	{
		sprintf( date_string, "%d-12-15", year );
	}
	else
	{
		sprintf( date_string, "%d-01-01", year );
	}

	d = date_yyyy_mm_dd_new( date_string );

	week_of_year = date_get_week_of_year( d );

	while ( !all_done )
	{
		if ( week_of_year == period_number )
		{
			if ( first_time )
			{
				printf( "%s %s\n",
					PAYROLL_BEGIN_DATE_LABEL,
					date_yyyy_mm_dd( d ) );
				first_time = 0;
			}
		}

		date_increment_days( d, 1 );

		week_of_year = date_get_week_of_year( d );

		if ( period_number == 1 )
		{
			if ( week_of_year == 2 ) all_done = 1;
		}
		else
		if ( period_number == 52 )
		{
			if ( week_of_year == 1 ) all_done = 1;
		}
		else
		if ( week_of_year > period_number )
		{
			all_done = 1;
		}
	}

	date_increment_days( d, -1 );

	printf( "%s %s\n",
		PAYROLL_END_DATE_LABEL,
		date_yyyy_mm_dd( d ) );

} /* payroll_period_weekly_period() */

void payroll_period_biweekly_date( char *date_string )
{
	DATE *d;
	int week_of_year;
	int payroll_period;

	d = date_yyyy_mm_dd_new( date_string );

	week_of_year = date_get_week_of_year( d );
	payroll_period = (int)( ( (double)week_of_year / 2.0 ) + 0.5 );

	printf( "%s %d\n",
		PAYROLL_PERIOD_NUMBER_LABEL,
		payroll_period );

} /* payroll_period_biweekly_date() */

void payroll_period_biweekly_period(
				int year,
				int period_number )
{
	DATE *d;
	char date_string[ 16 ];
	int week_of_year;
	boolean all_done = 0;
	boolean first_time = 1;

	if ( period_number == 1 )
	{
		sprintf( date_string, "%d-12-25", year - 1 );
	}
	else
	if ( period_number == 52 )
	{
		sprintf( date_string, "%d-12-15", year );
	}
	else
	{
		sprintf( date_string, "%d-01-01", year );
	}

	d = date_yyyy_mm_dd_new( date_string );

	week_of_year = date_get_week_of_year( d );

	while ( !all_done )
	{
		if ( week_of_year == period_number )
		{
			if ( first_time )
			{
				printf( "%s %s\n",
					PAYROLL_BEGIN_DATE_LABEL,
					date_yyyy_mm_dd( d ) );
				first_time = 0;
			}
		}

		date_increment_days( d, 1 );

		week_of_year = date_get_week_of_year( d );

		if ( period_number == 1 )
		{
			if ( week_of_year == 2 ) all_done = 1;
		}
		else
		if ( period_number == 52 )
		{
			if ( week_of_year == 1 ) all_done = 1;
		}
		else
		if ( week_of_year > period_number )
		{
			all_done = 1;
		}
	}

	date_increment_days( d, -1 );

	printf( "%s %s\n",
		PAYROLL_END_DATE_LABEL,
		date_yyyy_mm_dd( d ) );

} /* payroll_period_biweekly_period() */

