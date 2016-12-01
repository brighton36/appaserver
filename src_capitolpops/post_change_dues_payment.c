/* ---------------------------------------------------------------	*/
/* src_capitolpops/post_change_dues_payment.c				*/
/* ---------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* ---------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "application_constants.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "dues_payment.h"

/* Constants */
/* --------- */
#define STATUS_ACTIVE		"Active"

/* Prototypes */
/* ---------- */
void post_change_dues_payment_entity(
				char *application_name,
				char *full_name,
				char *street_address,
				char *payment_date,
				char *preupdate_full_name,
				char *preupdate_street_address );

void post_change_dues_payment_amount_update(
				DUES_PAYMENT *dues_payment,
				char *application_name );

void post_change_dues_payment_date_update(
				DUES_PAYMENT *dues_payment,
				char *preupdate_payment_date,
				char *application_name );

void post_change_dues_payment_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *payment_date,
				char *preupdate_full_name,
				char *preupdate_street_address,
				char *preupdate_payment_date,
				char *preupdate_payment_amount );

void post_change_dues_payment_delete(
				char *application_name,
				char *full_name,
				char *street_address,
				char *payment_date );

void post_change_dues_payment_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *payment_date );

void post_change_set_status_active(
				char *application_name,
				char *full_name,
				char *street_address );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *payment_date;
	char *state;
	char *preupdate_full_name;
	char *preupdate_street_address;
	char *preupdate_payment_date;
	char *preupdate_payment_amount;
	char *database_string = {0};

	if ( argc != 10 )
	{
		fprintf( stderr,
"Usage: %s application full_name street_address payment_date state preupdate_full_name preupdate_street_address preupdate_payment_date preupdate_payment_amount\n",
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

	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	payment_date = argv[ 4 ];
	state = argv[ 5 ];
	preupdate_full_name = argv[ 6 ];
	preupdate_street_address = argv[ 7 ];
	preupdate_payment_date = argv[ 8 ];
	preupdate_payment_amount = argv[ 9 ];

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	/* If changing full_name or street_address only. */
	/* --------------------------------------------- */
	if ( strcmp( payment_date, "payment_date" ) == 0 ) exit( 0 );

	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_dues_payment_insert(
				application_name,
				full_name,
				street_address,
				payment_date );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_dues_payment_update(
				application_name,
				full_name,
				street_address,
				payment_date,
				preupdate_full_name,
				preupdate_street_address,
				preupdate_payment_date,
				preupdate_payment_amount );
	}
	else
	if ( strcmp( state, "predelete" ) == 0 )
	{
		post_change_dues_payment_delete(
				application_name,
				full_name,
				street_address,
				payment_date );
	}

	return 0;

} /* main() */

void post_change_dues_payment_insert(
				char *application_name,
				char *full_name,
				char *street_address,
				char *payment_date )
{
	DUES_PAYMENT *dues_payment;
	APPLICATION_CONSTANTS *application_constants;

	application_constants = application_constants_new();

	application_constants->dictionary =
		application_constants_get_dictionary(
			application_name );

	if ( !( dues_payment =
			dues_payment_fetch(
				application_name,
				full_name,
				street_address,
				payment_date ) ) )
	{
		return;
	}

	dues_payment->payment_amount =
		atof( application_constants_safe_fetch(
			application_constants->dictionary,
			"dues_payment_amount" ) );

	dues_payment->transaction_date_time =
		ledger_get_transaction_date_time(
			dues_payment->payment_date );

	dues_payment->transaction =
		ledger_transaction_new(
			full_name,
			street_address,
			dues_payment->transaction_date_time,
			DUES_PAYMENT_MEMO );
	
	ledger_transaction_insert(
		application_name,
		dues_payment->transaction->full_name,
		dues_payment->transaction->street_address,
		dues_payment->transaction->transaction_date_time,
		dues_payment->payment_amount /* transaction_amount */,
		dues_payment->transaction->memo,
		dues_payment->check_number,
		1 /* lock_transaction */ );

	dues_payment_update(
			application_name,
			dues_payment->full_name,
			dues_payment->street_address,
			dues_payment->payment_date,
			dues_payment->payment_amount,
			dues_payment->database_payment_amount,
			dues_payment->transaction_date_time,
			dues_payment->database_transaction_date_time );

	dues_payment_journal_ledger_refresh(
		application_name,
		dues_payment->transaction->full_name,
		dues_payment->transaction->street_address,
		dues_payment->transaction->transaction_date_time,
		dues_payment->payment_amount );

	dues_payment_journal_ledger_propagate(
		application_name,
		dues_payment->transaction_date_time );

	post_change_set_status_active(
		application_name,
		dues_payment->full_name,
		dues_payment->street_address );

} /* post_change_dues_payment_insert() */

void post_change_dues_payment_delete(
			char *application_name,
			char *full_name,
			char *street_address,
			char *payment_date )
{
	DUES_PAYMENT *dues_payment;

	if ( !( dues_payment =
			dues_payment_fetch(
				application_name,
				full_name,
				street_address,
				payment_date ) ) )
	{
		return;
	}

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			dues_payment->transaction_date_time );

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			full_name,
			street_address,
			dues_payment->transaction_date_time );

	dues_payment_journal_ledger_propagate(
			application_name,
			dues_payment->transaction_date_time );

} /* post_change_dues_payment_delete() */

void post_change_dues_payment_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *payment_date,
				char *preupdate_full_name,
				char *preupdate_street_address,
				char *preupdate_payment_date,
				char *preupdate_payment_amount )
{
	enum preupdate_change_state full_name_change_state;
	enum preupdate_change_state street_address_change_state;
	enum preupdate_change_state payment_date_change_state;
	enum preupdate_change_state payment_amount_change_state;
	DUES_PAYMENT *dues_payment;

	full_name_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_full_name,
			full_name /* postupdate_data */,
			"preupdate_full_name" );

	street_address_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_street_address,
			street_address /* postupdate_data */,
			"preupdate_street_address" );

	payment_date_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_payment_date,
			payment_date /* postupdate_data */,
			"preupdate_payment_date" );

	payment_amount_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_payment_amount,
			(char *)0 /* postupdate_data */,
			"preupdate_payment_amount" );

	if ( full_name_change_state == from_something_to_something_else
	||   street_address_change_state == from_something_to_something_else )
	{
		post_change_dues_payment_entity(
			application_name,
			full_name,
			street_address,
			payment_date,
			preupdate_full_name,
			preupdate_street_address );
	}

	if (	payment_date_change_state !=
		from_something_to_something_else
	&&  	payment_amount_change_state !=
		from_something_to_something_else )
	{
		return;
	}

	dues_payment =
		dues_payment_fetch(
			application_name,
			full_name,
			street_address,
			payment_date );

	if ( !dues_payment )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find dues_payment.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( !dues_payment->transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty transaction for dues_payment.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( payment_date_change_state == from_something_to_something_else )
	{
		post_change_dues_payment_date_update(
				dues_payment,
				preupdate_payment_date,
				application_name );
	}

	if ( payment_amount_change_state == from_something_to_something_else )
	{
		post_change_dues_payment_amount_update(
				dues_payment,
				application_name );
	}

} /* post_change_dues_payment_update() */

void post_change_dues_payment_date_update(
				DUES_PAYMENT *dues_payment,
				char *preupdate_payment_date,
				char *application_name )
{
	char *propagate_transaction_date_time;

	dues_payment->transaction_date_time =
		ledger_get_transaction_date_time(
			dues_payment->payment_date );

	ledger_transaction_generic_update(
		application_name,
		dues_payment->full_name,
		dues_payment->street_address,
		dues_payment->transaction->transaction_date_time,
		"transaction_date_time" /* attribute */,
		dues_payment->transaction_date_time /* data */ );

	ledger_journal_generic_update(
		application_name,
		dues_payment->full_name,
		dues_payment->street_address,
		dues_payment->transaction->transaction_date_time,
		"transaction_date_time" /* attribute */,
		dues_payment->transaction_date_time /* data */ );

	dues_payment_update(
			application_name,
			dues_payment->full_name,
			dues_payment->street_address,
			dues_payment->payment_date,
			dues_payment->payment_amount,
			dues_payment->database_payment_amount,
			dues_payment->transaction_date_time,
			dues_payment->database_transaction_date_time );

	if ( strcmp(	preupdate_payment_date,
			dues_payment->payment_date ) < 0 )
	{
		propagate_transaction_date_time =
			dues_payment->transaction->transaction_date_time;
	}
	else
	{
		propagate_transaction_date_time =
			dues_payment->transaction_date_time;
	}

	dues_payment_journal_ledger_propagate(
			application_name,
			propagate_transaction_date_time );

} /* post_change_dues_payment_date_update() */

void post_change_dues_payment_amount_update(
				DUES_PAYMENT *dues_payment,
				char *application_name )
{
	dues_payment_journal_ledger_refresh(
		application_name,
		dues_payment->transaction->full_name,
		dues_payment->transaction->street_address,
		dues_payment->transaction->transaction_date_time,
		dues_payment->payment_amount );

	dues_payment_journal_ledger_propagate(
		application_name,
		dues_payment->transaction_date_time );

	ledger_transaction_amount_update(
		application_name,
		dues_payment->full_name,
		dues_payment->street_address,
		dues_payment->transaction_date_time,
		dues_payment->payment_amount,
		dues_payment->database_payment_amount );

} /* post_change_dues_payment_amount_update() */

void post_change_set_status_active(
				char *application_name,
				char *full_name,
				char *street_address )
{
	char sys_string[ 1024 ];
	char *table_name;
	char *key;
	FILE *output_pipe;

	table_name = get_table_name( application_name, "band_member" );
	key = "full_name,street_address";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y	|"
		 "sql.e						 ",
		 table_name,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
		 "%s^%s^status^%s\n",
		 full_name,
		 street_address,
		 STATUS_ACTIVE );

	pclose( output_pipe );

} /* post_change_set_status_active() */

void post_change_dues_payment_entity(
			char *application_name,
			char *full_name,
			char *street_address,
			char *payment_date,
			char *preupdate_full_name,
			char *preupdate_street_address )
{
	DUES_PAYMENT *dues_payment;

	fprintf( stderr,
		 "Warning in %s/%s()/%d: ignore following warning.\n",
		 __FILE__,
		 __FUNCTION__,
		 __LINE__ );

	dues_payment =
		dues_payment_fetch(
			application_name,
			full_name,
			street_address,
			payment_date );

	if ( !dues_payment )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find dues_payment.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( !dues_payment->transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty transaction for dues_payment.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	ledger_entity_update(
		application_name,
		full_name,
		street_address,
		dues_payment->transaction->transaction_date_time,
		preupdate_full_name,
		preupdate_street_address );

} /* post_change_dues_payment_entity() */

