/* station.c */
/* --------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "station_parse.h"
#include "piece.h"

STATION_PARSE *station_parse_new( char *station )
{
	STATION_PARSE *s =
		(STATION_PARSE *)
			calloc( 1, sizeof( STATION_PARSE ) );
	s->station = station;
	return s;
}

STATION_PARSE_RECORD_TYPE *record_type_new( int record_type )
{
	STATION_PARSE_RECORD_TYPE *r =
		(STATION_PARSE_RECORD_TYPE *)
			calloc( 1, sizeof( STATION_PARSE_RECORD_TYPE ) );
	r->record_type = record_type;
	return r;
}

void append_record_type( STATION_PARSE *station, int record_type )
{
	if ( station->number_record_types == MAX_RECORD_TYPES_PER_STATION )
	{
		fprintf( stderr,
	"ERROR: append_record_type(%d) exceeded max record types = %d\n",
			 record_type, MAX_RECORD_TYPES_PER_STATION );
		exit( 1 );
	}
	station->record_type_array[ station->number_record_types++ ] =
		record_type_new( record_type );
}

STATION_PARSE_RECORD_TYPE *get_record_type(
				STATION_PARSE *station,
				int record_type )
{
	int i;

	for ( i = 0; i < station->number_record_types; i++ )
		if ( station->record_type_array[ i]->record_type == record_type)
			return station->record_type_array[ i ];
	return (STATION_PARSE_RECORD_TYPE *)0;
}

void append_record_type_datatype(	STATION_PARSE_RECORD_TYPE *record_type, 
					char *datatype )
{
	if ( record_type->number_measurements_expected == 
	     MAX_DATATYPES_PER_RECORD )
	{
		fprintf( stderr,
	"ERROR: append_record_type_datatype(%s) exceeded max datatypes = %d\n",
			 datatype, MAX_DATATYPES_PER_RECORD );
		exit( 1 );
	}
	record_type->datatype_array[ 
			record_type->number_measurements_expected ] =
		strdup( datatype );

	record_type->number_measurements_expected++;
}

void zap_measurements( STATION_PARSE *station )
{
	station->number_measurements = 0;
}

void append_station_datatype( 	STATION_PARSE *station, 
				int record_type_int,
				char *datatype )
{
	STATION_PARSE_RECORD_TYPE *record_type;

	record_type = get_record_type( station, record_type_int );
	if ( !record_type )
	{
		fprintf( stderr,
"ERROR: append_station_datatype(station=%s,recordtype=%d,datatype=%s) cannot find record_type\n",
			 station->station,
			 record_type_int,
			 datatype );
		exit( 1 );
	}
	append_record_type_datatype( record_type, datatype );
}

void append_measurement( 	STATION_PARSE *station, 
				int record_type, 
				double measurement )
{
	if ( station->number_measurements == MAX_DATATYPES_PER_RECORD )
	{
		fprintf( stderr,
"ERROR: append_measurement(%s,%d) exceeded max datatypes per record = %d\n",
			 station->station,
			 record_type,
			 MAX_DATATYPES_PER_RECORD );
		exit( 1 );
	}
/*
fprintf( stderr, "for record_type = %d appending measurement = %lf\n",
record_type, measurement );
*/
	station->measurement_array[ station->number_measurements++ ] =
		measurement;
}

