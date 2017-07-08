/* -----------------------------------------------------------------	*/
/* $APPASERVER_HOME/src_accountancymodel/post_change_bank_upload.c	*/
/* -----------------------------------------------------------------	*/
/* 									*/
/* Freely available software: see Appaserver.org			*/
/* -----------------------------------------------------------------	*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "timlib.h"
#include "environ.h"
#include "piece.h"
#include "list.h"
#include "appaserver_library.h"
#include "appaserver_error.h"
#include "dictionary.h"

/* Constants */
/* --------- */

/* Structures */
/* ---------- */
typedef struct
{
	char *bank_date;
	char *bank_description;
	double bank_amount;
	double bank_running_balance;
	boolean touched;
} BANK_UPLOAD;

/* Prototypes */
/* ---------- */
void bank_upload_list_set(	LIST *bank_upload_list );

void bank_upload_list_update(	LIST *bank_upload_list );

void append_untouched_table_list(
				LIST *bank_upload_list,
				LIST *table_list );

BANK_UPLOAD *bank_upload_seek(
				LIST *table_list,
				char *dictionary_bank_date,
				char *dictionary_bank_description );

LIST *get_untouched_bank_upload_list(
				LIST *table_list,
				char *first_bank_date,
				char *first_bank_description );

BANK_UPLOAD *bank_upload_new(
				char *bank_date,
				char *bank_description );

LIST *bank_upload_get_dictionary_list(
				DICTIONARY *dictionary );

LIST *bank_upload_get_table_list(
				char *application_name,
				char *starting_bank_date,
				char *fund_name );

LIST *bank_upload_merge_bank_upload_list(
				LIST *dictionary_list,
				LIST *table_list );

boolean post_change_bank_upload_update(
				char *application_name,
				DICTIONARY *dictionary );

char *get_fund_name(
				char *application_name,
				char *bank_date,
				char *bank_description );

int get_bank_balance_sequence_number(
				char *application_name,
				char *bank_date,
				char *fund_name );

FILE *post_change_bank_upload_open_input_pipe(
				char *application_name,
				int bank_balance_sequence_number,
				char *fund_name );

FILE *post_change_bank_upload_open_output_pipe(
				void );

int main( int argc, char **argv )
{
	char *database_string = {0};
	char *application_name;
	DICTIONARY *dictionary;

	appaserver_error_output_starting_argv_stderr( argc, argv );

	if ( argc != 3 )
	{
		fprintf( stderr,
		"Usage: %s application dictionary\n",
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
	else
	{
		environ_set_environment(
			APPASERVER_DATABASE_ENVIRONMENT_VARIABLE,
			application_name );
	}

	dictionary =
		dictionary_string2dictionary(
			argv[ 2 ] );

	if ( !post_change_bank_upload_update(
			application_name,
			dictionary ) )
	{
		fprintf( stderr, "Error occurred.\n" );
	}

	return 0;

} /* main() */

boolean post_change_bank_upload_update(
			char *application_name,
			DICTIONARY *dictionary )
{
	LIST *dictionary_list;
	LIST *table_list;
	LIST *bank_upload_list;
	char *fund_name;
	BANK_UPLOAD *starting_bank_upload;

	dictionary_list = bank_upload_get_dictionary_list( dictionary );

	if ( !list_length( dictionary_list ) ) return 1;

	starting_bank_upload = list_get_first_pointer( dictionary_list );

	if ( ! ( fund_name = get_fund_name(
				application_name,
				starting_bank_upload->bank_date,
				starting_bank_upload->bank_description ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get fund_name for (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 starting_bank_upload->bank_date,
			 starting_bank_upload->bank_description );
		return 0;
	}

	table_list =
		bank_upload_get_table_list(
			application_name,
			starting_bank_upload->bank_date,
			fund_name );

	bank_upload_list =
		bank_upload_merge_bank_upload_list(
			dictionary_list,
			table_list );

	if ( !list_length( bank_upload_list ) ) return 1;

	bank_upload_list_set( bank_upload_list );
	bank_upload_list_update( bank_upload_list );

	return 1;

} /* post_change_bank_upload_update() */

LIST *bank_upload_merge_bank_upload_list(
			LIST *dictionary_list,
			LIST *table_list )
{
	LIST *bank_upload_list;
	BANK_UPLOAD *first_dictionary_bank_upload;
	BANK_UPLOAD *dictionary_bank_upload;
	BANK_UPLOAD *bank_upload;

	if ( !list_length( dictionary_list )
	||   !list_length( table_list ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: empty list: dictionary_list = %d, table_list = %d.\n",
		 	 __FILE__,
		 	 __FUNCTION__,
		 	 __LINE__,
	 		 list_length( dictionary_list ),
			 list_length( table_list ) );

		return (LIST *)0;
	}

	first_dictionary_bank_upload =
		list_get_first_pointer( dictionary_list );

	bank_upload_list =
		get_untouched_bank_upload_list(
			table_list,
			first_dictionary_bank_upload->bank_date,
			first_dictionary_bank_upload->bank_description );

	list_rewind( dictionary_list );

	do {
		dictionary_bank_upload =
			list_get_pointer( dictionary_list );

		bank_upload =
			bank_upload_seek(
				table_list,
				dictionary_bank_upload->bank_date,
				dictionary_bank_upload->bank_description );

		if ( !bank_upload ) continue;

		bank_upload->touched = 1;
		list_append_pointer( bank_upload_list, bank_upload );

	} while( list_next( dictionary_list ) );

	append_untouched_table_list(
		bank_upload_list,
		table_list );

	return bank_upload_list;

} /* bank_upload_merge_bank_upload_list() */

LIST *get_untouched_bank_upload_list(
			LIST *table_list,
			char *first_bank_date,
			char *first_bank_description )
{
	BANK_UPLOAD *bank_upload;
	LIST *bank_upload_list;

	bank_upload_list = list_new();

	if ( !list_rewind( table_list ) ) return bank_upload_list;

	do {
		bank_upload = list_get_pointer( table_list );

		bank_upload->touched = 1;
		list_append_pointer(
			bank_upload_list,
			bank_upload );

		if ( strcmp(	bank_upload->bank_date,
				first_bank_date ) == 0
		&&   strcmp(	bank_upload->bank_description,
				first_bank_description ) == 0 )
		{
			return bank_upload_list;
		}

	} while( list_next( table_list ) );

	return bank_upload_list;

} /* get_untouched_bank_upload_list() */

BANK_UPLOAD *bank_upload_new(
				char *bank_date,
				char *bank_description )
{
	BANK_UPLOAD *b;

	if ( ! ( b = (BANK_UPLOAD *)calloc( 1, sizeof( BANK_UPLOAD ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	b->bank_date = bank_date;
	b->bank_description = bank_description;
	return b;

} /* bank_upload_new() */

LIST *bank_upload_get_dictionary_list(
				DICTIONARY *dictionary )
{
	LIST *dictionary_list;
	BANK_UPLOAD *bank_upload;
	char *bank_date;
	char *bank_description;
	char key[ 128 ];
	int index;

	index = 1;
	dictionary_list = list_new();

	while( 1 )
	{
		sprintf( key, "%s_%d", "bank_date", index );

		if ( ! ( bank_date = dictionary_fetch( dictionary, key ) ) )
		{
			break;
		}

		sprintf( key, "%s_%d", "bank_description", index );

		if ( ! ( bank_description =
				dictionary_fetch( dictionary, key ) ) )
		{
			break;
		}

		bank_upload =
			bank_upload_new(
				bank_date,
				bank_description );

		list_append_pointer( dictionary_list, bank_upload );

		index++;
	}

	return dictionary_list;

} /* bank_upload_get_dictionary_list() */

LIST *bank_upload_get_table_list(
				char *application_name,
				char *starting_bank_date,
				char *fund_name )
{
	int bank_balance_sequence_number;
	FILE *input_pipe;
	char input_bank_date[ 32 ];
	char input_bank_description[ 512 ];
	char input_bank_amount[ 32 ];
	/* char input_sequence_number[ 32 ]; */
	char input_bank_running_balance[ 32 ];
	char input_buffer[ 1024 ];
	BANK_UPLOAD *bank_upload;
	LIST *table_list;

	bank_balance_sequence_number =
		get_bank_balance_sequence_number(
			application_name,
			starting_bank_date,
			fund_name );

	if ( !bank_balance_sequence_number )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get starting sequence number.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	table_list = list_new();

	input_pipe =
		post_change_bank_upload_open_input_pipe(
			application_name,
			bank_balance_sequence_number,
			fund_name );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece(	input_bank_date,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			0 );

		piece(	input_bank_description,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			1 );

		bank_upload =
			bank_upload_new(
				strdup( input_bank_date ),
				strdup( input_bank_description ) );

		piece(	input_bank_amount,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );
		bank_upload->bank_amount = atof( input_bank_amount );

/*
		piece(	input_sequence_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );
		bank_upload->sequence_number = atoi( input_sequence_number );
*/

		piece(	input_bank_running_balance,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );
		bank_upload->bank_running_balance =
			atof( input_bank_running_balance );

		list_append_pointer( table_list, bank_upload );
	}

	pclose( input_pipe );
	return table_list;

} /* bank_upload_get_table_list() */

char *get_fund_name(
			char *application_name,
			char *bank_date,
			char *bank_description )
{
	char where[ 512 ];
	char buffer[ 512 ];
	char *folder_name;
	char *select;
	char sys_string[ 1024 ];

	select = "fund";
	folder_name = "bank_upload";

	timlib_strcpy( buffer, bank_description, 512 );

	sprintf(	where,
			"bank_date = '%s' and bank_description = '%s'",
			bank_date,
			timlib_escape_field( buffer ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=%s			"
		 "			where=\"%s\"			",
		 application_name,
		 select,
		 folder_name,
		 where );

	return pipe2string( sys_string );

} /* get_fund_name() */

int get_bank_balance_sequence_number(
				char *application_name,
				char *starting_bank_date,
				char *fund_name )
{
	char where[ 512 ];
	char *folder_name;
	char *select;
	char sys_string[ 1024 ];
	char *results;

	select = "max(sequence_number)";
	folder_name = "bank_upload";

	if ( fund_name )
	{
		sprintf(where,
		"bank_amount is null and fund = '%s' and bank_date <= '%s'",
			fund_name,
			starting_bank_date );
	}
	else
	{
		sprintf(where,
		"bank_amount is null and bank_date <= %s",
			starting_bank_date );
	}

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=\"%s\"	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder_name,
		 where );

	results = pipe2string( sys_string );

	if ( !results || !*results ) return 0;

	return atoi( results );

} /* get_bank_balance_sequence_number() */

FILE *post_change_bank_upload_open_input_pipe(
			char *application_name,
			int bank_balance_sequence_number,
			char *fund_name )
{
	char where[ 512 ];
	char *folder_name;
	char *select;
	char *order;
	char sys_string[ 1024 ];

	select =
"bank_date,bank_description,bank_amount,sequence_number,bank_running_balance";
	folder_name = "bank_upload";

	if ( fund_name )
	{
		sprintf(	where,
				"fund = '%s' and sequence_number >= %d",
				fund_name,
				bank_balance_sequence_number );
	}
	else
	{
		sprintf(	where,
				"sequence_number >= %d",
				bank_balance_sequence_number );
	}

	order = "sequence_number";

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	"
		 "			order=%s	",
		 application_name,
		 select,
		 folder_name,
		 where,
		 order );

	return popen( sys_string, "r" );

} /* post_change_bank_upload_open_input_pipe() */

BANK_UPLOAD *bank_upload_seek(
				LIST *table_list,
				char *dictionary_bank_date,
				char *dictionary_bank_description )
{
	BANK_UPLOAD *bank_upload;

	list_rewind( table_list );

	do {
		bank_upload = list_get_pointer( table_list );

		if ( !bank_upload->touched
		&&   strcmp(	bank_upload->bank_date,
				dictionary_bank_date ) == 0
		&&   strcmp(	bank_upload->bank_description,
				dictionary_bank_description ) == 0 )
		{
			return bank_upload;
		}
	} while( list_next( table_list ) );

	return (BANK_UPLOAD *)0;

} /* bank_upload_seek() */

void append_untouched_table_list(
			LIST *bank_upload_list,
			LIST *table_list )
{
	BANK_UPLOAD *bank_upload;

	if ( list_at_tail( table_list ) ) return;

	do {
		bank_upload = list_get_pointer( table_list );

		if ( !bank_upload->touched )
		{
			bank_upload->touched = 1;

			list_append_pointer(
				bank_upload_list,
				bank_upload );;
		}
	} while( list_next( table_list ) );

} /* append_untouched_table_list() */

void bank_upload_list_set( LIST *bank_upload_list )
{
	BANK_UPLOAD *bank_upload;
	double prior_bank_running_balance = {0};
	/* int sequence_number = {0}; */

	if ( !list_rewind( bank_upload_list ) ) return;

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		if ( list_at_first( bank_upload_list ) )
		{
			if ( bank_upload->bank_amount )
			{
				fprintf( stderr,
		"ERROR in %s/%s()/%d: expecting Bank balance in (%s/%s).\n",
				 	__FILE__,
				 	__FUNCTION__,
				 	__LINE__,
				 	bank_upload->bank_date,
				 	bank_upload->bank_description );
				exit( 1 );
			}

			prior_bank_running_balance =
				bank_upload->bank_running_balance;

			/* sequence_number = bank_upload->sequence_number; */

			continue;
		}

		if ( !bank_upload->touched )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: not touched (%s/%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 bank_upload->bank_date,
				 bank_upload->bank_description );
			exit( 1 );
		}

		/* Skip Beginning balance records */
		/* ------------------------------ */
		if ( !bank_upload->bank_amount )
		{
			/* sequence_number++; */
			continue;
		}

		bank_upload->bank_running_balance =
			prior_bank_running_balance +
			bank_upload->bank_amount;

		prior_bank_running_balance =
			bank_upload->bank_running_balance;

/*
		bank_upload->sequence_number = ++sequence_number;
*/

	} while( list_next( bank_upload_list ) );

} /* bank_upload_list_set() */

FILE *post_change_bank_upload_open_output_pipe( void )
{
	char sys_string[ 1024 ];
	char *key = "bank_date,bank_description";

	sprintf( sys_string,
		 "update_statement.e	table=bank_upload	 "
		 "			key=%s			 "
		 "			carrot=y		|"
		 "sql.e						 ",
		 key );

	return popen( sys_string, "w" );

} /* post_change_bank_upload_open_output_pipe() */

void bank_upload_list_update(	LIST *bank_upload_list )
{
	FILE *output_pipe;
	BANK_UPLOAD *bank_upload;

	if ( !list_rewind( bank_upload_list ) ) return;

	output_pipe = post_change_bank_upload_open_output_pipe();

	do {
		bank_upload = list_get_pointer( bank_upload_list );

		if ( bank_upload->bank_amount )
		{
			fprintf( output_pipe,
			 	"%s^%s^bank_running_balance^%.2lf\n",
			 	bank_upload->bank_date,
			 	bank_upload->bank_description,
			 	bank_upload->bank_running_balance );
		}

	} while( list_next( bank_upload_list ) );

	pclose( output_pipe );

} /* bank_upload_list_update() */

