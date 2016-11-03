/* ---------------------------------------------------	*/
/* parse_datatype_speadsheet.c				*/
/* ---------------------------------------------------	*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include "timlib.h"
#include "piece.h"

#define ELEVATION_FILENAME		"elevation.sql"
#define ELEVATION_DATATYPE_FILENAME	"elevation_datatype.sql"
#define DATATYPE_FILENAME		"datatype.sql"
#define UNIT_FILENAME			"unit.sql"

double get_elevation_meters( char *spreadsheet_elevation_original );

int main( void )
{
	char input_string[ 1024 ];
	int spreadsheet_column_number = 5;
	char datatype[ 128 ];
	char datatype_output[ 128 ];
	char units[ 128 ];
	char spreadsheet_elevation_original[ 128 ];
	double elevation_meters;
	FILE *elevation_output_pipe;
	FILE *elevation_datatype_output_pipe;
	FILE *datatype_output_pipe;
	FILE *unit_output_pipe;
	char sys_string[ 1024 ];
	char *elevation_table_name;
	char *elevation_datatype_table_name;
	char *datatype_table_name;
	char *unit_table_name;

	elevation_table_name = "co2flux_elevation";
	elevation_datatype_table_name = "co2flux_elevation_datatype";
	datatype_table_name = "co2flux_datatype";
	unit_table_name = "co2flux_unit";

	/* Open elevation_output_pipe */
	/* -------------------------- */
	sprintf( sys_string,
"sort -u | insert_statement.e table=%s field=elevation_meters delimiter='%c' > %s",
		 elevation_table_name,
		 '|',
		 ELEVATION_FILENAME );

	elevation_output_pipe = popen( sys_string, "w" );

	/* Open elevation_datatype_output_pipe */
	/* ----------------------------------- */
	sprintf( sys_string,
"sort -u | insert_statement.e table=%s field=elevation_meters,datatype,spreadsheet_column_number,spreadsheet_elevation_original delimiter='%c' > %s",
		 elevation_datatype_table_name,
		 '|',
		 ELEVATION_DATATYPE_FILENAME );

	elevation_datatype_output_pipe = popen( sys_string, "w" );

	/* Open datatype_output_pipe */
	/* ------------------------- */
	sprintf( sys_string,
"sort -u | insert_statement.e table=%s field=datatype,units delimiter='%c' > %s",
		 datatype_table_name,
		 '|',
		 DATATYPE_FILENAME );

	datatype_output_pipe = popen( sys_string, "w" );

	/* Open unit_output_pipe */
	/* --------------------- */
	sprintf( sys_string,
"sort -u | insert_statement.e table=%s field=units delimiter='%c' > %s",
		 unit_table_name,
		 '|',
		 UNIT_FILENAME );

	unit_output_pipe = popen( sys_string, "w" );

	while( get_line( input_string, stdin ) )
	{
		piece_quoted(
			units,
			',',
			input_string,
			3,
			'"' );

		/* if ( strcmp( units, "%" ) == 0 ) continue; */
		if ( strcmp( units, "days" ) == 0 ) continue;
		if ( strcmp( units, "units" ) == 0 ) continue;
		if ( strcmp( units, "years" ) == 0 ) continue;
		if ( strcmp( units, "" ) == 0 ) continue;

		piece_quoted(
			datatype,
			',',
			input_string,
			2,
			'"' );

		if ( character_exists( datatype, ',' ) )
		{
			piece(	datatype_output,
				',',
				datatype,
				0 );
			piece(	spreadsheet_elevation_original,
				',',
				datatype,
				1 );
		}
		else
		{
			strcpy( datatype_output, datatype );
			*spreadsheet_elevation_original = '\0';
		}

		elevation_meters =
			get_elevation_meters(
				spreadsheet_elevation_original );

		fprintf( elevation_output_pipe,
			 "%.2lf\n",
			 elevation_meters );

		fprintf( elevation_datatype_output_pipe,
			 "%.2lf|%s|%d|%s\n",
			 elevation_meters,
			 datatype_output,
			 spreadsheet_column_number,
			 spreadsheet_elevation_original );

		fprintf( datatype_output_pipe,
			 "%s|%s\n",
			 datatype_output,
			 units );

		fprintf( unit_output_pipe,
			 "%s\n",
			 units );

		spreadsheet_column_number++;
	}

	pclose( elevation_output_pipe );
	pclose( elevation_datatype_output_pipe );
	pclose( datatype_output_pipe );
	pclose( unit_output_pipe );
	exit( 0 );
} /* main() */

double get_elevation_meters( char *spreadsheet_elevation_original )
{
	int elevation_original_units;
	char *units;

	if ( ! ( elevation_original_units =
			atoi( spreadsheet_elevation_original ) ) )
	{
		return 0.0;
	}

	/* Skip integer */
	/* ------------ */
	units = spreadsheet_elevation_original;
	while( *units )
	{
		if ( isalpha( *units ) ) break;
		units++;
	}

	if ( !*units ) return 0.0;
	trim( units );

	if ( strcmp( units, "ft" ) == 0 )
		return elevation_original_units * 0.3048;
	else
	if ( strcmp( units, "m" ) == 0 )
		return (double)elevation_original_units;
	else
	if ( strcmp( units, "cm" ) == 0 )
		return elevation_original_units * 0.01;
	else
	{
		fprintf(stderr,
			"ERROR in %s/%s()/%d: cannot get units of (%s)\n",
			__FILE__,
			__FUNCTION__,
			__LINE__,
			units );
		exit( 1 );
	}
} /* get_elevation_meters() */

