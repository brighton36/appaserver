/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/bank_upload.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef BANK_UPLOAD_H
#define BANK_UPLOAD_H

#include "ledger.h"
#include "reoccurring.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define INSERT_BANK_UPLOAD		\
	"bank_date,bank_description,sequence_number,bank_amount"

#define INSERT_BANK_UPLOAD_FUND		\
	"bank_date,bank_description,sequence_number,bank_amount,fund"

#define INSERT_BANK_UPLOAD_ARCHIVE	\
	"bank_date,bank_description,sequence_number,bank_amount,bank_running_balance"

#define INSERT_BANK_UPLOAD_ARCHIVE_FUND	\
	"bank_date,bank_description,sequence_number,bank_amount,bank_running_balance,fund"

/* Structures */
/* ---------- */
typedef struct
{
	char *bank_date;
	char *bank_description;
	int sequence_number;
	double bank_amount;
	double bank_running_balance;
	char *fund_name;
	TRANSACTION *transaction;
} BANK_UPLOAD;

typedef struct
{
	/* Gets populated from input_filename */
	/* ---------------------------------- */
	LIST *bank_upload_file_list;
	char *input_filename;
	char *minimum_bank_date;
	int file_row_count;
	int table_insert_count;
	int date_piece_offset;
	int description_piece_offset;
	int debit_piece_offset;
	int credit_piece_offset;
	int balance_piece_offset;
} BANK_UPLOAD_FILE;

typedef struct
{
	/* Gets populated from BANK_UPLOAD */
	/* ------------------------------- */
	LIST *bank_upload_table_list;
} BANK_UPLOAD_TABLE;

typedef struct
{
	BANK_UPLOAD_FILE file;
	BANK_UPLOAD_TABLE table;
	LIST *existing_cash_journal_ledger_list;
	REOCCURRING_STRUCTURE *reoccurring_structure;
	int starting_sequence_number;
	char *fund_name;
} BANK_UPLOAD_STRUCTURE;

/* Operations */
/* ---------- */
BANK_UPLOAD_STRUCTURE *bank_upload_structure_new(
					char *application_name,
					char *fund_name,
					char *input_filename,
					int date_piece_offset,
					int description_piece_offset,
					int debit_piece_offset,
					int credit_piece_offset,
					int balance_piece_offset );

BANK_UPLOAD *bank_upload_calloc(	void );

BANK_UPLOAD *bank_upload_new(		char *bank_date,
					char *bank_description );

#ifdef NOT_DEFINED
/* Returns table_insert_count */
/* -------------------------- */
int bank_upload_table_insert(		FILE *input_file,
					char **minimum_bank_date,
					char *application_name,
					char *fund_name,
					int date_piece_offset,
					int description_piece_offset,
					int debit_piece_offset,
					int credit_piece_offset,
					int balance_piece_offset,
					boolean execute,
					int starting_sequence_number );
#endif

/* Returns table_insert_count */
/* -------------------------- */
int bank_upload_insert(			char *application_name,
					LIST *bank_upload_list,
					char *fund_name );

int bank_upload_get_sequence_number(
					char *application_name,
					char *input_filename );

int bank_upload_get_line_count(		char *input_filename,
					int date_piece_offset );

boolean bank_upload_get_bank_date_international(
					char *bank_date_international,
					char *bank_date );

LIST *bank_upload_fetch_list(		char *application_name,
					int starting_sequence_number );

BANK_UPLOAD *bank_upload_fetch(		char *application_name,
					char *bank_date,
					char *bank_description );

LIST *bank_upload_fetch_existing_cash_journal_ledger_list(
					char *application_name,
					char *minimum_bank_date,
					char *fund_name );

void bank_upload_set_transaction(
				LIST *bank_upload_list,
				LIST *reoccurring_transaction_list,
				LIST *existing_cash_journal_ledger_list );

void bank_upload_insert_transaction(
					char *application_name,
					LIST *bank_upload_list );

char *bank_upload_get_select(		void );

void bank_upload_fetch_parse(		char **bank_date,
					char **bank_description,
					int *sequence_number,
					double *bank_amount,
					double *bank_running_balance,
					char *input_buffer );

void bank_upload_transaction_display(	LIST *bank_upload_list );

int bank_upload_get_starting_sequence_number(
					char *application_name,
					char *input_filename,
					int date_piece_offset );

BANK_UPLOAD *bank_upload_dictionary_extract(
					char *application_name,
					DICTIONARY *dictionary );

LIST *bank_upload_fetch_file_list(
					char **minimum_bank_date,
					char *application_name,
					char *input_filename,
					int date_piece_offset,
					int description_piece_offset,
					int debit_piece_offset,
					int credit_piece_offset,
					int balance_piece_offset,
					int starting_sequence_number,
					char *fund_name );
#endif

