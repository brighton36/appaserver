/* ---------------------------------------------------------------	*/
/* src_accountancymodel/post_change_bank_download.c			*/
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
	int sequence_number;
	double bank_amount;
	double bank_running_balance;
	boolean touched;
} BANK_DOWNLOAD;

/* Prototypes */
/* ---------- */
void bank_download_list_set(	LIST *bank_download_list );

void bank_download_list_update(	LIST *bank_download_list );

void append_untouched_table_list(
				LIST *bank_download_list,
				LIST *table_list );

BANK_DOWNLOAD *bank_download_seek(
				LIST *table_list,
				char *dictionary_bank_date,
				char *dictionary_bank_description );

LIST *get_untouched_bank_download_list(
				LIST *table_list,
				char *first_bank_date,
				char *first_bank_description );

BANK_DOWNLOAD *bank_download_new(
				char *bank_date,
				char *bank_description );

LIST *bank_download_get_dictionary_list(
				DICTIONARY *dictionary );

LIST *bank_download_get_table_list(
				char *application_name,
				char *starting_bank_date,
				char *fund_name );

LIST *bank_download_merge_bank_download_list(
				LIST *dictionary_list,
				LIST *table_list );

boolean post_change_bank_download_update(
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

FILE *post_change_bank_download_open_input_pipe(
				char *application_name,
				int bank_balance_sequence_number,
				char *fund_name );

FILE *post_change_bank_download_open_output_pipe(
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

	if ( !post_change_bank_download_update(
			application_name,
			dictionary ) )
	{
		fprintf( stderr, "Error occurred.\n" );
	}

	return 0;

} /* main() */

boolean post_change_bank_download_update(
			char *application_name,
			DICTIONARY *dictionary )
{
	LIST *dictionary_list;
	LIST *table_list;
	LIST *bank_download_list;
	char *fund_name;
	BANK_DOWNLOAD *starting_bank_download;

	dictionary_list = bank_download_get_dictionary_list( dictionary );

	if ( !list_length( dictionary_list ) ) return 1;

	starting_bank_download = list_get_first_pointer( dictionary_list );

	if ( ! ( fund_name = get_fund_name(
				application_name,
				starting_bank_download->bank_date,
				starting_bank_download->bank_description ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get fund_name for (%s/%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 starting_bank_download->bank_date,
			 starting_bank_download->bank_description );
		return 0;
	}

	table_list =
		bank_download_get_table_list(
			application_name,
			starting_bank_download->bank_date,
			fund_name );

	bank_download_list =
		bank_download_merge_bank_download_list(
			dictionary_list,
			table_list );

	if ( !list_length( bank_download_list ) ) return 1;

	bank_download_list_set( bank_download_list );
	bank_download_list_update( bank_download_list );

	return 1;

} /* post_change_bank_download_update() */

LIST *bank_download_merge_bank_download_list(
			LIST *dictionary_list,
			LIST *table_list )
{
	LIST *bank_download_list;
	BANK_DOWNLOAD *first_dictionary_bank_download;
	BANK_DOWNLOAD *dictionary_bank_download;
	BANK_DOWNLOAD *bank_download;

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

	first_dictionary_bank_download =
		list_get_first_pointer( dictionary_list );

	bank_download_list =
		get_untouched_bank_download_list(
			table_list,
			first_dictionary_bank_download->bank_date,
			first_dictionary_bank_download->bank_description );

	list_rewind( dictionary_list );

	do {
		dictionary_bank_download =
			list_get_pointer( dictionary_list );

		bank_download =
			bank_download_seek(
				table_list,
				dictionary_bank_download->bank_date,
				dictionary_bank_download->bank_description );

		if ( !bank_download )
		{
			fprintf( stderr,
			"Warning in %s/%s()/%d: cannot seek (%s -- %s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 	dictionary_bank_download->bank_date,
			 	dictionary_bank_download->bank_description );

			continue;
		}

		bank_download->touched = 1;
		list_append_pointer( bank_download_list, bank_download );

	} while( list_next( dictionary_list ) );

	append_untouched_table_list(
		bank_download_list,
		table_list );

	return bank_download_list;

} /* bank_download_merge_bank_download_list() */

LIST *get_untouched_bank_download_list(
			LIST *table_list,
			char *first_bank_date,
			char *first_bank_description )
{
	BANK_DOWNLOAD *bank_download;
	LIST *bank_download_list;

	bank_download_list = list_new();

	if ( !list_rewind( table_list ) ) return bank_download_list;

	do {
		bank_download = list_get_pointer( table_list );

		bank_download->touched = 1;
		list_append_pointer(
			bank_download_list,
			bank_download );

		if ( strcmp(	bank_download->bank_date,
				first_bank_date ) == 0
		&&   strcmp(	bank_download->bank_description,
				first_bank_description ) == 0 )
		{
			return bank_download_list;
		}

	} while( list_next( table_list ) );

	return bank_download_list;

} /* get_untouched_bank_download_list() */

BANK_DOWNLOAD *bank_download_new(
				char *bank_date,
				char *bank_description )
{
	BANK_DOWNLOAD *b;

	if ( ! ( b = (BANK_DOWNLOAD *)calloc( 1, sizeof( BANK_DOWNLOAD ) ) ) )
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

} /* bank_download_new() */

LIST *bank_download_get_dictionary_list(
				DICTIONARY *dictionary )
{
	LIST *dictionary_list;
	BANK_DOWNLOAD *bank_download;
	char bank_date[ 16 ];
	char bank_description[ 512 ];
	char key[ 16 ];
	char *data;
	int index;

	index = 1;
	dictionary_list = list_new();

	while( 1 )
	{
		sprintf( key, "%s_%d", SORT_ORDER_ATTRIBUTE_NAME, index );

		if ( ! ( data = dictionary_fetch( dictionary, key ) ) )
			break;

		piece(	bank_date,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			data,
			0 );

		piece(	bank_description,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			data,
			1 );

		bank_download =
			bank_download_new(
				strdup( bank_date ),
				strdup( bank_description ) );

		list_append_pointer( dictionary_list, bank_download );
		index++;
	}

	return dictionary_list;

} /* bank_download_get_dictionary_list() */

LIST *bank_download_get_table_list(
				char *application_name,
				char *starting_bank_date,
				char *fund_name )
{
	int bank_balance_sequence_number;
	FILE *input_pipe;
	char input_bank_date[ 32 ];
	char input_bank_description[ 512 ];
	char input_bank_amount[ 32 ];
	char input_sequence_number[ 32 ];
	char input_bank_running_balance[ 32 ];
	char input_buffer[ 1024 ];
	BANK_DOWNLOAD *bank_download;
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
		post_change_bank_download_open_input_pipe(
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

		bank_download =
			bank_download_new(
				strdup( input_bank_date ),
				strdup( input_bank_description ) );

		piece(	input_bank_amount,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );
		bank_download->bank_amount = atof( input_bank_amount );

		piece(	input_sequence_number,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			3 );
		bank_download->sequence_number = atoi( input_sequence_number );

		piece(	input_bank_running_balance,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			4 );
		bank_download->bank_running_balance =
			atof( input_bank_running_balance );

		list_append_pointer( table_list, bank_download );
	}

	pclose( input_pipe );
	return table_list;

} /* bank_download_get_table_list() */

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
	folder_name = "bank_download";

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
	folder_name = "bank_download";

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

FILE *post_change_bank_download_open_input_pipe(
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
	folder_name = "bank_download";

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

} /* post_change_bank_download_open_input_pipe() */

BANK_DOWNLOAD *bank_download_seek(
				LIST *table_list,
				char *dictionary_bank_date,
				char *dictionary_bank_description )
{
	BANK_DOWNLOAD *bank_download;

	list_rewind( table_list );

	do {
		bank_download = list_get_pointer( table_list );

		if ( !bank_download->touched
		&&   strcmp(	bank_download->bank_date,
				dictionary_bank_date ) == 0
		&&   strcmp(	bank_download->bank_description,
				dictionary_bank_description ) == 0 )
		{
			return bank_download;
		}
	} while( list_next( table_list ) );

	return (BANK_DOWNLOAD *)0;

} /* bank_download_seek() */

void append_untouched_table_list(
			LIST *bank_download_list,
			LIST *table_list )
{
	BANK_DOWNLOAD *bank_download;

	if ( list_at_tail( table_list ) ) return;

	do {
		bank_download = list_get_pointer( table_list );

		if ( !bank_download->touched )
		{
			bank_download->touched = 1;

			list_append_pointer(
				bank_download_list,
				bank_download );;
		}
	} while( list_next( table_list ) );

} /* append_untouched_table_list() */

void bank_download_list_set( LIST *bank_download_list )
{
	BANK_DOWNLOAD *bank_download;
	double prior_bank_running_balance = {0};
	int sequence_number = {0};

	if ( !list_rewind( bank_download_list ) ) return;

	do {
		bank_download = list_get_pointer( bank_download_list );

		if ( list_at_first( bank_download_list ) )
		{
			if ( bank_download->bank_amount )
			{
				fprintf( stderr,
		"ERROR in %s/%s()/%d: expecting Bank balance in (%s/%s).\n",
				 	__FILE__,
				 	__FUNCTION__,
				 	__LINE__,
				 	bank_download->bank_date,
				 	bank_download->bank_description );
				exit( 1 );
			}

			prior_bank_running_balance =
				bank_download->bank_running_balance;

			sequence_number =
				bank_download->sequence_number;

			continue;
		}

		if ( !bank_download->touched )
		{
			fprintf( stderr,
				 "ERROR in %s/%s()/%d: not touched (%s/%s).\n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 bank_download->bank_date,
				 bank_download->bank_description );
			exit( 1 );
		}

		/* Skip Beginning balance records */
		/* ------------------------------ */
		if ( !bank_download->bank_amount )
		{
			sequence_number++;
			continue;
		}

		bank_download->bank_running_balance =
			prior_bank_running_balance +
			bank_download->bank_amount;

		prior_bank_running_balance =
			bank_download->bank_running_balance;

		bank_download->sequence_number = ++sequence_number;

	} while( list_next( bank_download_list ) );

} /* bank_download_list_set() */

FILE *post_change_bank_download_open_output_pipe( void )
{
	char sys_string[ 1024 ];
	char *key = "bank_date,bank_description";

	sprintf( sys_string,
		 "update_statement.e	table=bank_download	 "
		 "			key=%s			 "
		 "			carrot=y		|"
		 "sql.e						 ",
		 key );

	return popen( sys_string, "w" );

} /* post_change_bank_download_open_output_pipe() */

void bank_download_list_update(	LIST *bank_download_list )
{
	FILE *output_pipe;
	BANK_DOWNLOAD *bank_download;

	if ( !list_rewind( bank_download_list ) ) return;

	output_pipe = post_change_bank_download_open_output_pipe();

	do {
		bank_download = list_get_pointer( bank_download_list );

		if ( bank_download->bank_amount )
		{
			fprintf( output_pipe,
			 	"%s^%s^bank_running_balance^%.2lf\n",
			 	bank_download->bank_date,
			 	bank_download->bank_description,
			 	bank_download->bank_running_balance );

			fprintf( output_pipe,
			 	"%s^%s^sequence_number^%d\n",
			 	bank_download->bank_date,
			 	bank_download->bank_description,
			 	bank_download->sequence_number );
		}

	} while( list_next( bank_download_list ) );

	pclose( output_pipe );

} /* bank_download_list_update() */

