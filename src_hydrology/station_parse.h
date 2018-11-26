/* station_parse.h */
/* --------------- */

#ifndef STATION_PARSE_H
#define STATION_PARSE_H

#define MAX_RECORD_TYPES_PER_STATION		255
#define MAX_DATATYPES_PER_RECORD		255

typedef struct
{
	int record_type;
	char *datatype_array[ MAX_DATATYPES_PER_RECORD ];
	int number_measurements_expected;
} STATION_PARSE_RECORD_TYPE;

typedef struct 
{
	char *station;
	double measurement_array[ MAX_DATATYPES_PER_RECORD ];
	int number_measurements;
	STATION_PARSE_RECORD_TYPE *record_type_array[
					MAX_RECORD_TYPES_PER_STATION ];
	int number_record_types;
} STATION_PARSE;

/* Operations */
/* ---------- */
STATION_PARSE *station_parse_new( char *station );
STATION_PARSE_RECORD_TYPE *record_type_new( int record_type );
STATION_PARSE_RECORD_TYPE *get_record_type(
				STATION_PARSE *station,
				int record_type );
void append_record_type( STATION_PARSE *station, int record_type );
void append_record_type_datatype(	STATION_PARSE_RECORD_TYPE *record_type, 
					char *datatype );
void append_station_datatype( 		STATION_PARSE *station, 
					int record_type, 
					char *datatype );
void append_measurement( 	STATION_PARSE *station, 
				int record_type, 
				double measurement );
void zap_measurements( STATION_PARSE *station );
#endif
