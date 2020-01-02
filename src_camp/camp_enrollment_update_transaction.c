/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_camp/camp_enrollment_transaction_update.c	*/
/* -------------------------------------------------------------------	*/
/* 									*/
/* Note: this is also the SERVICE_ENROLLMENT post change process.	*/
/* -------------------------------------------------------------------	*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "ledger.h"
#include "camp.h"

/* Constants */
/* --------- */
#define CAMP_ENROLLMENT_MEMO	"Camp Enrollment"

/* Prototypes */
/* ---------- */

void camp_enrollment_transaction_update(
				char *application_name,
				char *fund_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address );

void camp_enrollment_transaction_update_predelete(
				char *application_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address );

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *camp_begin_date;
	char *camp_title;
	char *full_name;
	char *street_address;
	char *state;

	/* exit() if it fails. */
	/* ------------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
		argc,
		argv,
		application_name );

	if ( argc != 7 )
	{
		fprintf( stderr,
"Usage: %s fund camp_begin_date camp_title full_name street_address state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 1 ];
	camp_begin_date = argv[ 2 ];
	camp_title = argv[ 3 ];
	full_name = argv[ 4 ];
	street_address = argv[ 5 ];
	state = argv[ 6 ];

	/* ------------------------------------------------------------ */
	/* CAMP_ENROLLMENT.transaction_date_time DOES exist, 		*/
	/* so execute predelete.					*/
	/* ------------------------------------------------------------ */
	if ( strcmp( state, "predelete" ) == 0 )
	{
		camp_enrollment_transaction_update_predelete(
			application_name,
			camp_begin_date,
			camp_title,
			full_name,
			street_address );
	}
	else
	/* ----------------------------------------------- */
	/* May be executed from SERVICE_ENROLLMENT delete. */
	/* ----------------------------------------------- */
	if ( strcmp( state, "delete" ) == 0
	||   strcmp( state, "update" ) == 0
	||   strcmp( state, "insert" ) == 0 )
	{
		camp_enrollment_transaction_update(
			application_name,
			fund_name,
			camp_begin_date,
			camp_title,
			full_name,
			street_address );
	}

	return 0;

} /* main() */

void camp_enrollment_transaction_update(
			char *application_name,
			char *fund_name,
			char *camp_begin_date,
			char *camp_title,
			char *full_name,
			char *street_address )
{
	CAMP *camp;
	char *existing_transaction_date_time;

	if ( ! ( camp =
			camp_fetch(
				application_name,
				camp_begin_date,
				camp_title ) ) )
	{
		return;
	}

	if ( ! ( camp->camp_enrollment =
			camp_enrollment_fetch(
				application_name,
				camp->camp_begin_date,
				camp->camp_title,
				full_name,
				street_address,
				camp->enrollment_cost ) ) )
	{
		return;
	}

	if ( camp->camp_enrollment->camp_enrollment_transaction )
	{
		existing_transaction_date_time =
			camp->
				camp_enrollment->
				camp_enrollment_transaction->
				transaction_date_time;
	}
	else
	{
		existing_transaction_date_time = (char *)0;
	}

	if ( ! ( camp->camp_enrollment->camp_enrollment_transaction =
			camp_enrollment_transaction(
				application_name,
				fund_name,
				camp->camp_enrollment->full_name,
				camp->camp_enrollment->street_address,
				existing_transaction_date_time,
				camp->
					camp_enrollment->
					camp_enrollment_invoice_amount ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: camp_enrollment_transaction() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	camp->
		camp_enrollment->
		camp_enrollment_transaction->
		transaction_date_time =
			/* -------------------------------------- */
			/* Returns inserted transaction_date_time */
			/* -------------------------------------- */
			ledger_transaction_refresh(
				application_name,
				camp->
					camp_enrollment->
					camp_enrollment_transaction->
					full_name,
				camp->
					camp_enrollment->
					camp_enrollment_transaction->
					street_address,
				camp->
					camp_enrollment->
					camp_enrollment_transaction->
					transaction_date_time,
				camp->
					camp_enrollment->
					camp_enrollment_transaction->
					transaction_amount,
				CAMP_ENROLLMENT_MEMO,
				0 /* check_number */,
				1 /* lock_transaction */,
				camp->
					camp_enrollment->
					camp_enrollment_transaction->
					journal_ledger_list );

	camp_enrollment_update(
		camp->camp_begin_date,
		camp->camp_title,
		camp->camp_enrollment->full_name,
		camp->camp_enrollment->street_address,
		camp->camp_enrollment->camp_enrollment_invoice_amount,
		camp->camp_enrollment->camp_enrollment_total_payment_amount,
		camp->camp_enrollment->camp_enrollment_amount_due,
		camp->
			camp_enrollment->
			camp_enrollment_transaction->
			transaction_date_time );

} /* camp_enrollment_transaction_update() */

void camp_enrollment_transaction_update_predelete(
			char *application_name,
			char *camp_begin_date,
			char *camp_title,
			char *full_name,
			char *street_address )
{
	CAMP *camp;

	if ( ! ( camp =
			camp_fetch(
				application_name,
				camp_begin_date,
				camp_title ) ) )
	{
		return;
	}

	if ( ! ( camp->camp_enrollment =
			camp_enrollment_fetch(
				application_name,
				camp->camp_begin_date,
				camp->camp_title,
				full_name,
				street_address,
				camp->enrollment_cost ) ) )
	{
		return;
	}

	if ( !camp->camp_enrollment->camp_enrollment_transaction )
		return;

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			camp->camp_enrollment->full_name,
			camp->camp_enrollment->street_address,
			camp->camp_enrollment->
				camp_enrollment_transaction->
				transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			camp->camp_enrollment->full_name,
			camp->camp_enrollment->street_address,
			camp->camp_enrollment->
				camp_enrollment_transaction->
				transaction_date_time );

	ledger_journal_ledger_list_propagate( 
		camp->
			camp_enrollment->
			camp_enrollment_transaction->
			journal_ledger_list,
		application_name );

} /* camp_enrollment_transaction_update_predelete() */

