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

/* Prototypes */
/* ---------- */
double get_starting_bank_running_balance(
				char *application_name,
				char *bank_date,
				char *bank_description );

FILE *post_change_bank_download_open_output_pipe(
				void );

FILE *post_change_bank_download_open_input_pipe(
				char *application_name,
				char *sequence_number );

char *get_sequence_number(	char *application_name,
				char *bank_date,
				char *bank_description );

void post_change_bank_download_update(
				char *application_name,
				char *bank_date,
				char *bank_description );

int main( int argc, char **argv )
{
	char *database_string = {0};
	char *application_name;
	DICTIONARY *dictionary;
	char bank_date[ 16 ];
	char bank_description[ 128 ];
	char key[ 16 ];
	char *data;

	appaserver_error_output_starting_argv_stderr(
				argc,
				argv );

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

	sprintf( key, "%s_1", SORT_ORDER_ATTRIBUTE_NAME );
	data = dictionary_fetch( dictionary, key );

	if ( data )
	{
		piece(	bank_date,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			data,
			0 );

		piece(	bank_description,
			MULTI_ATTRIBUTE_DROP_DOWN_DELIMITER,
			data,
			1 );

		post_change_bank_download_update(
			application_name,
			bank_date,
			bank_description );
	}

	return 0;

} /* main() */

void post_change_bank_download_update(
			char *application_name,
			char *bank_date,
			char *bank_description )
{
	char *sequence_number;
	FILE *input_pipe;
	FILE *output_pipe;
	char input_bank_date[ 16 ];
	char input_bank_description[ 128 ];
	char input_bank_amount[ 16 ];
	double bank_running_balance;
	char input_buffer[ 512 ];

	sequence_number =
		get_sequence_number(
			application_name,
			bank_date,
			bank_description );

	bank_running_balance =
		get_starting_bank_running_balance(
			application_name,
			bank_date,
			bank_description );

	input_pipe =
		post_change_bank_download_open_input_pipe(
			application_name,
			sequence_number );

	output_pipe = post_change_bank_download_open_output_pipe();

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

		piece(	input_bank_amount,
			FOLDER_DATA_DELIMITER,
			input_buffer,
			2 );

		bank_running_balance += atof( input_bank_amount );

		fprintf( output_pipe,
			 "%s^%s^bank_running_balance^%.2lf\n",
			 input_bank_date,
			 input_bank_description,
			 bank_running_balance );
	}

	pclose( input_pipe );
	pclose( output_pipe );

} /* post_change_bank_download_update() */

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

FILE *post_change_bank_download_open_input_pipe(
			char *application_name,
			char *sequence_number )
{
	char where[ 512 ];
	char *folder_name;
	char *select;
	char *order;
	char sys_string[ 1024 ];

	select = "bank_date,bank_description,bank_amount";
	folder_name = "bank_download";

	sprintf(	where,
			"sequence_number >= %s",
			sequence_number );

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

char *get_sequence_number(	char *application_name,
				char *bank_date,
				char *bank_description )
{
	char where[ 512 ];
	char buffer[ 512 ];
	char *folder_name;
	char *select;
	char sys_string[ 1024 ];

	select = "sequence_number";
	folder_name = "bank_download";

	timlib_strcpy( buffer, bank_description, 512 );

	sprintf(	where,
			"bank_date = '%s' and bank_description = '%s'",
			bank_date,
			timlib_escape_field( buffer ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder_name,
		 where );

	return pipe2string( sys_string );

} /* get_sequence_number() */

double get_starting_bank_running_balance(
			char *application_name,
			char *bank_date,
			char *bank_description )
{
	char where[ 512 ];
	char buffer[ 512 ];
	char *folder_name;
	char *select;
	char *results;
	char bank_amount[ 16 ];
	char bank_running_balance[ 16 ];
	char sys_string[ 1024 ];
	double starting_bank_running_balance;

	select = "bank_amount,bank_running_balance";
	folder_name = "bank_download";

	timlib_strcpy( buffer, bank_description, 512 );

	sprintf(	where,
			"bank_date = '%s' and bank_description = '%s'",
			bank_date,
			timlib_escape_field( buffer ) );

	sprintf( sys_string,
		 "get_folder_data	application=%s	"
		 "			select=%s	"
		 "			folder=%s	"
		 "			where=\"%s\"	",
		 application_name,
		 select,
		 folder_name,
		 where );

	if ( ! ( results = pipe2string( sys_string ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot fetch (%s/%s)\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 bank_date,
			 bank_description );
		exit( 1 );
	}

	piece(	bank_amount,
		FOLDER_DATA_DELIMITER,
		results,
		0 );

	piece(	bank_running_balance,
		FOLDER_DATA_DELIMITER,
		results,
		1 );

	starting_bank_running_balance =
		atof( bank_running_balance ) -
		atof( bank_amount );

	return starting_bank_running_balance;

} /* get_starting_bank_running_balance() */

