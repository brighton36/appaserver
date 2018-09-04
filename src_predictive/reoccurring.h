/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/reoccurring.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef REOCCURRING_STRUCTURE_H
#define REOCCURRING_STRUCTURE_H

#include "ledger.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define INSERT_REOCCURRING_STRUCTURE		\
	"bank_date,bank_description,sequence_number,bank_amount"

#define INSERT_REOCCURRING_STRUCTURE_FUND		\
	"bank_date,bank_description,sequence_number,bank_amount,fund"

#define INSERT_REOCCURRING_STRUCTURE_ARCHIVE	\
	"bank_date,bank_description,sequence_number,bank_amount,bank_running_balance"

#define INSERT_REOCCURRING_STRUCTURE_ARCHIVE_FUND	\
	"bank_date,bank_description,sequence_number,bank_amount,bank_running_balance,fund"

#define REOCCURRING_TRANSACTION_FOLDER_NAME \
					"reoccurring_transaction"

/* Structures */
/* ---------- */
typedef struct
{
	char *full_name;
	char *street_address;
	char *transaction_description;
	char *debit_account;
	char *credit_account;
	double transaction_amount;
	char *bank_upload_search_phrase;
	double accrued_daily_amount;
	double accrued_monthly_amount;
	char *rental_property_street_address;
} REOCCURRING_TRANSACTION;

typedef struct
{
	LIST *reoccurring_transaction_list;
} REOCCURRING_STRUCTURE;

typedef struct
{
	char *fund_name;
	char *input_filename;
	int starting_sequence_number;
	int file_record_count;
	int table_insert_count;
	char *minimum_bank_date;
	LIST *reoccurring_list;
	LIST *existing_cash_journal_ledger_list;
	LIST *reoccurring_transaction_list;
} REOCCURRING_STRUCTURE_STRUCTURE;

/* Operations */
/* ---------- */
REOCCURRING_STRUCTURE_STRUCTURE *reoccurring_structure_new(
					void );

REOCCURRING_TRANSACTION *reoccurring_reoccurring_transaction_calloc(
					void );

REOCCURRING_TRANSACTION *reoccurring_reoccurring_transaction_new(
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_description,
					double transaction_amount );

boolean reoccurring_reoccurring_transaction_load(
					char **debit_account,
					char **credit_account,
					double *transaction_amount,
					double *accrued_daily_amount,
					double *accrued_monthly_amount,
					char *application_name,
					char *full_name,
					char *street_address,
					char *transaction_description );

#ifdef NOT_DEFINED
/* Returns table_insert_count */
/* -------------------------- */
int reoccurring_table_insert(		FILE *input_file,
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
int reoccurring_insert(			char *application_name,
					LIST *reoccurring_list,
					char *fund_name );

int reoccurring_get_sequence_number(
					char *application_name,
					char *input_filename );

int reoccurring_get_line_count(		char *input_filename,
					int date_piece_offset );

boolean reoccurring_get_bank_date_international(
					char *bank_date_international,
					char *bank_date );

LIST *reoccurring_fetch_list(		char *application_name,
					int starting_sequence_number );

REOCCURRING_STRUCTURE *reoccurring_fetch(		char *application_name,
					char *bank_date,
					char *bank_description );

LIST *reoccurring_fetch_existing_cash_journal_ledger_list(
					char *application_name,
					char *minimum_bank_date,
					char *fund_name );

LIST *reoccurring_fetch_reoccurring_transaction_list(
					char *application_name );

void reoccurring_set_transaction(
				LIST *reoccurring_list,
				LIST *reoccurring_transaction_list,
				LIST *existing_cash_journal_ledger_list );

void reoccurring_insert_transaction(
					char *application_name,
					LIST *reoccurring_list );

char *reoccurring_get_select(		void );

void reoccurring_fetch_parse(		char **bank_date,
					char **bank_description,
					int *sequence_number,
					double *bank_amount,
					double *bank_running_balance,
					char *input_buffer );

void reoccurring_reoccurring_transaction_parse(
					char **full_name,
					char **street_address,
					char **debit_account,
					char **credit_account,
					double *transaction_amount,
					char **bank_upload_search_phrase,
					double *accrued_daily_amount,
					double *accrued_monthly_amount,
					char *input_buffer );

REOCCURRING_TRANSACTION *reoccurring_seek_bank_upload_search_phrase(
					LIST *reoccurring_transaction_list,
					char *bank_description );

void reoccurring_transaction_display(	LIST *reoccurring_list );

int reoccurring_get_starting_sequence_number(
					char *application_name,
					char *input_filename,
					int date_piece_offset );

char *reoccurring_reoccurring_transaction_get_select(
					void );

REOCCURRING_STRUCTURE *reoccurring_dictionary_extract(
					char *application_name,
					DICTIONARY *dictionary );

LIST *reoccurring_spreadsheet_get_list(
					char *input_filename,
					char **minimum_bank_date,
					char *application_name,
					int date_piece_offset,
					int description_piece_offset,
					int debit_piece_offset,
					int credit_piece_offset,
					int balance_piece_offset,
					int starting_sequence_number,
					char *fund_name );

#endif

