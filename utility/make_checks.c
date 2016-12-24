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
void make_checks_document_heading(
			FILE *latex_file );

void make_checks_document_footer(
			FILE *latex_file );

void make_checks_argv(	int argc, char **argv );

void make_checks_stdin(	void );

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
			char *check_date,
			boolean with_newpage );

int main( int argc, char **argv )
{
	if ( argc == 2 && strcmp( argv[ 1 ], "stdin" ) == 0 )
	{
		make_checks_stdin();
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

	if ( argc != 4 )
	{
		fprintf(stderr,
			"Usage: %s payable_to|stdin dollar_amount memo\n",
			argv[ 0 ] );
		exit( 1 );
	}

	payable_to = argv[ 1 ];
	dollar_amount = atof( argv[ 2 ] );
	memo = argv[ 3 ];

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

	make_checks_document_heading( latex_file );

	make_checks(	latex_file,
			payable_to,
			dollar_amount,
			memo,
			check_date,
			0 /* not with_newpage */ );

	make_checks_document_footer( latex_file );

	fclose( latex_file );

	/* Creates pdf_filename */
	/* -------------------- */
	latex_tex2pdf(	latex_filename,
			appaserver_data_directory
				/* working_directory */ );

	printf(	"%s\n", pdf_filename );

} /* make_checks_argv() */

void make_checks_stdin( void )
{
	char payable_to[ 128 ];
	char dollar_amount_string[ 128 ];
	char memo[ 128 ];
	double dollar_amount_double;
	char *appaserver_data_directory;
	char latex_filename[ 128 ];
	char pdf_filename[ 128 ];
	pid_t process_id = getpid();
	char input_buffer[ 1024 ];
	FILE *latex_file;
	boolean first_time = 1;
	char *check_date;

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

	make_checks_document_heading( latex_file );

	while( get_line( input_buffer, stdin ) )
	{
		if ( character_count( '^', input_buffer ) != 2 )
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

		dollar_amount_double = atof( dollar_amount_string );

		make_checks(	latex_file,
				payable_to,
				dollar_amount_double,
				memo,
				check_date,
				1 - first_time /* with_newpage */ );

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
			char *check_date,
			boolean with_newpage )
{
	char dollar_text_string[ 256 ];

	dollar_text( dollar_text_string, dollar_amount );

	make_checks_dollar_text(
		latex_file,
		payable_to,
		dollar_amount,
		dollar_text_string,
		memo,
		check_date,
		with_newpage );

} /* make_checks() */

void make_checks_document_heading(
			FILE *latex_file )
{
	fprintf( latex_file,
"\\documentclass{report}\n"
"\\usepackage[	top=0in,\n"
"		left=0in,\n"
"		textheight=8in,\n"
"		textwidth=3.0in,\n"
"		noheadfoot]{geometry}\n"
"\\usepackage{rotating}\n"
"\\pagenumbering{gobble}\n"
"\\begin{document}\n" );

} /* make_checks_document_heading() */

void make_checks_document_footer(
			FILE *latex_file )
{
	fprintf( latex_file,
"\\end{document}\n" );

} /* make_checks_document_footer() */

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

	fprintf( latex_file,
"& %s & %.2lf\n",
		 payable_to_escaped,
		 dollar_amount );

	fprintf( latex_file,
"\\end{tabular}\n\n"
"\\vspace{0.20in}\n\n"
"\\begin{tabular}{p{0.85in}l}\n" );

	fprintf( latex_file,
"& %s\n",
		 dollar_text_string );

	fprintf( latex_file,
"\\end{tabular}\n\n"
"\\vspace{0.52in}\n\n"
"\\begin{tabular}{p{0.30in}l}\n" );

	fprintf( latex_file,
"& %s\n",
		 memo );

	fprintf( latex_file,
"\\end{tabular}\n"
"\\end{minipage}\n"
"\\end{sideways}\n" );

} /* make_checks_dollar_text() */

