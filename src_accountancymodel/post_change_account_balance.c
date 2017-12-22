/* --------------------------------------------------------------------	 */
/* $APPASERVER_HOME/src_accountancymodel/post_change_account_balance.c   */
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
#define PRIOR_FIXED_ASSET_FOLDER_NAME	"account_balance"

/* Prototypes */
/* ---------- */
void post_change_account_balance_update(
				char *application_name,
				char *asset_name,
				char *serial_number,
				char *preupdate_extension );

void post_change_prior_fixed_extension_update(
				char *application_name,
				char *asset_name,
				char *serial_number );

void post_change_account_balance_fetch_row(
				char **full_name,
				char **street_address,
				char **transaction_date_time,
				double *extension,
				char *application_name,
				char *asset_name,
				char *serial_number );

void post_change_account_balance_delete(
				char *application_name,
				char *asset_name,
				char *serial_number );

LIST *post_change_account_balance_get_primary_data_list(
				char *asset_name,
				char *serial_number );

void post_change_account_balance_insert(
				char *application_name,
				char *asset_name,
				char *serial_number );

void account_balance_transaction_date_time_update(
				char *application_name,
				char *asset_name,
				char *serial_number,
				char *transaction_date_time,
				char *full_name,
				char *street_address );

int main( int argc, char **argv )
{
	char *application_name;
	char *full_name;
	char *street_address;
	char *account_number;
	char *date;
	char *state;
	char *preupdate_balance;
	char *database_string = {0};

	if ( argc != 8 )
	{
		fprintf( stderr,
"Usage: %s application full_name street_address account_number date state preupdate_balance\n",
			 argv[ 0 ] );
		exit ( 1 );
	}

	application_name = argv[ 1 ];
	full_name = argv[ 2 ];
	street_address = argv[ 3 ];
	account_number = argv[ 4 ];
	date = argv[ 5 ];
	state = argv[ 6 ];
	preupdate_balance = argv[ 7 ];

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
		post_change_account_balance_insert(
				application_name,
				asset_name,
				serial_number );
	}
	else
	if ( strcmp( state, "update" ) == 0 )
	{
		post_change_account_balance_update(
				application_name,
				asset_name,
				serial_number,
				preupdate_extension );
	}
	else
	if ( strcmp( state, "predelete" ) == 0 )
	/* -------------------------------------------- */
	/* Need to execute on predelete to get		*/
	/* PRIOR_FIXED_ASSET.transaction_date_time. 	*/
	/* -------------------------------------------- */
	{
		post_change_account_balance_delete(
				application_name,
				asset_name,
				serial_number );
	}

	return 0;

} /* main() */

void post_change_account_balance_update(
				char *application_name,
				char *asset_name,
				char *serial_number,
				char *preupdate_extension )
{
	enum preupdate_change_state extension_change_state;

	extension_change_state =
		appaserver_library_get_preupdate_change_state(
			preupdate_extension,
			(char *)0 /* postupdate_data */,
			"preupdate_extension" );

	if (	extension_change_state ==
		from_something_to_something_else )
	{
		post_change_prior_fixed_extension_update(
			application_name,
			asset_name,
			serial_number );
	}


} /* post_change_account_balance_update() */

void post_change_account_balance_insert(
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
		post_change_account_balance_get_primary_data_list(
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

	account_balance_transaction_date_time_update(
		application_name,
		asset_name,
		serial_number,
		transaction->transaction_date_time,
		transaction->full_name,
		transaction->street_address );

} /* post_change_account_balance_insert() */

void post_change_account_balance_delete(
			char *application_name,
			char *asset_name,
			char *serial_number )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
	LIST *primary_data_list;
	char *full_name = {0};
	char *street_address = {0};
	char *transaction_date_time = {0};
	double extension = {0};

	post_change_account_balance_fetch_row(
			&full_name,
			&street_address,
			&transaction_date_time,
			&extension,
			application_name,
			asset_name,
			serial_number );

	if (	!full_name
	||	!street_address
	||	!transaction_date_time
	||	!*transaction_date_time )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot post_change_account_balance_fetch_row(%s/%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 asset_name,
			 serial_number );
		exit( 1 );
	}

	primary_data_list =
		post_change_account_balance_get_primary_data_list(
			asset_name,
			serial_number );

	subsidiary_transaction =
		subsidiary_new(	application_name,
				PRIOR_FIXED_ASSET_FOLDER_NAME,
				primary_data_list,
				full_name,
				street_address,
				extension /* transaction_amount */ );

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

	if ( !subsidiary_transaction )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: subsidiary_transaction is null. Can't propagate ledger.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
	}
	else
	{
		ledger_propagate(
			application_name,
			transaction_date_time,
			subsidiary_transaction->process.debit_account_name );

		ledger_propagate(
			application_name,
			transaction_date_time,
			subsidiary_transaction->process.credit_account_name );
	}

} /* post_change_account_balance_delete() */

LIST *post_change_account_balance_get_primary_data_list(
				char *asset_name,
				char *serial_number )
{
	LIST *primary_data_list = list_new();
	list_append_pointer( primary_data_list, asset_name );
	list_append_pointer( primary_data_list, serial_number );

	return primary_data_list;

} /* post_change_account_balance_get_primary_data_list() */

void account_balance_transaction_date_time_update(
				char *application_name,
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

} /* account_balance_transaction_date_time_update() */

void post_change_account_balance_fetch_row(
			char **full_name,
			char **street_address,
			char **transaction_date_time,
			double *extension,
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

	select = "full_name,street_address,transaction_date_time,extension";

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

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 3 );
	*extension = atof( piece_buffer );

} /* post_change_account_balance_fetch_row() */

void post_change_prior_fixed_extension_update(
				char *application_name,
				char *asset_name,
				char *serial_number )
{
	SUBSIDIARY_TRANSACTION *subsidiary_transaction;
	LIST *primary_data_list;
	char *full_name = {0};
	char *street_address = {0};
	char *transaction_date_time = {0};
	double extension = {0};

	post_change_account_balance_fetch_row(
			&full_name,
			&street_address,
			&transaction_date_time,
			&extension,
			application_name,
			asset_name,
			serial_number );

	if (	!full_name
	||	!street_address
	||	!transaction_date_time
	||	!*transaction_date_time )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot post_change_account_balance_fetch_row(%s/%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 asset_name,
			 serial_number );
		exit( 1 );
	}

	primary_data_list =
		post_change_account_balance_get_primary_data_list(
			asset_name,
			serial_number );

	subsidiary_transaction =
		subsidiary_new(	application_name,
				PRIOR_FIXED_ASSET_FOLDER_NAME,
				primary_data_list,
				full_name,
				street_address,
				extension /* transaction_amount */ );

	if ( !subsidiary_transaction )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot build transaction for (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 full_name,
			 street_address );
		exit( 1 );
	}

	ledger_transaction_amount_update(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		extension,
		0.0 /* database_amount */ );

	ledger_debit_credit_update(
		application_name,
		full_name,
		street_address,
		transaction_date_time,
		subsidiary_transaction->process.debit_account_name,
		subsidiary_transaction->process.credit_account_name,
		extension /* transaction_amount  */ );

	ledger_propagate(
		application_name,
		transaction_date_time,
		subsidiary_transaction->process.debit_account_name );

	ledger_propagate(
		application_name,
		transaction_date_time,
		subsidiary_transaction->process.credit_account_name );

} /* post_change_account_balance_amount_update() */

