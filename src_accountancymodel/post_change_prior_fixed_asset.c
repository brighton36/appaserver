/* --------------------------------------------------------------------	 */
/* $APPASERVER_HOME/src_accountancymodel/post_change_prior_fixed_asset.c */
/* --------------------------------------------------------------------- */
/* 									 */
/* Freely available software: see Appaserver.org			 */
/* --------------------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "inventory.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "ledger.h"
#include "entity.h"
#include "subsidiary_transaction.h"

/* Constants */
/* --------- */
#define PRIOR_FIXED_ASSET_FOLDER_NAME	"prior_fixed_asset"

/* Prototypes */
/* ---------- */
/*
void post_change_prior_fixed_asset_amount_update(
				char *application_name,
				CUSTOMER_SALE *customer_sale,
				PRIOR_FIXED_ASSET *prior_fixed_asset );

void post_change_prior_fixed_asset_date_time_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time );

void post_change_prior_fixed_asset_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time,
				char *preupdate_payment_amount );

*/

void post_change_prior_fixed_asset_fetch_row(
				char **full_name,
				char **street_address,
				char **transaction_date_time,
				char *application_name,
				char *asset_name,
				char *serial_number );

void post_change_prior_fixed_asset_delete(
				char *application_name,
				char *asset_name,
				char *serial_number );

LIST *post_change_prior_fixed_asset_get_primary_data_list(
				char *asset_name,
				char *serial_number );

void post_change_prior_fixed_asset_insert(
				char *application_name,
				char *asset_name,
				char *serial_number );

void prior_fixed_asset_update(	char *application_name,
				char *asset_name,
				char *serial_number,
				char *transaction_date_time,
				char *full_name,
				char *street_address );

int main( int argc, char **argv )
{
	char *application_name;
	char *asset_name;
	char *serial_number;
	char *state;
	char *database_string = {0};

	if ( argc != 5 )
	{
		fprintf( stderr,
"Usage: %s application asset_name serial_number state\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	asset_name = argv[ 2 ];
	serial_number = argv[ 3 ];
	state = argv[ 4 ];

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

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

	if ( strcmp( state, "delete" ) == 0 ) exit( 0 );

	if ( strcmp( state, "insert" ) == 0 )
	{
		post_change_prior_fixed_asset_insert(
				application_name,
				asset_name,
				serial_number );
	}
#ifdef NOT_DEFINED
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_prior_fixed_asset_update(
				application_name,
				full_name,
				street_address,
				sale_date_time,
				payment_date_time,
				preupdate_payment_date_time,
				preupdate_payment_amount );
	}
#endif
	else
	if ( strcmp( state, "predelete" ) == 0 )
	/* -------------------------------------------- */
	/* Need to execute on predelete to get		*/
	/* PRIOR_FIXED_ASSET.transaction_date_time. 	*/
	/* -------------------------------------------- */
	{
		post_change_prior_fixed_asset_delete(
				application_name,
				asset_name,
				serial_number );
	}

	return 0;

} /* main() */

void post_change_prior_fixed_asset_insert(
				char *application_name,
				char *asset_name,
				char *serial_number )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
	LIST *primary_data_list;
	TRANSACTION *transaction;
	ENTITY_SELF *entity_self;

	entity_self = entity_self_load(	application_name );

	primary_data_list =
		post_change_prior_fixed_asset_get_primary_data_list(
			asset_name,
			serial_number );

	subsidiary_transaction =
		subsidiary_new(	application_name,
				PRIOR_FIXED_ASSET_FOLDER_NAME,
				primary_data_list,
				entity_self->entity->full_name,
				entity_self->entity->street_address,
				0.0 /* transaction_amount */ );

	if ( !subsidiary_transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot build transaction for (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 entity_self->entity->full_name,
			 entity_self->entity->street_address );
		exit( 1 );
	}

	subsidiary_transaction->output.transaction =
		subsidiary_get_transaction(
			subsidiary_transaction->input.full_name,
			subsidiary_transaction->input.street_address,
			subsidiary_transaction->process.debit_account_name,
			subsidiary_transaction->process.credit_account_name,
			subsidiary_transaction->process.transaction_amount,
			subsidiary_transaction->process.memo );

	if ( !subsidiary_transaction->output.transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot build transaction for (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 entity_self->entity->full_name,
			 entity_self->entity->street_address );
		exit( 1 );
	}

	transaction = subsidiary_transaction->output.transaction;

	transaction->transaction_date_time =
		ledger_transaction_journal_ledger_insert(
			application_name,
			transaction->full_name,
			transaction->street_address,
			transaction->transaction_date_time,
			transaction->transaction_amount,
			transaction->memo,
			transaction->check_number,
			transaction->lock_transaction,
			transaction->journal_ledger_list );

	prior_fixed_asset_update(
		application_name,
		asset_name,
		serial_number,
		transaction->transaction_date_time,
		transaction->full_name,
		transaction->street_address );

} /* post_change_prior_fixed_asset_insert() */

void post_change_prior_fixed_asset_delete(
			char *application_name,
			char *asset_name,
			char *serial_number )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
	LIST *primary_data_list;
	char *full_name = {0};
	char *street_address = {0};
	char *transaction_date_time = {0};

	post_change_prior_fixed_asset_fetch_row(
			&full_name,
			&street_address,
			&transaction_date_time,
			application_name,
			asset_name,
			serial_number );

	if ( !full_name || !street_address || !transaction_date_time )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot post_change_prior_fixed_asset_fetch_row(%s/%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 asset_name,
			 serial_number );
		exit( 1 );
	}

	primary_data_list =
		post_change_prior_fixed_asset_get_primary_data_list(
			asset_name,
			serial_number );

	subsidiary_transaction =
		subsidiary_new(	application_name,
				PRIOR_FIXED_ASSET_FOLDER_NAME,
				primary_data_list,
				full_name,
				street_address,
				0.0 /* transaction_amount */ );

	ledger_delete(	application_name,
			TRANSACTION_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	ledger_delete(	application_name,
			LEDGER_FOLDER_NAME,
			full_name,
			street_address,
			transaction_date_time );

	ledger_propagate(
		application_name,
		transaction_date_time,
		subsidiary_transaction->process.debit_account_name );

	ledger_propagate(
		application_name,
		transaction_date_time,
		subsidiary_transaction->process.credit_account_name );

} /* post_change_prior_fixed_asset_delete() */

#ifdef NOT_DEFINED
void post_change_prior_fixed_asset_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time,
				char *preupdate_payment_amount )
{
	enum preupdate_change_state payment_date_time_change_state;
	enum preupdate_change_state payment_amount_change_state;
	CUSTOMER_SALE *customer_sale;
	PRIOR_FIXED_ASSET *prior_fixed_asset;

	customer_sale =
		customer_sale_new(
			application_name,
			full_name,
			street_address,
			sale_date_time );

	if ( !customer_sale )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find customer sale.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( ! ( prior_fixed_asset =
			prior_fixed_asset_seek(
				customer_sale->payment_list,
				payment_date_time ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot find customer payment.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 0 );
	}

	if ( !prior_fixed_asset->transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot empty transaction for payment_date_time = (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 payment_date_time );
		exit( 1 );
	}

	customer_sale_update(
		customer_sale->sum_extension,
		customer_sale->database_sum_extension,
		customer_sale->sales_tax,
		customer_sale->database_sales_tax,
		customer_sale->invoice_amount,
		customer_sale->database_invoice_amount,
		customer_sale->completed_date_time,
		customer_sale->
			database_completed_date_time,
		customer_sale->shipped_date_time,
		customer_sale->database_shipped_date_time,
		customer_sale->arrived_date,
		customer_sale->database_arrived_date,
		customer_sale->total_payment,
		customer_sale->database_total_payment,
		customer_sale->amount_due,
		customer_sale->database_amount_due,
		customer_sale->transaction_date_time,
		customer_sale->
			database_transaction_date_time,
		customer_sale->full_name,
		customer_sale->street_address,
		customer_sale->sale_date_time,
		application_name );

	payment_date_time_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_payment_date_time,
			prior_fixed_asset->transaction_date_time
				/* postupdate_data */,
			"preupdate_payment_date_time" );

	payment_amount_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_payment_amount,
			(char *)0 /* postupdate_data */,
			"preupdate_payment_amount" );

	if (	payment_date_time_change_state ==
		from_something_to_something_else )
	{
		post_change_prior_fixed_asset_date_time_update(
			application_name,
			full_name,
			street_address,
			sale_date_time,
			payment_date_time,
			preupdate_payment_date_time );
	}

	if (	payment_amount_change_state ==
		from_something_to_something_else )
	{
		post_change_prior_fixed_asset_amount_update(
			application_name,
			customer_sale,
			prior_fixed_asset );
	}

	if ( prior_fixed_asset->check_number
	!=   prior_fixed_asset->transaction->check_number )
	{
		char check_number_string[ 16 ];

		sprintf( check_number_string,
			 "%d",
			 prior_fixed_asset->check_number );

		ledger_transaction_generic_update(
			application_name,
			customer_sale->full_name,
			customer_sale->street_address,
			prior_fixed_asset->transaction_date_time,
			"check_number" /* attribute_name */,
			check_number_string /* data */ );
	}

} /* post_change_prior_fixed_asset_update() */

void post_change_prior_fixed_asset_amount_update(
				char *application_name,
				CUSTOMER_SALE *customer_sale,
				PRIOR_FIXED_ASSET *prior_fixed_asset )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;

	subsidiary_transaction = subsidiary_transaction_calloc();

	if ( !subsidiary_transaction_fetch(
		&subsidiary_transaction->process.attribute_name,
		&subsidiary_transaction->process.debit_account_name,
		&subsidiary_transaction->process.credit_account_name,
		(char **)0 /* debit_account_folder_name */,
		application_name,
		PRIOR_FIXED_ASSET_FOLDER_NAME ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot subsidiary_transaction_fetch (%s).\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
		 	PRIOR_FIXED_ASSET_FOLDER_NAME );
		exit( 1 );
	}

	ledger_transaction_amount_update(
		application_name,
		customer_sale->full_name,
		customer_sale->street_address,
		prior_fixed_asset->transaction_date_time,
		prior_fixed_asset->payment_amount,
		0.0 /* database_payment_amount */ );

	ledger_debit_credit_update(
		application_name,
		customer_sale->full_name,
		customer_sale->street_address,
		prior_fixed_asset->transaction_date_time,
		subsidiary_transaction->process.debit_account_name,
		subsidiary_transaction->process.credit_account_name,
		prior_fixed_asset->payment_amount
			/* transaction_amount  */ );

	ledger_propagate(
		application_name,
		prior_fixed_asset->transaction_date_time,
		subsidiary_transaction->process.debit_account_name );

	ledger_propagate(
		application_name,
		prior_fixed_asset->transaction_date_time,
		subsidiary_transaction->process.credit_account_name );

} /* post_change_prior_fixed_asset_amount_update() */

void post_change_prior_fixed_asset_date_time_update(
				char *application_name,
				char *full_name,
				char *street_address,
				char *sale_date_time,
				char *payment_date_time,
				char *preupdate_payment_date_time )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
	char *propagate_transaction_date_time;

	ledger_transaction_generic_update(
		application_name,
		full_name,
		street_address,
		preupdate_payment_date_time,
		"transaction_date_time",
		payment_date_time );

	ledger_journal_generic_update(
		application_name,
		full_name,
		street_address,
		preupdate_payment_date_time,
		"transaction_date_time",
		payment_date_time );

	if ( strcmp(	preupdate_payment_date_time,
			payment_date_time ) < 0 )
	{
		propagate_transaction_date_time =
			preupdate_payment_date_time;
	}
	else
	{
		propagate_transaction_date_time =
			payment_date_time;
	}

	subsidiary_transaction = subsidiary_transaction_calloc();

	if ( !subsidiary_transaction_fetch(
		&subsidiary_transaction->process.attribute_name,
		&subsidiary_transaction->process.debit_account_name,
		&subsidiary_transaction->process.credit_account_name,
		(char **)0 /* debit_account_folder_name */,
		application_name,
		PRIOR_FIXED_ASSET_FOLDER_NAME ) )
	{
		fprintf( stderr,
	"ERROR in %s/%s()/%d: cannot subsidiary_transaction_fetch (%s).\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
		 	PRIOR_FIXED_ASSET_FOLDER_NAME );
		exit( 1 );
	}

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		subsidiary_transaction->process.debit_account_name );

	ledger_propagate(
		application_name,
		propagate_transaction_date_time,
		subsidiary_transaction->process.credit_account_name );

	prior_fixed_asset_update(
		application_name,
		full_name,
		street_address,
		sale_date_time,
		payment_date_time,
		payment_date_time
			/* transaction_date_time */,
		preupdate_payment_date_time
			/* database_transaction_date_time */ );

} /* post_change_prior_fixed_asset_date_time_update() */
#endif

LIST *post_change_prior_fixed_asset_get_primary_data_list(
				char *asset_name,
				char *serial_number )
{
	LIST *primary_data_list = list_new();
	list_append_pointer( primary_data_list, asset_name );
	list_append_pointer( primary_data_list, serial_number );

	return primary_data_list;

} /* post_change_prior_fixed_asset_get_primary_data_list() */

void prior_fixed_asset_update(	char *application_name,
				char *asset_name,
				char *serial_number,
				char *transaction_date_time,
				char *full_name,
				char *street_address )
{
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *table_name;
	char *key;
	char buffer1[ 128 ];
	char buffer2[ 128 ];

	if ( !transaction_date_time || !*transaction_date_time ) return;

	key = "asset_name,serial_number";

	table_name =
		get_table_name(
			application_name,
			PRIOR_FIXED_ASSET_FOLDER_NAME );

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s	|"
		 "sql.e					 ",
		 table_name,
		 key );

	output_pipe = popen( sys_string, "w" );

	fprintf( output_pipe,
	 "%s^%s|full_name=%s|street_address=%s|transaction_date_time=%s\n",
	 	escape_character(	buffer1,
					asset_name,
					'\'' ),
	 	serial_number,
	 	escape_character(	buffer2,
					full_name,
					'\'' ),
		street_address,
	 	transaction_date_time );

	pclose( output_pipe );

} /* prior_fixed_asset_update() */

void post_change_prior_fixed_asset_fetch_row(
			char **full_name,
			char **street_address,
			char **transaction_date_time,
			char *application_name,
			char *asset_name,
			char *serial_number )
{
	char sys_string[ 1024 ];
	char where[ 512 ];
	char buffer[ 128 ];
	char *results;
	char *select;
	char piece_buffer[ 128 ];

	select = "full_name,street_address,transaction_date_time";

	sprintf( where,
		 "asset_name = '%s' and serial_number = '%s'",
	 	 escape_character(	buffer,
					asset_name,
					'\'' ),
		 serial_number );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 PRIOR_FIXED_ASSET_FOLDER_NAME,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*full_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*street_address = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );
	*transaction_date_time = strdup( piece_buffer );

} /* post_change_prior_fixed_asset_fetch_row() */

