/* ---------------------------------------------------------------------*/
/* $APPASERVER_HOME/src_accountancymodel/post_change_employee_work_day.c*/
/* ---------------------------------------------------------------------*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "employee.h"

/* Constants */
/* --------- */

/* Prototypes */
/* ---------- */

int main( int argc, char **argv )
{
	char *application_name;
	char *database_string = {0};
	char *full_name;
	char *street_address;
	char *begin_work_date_time;
	char *state;
	char *preupdate_begin_work_date_time;
	char *preupdate_end_work_date_time;
	char sys_string[ 128 ];

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s application full_name street_address begin_work_date_time state preupdate_begin_work_date_time preupdate_end_work_date_time\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];

	if ( timlib_parse_database_string(	&database_string,
						application_name ) )
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			database_string );
	}
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	begin_work_date_time = argv[ 4 ];
	state = argv[ 5 ];
	preupdate_begin_work_date_time = argv[ 6 ];
	preupdate_end_work_date_time = argv[ 7 ];

	return 0;

} /* main() */

