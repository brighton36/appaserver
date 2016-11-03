/* table_dvr.c			         */
/* ------------------------------------- */
/* Test drive program for TABLE ADT      */
/* ------------------------------------- */
/* Tim Riley				 */
/* ------------------------------------- */

#include <stdio.h>
#include "table.h"

#define CONFIGURATION_FILE  "/ora_work_disk/tim/physical/report/statistics/measurement_configure.dat"

#define DATA_FILE  "/ora_work_disk/tim/physical/report/statistics/datafile/calculate_annual_BA_1997.dat"

/* Prototypes */
/* ---------- */
void set_calories_burned( TABLE *t );
void set_calories_ingested( TABLE *t );
void set_day_of_week( TABLE *t );
void set_day_key( TABLE *t );

main()
{
	/* test_1(); */
	/* test_2(); */
	test_3();
} /* main() */

test_3()
{
	TABLE *t = table_new();
	FILE *f = fopen( DATA_FILE, "r" );

	if ( !f )
	{
		fprintf( stderr, 
			 "test_3 file open error: %s.\n",
			 DATA_FILE );
		exit( 1 );
	}

	load_configuration_file( t, CONFIGURATION_FILE );
	load_data_labeled( t, f );
	table_display( stdout, t );
	fclose( f );
}

test_2()
{
	TABLE *t = table_new();
	load_configuration_file( t, "table_configure.dat" );
	set_day_key( t );
	set_day_of_week( t );
	set_calories_ingested( t );
	set_calories_burned( t );
	table_display( stdout, t );
}

test_1()
{
	TABLE *t = table_new();

	set_title( t, "Tim's Table" );
	set_rows_per_page( t, 3 );
	set_width( t, 80 );
	set_footer( t, "Tim's footer" );
	set_title_continued( t, "Tim's Table (cont)" );

	append_column_group( t, "date", "Date" );
	set_column( 	t, 
			"date", 
			"day_key", 
			DATATYPE_STRING,
			"Not used", 
			'-', 
			9, 
			0,
			"empty" );
	set_key_column_name( t, "day_key" );
	set_column_display_off( t, "date", "day_key" );

	set_column( 	t, 
			"date", 
			"day_of_week", 
			DATATYPE_STRING,
			"Day Of Week", 
			'-', 
			9, 
			0,
			"empty" );

	append_column_group( t, "calories", "Calories" );
	set_column(	t, 
			"calories", 
			"calories_ingested", 
			DATATYPE_INTEGER,
			"Ingested",
			' ',
			10,
			0,
			"empty" );
	set_column(	t, 
			"calories", 
			"calories_burned", 
			DATATYPE_INTEGER,
			"Burned",
			' ',
			10,
			0,
			"empty" );
	set_column(	t, 
			"calories", 
			"gain_loss", 
			DATATYPE_INTEGER,
			"Gained",
			' ',
			10,
			0,
			"empty" );

	set_day_key( t );
	set_day_of_week( t );
	set_calories_ingested( t );
	set_calories_burned( t );
	table_display( stdout, t );
}

void set_calories_burned( TABLE *t )
{
	int integer;

	integer = 2200;
	set_value( t, "calories", "calories_burned", "sunday", &integer );

	integer = 3000;
	set_value( t, "calories", "calories_burned", "monday", &integer );

	integer = 2750;
	set_value( t, "calories", "calories_burned", "tuesday", &integer );

	integer = 3500;
	set_value( t, "calories", "calories_burned", "wednesday", &integer );

	integer = 3190;
	set_value( t, "calories", "calories_burned", "thursday", &integer );

	integer = 2800;
	set_value( t, "calories", "calories_burned", "friday", &integer );

	integer = 2000;
	set_value( t, "calories", "calories_burned", "saturday", &integer );
}

void set_calories_ingested( TABLE *t )
{
	int integer = 3000;

	set_value( t, "calories", "calories_ingested", "sunday", &integer );

	integer = 2700;
	set_value( t, "calories", "calories_ingested", "monday", &integer );

	integer = 3500;
	set_value( t, "calories", "calories_ingested", "tuesday", &integer );

	integer = 2850;
	set_value( t, "calories", "calories_ingested", "wednesday", &integer );

	integer = 2290;
	set_value( t, "calories", "calories_ingested", "thursday", &integer );

	integer = 3100;
	set_value( t, "calories", "calories_ingested", "friday", &integer );

	integer = 3000;
	set_value( t, "calories", "calories_ingested", "saturday", &integer );
}

void set_day_key( TABLE *t )
{
	set_value( t, "date", "day_key", "sunday", "sunday" );
	set_value( t, "date", "day_key", "monday", "monday" );
	set_value( t, "date", "day_key", "tuesday", "tuesday" );
	set_value( t, "date", "day_key", "wednesday", "wednesday" );
	set_value( t, "date", "day_key", "thursday", "thursday" );
	set_value( t, "date", "day_key", "friday", "friday" );
	set_value( t, "date", "day_key", "saturday", "saturday" );
}

void set_day_of_week( TABLE *t )
{
	set_value( t, "date", "day_of_week", "sunday", "Sunday" );
	set_value( t, "date", "day_of_week", "monday", "Monday" );
	set_value( t, "date", "day_of_week", "tuesday", "Tuesday" );
	set_value( t, "date", "day_of_week", "wednesday", "Wednesday" );
	set_value( t, "date", "day_of_week", "thursday", "Thursday" );
	set_value( t, "date", "day_of_week", "friday", "Friday" );
	set_value( t, "date", "day_of_week", "saturday", "Saturday" );
}
