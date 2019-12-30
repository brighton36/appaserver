/* -------------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_camp/post_change_enrollment_payment.c		*/
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

void post_change_enrollment_payment_insert(
				char *application_name,
				char *fund_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				char *payment_date_time );

void post_change_enrollment_payment_update(
				char *application_name,
				char *fund_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				char *payment_date_time );

void post_change_enrollment_payment_predelete(
				char *application_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				char *payment_date_time );

int main( int argc, char **argv )
{
	char *application_name;
	char *fund_name;
	char *camp_begin_date;
	char *camp_title;
	char *full_name;
	char *street_address;
	char *payment_date_time;
	char *state;

	/* exit() if it fails. */
	/* ------------------- */
	application_name = environ_get_application_name( argv[ 0 ] );

	appaserver_output_starting_argv_append_file(
				argc,
				argv,
				application_name );

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s fund camp_begin_date camp_title full_name street_address payment_date_time state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	fund_name = argv[ 1 ];
	camp_begin_date = argv[ 2 ];
	camp_title = argv[ 3 ];
	full_name = argv[ 4 ];
	street_address = argv[ 5 ];
	payment_date_time = argv[ 6 ];
	state = argv[ 7 ];

	/* ------------------------------------------------------------ */
	/* ENROLLMENT_PAYMENT.transaction_date_time DOES exist, 	*/
	/* so execute predelete.					*/
	/* ------------------------------------------------------------ */
	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_enrollment_payment_predelete(
			application_name,
			camp_begin_date,
			camp_title,
			full_name,
			street_address,
			payment_date_time );
	}
	else
	if ( strcmp( state, "delete" ) == 0 )
	{
		/* Update ENROLLMENT.amount_due and total_payment */
		/* ---------------------------------------------- */
		post_change_enrollment_payment_update(
			application_name,
			fund_name,
			camp_begin_date,
			camp_title,
			full_name,
			street_address,
			payment_date_time );
	}
	else
	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_enrollment_payment_insert(
			application_name,
			fund_name,
			camp_begin_date,
			camp_title,
			full_name,
			street_address,
			payment_date_time );
	}
	else
	{
		post_change_enrollment_payment_update(
			application_name,
			fund_name,
			camp_begin_date,
			camp_title,
			full_name,
			street_address,
			payment_date_time );
	}

	return 0;

} /* main() */

void post_change_enrollment_payment_insert(
			char *application_name,
			char *fund_name,
			char *camp_begin_date,
			char *camp_title,
			char *full_name,
			char *street_address,
			char *payment_date_time )
{
	CAMP *camp;
	ENROLLMENT_PAYMENT *e;

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

	if ( ! ( camp->camp_enrollment =
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

	if ( ! ( e = camp_enrollment_payment_seek(
			camp->camp_enrollment->camp_enrollment_payment_list,
			payment_date_time ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot camp_enrollment_payment_seek(%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
	}

	e->camp_enrollment_payment_transaction =
		camp_enrollment_payment_transaction(
			application_name,
			fund_name,
			camp->camp_enrollment->full_name,
			camp->camp_enrollment->street_address,
			e->payment_amount );

	e->camp_enrollment_payment_transaction->transaction_date_time =
		/* -------------------------------------- */
		/* Returns inserted transaction_date_time */
		/* -------------------------------------- */
		ledger_transaction_refresh(
			application_name,
			e->camp_enrollment_payment_transaction->full_name,
			e->camp_enrollment_payment_transaction->street_address,
			e->
				camp_enrollment_payment_transaction->
				transaction_date_time,
			e->
				camp_enrollment_payment_transaction->
				transaction_amount,
			(char *)0 /* memo */,
			0 /* check_number */,
			1 /* lock_transaction */,
			e->
				camp_enrollment_payment_transaction->
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

	camp_enrollment_payment_update(
		camp->camp_begin_date,
		camp->camp_title,
		camp->camp_enrollment->full_name,
		camp->camp_enrollment->street_address,
		payment_date_time,
		e->
			camp_enrollment_payment_transaction->
			transaction_date_time );

} /* post_change_enrollment_payment_insert() */

void post_change_enrollment_payment_update(
			char *application_name,
			char *fund_name,
			char *camp_begin_date,
			char *camp_title,
			char *full_name,
			char *street_address,
			char *payment_date_time )
{
	CAMP *camp;
	ENROLLMENT_PAYMENT *e;

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

	if ( ! ( camp->camp_enrollment =
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

	/* --------------------------------------------------------- */
	/* This fails when deleting a parent. Still, need to update. */
	/* --------------------------------------------------------- */
	e = camp_enrollment_payment_seek(
			camp->camp_enrollment->camp_enrollment_payment_list,
			payment_date_time );

	if ( e )
	{
		e->camp_enrollment_payment_transaction =
			camp_enrollment_payment_transaction(
				application_name,
				fund_name,
				camp->camp_enrollment->full_name,
				camp->camp_enrollment->street_address,
				e->payment_amount );

		e->camp_enrollment_payment_transaction->transaction_date_time =
			/* -------------------------------------- */
			/* Returns inserted transaction_date_time */
			/* -------------------------------------- */
			ledger_transaction_refresh(
				application_name,
				e->
					camp_enrollment_payment_transaction->
					full_name,
				e->
					camp_enrollment_payment_transaction->
					street_address,
				e->
					camp_enrollment_payment_transaction->
					transaction_date_time,
				e->
					camp_enrollment_payment_transaction->
					transaction_amount,
				(char *)0 /* memo */,
				0 /* check_number */,
				1 /* lock_transaction */,
				e->
				camp_enrollment_payment_transaction->
				journal_ledger_list );
	}

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

	if ( e )
	{
		camp_enrollment_payment_update(
			camp->camp_begin_date,
			camp->camp_title,
			camp->camp_enrollment->full_name,
			camp->camp_enrollment->street_address,
			payment_date_time,
			e->
				camp_enrollment_payment_transaction->
				transaction_date_time );
	}

} /* post_change_enrollment_payment_update() */

void post_change_enrollment_payment_predelete(
			char *application_name,
			char *camp_begin_date,
			char *camp_title,
			char *full_name,
			char *street_address,
			char *payment_date_time )
{
	CAMP *camp;
	ENROLLMENT_PAYMENT *e;

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

	if ( ! ( camp->camp_enrollment =
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

	if ( ! ( e = camp_enrollment_payment_seek(
			camp->camp_enrollment->camp_enrollment_payment_list,
			payment_date_time ) ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot camp_enrollment_payment_seek(%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
	}

	if ( !e->camp_enrollment_payment_transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: no transaction for (%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
	}

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			camp->camp_enrollment->full_name,
			camp->camp_enrollment->street_address,
			e->
				camp_enrollment_payment_transaction->
				transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			camp->camp_enrollment->full_name,
			camp->camp_enrollment->street_address,
			e->
				camp_enrollment_payment_transaction->
				transaction_date_time );

	ledger_journal_ledger_list_propagate( 
		e->
			camp_enrollment_payment_transaction->
			journal_ledger_list,
		application_name );

} /* post_change_enrollment_payment_predelete() */

