/* measurement_backup.c 						*/
/* -------------------------------------------------------------------- */
/* This is the hydrology measurement_backup ADT.			*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include "measurement_backup.h"
#include "appaserver_library.h"
#include "date.h"
#include "piece.h"
#include "timlib.h"

MEASUREMENT_BACKUP *measurement_backup_new(	
					char *application_name,
					char *measurement_update_method,
					char *login_name,
					char *measurement_update_date,
					char *measurement_update_time )
{
	return new_measurement_backup(	
			application_name,
			measurement_update_method,
			login_name,
			measurement_update_date,
			measurement_update_time );
}

MEASUREMENT_BACKUP *new_measurement_backup(	
					char *application_name,
					char *measurement_update_method,
					char *login_name,
					char *measurement_update_date,
					char *measurement_update_time )
{
	MEASUREMENT_BACKUP *m;

	m = (MEASUREMENT_BACKUP *)calloc( 1, sizeof( MEASUREMENT_BACKUP ) );
	m->application_name = application_name;
	m->measurement_update_method = measurement_update_method;
	m->login_name = login_name;
	m->measurement_update_date = measurement_update_date;
	m->measurement_update_time = measurement_update_time;
	return m;
} /* new_measurement_backup() */

FILE *measurement_backup_open_insert_pipe(
					char *application_name )
{
	char sys_string[ 1024 ];
	char *table_name;

	table_name = 
		get_table_name( application_name, "measurement_backup" );

	sprintf( sys_string,
		 "insert_statement.e %s %s | sql.e 2>&1 | grep -v Duplicate",
		 table_name,
		 MEASUREMENT_BACKUP_INSERT_COLUMN_COMMA_LIST );

	return popen( sys_string, "w" );
} /* measurement_backup_open_insert_pipe() */


void measurement_backup_insert(	FILE *insert_pipe,
				char *measurement_update_date,
				char *measurement_update_time,
				char *measurement_update_method,
				char *login_name,
				char *comma_delimited_record,
				char really_yn,
				char delimiter )
{
	char station[ 128 ];
	char datatype[ 128 ];
	char measurement_date[ 128 ];
	char measurement_time[ 128 ];
	char original_value[ 128 ];

	if ( really_yn != 'y' ) return;

	if ( character_count( delimiter, comma_delimited_record ) <
	     MEASUREMENT_BACKUP_VALUE_PIECE )
	{
		fprintf( stderr,
	"WARNING in %s(): not enough fields in (%s)\n",
			 __FUNCTION__,
			 comma_delimited_record );
		return;
	}

	piece( 	station,
		delimiter,
		comma_delimited_record,
		MEASUREMENT_BACKUP_STATION_PIECE );

	piece( 	datatype,
		delimiter,
		comma_delimited_record,
		MEASUREMENT_BACKUP_DATATYPE_PIECE );

	piece(	measurement_date,
		delimiter,
		comma_delimited_record,
		MEASUREMENT_BACKUP_DATE_PIECE );

	piece(	measurement_time,
		delimiter,
		comma_delimited_record,
		MEASUREMENT_BACKUP_TIME_PIECE );

	piece( 	original_value,
		delimiter,
		comma_delimited_record,
		MEASUREMENT_BACKUP_VALUE_PIECE );

	fprintf( insert_pipe,
		 "%s|%s|%s|%s|%s|%s|%s|%s|%s\n",
		 station,
		 datatype,
		 measurement_date,
		 measurement_time,
		 measurement_update_date,
		 measurement_update_time,
		 measurement_update_method,
		 login_name,
		 original_value );

	return;
} /* measurement_backup_insert() */

void measurement_backup_close( FILE *insert_pipe )
{
	pclose( insert_pipe );
}

