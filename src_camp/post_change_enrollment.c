/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_camp/post_change_enrollment.c			*/
/* -------------------------------------------------------------------	*/
/* 									*/
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

/* Prototypes */
/* ---------- */

void post_change_enrollment_insert(
				char *application_name,
				char *fund_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address );

void post_change_enrollment_update(
				char *application_name,
				char *fund_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address );

void post_change_enrollment_predelete(
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
	/* ENROLLMENT.transaction_date_time DOES exist, 		*/
	/* so execute predelete.					*/
	/* ------------------------------------------------------------ */
	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_enrollment_predelete(
			application_name,
			camp_begin_date,
			camp_title,
			full_name,
			street_address );
	}
	else
	if ( strcmp( state, "delete" ) == 0 )
	{
		/* ---------- */
		/* Do nothing */
		/* ---------- */
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_enrollment_insert(
			application_name,
			fund_name,
			camp_begin_date,
			camp_title,
			full_name,
			street_address );
	}
	else
	{
		post_change_enrollment_update(
			application_name,
			fund_name,
			camp_begin_date,
			camp_title,
			full_name,
			street_address );
	}

	return 0;

} /* main() */

void post_change_enrollment_insert(
			char *application_name,
			char *fund_name,
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
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot camp_fetch(%s,%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 camp_begin_date,
			 camp_title );
		exit( 1 );
	}

	if ( ! ( camp->enrollment =
			camp_enrollment_fetch(
				application_name,
				camp->camp_begin_date,
				camp->camp_title,
				full_name,
				street_address,
				camp->enrollment_cost ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot camp_enrollment_fetch(%s,%s,%s,%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 camp->camp_begin_date,
			 camp->camp_title,
			 full_name,
			 street_address );
		exit( 1 );
	}

	if ( ! ( camp->enrollment->camp_enrollment_transaction =
			camp_enrollment_transaction(
				application_name,
				fund_name,
				camp->enrollment->full_name,
				camp->enrollment->street_address,
				camp->
					enrollment->
					camp_enrollment_invoice_amount ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: camp_enrollment_transaction() failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	camp->enrollment->camp_enrollment_transaction->transaction_date_time =
		/* -------------------------------------- */
		/* Returns inserted transaction_date_time */
		/* -------------------------------------- */
		ledger_transaction_refresh(
			application_name,
			camp->
				enrollment->
				camp_enrollment_transaction->
				full_name,
			camp->
				enrollment->
				camp_enrollment_transaction->
				street_address,
			camp->
				enrollment->
				camp_enrollment_transaction->
				transaction_date_time,
			camp->
				enrollment->
				camp_enrollment_transaction->
				transaction_amount,
			(char *)0 /* memo */,
			0 /* check_number */,
			1 /* lock_transaction */,
			camp->
				enrollment->
				camp_enrollment_transaction->
				journal_ledger_list );

	camp_enrollment_update(
		camp->camp_begin_date,
		camp->camp_title,
		camp->enrollment->full_name,
		camp->enrollment->street_address,
		camp->enrollment->camp_enrollment_invoice_amount,
		camp->enrollment->camp_enrollment_total_payment_amount,
		camp->enrollment->camp_enrollment_amount_due,
		camp->
			enrollment->
			camp_enrollment_transaction->
			transaction_date_time );

} /* post_change_enrollment_insert() */

void post_change_enrollment_update(
			char *application_name,
			char *fund_name,
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
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot camp_fetch(%s,%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 camp_begin_date,
			 camp_title );
		exit( 1 );
	}

	if ( ! ( camp->enrollment =
			camp_enrollment_fetch(
				application_name,
				camp->camp_begin_date,
				camp->camp_title,
				full_name,
				street_address,
				camp->enrollment_cost ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot camp_enrollment_fetch(%s,%s,%s,%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 camp->camp_begin_date,
			 camp->camp_title,
			 full_name,
			 street_address );
		exit( 1 );
	}

	if ( ! ( camp->enrollment->camp_enrollment_transaction =
			camp_enrollment_transaction(
				application_name,
				fund_name,
				camp->enrollment->full_name,
				camp->enrollment->street_address,
				camp->
					enrollment->
					camp_enrollment_invoice_amount ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: camp_enrollment_transaction(%s,%s) failed.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 camp->enrollment->full_name,
			 camp->enrollment->street_address );
		exit( 1 );
	}

	camp->enrollment->camp_enrollment_transaction->transaction_date_time =
		/* -------------------------------------- */
		/* Returns inserted transaction_date_time */
		/* -------------------------------------- */
		ledger_transaction_refresh(
			application_name,
			camp->
				enrollment->
				camp_enrollment_transaction->
				full_name,
			camp->
				enrollment->
				camp_enrollment_transaction->
				street_address,
			camp->
				enrollment->
				camp_enrollment_transaction->
				transaction_date_time,
			camp->
				enrollment->
				camp_enrollment_transaction->
				transaction_amount,
			(char *)0 /* memo */,
			0 /* check_number */,
			1 /* lock_transaction */,
			camp->
				enrollment->
				camp_enrollment_transaction->
				journal_ledger_list );

	camp_enrollment_update(
		camp->camp_begin_date,
		camp->camp_title,
		camp->enrollment->full_name,
		camp->enrollment->street_address,
		camp->enrollment->camp_enrollment_invoice_amount,
		camp->enrollment->camp_enrollment_total_payment_amount,
		camp->enrollment->camp_enrollment_amount_due,
		camp->
			enrollment->
			camp_enrollment_transaction->
			transaction_date_time );

} /* post_change_enrollment_update() */

void post_change_enrollment_predelete(
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
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot camp_fetch(%s,%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 camp_begin_date,
			 camp_title );
		exit( 1 );
	}

	if ( ! ( camp->enrollment =
			camp_enrollment_fetch(
				application_name,
				camp->camp_begin_date,
				camp->camp_title,
				full_name,
				street_address,
				camp->enrollment_cost ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot camp_enrollment_fetch(%s,%s,%s,%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 camp->camp_begin_date,
			 camp->camp_title,
			 full_name,
			 street_address );
		exit( 1 );
	}

	if ( !camp->enrollment->camp_enrollment_transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: no transaction for (%s,%s,%s,%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 camp->camp_begin_date,
			 camp->camp_title,
			 camp->enrollment->full_name,
			 camp->enrollment->street_address );
		exit( 1 );
	}

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			camp->enrollment->full_name,
			camp->enrollment->street_address,
			camp->enrollment->
				camp_enrollment_transaction->
				transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			camp->enrollment->full_name,
			camp->enrollment->street_address,
			camp->enrollment->
				camp_enrollment_transaction->
				transaction_date_time );

	ledger_journal_ledger_list_propagate( 
		camp->
			enrollment->
			camp_enrollment_transaction->
			journal_ledger_list,
		application_name );

} /* post_change_enrollment_predelete() */

