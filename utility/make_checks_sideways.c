/* --------------------------------------------------- 	*/
/* $APPASERVER_HOME/utility/make_checks.c	       	*/
/* --------------------------------------------------- 	*/
/* Freely available software: see Appaserver.org	*/
/* --------------------------------------------------- 	*/

/* Includes */
/* -------- */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include "appaserver_error.h"
#include "timlib.h"
#include "latex.h"
#include "piece.h"
#include "dollar.h"

/* Constants */
/* --------- */
#define LATEX_FILENAME_TEMPLATE	"%s/make_checks_%d.tex"
#define PDF_FILENAME_TEMPLATE	"%s/make_checks_%d.pdf"

/* Prototypes */
/* ---------- */
int get_text_height(	boolean with_stub );

void make_checks_document_heading(
			FILE *latex_file,
			int text_height );

void make_checks_document_footer(
			FILE *latex_file );

void make_checks_argv(	int argc, char **argv );

void make_checks_stdin(	boolean with_stub );

void make_checks_dollar_text_with_stub(
			FILE *latex_file,
			char *payable_to,
			double dollar_amount,
			char *dollar_text_string,
			char *memo,
			int check_number,
			char *check_date,
			boolean with_newpage );

void make_checks_dollar_text(
			FILE *latex_file,
			char *payable_to,
			double dollar_amount,
			char *dollar_text_string,
			char *memo,
			char *check_date,
			boolean with_newpage );

void make_checks(	FILE *latex_file,
			char *payable_to,
			double dollar_amount,
			char *memo,
			int check_number,
			char *check_date,
			boolean with_newpage,
			boolean with_stub );

int main( int argc, char **argv )
{
	if ( argc == 3 && strcmp( argv[ 1 ], "stdin" ) == 0 )
	{
		boolean with_stub;

		with_stub = ( *argv[ 2 ] == 'y' );

		make_checks_stdin( with_stub );
	}
	else
	{
		make_checks_argv( argc, argv );
	}

	return 0;

} /* main() */

void make_checks_argv( int argc, char **argv )
{
	char *payable_to;
	double dollar_amount;
	char *memo;
	char *appaserver_data_directory;
	char latex_filename[ 128 ];
	char pdf_filename[ 128 ];
	pid_t process_id = getpid();
	FILE *latex_file;
	char *check_date;
	boolean with_stub;
	int check_number;
	int text_height;

	if ( argc != 6 )
	{
		fprintf(stderr,
"Usage: %s payable_to|stdin dollar_amount memo check_number with_stub_yn\n",
			argv[ 0 ] );
		exit( 1 );
	}

	payable_to = argv[ 1 ];
	dollar_amount = atof( argv[ 2 ] );
	memo = argv[ 3 ];
	check_number = atoi( argv[ 4 ] );
	with_stub = ( *argv[ 5 ] == 'y' );

	appaserver_error_stderr( argc, argv );

	check_date = pipe2string( "now.sh full" );

	appaserver_data_directory =
		pipe2string(
			"get_appaserver_data_directory.e" );

	if ( !appaserver_data_directory
	||   !*appaserver_data_directory )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get appaserver_data_directory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf( latex_filename,
		 LATEX_FILENAME_TEMPLATE,
		 appaserver_data_directory,
		 process_id );

	sprintf( pdf_filename,
		 PDF_FILENAME_TEMPLATE,
		 appaserver_data_directory,
		 process_id );

	if ( ! ( latex_file = fopen( latex_filename, "w" ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 latex_filename );
		exit( 1 );
	}

	text_height = get_text_height( with_stub );

	make_checks_document_heading( latex_file, text_height );

	make_checks(	latex_file,
			payable_to,
			dollar_amount,
			memo,
			check_number,
			check_date,
			0 /* not with_newpage */,
			with_stub );

	make_checks_document_footer( latex_file );

	fclose( latex_file );

	/* Creates pdf_filename */
	/* -------------------- */
	latex_tex2pdf(	latex_filename,
			appaserver_data_directory
				/* working_directory */ );

	printf(	"%s\n", pdf_filename );

} /* make_checks_argv() */

void make_checks_stdin( boolean with_stub )
{
	char payable_to[ 128 ];
	char dollar_amount_string[ 128 ];
	char memo[ 128 ];
	double dollar_amount_double;
	char check_number_string[ 16 ];
	char *appaserver_data_directory;
	char latex_filename[ 128 ];
	char pdf_filename[ 128 ];
	pid_t process_id = getpid();
	char input_buffer[ 1024 ];
	FILE *latex_file;
	boolean first_time = 1;
	char *check_date;
	int text_height;

	check_date = pipe2string( "now.sh full" );

	appaserver_data_directory =
		pipe2string(
			"get_appaserver_data_directory.e" );

	if ( !appaserver_data_directory
	||   !*appaserver_data_directory )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot get appaserver_data_directory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	sprintf( latex_filename,
		 LATEX_FILENAME_TEMPLATE,
		 appaserver_data_directory,
		 process_id );

	sprintf( pdf_filename,
		 PDF_FILENAME_TEMPLATE,
		 appaserver_data_directory,
		 process_id );

	if ( ! ( latex_file = fopen( latex_filename, "w" ) ) )
	{
		fprintf( stderr,
		"ERROR in %s/%s()/%d: cannot open %s for write.\n",
		 	__FILE__,
		 	__FUNCTION__,
		 	__LINE__,
			latex_filename );
		exit( 1 );
	}

	text_height = get_text_height( with_stub );

	make_checks_document_heading( latex_file, text_height );

	while( get_line( input_buffer, stdin ) )
	{
		if ( character_count( '^', input_buffer ) != 3 )
		{
			fprintf( stderr,
				 "Warning in %s/%s()/%d: ignoring (%s)n",
				 __FILE__,
				 __FUNCTION__,
				 __LINE__,
				 input_buffer );
			continue;
		}

		piece( payable_to, '^', input_buffer, 0 );
		piece( dollar_amount_string, '^', input_buffer, 1 );
		piece( memo, '^', input_buffer, 2 );

		if ( strcmp( memo, "memo" ) == 0 ) *memo = '\0';

		piece( check_number_string, '^', input_buffer, 3 );

		dollar_amount_double = atof( dollar_amount_string );

		make_checks(	latex_file,
				payable_to,
				dollar_amount_double,
				memo,
				atoi( check_number_string ),
				check_date,
				1 - first_time /* with_newpage */,
				with_stub );

		first_time = 0;

	} /* while( get_line() */

	make_checks_document_footer( latex_file );

	fclose( latex_file );

	/* Creates pdf_filename */
	/* -------------------- */
	latex_tex2pdf(	latex_filename,
			appaserver_data_directory
				/* working_directory */ );

	printf(	"%s\n", pdf_filename );

} /* make_checks_stdin */

void make_checks(	FILE *latex_file,
			char *payable_to,
			double dollar_amount,
			char *memo,
			int check_number,
			char *check_date,
			boolean with_newpage,
			boolean with_stub )
{
	char dollar_text_string[ 256 ];

	dollar_text( dollar_text_string, dollar_amount );

	if ( with_stub )
	{
		make_checks_dollar_text_with_stub(
			latex_file,
			payable_to,
			dollar_amount,
			dollar_text_string,
			memo,
			check_number,
			check_date,
			with_newpage );
	}
	else
	{
		make_checks_dollar_text(
			latex_file,
			payable_to,
			dollar_amount,
			dollar_text_string,
			memo,
			check_date,
			with_newpage );
	}

} /* make_checks() */

void make_checks_document_heading(
			FILE *latex_file,
			int text_height )
{
	fprintf( latex_file,
"\\documentclass{report}\n"
"\\usepackage[	top=0in,\n"
"		left=0in,\n"
"		paperheight=%din,\n"
"		paperwidth=3.0in,\n"
"		textheight=%din,\n"
"		textwidth=3.0in,\n"
"		noheadfoot]{geometry}\n"
"\\usepackage{rotating}\n"
"\\pagenumbering{gobble}\n"
"\\begin{document}\n",
		 text_height,
		 text_height );

} /* make_checks_document_heading() */

void make_checks_document_footer(
			FILE *latex_file )
{
	fprintf( latex_file,
"\\end{document}\n" );

} /* make_checks_document_footer() */

int get_text_height( boolean with_stub )
{
	int text_height;

	if ( with_stub )
		text_height = 12;
	else
		text_height = 8;

	return text_height;
}

void make_checks_dollar_text(
			FILE *latex_file,
			char *payable_to,
			double dollar_amount,
			char *dollar_text_string,
			char *memo,
			char *check_date,
			boolean with_newpage )
{
	char payable_to_escaped[ 128 ];

	latex_escape_data( payable_to_escaped, payable_to, 128 );

	if ( with_newpage )
	{
		fprintf( latex_file,
"\\newpage\n\n" );
	}

	fprintf( latex_file,
"\\begin{sideways}\n"
"\\begin{minipage}{8.0in}\n"
"\\vspace{0.50in}\n"
"\\begin{tabular}{p{4.9in}l}\n" );

	fprintf( latex_file,
"& %s\n",
		 check_date );

	fprintf( latex_file,
"\\end{tabular}\n\n"
"\\vspace{0.25in}\n\n"
"\\begin{tabular}{p{1.3in}p{4.9in}l}\n" );

/*
	fprintf( latex_file,
"& %s & %.2lf\n",
		 payable_to_escaped,
		 dollar_amount );
*/

	fprintf( latex_file,
"& %s & %s\n",
		 payable_to_escaped,
		 place_commas_in_money( dollar_amount ) );

	fprintf( latex_file,
"\\end{tabular}\n\n"
"\\vspace{0.20in}\n\n"
"\\begin{tabular}{p{0.85in}l}\n" );

	fprintf( latex_file,
"& %s\n",
		 dollar_text_string );

	fprintf( latex_file,
"\\end{tabular}\n\n" );

	if ( memo && *memo && strcmp( memo, "memo" ) != 0 )
	{
		fprintf( latex_file,
"\\vspace{0.52in}\n\n"
"\\begin{tabular}{p{0.30in}l}\n"
"& %s\n"
"\\end{tabular}\n",
		 	 memo );
	}

	fprintf( latex_file,
"\\end{minipage}\n"
"\\end{sideways}\n" );

} /* make_checks_dollar_text() */

void make_checks_dollar_text_with_stub(
			FILE *latex_file,
			char *payable_to,
			double dollar_amount,
			char *dollar_text_string,
			char *memo,
			int check_number,
			char *check_date,
			boolean with_newpage )
{
	char payable_to_escaped[ 128 ];

	latex_escape_data( payable_to_escaped, payable_to, 128 );

	if ( with_newpage )
	{
		fprintf( latex_file,
"\\newpage\n\n" );
	}

	fprintf( latex_file,
"\\begin{sideways}\n"
"\\begin{minipage}{12.0in}\n" );

	fprintf( latex_file,
"\\vspace{0.25in}\n"
"\\begin{tabular}{p{0.2in}l}\n"
"& %s\n"
"\\end{tabular}\n\n",
		 check_date );

	fprintf( latex_file,
"\\vspace{0.14in}\n"
"\\begin{tabular}{p{0.2in}p{8.7in}l}\n"
"& %s & %s\n"
"\\end{tabular}\n\n",
		 payable_to_escaped,
		 check_date );

	fprintf( latex_file,
"\\vspace{0.16in}\n\n"
"\\begin{tabular}{p{5.3in}p{4.9in}l}\n"
"& %s & %s\n"
"\\end{tabular}\n\n",
		 payable_to_escaped,
		 place_commas_in_money( dollar_amount ) );

	if ( memo && *memo && strcmp( memo, "memo" ) != 0 )
	{
		fprintf( latex_file,
"\\vspace{0.20in}\n\n"
"\\begin{tabular}{p{0.2in}p{4.65in}l}\n"
"& %s & %s\n"
"\\end{tabular}\n\n",
			memo,
		 	dollar_text_string );
	}
	else
	{
		fprintf( latex_file,
"\\vspace{0.20in}\n\n"
"\\begin{tabular}{p{4.85in}l}\n"
"& %s\n"
"\\end{tabular}\n\n",
		 	dollar_text_string );
	}

	if ( memo && *memo && strcmp( memo, "memo" ) != 0 )
	{
		fprintf( latex_file,
"\\vspace{0.52in}\n\n"
"\\begin{tabular}{p{0.2in}p{2.2in}p{1.6in}l}\n"
"& Check: %d & %s & %s\n"
"\\end{tabular}\n",
			 check_number,
		 	 place_commas_in_money( dollar_amount ),
			 memo );
	}
	else
	{
		fprintf( latex_file,
"\\vspace{0.43in}\n\n"
"\\begin{tabular}{p{0.2in}p{2.45in}l}\n"
"& Check: %d & %s\n"
"\\end{tabular}\n",
			 check_number,
		 	 place_commas_in_money( dollar_amount ) );
	}

	fprintf( latex_file,
"\\end{minipage}\n"
"\\end{sideways}\n" );

} /* make_checks_dollar_text_with_stub() */

