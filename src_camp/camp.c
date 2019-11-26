/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_camp/camp.c					*/
/* -------------------------------------------------------------------- */
/* This is the PredictiveBooks CAMP ADT.				*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <string.h>
#include "timlib.h"
#include "appaserver_library.h"
#include "piece.h"
#include "camp.h"

CAMP *camp_new(		char *camp_begin_date,
			char *camp_title )
{
	CAMP *c;

	if ( ! ( c = calloc( 1, sizeof( CAMP ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	c->camp_begin_date = camp_begin_date;
	c->camp_title = camp_title;

	return c;

} /* camp_new() */

ENROLLMENT *camp_enrollment_new(
				char *full_name,
				char *street_address )
{
	ENROLLMENT *c;

	if ( ! ( c = calloc( 1, sizeof( ENROLLMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	c->full_name = full_name;
	c->street_address = street_address;

	return c;

} /* camp_enrollment_new() */

ENROLLMENT_PAYMENT *camp_enrollment_payment_new(
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address,
				char *payment_date_time )
{
	ENROLLMENT_PAYMENT *c;

	if ( ! ( c = calloc( 1, sizeof( ENROLLMENT_PAYMENT ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	c->camp_begin_date = camp_begin_date;
	c->camp_title = camp_title;
	c->full_name = full_name;
	c->street_address = street_address;
	c->payment_date_time = payment_date_time;

	return c;

} /* camp_enrollment_payment_new() */

TRANSACTION *camp_enrollment_transaction(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				double enrollment_cost )
{
	char *checking_account = {0};
	char *receivable_account = {0};
	char *revenue_account = {0};

	ledger_order_entry_account_names(
				&checking_account,
				&receivable_account,
				&revenue_account,
				application_name,
				fund_name );

	return ledger_binary_transaction(
				full_name,
				street_address,
				ledger_get_transaction_date_time(
					(char *)0 /* transaction_date */ ),
				receivable_account /* debit_account */,
				revenue_account /* credit_account */,
				enrollment_cost /* transaction_amount */,
				(char *)0 /* memo */ );

} /* camp_enrollment_transaction() */

TRANSACTION *camp_enrollment_payment_transaction(
				char *application_name,
				char *fund_name,
				char *full_name,
				char *street_address,
				double payment_amount )
{
	char *checking_account = {0};
	char *receivable_account = {0};
	char *revenue_account = {0};

	ledger_order_entry_account_names(
				&checking_account,
				&receivable_account,
				&revenue_account,
				application_name,
				fund_name );

	return ledger_binary_transaction(
				full_name,
				street_address,
				ledger_get_transaction_date_time(
					(char *)0 /* transaction_date */ ),
				checking_account /* debit_account */,
				receivable_account /* credit_account */,
				payment_amount /* transaction_amount */,
				(char *)0 /* memo */ );

} /* camp_enrollment_payment_transaction() */

ENROLLMENT *camp_enrollment_fetch(
				char *application_name,
				char *camp_begin_date,
				char *camp_title,
				char *full_name,
				char *street_address )
{
	char sys_string[ 1024 ];
	char *select;
	char where[ 512 ];
	char buffer1[ 256 ];
	char buffer2[ 256 ];
	ENROLLMENT *e;
	char *results;

	sprintf( where,
		 "camp_begin_date = '%s' and	"
		 "camp_title = '%s' and		"
		 "full_name = '%s' and		"
		 "street_address = '%s'		",
		 camp_begin_date,
		 escape_character(	buffer1,
					camp_title,
					'\'' ),
		 escape_character(	buffer2,
					full_name,
					'\'' ),
		 street_address );

	select = "amount_due,total_payment,transaction_date_time";

	sprintf( sys_string,
		 "get_folder_data	application=%s		"
		 "			select=%s		"
		 "			folder=enrollment	"
		 "			where=\"%s\"		",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot fetch where = [%s].\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 where );
		exit( 1 );
	}

	e = camp_enrollment_new(
		full_name,
		street_address );

	piece( buffer1, FOLDER_DATA_DELIMITER, results, 0 );
	e->camp_enrollment_amount_due = atof( buffer1 );

	piece( buffer1, FOLDER_DATA_DELIMITER, results, 1 );
	e->camp_enrollment_total_payment_amount = atof( buffer1 );

	piece( buffer1, FOLDER_DATA_DELIMITER, results, 2 );
	if ( *buffer1 )
	{
		e->camp_enrollment_transaction =
			ledger_transaction_fetch(
				application_name,
				full_name,
				street_address,
				buffer1 /* transaction_date_time */ );
	}

	e->camp_enrollment_payment_list =
		camp_enrollment_payment_list(
			application_name,
			camp_begin_date,
			camp_title,
			e->full_name,
			e->street_address );

	return e;

} /* camp_enrollment_fetch() */

LIST *camp_enrollment_payment_list(
			char *application_name,
			char *camp_begin_date,
			char *camp_title,
			char *full_name,
			char *street_address )
{
	char sys_string[ 1024 ];
	char *select;
	char where[ 512 ];
	char buffer1[ 256 ];
	char buffer2[ 256 ];
	ENROLLMENT_PAYMENT *e;
	FILE *input_pipe;
	char input_buffer[ 512 ];
	LIST *payment_list = list_new();

	sprintf( where,
		 "camp_begin_date = '%s' and	"
		 "camp_title = '%s' and		"
		 "full_name = '%s' and		"
		 "street_address = '%s'		",
		 camp_begin_date,
		 escape_character(	buffer1,
					camp_title,
					'\'' ),
		 escape_character(	buffer2,
					full_name,
					'\'' ),
		 street_address );

	select = "payment_date_time,payment_amount,transaction_date_time";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=enrollment_payment	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );


	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( buffer1, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		e = camp_enrollment_payment_new(
			camp_begin_date,
			camp_title,
			full_name,
			street_address,
			strdup( buffer1 )
				/* payment_date_time */ );

		piece( buffer1, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		e->payment_amount = atof( buffer1 );

		piece( buffer1, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		if ( *buffer1 )
		{
			e->camp_enrollment_payment_transaction =
				ledger_transaction_fetch(
					application_name,
					full_name,
					street_address,
					buffer1 /* transaction_date_time */ );
		}

		list_append_pointer( payment_list, e );
	}

	pclose( input_pipe );
	return payment_list;

} /* camp_enrollment_payment_list() */

void camp_enrollment_payment_update(
			char *camp_begin_date,
			char *camp_title,
			char *full_name,
			char *street_address,
			char *payment_date_time,
			char *transaction_date_time )
{

	FILE *update_pipe;
	char sys_string[ 1024 ];
	char *key_column_list_string;

	key_column_list_string =
	"camp_begin_date,camp_title,full_name,street_address,payment_date_time";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 "enrollment_payment",
		 key_column_list_string );

	update_pipe = popen( sys_string, "w" );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^%s^transaction_date_time^%s\n",
		camp_begin_date,
		camp_title,
	 	full_name,
	 	street_address,
		payment_date_time,
		(transaction_date_time) ? transaction_date_time
					: "" );

	pclose( update_pipe );

} /* camp_enrollment_payment_update() */

void camp_enrollment_update(
			char *camp_begin_date,
			char *camp_title,
			char *full_name,
			char *street_address,
			double amount_due,
			double total_payment,
			char *transaction_date_time )
{

	FILE *update_pipe;
	char sys_string[ 1024 ];
	char *key_column_list_string;

	key_column_list_string =
		"camp_begin_date,camp_title,full_name,street_address";

	sprintf( sys_string,
		 "update_statement.e table=%s key=%s carrot=y		|"
		 "sql.e							 ",
		 "enrollment",
		 key_column_list_string );

	update_pipe = popen( sys_string, "w" );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^amount_due^%.2lf\n",
		camp_begin_date,
		camp_title,
	 	full_name,
	 	street_address,
		amount_due );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^total_payment^%.2lf\n",
		camp_begin_date,
		camp_title,
	 	full_name,
	 	street_address,
		total_payment );

	fprintf(update_pipe,
	 	"%s^%s^%s^%s^transaction_date_time^%s\n",
		camp_begin_date,
		camp_title,
	 	full_name,
	 	street_address,
		(transaction_date_time) ? transaction_date_time
					: "" );

	pclose( update_pipe );

} /* camp_enrollment_update() */

double camp_enrollment_total_payment_amount(
			LIST *camp_enrollment_payment_list )
{
	ENROLLMENT_PAYMENT *e;
	LIST *l;
	double total_payment_amount;

	l = camp_enrollment_payment_list;

	if ( !list_rewind( l ) ) return 0.0;

	total_payment_amount = 0.0;

	do {
		e = list_get( l );

		total_payment_amount += e->payment_amount;

	} while ( list_next( l ) );

	return total_payment_amount;

} /* camp_enrollment_total_payment_amount() */

double camp_enrollment_amount_due(
			double camp_enrollment_total_payment_amount,
			double enrollment_cost )
{
	return enrollment_cost - camp_enrollment_total_payment_amount;
}
