/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_accountancymodel/subsidiary_transaction.c	*/
/* -------------------------------------------------------------------- */
/* This is the appaserver subsidiary_transaction ADT.			*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "boolean.h"
#include "query.h"
#include "subsidiary_transaction.h"

SUBSIDIARY_TRANSACTION *subsidiary_calloc( void )
{
	SUBSIDIARY_TRANSACTION *p;

	if ( ! ( p = (SUBSIDIARY_TRANSACTION *)
			calloc( 1, sizeof( SUBSIDIARY_TRANSACTION ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	return p;

} /* subsidiary_calloc() */

SUBSIDIARY_TRANSACTION *subsidiary_new(	char *application_name,
					char *folder_name,
					LIST *primary_data_list,
					char *full_name,
					char *street_address )
{
	SUBSIDIARY_TRANSACTION *p;
	char *debit_account_folder_name = {0};

	p = subsidiary_calloc();

	/* Input */
	/* ===== */
	p->input.folder_name = folder_name;
	p->input.primary_data_list = primary_data_list;
	p->input.full_name = full_name;
	p->input.street_address = street_address;

	/* Process */
	/* ======= */
	p->process.subsidiary_transaction_folder =
		folder_new(	application_name,
				BOGUS_SESSION,
				p->input.folder_name );

	p->process.subsidiary_transaction_folder->attribute_list =
		folder_get_attribute_list(
			application_name,
			p->process.subsidiary_transaction_folder->folder_name );

	p->process.subsidiary_transaction_folder->primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			p->process.
				subsidiary_transaction_folder->
				attribute_list );

	p->process.subsidiary_transaction_folder->primary_data_list =
		primary_data_list;

	if ( !subsidiary_transaction_fetch(
		&p->process.attribute_name,
		&p->process.credit_account_name,
		&p->process.debit_account_name,
		&debit_account_folder_name,
		application_name,
		p->input.folder_name ) )
	{	
		return ( SUBSIDIARY_TRANSACTION *)0;
	}

	if ( p->process.debit_account_name ) goto set_transaction_amount;

	if ( !debit_account_folder_name )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: empty debit_account_folder_name.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );

		return ( SUBSIDIARY_TRANSACTION *)0;
	}

	p->process.debit_account_name =
		subsidiary_process_fetch_debit_account_name(
			application_name,
			p->process.debit_account_folder->folder_name,
			list_get_first_pointer(
				p->process.debit_account_folder->
					primary_attribute_name_list ),
			p->process.subsidiary_transaction_folder->
				folder_name,
			p->process.subsidiary_transaction_folder->
				primary_attribute_name_list,
			p->process.subsidiary_transaction_folder->
				primary_data_list );

	if ( !p->process.debit_account_name )
	{
		fprintf( stderr,
		 "ERROR in %s/%s()/%d: empty debit_account_name.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__ );

		return (SUBSIDIARY_TRANSACTION *)0;
	}

set_transaction_amount:

	p->process.transaction_amount =
		subsidiary_fetch_transaction_amount(
			application_name,
			p->process.subsidiary_transaction_folder->folder_name,
			p->process.attribute_name,
			p->process.subsidiary_transaction_folder->
				primary_attribute_name_list,
			p->process.subsidiary_transaction_folder->
				primary_data_list );

	if ( timlib_dollar_virtually_same(
		p->process.transaction_amount, 0.0 ) )
	{
		return (SUBSIDIARY_TRANSACTION *)0;
	}

	/* Output */
	/* ------ */

	return p;

} /* subsidiary_new() */

boolean subsidiary_transaction_fetch(
				char **attribute_name,
				char **credit_account_name,
				char **debit_account_name,
				char **debit_account_folder_name,
				char *application_name,
				char *input_folder_name )
{
	char *select;
	char where[ 128 ];
	char sys_string[ 1024 ];
	char *results;
	char piece_buffer[ 128 ];

	select =
	"attribute,credit_account,debit_account,debit_account_folder";

	sprintf( where,
		 "folder = '%s'",
		 input_folder_name );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=subsidiary_transaction	"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0;

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 0 );
	*attribute_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 1 );
	*credit_account_name = strdup( piece_buffer );

	piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );

	if ( *piece_buffer )
	{
		*debit_account_name = strdup( piece_buffer );
	}
	else
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );

		if ( !*piece_buffer ) return 0;

		*debit_account_folder_name = strdup( piece_buffer );
	}

	return 1;

}  /* subsidiary_transaction_fetch() */

char *subsidiary_process_fetch_debit_account_name(
		char *application_name,
		char *debit_account_folder_name,
		char *debit_account_folder_primary_attribute_name,
		char *subsidiary_transaction_folder_name,
		LIST *subsidiary_transaction_folder_primary_attribute_name_list,
		LIST *subsidiary_transaction_folder_primary_data_list )
{
	char where_clause[ 256 ];
	char *data;
	char sys_string[ 1024 ];

	strcpy( where_clause,
		query_get_simple_where_clause(
		      (FOLDER *)0,
		      subsidiary_transaction_folder_primary_attribute_name_list,
		      subsidiary_transaction_folder_primary_data_list,
		      (char *)0 /* login_name */ ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 debit_account_folder_primary_attribute_name,
		 subsidiary_transaction_folder_name,
		 where_clause );

	if ( ! ( data = pipe2string( sys_string ) ) ) return (char *)0;

	sprintf( where_clause,
		 "%s = '%s'",
		 debit_account_folder_primary_attribute_name,
		 data );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=account	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 debit_account_folder_name,
		 where_clause );

	return pipe2string( sys_string );

} /* subsidiary_process_fetch_debit_account_name() */

double subsidiary_fetch_transaction_amount(
			char *application_name,
			char *subsidiary_transaction_folder_name,
			char *subsidiary_transaction_attribute_name,
			LIST *primary_attribute_name_list,
			LIST *primary_data_list )
{
	char sys_string[ 1024 ];
	char *where_clause;
	char *results;

	where_clause =
		query_get_simple_where_clause(
		      (FOLDER *)0,
		      primary_attribute_name_list,
		      primary_data_list,
		      (char *)0 /* login_name */ );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 subsidiary_transaction_attribute_name,
		 subsidiary_transaction_folder_name,
		 where_clause );

	if ( ! ( results = pipe2string( sys_string ) ) ) return 0.0;

	return atof( results );

} /* subsidiary_fetch_transaction_amount() */

