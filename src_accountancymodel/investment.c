/* ---------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/investment.c	*/
/* ---------------------------------------------------- */
/* This is the appaserver investment ADT.		*/
/*							*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "piece.h"
#include "investment.h"

ACCOUNT_BALANCE *investment_account_balance_new(
					char *full_name,
					char *street_address,
					char *account_number,
					char *date )
{
	ACCOUNT_BALANCE *t;

	if ( ! ( t = calloc( 1, sizeof( ACCOUNT_BALANCE ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	t->full_name = full_name;
	t->street_address = street_address;
	t->account_number = account_number;
	t->date = date;
	return t;

} /* investment_account_balance_new() */

ACCOUNT_BALANCE *investment_account_balance_fetch(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date )
{
	char sys_string[ 1024 ];
	char where[ 256 ];
	char buffer[ 128 ];
	char *select;
	char *folder;
	ACCOUNT_BALANCE *account_balance;
	char *results;

	select =
	"investment_operation,balance,balance_change,transaction_date_time";

	folder = "account_balance";

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "account_number = '%s' and		"
		 "date = '%s'				",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 account_number,
		 date );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
		return (ACCOUNT_BALANCE *)0;

	account_balance =
		investment_account_balance_new(
			full_name,
			street_address,
			account_number,
			date );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 0 );
	account_balance->investment_operation = strdup( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 1 );
	account_balance->balance = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 2 );
	account_balance->balance_change = atof( buffer );

	piece( buffer, FOLDER_DATA_DELIMITER, results, 3 );
	account_balance->transaction_date_time = strdup( buffer );

	return account_balance;

} /* investment_account_balance_fetch() */

char *investment_account_balance_fetch_prior_date(
					char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date )
{
	char buffer[ 128 ];
	char sys_string[ 1024 ];
	char where[ 256 ];
	char *select;
	char *folder;

	select = "max( date )";

	folder = "account_balance";

	sprintf( where,
		 "full_name = '%s' and			"
		 "street_address = '%s' and		"
		 "account_number = '%s' and		"
		 "date < '%s'				",
		 escape_character(	buffer,
					full_name,
					'\'' ),
		 street_address,
		 account_number,
		 date );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder,
		 where );

	return pipe2string( sys_string );

} /* investment_account_balance_fetch_prior_date() */

void investment_account_balance_update(	char *application_name,
					char *full_name,
					char *street_address,
					char *account_number,
					char *date,
					double balance_change,
					char *transaction_date_time )
{
} /* investment_account_balance_update() */

/* Returns transaction_date_time */
/* ----------------------------- */
char *investment_time_passage_transaction_insert(
					char *application_name,
					char *full_name,
					char *street_address,
					char *date,
					double balance_change,
					char *investment_account,
					char *unrealized_gain,
					char *unrealized_loss )
{
	char *transaction_date_time = {0};

	return transaction_date_time;

} /* investment_time_passage_transaction_insert() */

