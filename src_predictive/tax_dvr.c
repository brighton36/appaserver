#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "date.h"
#include "tax.h"

void test_recovery( void );
void test_recovery_life_integer( void );
void test_recovery_life_float( void );

int main( void )
{
	/* test_recovery(); */
	test_recovery_life_integer();

	return 0;
}

void test_recovery()
{
	printf( "should be %.2lf, is %.2lf\n",
		29091.20,
		tax_calculate_real_estate_recovery(
				800000.0,
				"1993-04-01"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				27.5 /* recovery_period_years */,
				2007 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		29091.20,
		tax_calculate_real_estate_recovery(
				800000.0,
				"1993-04-01"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				27.5 /* recovery_period_years */,
				2019 /* current_year */ ) );

#ifdef NOT_DEFINED
	printf( "should be %.2lf, is %.2lf\n",
		23031.04,
		tax_calculate_real_estate_recovery(
				800000.0,
				"1993-04-01"
					/* service_placement_date_string */,
				"2007-10-07" /* sale_date_string */,
				27.5 /* recovery_period_years */,
				2007 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		31746.0,
		tax_calculate_real_estate_recovery(
				1000000.0,
				"1993-03-02"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				31.5 /* recovery_period_years */,
				2007 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		1322.8,
		tax_calculate_real_estate_recovery(
				1000000.0,
				"1993-03-02"
					/* service_placement_date_string */,
				"2007-01-05" /* sale_date_string */,
				31.5 /* recovery_period_years */,
				2007 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		3846.15,
		tax_calculate_real_estate_recovery(
				1200000.0,
				"2007-11-19"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				39.0 /* recovery_period_years */,
				2007 /* current_year */ ) );

	printf( "should be %.2lf, is %.2lf\n",
		11538.46,
		tax_calculate_real_estate_recovery(
				1200000.0,
				"2007-11-19"
					/* service_placement_date_string */,
				"2008-05-21" /* sale_date_string */,
				39.0 /* recovery_period_years */,
				2008 /* current_year */ ) );
#endif
}

void test_recovery_life_float()
{
	DATE *annual;
	int year;
	char beginning[ 16 ];
	int y;
	int years;
	double recovery;
	double total = 0.0;

	year = 1993;
	years = 30;
	sprintf( beginning, "%d-12-31", year );

	annual = date_yyyy_mm_dd_new( beginning, date_get_utc_offset() );

	for( y = 0; y < years; y++ )
	{
		recovery =
			 tax_calculate_real_estate_recovery(
				800000.0,
				"1993-04-01"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				27.5 /* recovery_period_years */,
				date_get_year( annual ) /* current_year */ );

		printf( "%.2d: %s: %.2lf\n",
			y + 1,
			date_display( annual ),
			recovery );

		date_increment_months( annual, 12.0, date_get_utc_offset() );
		total += recovery;
	}

	printf( "%.2lf\n", total );
}

void test_recovery_life_integer()
{
	DATE *annual;
	int year;
	char beginning[ 16 ];
	int y;
	int years;
	double recovery;
	double total = 0.0;

	year = 2007;
	years = 40;
	sprintf( beginning, "%d-12-31", year );

	annual = date_yyyy_mm_dd_new( beginning, date_get_utc_offset() );

	for( y = 0; y < years; y++ )
	{
		recovery =
			 tax_calculate_real_estate_recovery(
				800000.0,
				"2007-11-19"
					/* service_placement_date_string */,
				(char *)0 /* sale_date_string */,
				39.0 /* recovery_period_years */,
				date_get_year( annual ) /* current_year */ );

		printf( "%.2d: %s: %.2lf\n",
			y + 1,
			date_display( annual ),
			recovery );

		date_increment_months( annual, 12.0, date_get_utc_offset() );
		total += recovery;
	}

	printf( "%.2lf\n", total );
}

