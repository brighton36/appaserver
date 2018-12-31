/* library/date_convert.h */
/* ---------------------- */

#ifndef DATE_CONVERT_H
#define DATE_CONVERT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "boolean.h"

/* Constants */
/* --------- */
#define CURRENT_CENTURY		2000

/* Enumerated types */
/* ---------------- */
enum date_convert_format {	international,
				american,
				military,
				date_convert_unknown };

/* Structures */
/* ---------- */
typedef struct
{
	enum date_convert_format source_format;
	enum date_convert_format destination_format;
	char return_date[ 256 ];
} DATE_CONVERT;

/* Prototypes */
/* ---------- */
DATE_CONVERT *date_convert_new_date_convert(
				enum date_convert_format destination_format,
				char *date_string );

DATE_CONVERT *date_convert_new_database_format_date_convert(
				char *application_name,
				char *date_string );

DATE_CONVERT *date_convert_new_user_format_date_convert(
				char *application_name,
				char *login_name,
				char *date_string );

void date_convert_free(		DATE_CONVERT *date_convert );

boolean date_convert_source_american(
				char *return_date,
				enum date_convert_format destination_format,
				char *date_string );

boolean date_convert_date_time_source_unknown(
				char *return_date_time,
				enum date_convert_format destination_format,
				char *date_time_string );

boolean date_convert_source_unknown(
				char *return_date,
				enum date_convert_format destination_format,
				char *date_string );

boolean date_convert_source_military(
				char *return_date,
				enum date_convert_format destination_format,
				char *date_string );

boolean date_convert_source_international(
				char *return_date,
				enum date_convert_format destination_format,
				char *date_string );

char *date_convert_international2american(
				char *international );

enum date_convert_format date_convert_date_get_format(
				char *date_string );

enum date_convert_format date_convert_populate_return_date(
				char *return_date,
				enum date_convert_format source_format,
				enum date_convert_format destination_format,
				char *date_string );

char *date_convert_american2international(
				char *american );

DATE_CONVERT *date_convert_new_database_date_convert(
				char *application_name,
				char *login_name,
				char *date_string );

enum date_convert_format date_convert_get_date_convert_format(
				char *format_string );

enum date_convert_format date_convert_get_user_date_format(
				char *application_name,
				char *login_name );

enum date_convert_format date_convert_get_database_date_format(
				char *application_name );

char *date_convert_get_date_format_string(
				enum date_convert_format );

char date_convert_get_delimiter(char *date_string );

boolean date_convert_international_correct_format(
				char *date_string );

boolean date_convert_is_valid_international(
				char *date_string );

boolean date_convert_is_valid_american(
				char *date_string );

boolean date_convert_is_valid_military(
				char *date_string );

char *date_convert_american_sans_slashes(
				char *compressed_date_string );

boolean date_convert_is_valid_integers(
				int year_integer,
				int month_integer,
				int day_integer );

char *date_convert_display(	enum date_convert_format );

boolean date_convert_date_time_source_unknown(
			char *return_date_time,
			enum date_convert_format destination_format,
			char *date_time_string );

#endif
