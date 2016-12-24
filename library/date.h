/* library/date.h */
/* -------------- */

#ifndef DATE_H
#define DATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "dictionary.h"

/* Constants */
/* --------- */
#define SECONDS_IN_DAY		86400
#define SECONDS_IN_MINUTE	60
#define SECONDS_IN_WEEK		( SECONDS_IN_DAY * 7 )
#define SECONDS_IN_HOUR		( SECONDS_IN_MINUTE * 60 )
#define SECONDS_IN_HALF_HOUR	( SECONDS_IN_MINUTE * 30 )

#define WDAY_SUNDAY			0
#define WDAY_MONDAY			1
#define WDAY_TUESDAY			2
#define WDAY_WEDNESDAY			3
#define WDAY_THURSDAY			4
#define WDAY_FRIDAY			5
#define WDAY_SATURDAY			6

#define DATE_END_OF_WEEK_INDICATOR	4

/* Structures */
/* ---------- */
typedef struct
{
	time_t current;
	int format_yyyy_mm_dd;
	DICTIONARY *week_of_year_dictionary;
	struct tm *tm;
} DATE;

/* Prototypes */
/* ---------- */
DATE *date_new( int year, int month, int day );

DATE *date_time_new( int year, int month, int day, int hour, int minute );

void increment_week( DATE *d );

void date_increment_day( DATE *d );

void date_decrement_day( DATE *d );

void date_increment( DATE *d );

DATE *date_yyyy_mm_dd_new( char *date_string );

DATE *date_yyyy_mm_dd_hms_new( char *date_time_string );

void date_increment_days( DATE *d, float days );

void date_increment_hours( DATE *d, float hours );

void date_increment_seconds( DATE *d, int seconds );

void date_increment_minutes( DATE *d, int minutes );

void date_add_minutes( DATE *d, int minutes );
int get_month( DATE *d );
int get_year( DATE *d );

int date_years_between(		char *from_date, char *to_date );

int date_minutes_between(	char *from_date_string,
				char *from_time_string,
				char *to_date_string,
	       			char *to_time_string,
				boolean add_one );

int age( char *birth_date );
void add_slashes_maybe( char *d, char *s );
char *date_yyyy_mm_dd( DATE *date );
char *date_get_yyyy_mm_dd( char *destination, DATE *date );
char *date_get_hhmm( char *destination, DATE *date );

char *date_get_colon_hhmmss(	char *destination,
				DATE *date );

char *date_get_hhmmss( 		char *destination,
				DATE *date );

char *date_display_yyyy_mm_dd(	DATE *date );

char *date_display_yyyy_mm_dd_colon_hms(
				DATE *date );

char *date_get_yyyy_mm_dd_string( DATE *date );

char *date_get_yesterday_yyyy_mm_dd_string( void );
DATE *date_get_today_new( void );

DATE *date_new_now( void );

DATE *date_today_new( void );

char *date_get_now_yyyy_mm_dd( void );
char *date_get_now_hhmm( void );
char *date_get_now_date_yyyy_mm_dd( void );
char *date_get_now_date_oracle_format( void );
char *date_get_now_date_hhmm( void );
char *date_get_today_yyyy_mm_dd( void );
char *date_get_yesterday_yyyy_mm_dd( void );
char *date_get_now_time_hhmm( void );
char *date_get_now_time_hhmm_colon_ss( void );
char *date_get_now_hhmm_colon_ss( void );
char *date_get_now_hh_colon_mm_colon_ss( void );
DATE *date_new_yyyy_mm_dd_date( char *date_field );
DATE *date_new_yyyy_mm_dd(	char *date_field );
DATE *date_new_yyyy_mm_dd_hhmm(	char *date_string,
				char *time_string );
int date_set_yyyy_mm_dd( DATE *date, char *yyyy_mm_dd );
int date_set_yyyy_mm_dd_hhmm(	DATE *date,
				char *yyyy_mm_dd_hhmm,
				char delimiter );
char *date_this_saturday_yyyy_mm_dd_string( DATE *date );
char *date_new_get_yyyy_mm_dd( time_t current );
DATE *date_populate_week_of_year_dictionary( char *year );
int date_get_week_of_year( DATE *date, char *yyyy_mm_dd );
void date_free( DATE *d );
DATE *new_date( void );
DATE *date_yyyy_mm_dd_hhmm_new( char *date_string, char *time_string );
void date_set_time( DATE *date, int hour, int minutes );
char *date_display_hhmm( DATE *date );
char *date_display_hhmmss( DATE *date );
int date_subtract_days( DATE *later_date, DATE *earlier_date );
int date_get_week_number( DATE *date );
int date_get_month_number( DATE *date );
int date_get_year_number( DATE *date );
char *date_get_yyyy_mm_dd_string( DATE *date );
int date_get_day_number( DATE *date );
int date_set_time_hhmm( DATE *date, char *hhmm );
int date_get_minutes_number( DATE *date );
int date_get_hour_number( DATE *date );
double date_yyyy_mm_dd_to_julian( char *yyyy_mm_dd );
double date_yyyy_mm_dd_hhmm_to_julian( char *yyyy_mm_dd, char *hhmm );
boolean date_copy( DATE *d1, DATE *d2 );
void date_increment( DATE *d );
void date_round2five_minutes( DATE *date );
void date_decrement_hour( DATE *d );
void date_increment_hour( DATE *d );
void date_decrement_minute( DATE *d );
void date_increment_minute( DATE *d );
char *date_get_current_yyyy_mm_dd();
char *date_get_current_hhmm();
void date_decrement_days( DATE *d, float days );
char *date_hhmm( DATE *d );
char *date_get_hhmm_string( DATE *d );
DATE *date_new_date( void );

DATE *date_new_date_time(
				int year,
				int month,
				int day,
				int hour,
				int minutes,
				int seconds );

void date_set_date_integers(
				DATE *date,
				int year,
				int month,
				int day );

void date_set_time_integers(	DATE *date,
				int hour,
				int minute,
				int seconds );

char *date_display_yyyy_mm_dd_hhmm( 
				DATE *date );

char *date_get_yyyy_mm_dd_hhmm( 
				DATE *date );

void date_increment_weekly_ceiling(
				DATE *date );

int date_get_day_of_week(	DATE *d );

char *date_display(		DATE *date );

boolean date_same_day(		DATE *old_date,
				DATE *new_date );

boolean date_compare(		DATE *date1,
				DATE *date2 );
boolean date_tomorrow( 		DATE *old_date, 
				DATE *new_date );
boolean date_year_in_future(	DATE *date,
				int year );
char *date_get_day_of_week_yyyy_mm_dd(
				int wday_of_week);
time_t date_yyyy_mm_dd_time_hhmm_to_time_t(
				char *date_string,
				char *time_string );
void date_set_tm_structures(	DATE *d, time_t current );
void date_output_hour_error(	char *function_name,
				int hour );
void date_set_tm_structures_pre_1970(
				DATE *d,
				time_t current );
int date_get_year(		DATE *d );
int get_hour(			DATE *d );
int date_get_hour(		DATE *d );
int date_get_minutes(		DATE *d );
int date_get_seconds(		DATE *d );
int get_day_of_month(		DATE *d );
int date_get_day_of_month(	DATE *d );
int date_get_month(		DATE *d );
time_t date_tm_to_current(	struct tm *tm );
time_t date_tm_to_current_pre_1970(
				struct tm *tm );
time_t date_mktime(		struct tm *tm );

char *date_get_day_of_week_string(
				DATE *d );

char *date_display_mdy(		DATE *date );

int date_get_month_day_number(	DATE *date );

int date_days_in_month(		int month,
				int year );

char *date_prior_day_of_week_yyyy_mm_dd_string(
				DATE *d,
				int day_of_week );

char *date_static_display_yyyy_mm_dd(
				DATE *date );
char *date_current_to_static_yyyy_mm_dd_string(
				time_t current );
char *date_static_display(	DATE *date );

boolean date_parse(		int *year,
				int *month,
				int *day,
				char *date_string );

char *date_get_unix_now_string(	void );

int date_get_day(		DATE *date );

boolean date_is_greatgreatgrandfather(
				DATE *d );

boolean date_is_greatgrandfather(
				DATE *d );

boolean date_is_grandfather(	DATE *d );

boolean date_is_sunday(		DATE *d );
boolean date_is_monday(		DATE *d );
boolean date_is_tuesday(	DATE *d );
boolean date_is_wednesday(	DATE *d );
boolean date_is_thursday(	DATE *d );
boolean date_is_friday(		DATE *d );
boolean date_is_saturday(	DATE *d );

boolean date_is_father(		DATE *d );

boolean date_is_day_of_week(	DATE *d,
				int day_of_week );

DATE *date_back_to_first_month(	DATE *d );

DATE *date_current_new(		time_t current );

void date_decrement_years(	DATE *d, int years );

void date_increment_months(	DATE *d, int months );

DATE *date_forward_to_first_month(
				DATE *d );

char *date_get_colon_now_time(
				void );

void date_place_colon_in_time(	char *time_string );

DATE *date_get_prior_day(	DATE *date,
				int wday_parameter );

DATE *date_get_prior_saturday(	DATE *date );

char *date_append_hhmmss(	char *date_string );

char *date_get_now_time_second(	void );

char *date_subtract_colon_from_time(
				char *time_string );

DATE *date_now_new(		void );

#endif
