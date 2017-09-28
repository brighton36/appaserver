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

/* See also $APPASERVER_HOME/src_accountancymodel/employee.h */
/* --------------------------------------------------------- */
#define PAYROLL_PERIOD_NUMBER_LABEL	".payroll_period_number"
#define PAYROLL_BEGIN_DATE_LABEL	".payroll_begin_date"
#define PAYROLL_END_DATE_LABEL		".payroll_end_date"

#define MIDMONTH_DAY			15

/* Prototypes */
/* ---------- */
char *payroll_period_get_begin_date_string(
				char *period,
				int year,
				int period_number );

void payroll_period_prior(	char *period );

int payroll_period_output_monthly_period(
				char *date_string );

void payroll_period_output_monthly_dates(
				int year,
				int period_number );

int payroll_period_output_semimonthly_period(
				char *date_string );

void payroll_period_output_semimonthly_dates(
				int year,
				int period_number );

int payroll_period_output_biweekly_period(
				char *date_string );

void payroll_period_output_biweekly_dates(
				int year,
				int period_number );

int payroll_period_output_weekly_period(
				char *date_string );

void payroll_period_output_weekly_dates(
				int year,
				int period_number );

int payroll_period_get_biweekly_period(
				int week_of_year );

boolean payroll_period_get_weekly_dates(
				char *begin_work_date,
				char *end_work_date,
				int year,
				int period_number );

boolean payroll_period_get_semimonthly_dates(
				char *begin_work_date,
				char *end_work_date,
				int year,
				int period_number );

boolean payroll_period_get_monthly_dates(
				char *begin_work_date,
				char *end_work_date,
				int year,
				int period_number );

/* Returns period_number (maybe) */
/* ----------------------------- */
int payroll_period(		char *argv_0,
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
				char **prior_yn,
				NAME_ARG *arg );

int main( int argc, char **argv )
{
        NAME_ARG *arg;
	char *period = {0};
	char *prior_yn = "";
	char *date_string = "";
	char *period_number_string = "";
	char *year_string = "";
	int period_number;

	arg = init_arg( argv[ 0 ] );

	setup_arg( arg, argc, argv );

	fetch_parameters(	&period,
				&date_string,
				&period_number_string,
				&year_string,
				&prior_yn,
				arg );

	if ( strcmp( prior_yn, "yes" ) == 0 )
	{
		payroll_period_prior( period );
	}
	else
	{
		period_number =
			payroll_period(
				argv[ 0 ],
				period,
				date_string,
				atoi( period_number_string ),
				atoi( year_string ) );

		if ( *date_string && period_number )
		{
			char sys_string[ 1024 ];

			sprintf(sys_string,
				"payroll_period.e"
				" period=%s"
				" number=%d"
				" year=%d",
				period,
				period_number,
				atoi( date_string ) /* year */ );

			system( sys_string );
		}
	}

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

        ticket = add_valid_option( arg, "prior" );
	add_possible_value( arg, ticket, "yes" );
	add_possible_value( arg, ticket, "no" );
        set_default_value( arg, ticket, "no" );

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
			char **prior_yn,
			NAME_ARG *arg )
{
	*period = fetch_arg( arg, "period" );
	*date_string = fetch_arg( arg, "date" );
	*period_number_string = fetch_arg( arg, "number" );
	*year_string = fetch_arg( arg, "year" );
	*prior_yn = fetch_arg( arg, "prior" );
}

int payroll_period(	char *argv_0,
			char *period,
			char *date_string,
			int period_number,
			int year )
{
	boolean output_error = 0;
	int local_period_number = 0;

	if ( timlib_strcmp( period, "weekly" ) == 0 )
	{
		if ( *date_string )
		{
			local_period_number =
				payroll_period_output_weekly_period(
					date_string );
		}
		else
		if ( period_number )
		{
			payroll_period_output_weekly_dates(
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
			local_period_number =
				payroll_period_output_biweekly_period(
					date_string );
		}
		else
		if ( period_number )
		{
			payroll_period_output_biweekly_dates(
				year,
				period_number );
		}
		else
		{
			output_error = 1;		
		}
	}
	else
	if ( timlib_strcmp( period, "semimonthly" ) == 0 )
	{
		if ( *date_string )
		{
			local_period_number =
				payroll_period_output_semimonthly_period(
					date_string );
		}
		else
		if ( period_number )
		{
			payroll_period_output_semimonthly_dates(
				year,
				period_number );
		}
		else
		{
			output_error = 1;		
		}
	}
	else
	if ( timlib_strcmp( period, "monthly" ) == 0 )
	{
		if ( *date_string )
		{
			local_period_number =
				payroll_period_output_monthly_period(
					date_string );
		}
		else
		if ( period_number )
		{
			payroll_period_output_monthly_dates(
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

	return local_period_number;

} /* payroll_period() */

int payroll_period_output_weekly_period( char *date_string )
{
	DATE *d;
	int period_number;

	d = date_yyyy_mm_dd_new( date_string );

	period_number = date_get_week_of_year( d );

	printf( "%s %d\n",
		PAYROLL_PERIOD_NUMBER_LABEL,
		period_number );

	return period_number;

} /* payroll_period_output_weekly_period() */

void payroll_period_output_weekly_dates(
				int year,
				int period_number )
{
	char begin_work_date[ 16 ];
	char end_work_date[ 16 ];

	*begin_work_date = '\0';
	*end_work_date = '\0';

	if ( period_number < 1 || period_number > 53 ) return;

	if ( payroll_period_get_weekly_dates(
				begin_work_date,
				end_work_date,
				year,
				period_number ) )
	{
		printf( "%s %s\n",
			PAYROLL_BEGIN_DATE_LABEL,
			begin_work_date );

		printf( "%s %s\n",
			PAYROLL_END_DATE_LABEL,
			end_work_date );
	}

} /* payroll_period_output_weekly_dates() */

boolean payroll_period_get_weekly_dates(
				char *begin_work_date,
				char *end_work_date,
				int year,
				int period_number )
{
	DATE *d;
	char date_string[ 16 ];
	int week_of_year;
	boolean all_done = 0;
	boolean first_time = 1;

	if ( period_number > 53 ) return 0;

	if ( period_number == 1 )
	{
		sprintf( date_string, "%d-12-25", year - 1 );
	}
	else
	if ( period_number >= 52 )
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
				strcpy( begin_work_date,
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
			if ( week_of_year == 53
			||   week_of_year == 1 )
			{
				all_done = 1;
			}
		}
		else
		if ( period_number == 53 )
		{
			if ( week_of_year == 1 )
			{
				all_done = 1;
			}
		}
		else
		if ( week_of_year > period_number )
		{
			all_done = 1;
		}
	}

	date_increment_days( d, -1 );

	strcpy( end_work_date,
		date_yyyy_mm_dd( d ) );

	return 1;

} /* payroll_period_get_weekly_dates() */

int payroll_period_output_biweekly_period( char *date_string )
{
	DATE *d;
	int week_of_year;
	int period_number;

	d = date_yyyy_mm_dd_new( date_string );

	week_of_year = date_get_week_of_year( d );

	period_number =
		payroll_period_get_biweekly_period(
			week_of_year );

	printf( "%s %d\n",
		PAYROLL_PERIOD_NUMBER_LABEL,
		period_number );

	return period_number;

} /* payroll_period_output_biweekly_period() */

void payroll_period_output_biweekly_dates(
				int year,
				int period_number )
{
	char begin_work_date[ 16 ];
	char ignore_work_date[ 16 ];
	char end_work_date[ 16 ];
	int period_number_times_two;

	if ( period_number < 1 || period_number > 27 ) return;

	*begin_work_date = '\0';
	*end_work_date = '\0';

	if ( period_number == 27 )
	{
		payroll_period_get_weekly_dates(
				begin_work_date,
				end_work_date,
				year,
				53 );
	}
	else
	{
		period_number_times_two = period_number * 2;

		if ( period_number_times_two >= 54 )
			period_number_times_two = 53;

		if ( !payroll_period_get_weekly_dates(
					begin_work_date,
					ignore_work_date,
					year,
					period_number_times_two - 1 ) )
		{
			return;
		}

		if ( !payroll_period_get_weekly_dates(
					ignore_work_date,
					end_work_date,
					year,
					period_number_times_two ) )
		{
			strcpy( end_work_date, ignore_work_date );
		}
	} /* if not ( period_number == 27 ) */

	printf( "%s %s\n",
		PAYROLL_BEGIN_DATE_LABEL,
		begin_work_date );

	printf( "%s %s\n",
		PAYROLL_END_DATE_LABEL,
		end_work_date );

} /* payroll_period_output_biweekly_dates() */

int payroll_period_get_biweekly_period(
			int week_of_year )
{
	int period_number;

	period_number = (int)( ( (double)week_of_year / 2.0 ) + 0.5 );

	return period_number;

} /* payroll_period_get_biweekly_period() */

int payroll_period_output_semimonthly_period( char *date_string )
{
	DATE *d;
	int month;
	int day;
	int period_number;

	d = date_yyyy_mm_dd_new( date_string );

	if ( !d ) return 0;

	month = date_get_month( d );
	day = date_get_day( d );

	if ( day <= MIDMONTH_DAY )
	{
		period_number = ( month * 2 ) - 1;
	}
	else
	{
		period_number = month * 2;
	}

	printf( "%s %d\n",
		PAYROLL_PERIOD_NUMBER_LABEL,
		period_number );

	return period_number;

} /* payroll_period_output_semimonthly_period() */

void payroll_period_output_semimonthly_dates(
				int year,
				int period_number )
{
	char begin_work_date[ 16 ];
	char end_work_date[ 16 ];

	if ( period_number < 1 || period_number > 24 ) return;

	if ( payroll_period_get_semimonthly_dates(
				begin_work_date,
				end_work_date,
				year,
				period_number ) )
	{
		printf( "%s %s\n",
			PAYROLL_BEGIN_DATE_LABEL,
			begin_work_date );

		printf( "%s %s\n",
			PAYROLL_END_DATE_LABEL,
			end_work_date );
	}

} /* payroll_period_output_semimonthly_dates() */

boolean payroll_period_get_semimonthly_dates(
				char *begin_work_date,
				char *end_work_date,
				int year,
				int period_number )
{
	boolean payroll_period_odd;
	int month;
	int begin_day;
	int end_day;

	payroll_period_odd = ( period_number % 2 );

	if ( payroll_period_odd )
	{
		month = ( period_number / 2 ) + 1;
		begin_day = 1;
		end_day = MIDMONTH_DAY;
	}
	else
	{
		month = period_number / 2;
		begin_day = MIDMONTH_DAY + 1;
		end_day = date_get_last_month_day(
				month,
				year );
	}

	sprintf( begin_work_date, "%d-%.2d-%.2d", year, month, begin_day );
	sprintf( end_work_date, "%d-%.2d-%d", year, month, end_day );

	return 1;

} /* payroll_period_get_semimonthly_dates() */

boolean payroll_period_get_monthly_dates(
				char *begin_work_date,
				char *end_work_date,
				int year,
				int period_number )
{
	int end_day;

	end_day = date_get_last_month_day(
			period_number,
			year );

	sprintf( begin_work_date, "%d-%.2d-01", year, period_number );
	sprintf( end_work_date, "%d-%.2d-%d", year, period_number, end_day );

	return 1;

} /* payroll_period_get_monthly_dates() */

int payroll_period_output_monthly_period(
				char *date_string )
{
	DATE *d;
	int period_number;

	d = date_yyyy_mm_dd_new( date_string );

	if ( !d ) return 0;

	period_number = date_get_month( d );

	printf( "%s %d\n",
		PAYROLL_PERIOD_NUMBER_LABEL,
		period_number );

	return period_number;

} /* payroll_period_output_monthly_period() */

void payroll_period_output_monthly_dates(
				int year,
				int period_number )
{
	char begin_work_date[ 16 ];
	char end_work_date[ 16 ];

	if ( period_number < 1 || period_number > 12 ) return;

	if ( payroll_period_get_monthly_dates(
				begin_work_date,
				end_work_date,
				year,
				period_number ) )
	{
		printf( "%s %s\n",
			PAYROLL_BEGIN_DATE_LABEL,
			begin_work_date );

		printf( "%s %s\n",
			PAYROLL_END_DATE_LABEL,
			end_work_date );
	}

} /* payroll_period_output_monthly_dates() */

char *payroll_period_get_begin_date_string(
					char *period,
					int year,
					int period_number )
{
	char sys_string[ 1024 ];
	char *results;

	sprintf(sys_string,
		"payroll_period.e	 "
		" period=%s		 "
		" number=%d		 "
		" year=%d		|"
		"grep %s		|"
		"column.e 1		 ",
		period,
		period_number,
		year,
		PAYROLL_BEGIN_DATE_LABEL );

	results = pipe2string( sys_string );

	if ( !results || !*results )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot get begin_date_string.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return results;

} /* payroll_period_get_begin_date_string() */

void payroll_period_prior( char *period )
{
	DATE *d;
	int week_of_year;
	int period_number;
	int month;
	int day;
	int year;
	char *begin_date_string;
	char sys_string[ 1024 ];

	d = date_now_new();

	month = date_get_month( d );
	day = date_get_day( d );
	year = date_get_year( d );

	if ( timlib_strcmp( period, "weekly" ) == 0 )
	{
		period_number = date_get_week_of_year( d );

	}
	else
	if ( timlib_strcmp( period, "biweekly" ) == 0 )
	{
		week_of_year = date_get_week_of_year( d );

		period_number =
			payroll_period_get_biweekly_period(
				week_of_year );
	}
	else
	if ( timlib_strcmp( period, "semimonthly" ) == 0 )
	{
		if ( day < MIDMONTH_DAY )
		{
			period_number = ( month * 2 ) - 1;
		}
		else
		{
			period_number = month * 2;
		}
	}
	else
	if ( timlib_strcmp( period, "monthly" ) == 0 )
	{
		period_number = month;
	}

	begin_date_string =
		payroll_period_get_begin_date_string(
			period,
			year,
			period_number );

	d = date_yyyy_mm_dd_new( begin_date_string );
	date_increment_days( d, -1 );
	begin_date_string = date_yyyy_mm_dd( d );

	sprintf(sys_string,
		"payroll_period.e"
		" period=%s"
		" date=%s",
		period,
		begin_date_string );

	system( sys_string );

} /* payroll_period_prior() */

