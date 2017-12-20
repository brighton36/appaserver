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
	LIST *attribute_list;

	p = subsidiary_calloc();

	/* Input */
	/* ===== */
	p->input.folder_name = folder_name;
	p->input.primary_data_list = primary_data_list;
	p->input.full_name = full_name;
	p->input.street_address = street_address;

	/* Process */
	/* ======= */
	if ( !subsidiary_transaction_fetch(
		&p->process.attribute_name,
		&p->process.debit_account_name,
		&p->process.credit_account_name,
		&p->process.debit_account_folder,
		application_name,
		p->input.folder_name ) )
	{	
		return ( SUBSIDIARY_TRANSACTION *)0;
	}

	attribute_list =
		folder_get_attribute_list(
			application_name,
			folder_name );

	p->process.primary_attribute_name_list =
		folder_get_primary_attribute_name_list(
			attribute_list );

	/* Output */
	/* ------ */

	return p;

} /* subsidiary_new() */

boolean subsidiary_transaction_fetch(
				char **attribute_name,
				char **debit_account_name,
				char **credit_account_name,
				FOLDER **debit_account_folder,
				char *application_name,
				char *folder_name )
{
	char *debit_account_folder_name;
	char *select;
	char where[ 128 ];
	char sys_string[ 1024 ];
	char *results;
	char piece_buffer[ 128 ];

	select =
	"attribute,debit_account,debit_account_folder,credit_account";

	sprintf( where,
		 "folder = '%s'",
		 folder_name );

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

	if ( *piece_buffer )
	{
		*debit_account_name = strdup( piece_buffer );
	}
	else
	{
		FOLDER *folder;

		piece( piece_buffer, FOLDER_DATA_DELIMITER, results, 2 );

		if ( !*piece_buffer ) return 0;

		folder =
			folder_new(	application_name,
					BOGUS_SESSION,
					strdup( piece_buffer ) );

		if ( !folder ) return 0;

		*debit_account_folder = folder;

	}

	return 1;

}  /* subsidiary_transaction_fetch() */

