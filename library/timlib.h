/* library/timlib.h					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#ifndef TIMLIB_H
#define TIMLIB_H

/* Includes */
/* -------- */
#include <stdio.h>
#include <values.h>
#include <math.h>
#include "boolean.h"
#include "list.h"
#include "aggregate_level.h"

/* Constants */
/* --------- */
#define NULL_STRING		"null"
#define DATABASE_NULL		"is_null"
#define DATE_MINIMUM		"Date Minimum"
#define DATE_MAXIMUM		"Date Maximum"
#define DATE_COUNT		"Date Count"
#define DATE_PERCENT_MISSING	"Date Percent Missing"

#define LF	10
#define CR	13

#define QUEUE_STATIC_BUFFER_SIZE		4096

/*
#define DOUBLE_MINIMUM				DBL_MIN
*/
#define DOUBLE_MINIMUM				-10000000000000000000000.0
#define DOUBLE_MAXIMUM				9999999999999999999999.9

/* Prototypes */
/* ---------- */
void mail_tim( char *message );
int get_index( char *attribute_name );

char *timlib_trim_index( char *destination, char *attribute_name );

char *trim_index( char *destination, char *attribute_name );

char *trim_index_zero( char *destination, char *attribute_name );

boolean exists_index( char *attribute_name );

char *format_plural( char *destination, char *string );

char *format_initial_capital_cr( char *destination, char *string );

char *format_initial_capital( char *destination, char *string );

char *format_initial_capital_not_parens( char *destination, char *string );

char *format_initial_capital_delimiter(
			char *destination,
			char *string,
			char delimiter,
			boolean with_inside_parens );

char *format_mnemonic( char *mnemonic, char *string );

char *format_first_initial_capital( char *destination, char *string );
char *pipe2string( char *pipe_string );
char *pipe_multiple_lines2string( char *pipe_string, char delimiter );
char *get_line_system( char *sys_string );
char **pipe_list2list( char *pipe_list, int max_items );
int exists_character( char *buffer, int c );

char *trim_until_character(	char *destination,
				char c,
				char *source,
				int num2trimg );

char *trim_after_character(	char *destination,
				char c,
				char *source,
				int num2trimg );

void trim_character( char *destination, char c, char *source );
char *reformat_double_return_string( double d, char *format_string );
int character_count( char char_to_count, char *source );
int timlib_count_delimiters( int char_to_count, char *source );
FILE *open_file( char *s, char *mode );
char *skip_spaces( char *s );
char *get_system( char *sys_str );
char *midstr( char *d, char *s, int start, int how_many );
char *center( char *string, int screen_len );
char *center_rpad( char *string, int screen_len );
char *trim( char * );
char *up_string( char * );
char *low_string( char * );
int get_line( char *destination, FILE *infile );
boolean timlib_get_line( char *destination, FILE *infile, int buffer_size );
void skip_line( FILE *infile );
int instr( char *substr, char *string, int occurrence );
int instr_character( char delimiter, char *string );
char *delete_str( char *string, int start, int num_chars );
char *insert_str( char *sub, char *string, int pos );

char *search_replace_word( 	char *source_destination,
				char *search_str, 
				char *replace_str );

char *search_replace( 	char *search_str, 
			char *replace_str,
			char *source_destination );

char *search_replace_character(
			char *source_destination,
			char search_character, 
			char replace_character );

char *search_replace_string(
			char *source_destination,
			char *search_str, 
			char *replace_str );

char *search_replace_once(
			char *source_destination,
			char *search_str, 
			char *replace_str );

char *timlib_delete_character(
			char *source_destination,
			char character );

char *skip_words( char *source, int number_words );

int get_parameter(	char *destination, 
			char *source, 
			int parameter_index 	/* zero based */  );

void remove_parans( char *without_parans, char *source );

int number_occurrences_char( int char_to_count, char *source );

char *remove_quotes(		char *d, char *s );

char *timlib_place_commas_in_dollars(
				double d );

char *place_commas_in_double(	double d );

char *place_commas_in_number_string(
				char *s );

char *place_commas_in_unsigned_int(
				unsigned int n );

char *place_commas_in_integer(	int n );

char *place_commas_in_long(	long n );

char *place_commas_in_unsigned_long(
				unsigned long n );

char *place_commas_in_unsigned_long_long(
				unsigned long long n );

char *place_commas_in_money(	double d );

char *timlib_place_commas_in_money(
				double d );

char *get_date( char *d );
void extract_delimited( char *destination, char *source );
int get_index_from_string( char *s );
char *single_quotes_around( char *d, char *s );
char *quotes_around( char *destination, char *s, int c );
char *double_quotes_around( char *destination, char *s );
char *timlib_pad_spaces_to_justify_left( char *d, char *s, int size );
char *timlib_pad_spaces_to_justify_right( char *d, char *s, int size );
double *linear_interpolate( 	double *buffer,
				int from_x,
				int to_x );
void atof_array( double *f_array, char *pipe_delimited_string );
int count_characters( int char_to_count, char *source );
double time2day_percentage( int hour, int minute );
void unget_line_queue( char *in_line );
int get_line_queue( char *in_line, FILE *infile );
double abs_float( double f );
double float_abs( double f );
char *insert_string( char *source_destination, char *substring, int pos );
double get_percent_of_day( char *time_string );
void increment_time_one_hour(		char *time_string );
int zap_file(				char *filename );
boolean double_virtually_same( 		double d1, double d2 );
boolean dollar_virtually_same( 		double d1, double d2 );
boolean timlib_double_virtually_same( 	double d1, double d2 );
boolean timlib_dollar_virtually_same( 	double d1, double d2 );
double round_double(			double d );
double ceiling( 			double d );
double floor(				double d );
char *get_node_name(			void );

char *escape_character( 		char *destination,
					char *data,
					int character_to_escape );

char *unescape_character( 		char *destination,
					char *data,
					int character_to_unescape );

int timlib_get_block_delimiter( 	char **block,
			 		int delimiter,
			 		int columns_to_block,
			 		FILE *input_file,
					int max_lines );

int string_array_search_offset( 	char **string_array, 
					char *search_string );

int count_character( 			int char_to_count, char *source );

char *last_word_string(			char *s );

char *mnemonic2upper_case( 		char *d, char *s );

int is_mnemonic( 			char *s );

char *search_replace_single_quoted_string(
					char *source_destination,
					char *search_string,
					char *replace_string );

char *timlib_escape_field(		char *source_destination );

char *escape_field(			char *source_destination );

char *escape_string( 			char *source_destination );

void free_array_string_with_count( 	char **block, 
					int block_count );

char *right(				char *string,
					char *substr,
					int width );
void unescape_string(			char *s );
char *escape_dollar_sign(		char *data );
unsigned long get_file_size(		char *filename );
char *escape_dollar_signs(		char *data );
char *escape_single_quotes(		char *data );
char *escape_single_quote(		char *data );
char *timlib_escape_single_quotes(	char *destination, char *source );
int instr_exclude_double_quoted(	char *substr,
					char *string,
					int occurrence );
char *rtrim(				char *buffer );
void search_replace_special_characters(
					char *buffer );
void escape_special_characters(		char *data );
boolean timlib_exists_special_character(char *data );
char *mysql2american_date(		char *date_buffer,
					char *mysql_date );

int timlib_character_exists(		char *buffer, int c );
int character_exists( 			char *buffer, int c );

void get_clean_begin_end_date(		char **begin_date,
					char **end_date );
char *up_string_to_delimiter (		char *s, char delimiter );
char *right_trim(			char *buffer );
char *right_trim_session(		char *buffer );
char *trim_quotes(			char *d, char *s );

char *commas_in_double(			char *destination, 
					double d );

char *reverse_string(			char *destination,
					char *source );
double ceiling_double(			double d );
double floor_double(			double d );
void make_single_quotes_double_single_quotes(
					char *d );
boolean timlib_file_populated(		char *filename );
char *timlib_ora_date_to_yyyy_mm_dd(	char *s );
char *timlib_oracle_date2mysql_date_string(
					char *s );
int timlib_ora_month2integer(		char *ora_month );
int timlib_full_month2integer(		char *full_month );
char *timlib_integer2full_month(	int month_integer );
boolean timlib_exists_character( 	char *s,
					char ch );

boolean timlib_exists_string(	 	char *string,
					char *substring );

boolean timlib_begins_string(	 	char *string,
					char *substring );

boolean timlib_ends_string(	 	char *string,
					char *substring );

long timlib_random(			long up_to );
char *timlib_mysql_date2ddmmyyyy(	char *destination,
					char *source );
char *timlib_trim_double_quotes(	char *s );
char *timlib_yyyymmdd_to_ora_date(	char *s );
char *timlib_yyyymmdd_to_oracle_format(	char *s );
char *timlib_oracle_date2mysql(		char *s );
boolean timlib_exists_alpha(		char *s );
boolean timlib_exists_numeric(		char *s );
boolean timlib_parse_database_string(	char **database_string,
					char *application_name );

char *timlib_get_parameter_application_name(
					char *application_name,
					char *database_string );

double timlib_get_x_increment(	int expected_count_per_day,
				enum aggregate_level aggregate_level );

int timlib_get_x_increment_seconds(
				int expected_count_per_day,
				enum aggregate_level aggregate_level );

double timlib_get_x_increment_days(
				int expected_count_per_day,
				enum aggregate_level aggregate_level );

char *timlib_yyyy_mm_dd_to_julian(	char *yyyy_mm_dd );
char *timlib_integer2binary(		unsigned integer );
boolean timlib_string_until_character_matches(
					char *source,
					char *string_compare,
					char delimiter );
unsigned int timlib_factoral(		int n );

char timlib_get_delimiter(		char *string );

int timlib_get_index(			char *s );
char *timlib_search_replace_character_limited(
					char *source_destination,
					char search_character,
					char replace_character,
					int how_many );
int character_position(			char *buffer, int c );

char *timlib_get_three_character_month_string(
					int month_offset );
int timlib_delimiter_independent_strcmp(
				char *string1,
				char *string2 );

char *timlib_get_in_clause(	char *data_list_string );

char *timlib_with_list_get_in_clause(
				LIST *data_list );

boolean timlib_file_exists(	char *filename );

boolean timlib_strcpy(		char *d, char *s, int buffer_size );

char *timlib_strcat(		char *d, char *s );

double timlib_latitude_longitude_degrees_minutes_to_decimal(
					char *latitude_longitude );

char *timlib_latitude_longitude_degrees_decimal_to_minutes(
					char *latitude_longitude_decimal );

void **timlib_allocate_pointer_array(
				int array_pointer_size,
				int item_pointer_size,
				int array_length );

int timlib_strncmp(		char *s1, char *s2 );

int timlib_strcmp(		char *s1, char *s2 );

char *timlib_strncpy(		char *s1, char *s2, int count );

boolean timlib_pad_time_four(	char *time_string );

int timlib_strlen(		char *s );

char *timlib_with_list_get_in_clause(
				LIST *data_list );

int timlib_sum_delimited_string(char *string,
				char delimiter,
				int starting_piece,
				int ending_piece );

char *timlib_character_pad(	char delimiter,
				int column_count );

void timlib_unget_line(		char *in_line );

void timlib_reset_line_queue(	void );

int timlib_get_seed(		void );

char *timlib_escape_character_array(
				char *destination,
				char *source,
				char *character_array );

char *timlib_sql_injection_escape(
				char *source );

void remove_end_characters( char *d, char *s, char end_character );

void timlib_remove_character(	char *source_destination,
				char character );

void remove_character(		char *source_destination,
				char character );

int timlib_get_line(		char *in_line,
				FILE *infile,
				int buffer_size );

char *timlib_string_array_display(
				char **string_array );

void timlib_display_error_file(	char *error_filename );

boolean timlib_is_valid_time(	char *time_string );

char *timlib_trim_money_characters(
				char *amount );

void output_starting_argv_stderr( int argc, char **argv );

char *timlib_get_now_date_time(	void );

boolean timlib_string_empty(	char *s,
				char *empty_label );

char *timlib_directory_filesystem(
				long int *available_megabytes,
				char *directory );

char *timlib_get_latest_filename_datestamp(
				char *directory );

boolean timlib_login_name_email_address(
				char *login_name );

boolean timlib_double_virtually_same_places(
				double d1, double d2, int places );

char *trim_length(		char *buffer,
				int length );

char *timlib_escape_character_string(
				char *source_destination,
				char *character_string );

void timlib_reset_get_line_check_utf_16(
				void );

char *timlib_generate_password(	void );

char *timlib_commas_in_dollars( double d );

#endif
