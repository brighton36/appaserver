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
enum bank_upload_exception {	bank_upload_exception_none,
				duplicated_spreadsheet_file,
				empty_transaction_rows,
				sequence_number_not_generated,
				internal_read_permission };

/* Constants */
/* --------- */
#define INSERT_BANK_UPLOAD		\
	"bank_date,bank_description,sequence_number,bank_amount,bank_upload_date_time"

#define INSERT_BANK_UPLOAD_EVENT		\
	"bank_upload_date_time,login_name,bank_upload_filename,file_sha256sum,feeder_account"

#define INSERT_BANK_UPLOAD_EVENT_FUND		\
	"bank_upload_date_time,login_name,bank_upload_filename,file_sha256sum,feeder_account,fund"

#define INSERT_BANK_UPLOAD_ARCHIVE	\
	"bank_date,bank_description,sequence_number,bank_amount,bank_running_balance,bank_upload_date_time"

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
	char *file_sha256sum;
	char *minimum_bank_date;
	int file_row_count;
	int table_insert_count;
	int date_piece_offset;
	int description_piece_offset;
	int debit_piece_offset;
	int credit_piece_offset;
	int balance_piece_offset;
	LIST *error_line_list;
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
	char *feeder_account;
	char *bank_upload_date_time;
} BANK_UPLOAD_STRUCTURE;

/* Operations */
/* ---------- */
BANK_UPLOAD_STRUCTURE *bank_upload_structure_new(
					char *application_name,
					char *fund_name,
					char *feeder_account,
					char *input_filename,
					int date_piece_offset,
					int description_piece_offset,
					int debit_piece_offset,
					int credit_piece_offset,
					int balance_piece_offset );

BANK_UPLOAD *bank_upload_calloc(	void );

BANK_UPLOAD *bank_upload_new(		char *bank_date,
					char *bank_description );

/* Returns table_insert_count */
/* -------------------------- */
int bank_upload_insert(			char *application_name,
					LIST *bank_upload_list,
					char *bank_upload_date_time );

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

void bank_upload_table_display(		LIST *bank_upload_list );

int bank_upload_get_starting_sequence_number(
					char *application_name,
					char *input_filename,
					int date_piece_offset );

BANK_UPLOAD *bank_upload_dictionary_extract(
					char *application_name,
					DICTIONARY *dictionary );

LIST *bank_upload_fetch_file_list(
					LIST *error_line_list,
					char **file_sha256sum,
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

char *bank_upload_get_transaction_memo(
					char *full_name,
					char *street_address,
					char *transaction_date_time );

void bank_upload_event_insert(		char *application_name,
					char *bank_upload_date_time,
					char *login_name,
					char *bank_upload_filename,
					char *file_sha256sum,
					char *fund_name,
					char *feeder_account );

boolean bank_upload_sha256sum_exists(
					char *application_name,
					char *file_sha256sum );

void bank_upload_archive_insert(	char *application_name,
					LIST *bank_upload_list,
					char *bank_upload_date_time );

int bank_upload_transaction_count(
					LIST *bank_upload_list );

#endif

