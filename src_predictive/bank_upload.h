/* -------------------------------------------------------------------- */
/* $APPASERVER_HOME/src_predictive/bank_upload.h			*/
/* -------------------------------------------------------------------- */
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#ifndef BANK_UPLOAD_H
#define BANK_UPLOAD_H

#include "ledger.h"

/* Enumerated types */
/* ---------------- */

/* Constants */
/* --------- */
#define INSERT_BANK_UPLOAD		\
	"bank_date,bank_description,sequence_number,bank_amount,bank_running_balance"

#define INSERT_BANK_UPLOAD_FUND		\
	"bank_date,bank_description,sequence_number,bank_amount,bank_running_balance,fund"

#define REOCCURRING_TRANSACTION_FOLDER_NAME \
					"reoccurring_transaction"

/* Structures */
/* ---------- */
typedef struct
{
	char *bank_date;
	char *bank_description;
	int sequence_number;
	double bank_amount;
	double bank_running_balance;
	TRANSACTION *transaction;
} BANK_UPLOAD;

typedef struct
{
	char *full_name;
	char *street_address;
	char *debit_account;
	char *credit_account;
	double transaction_amount;
	char *bank_upload_search_phrase;
} REOCCURRING_TRANSACTION;

typedef struct
{
	char *fund_name;
	char *input_filename;
	int starting_sequence_number;
	int file_record_count;
	int table_insert_count;
	char *minimum_bank_date;
	LIST *bank_upload_list;
	LIST *existing_cash_journal_ledger_list;
	LIST *reoccurring_transaction_list;
} BANK_UPLOAD_STRUCTURE;

/* Operations */
/* ---------- */
BANK_UPLOAD_STRUCTURE *bank_upload_structure_new(
					char *fund_name,
					char *input_filename );

BANK_UPLOAD *bank_upload_calloc(	void );

REOCCURRING_TRANSACTION *bank_upload_reoccurring_transaction_calloc(
					void );

REOCCURRING_TRANSACTION *bank_upload_reoccurring_transaction_new(
					char *application_name,
					char *full_name,
					char *street_address,
					double transaction_amount );

boolean bank_upload_reoccurring_transaction_load(
					char **debit_account,
					char **credit_account,
					double *transaction_amount,
					char *application_name,
					char *full_name,
					char *street_address );

BANK_UPLOAD *bank_upload_new(
					char *bank_date,
					char *bank_description );

/* Returns table_insert_count */
/* -------------------------- */
int bank_upload_table_insert(		FILE *input_file,
					char **minimum_bank_date,
					char *application_name,
					char *fund_name,
					boolean execute,
					int starting_sequence_number );

int bank_upload_get_sequence_number(
					char *application_name,
					char *input_filename );

int bank_upload_get_line_count(		char *input_filename );

boolean bank_upload_get_bank_date_international(
					char *bank_date_international,
					char *bank_date );

LIST *bank_upload_fetch_list(		char *application_name,
					int starting_sequence_number );

LIST *bank_upload_fetch_existing_cash_journal_ledger_list(
					char *application_name,
					char *minimum_bank_date,
					char *fund_name );

LIST *bank_upload_fetch_reoccurring_transaction_list(
					char *application_name );

void bank_upload_set_transaction(
				LIST *bank_upload_list,
				LIST *reoccurring_transaction_list,
				LIST *existing_cash_journal_ledger_list );

void bank_upload_insert_transaction(
					char *application_name,
					LIST *bank_upload_list );

char *bank_upload_get_select(		void );

void bank_upload_parse(			char **bank_date,
					char **bank_description,
					int *sequence_number,
					double *bank_amount,
					double *bank_running_balance,
					char *input_buffer );

void bank_upload_reoccurring_transaction_parse(
					char **full_name,
					char **street_address,
					char **debit_account,
					char **credit_account,
					double *transaction_amount,
					char **bank_upload_search_phrase,
					char *input_buffer );

REOCCURRING_TRANSACTION *bank_upload_seek_bank_upload_search_phrase(
					LIST *reoccurring_transaction_list,
					char *bank_description );

#endif
