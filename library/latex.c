/* library/latex.c					   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "timlib.h"
#include "latex.h"

LATEX *latex_new_latex(		char *tex_filename,
				char *dvi_filename,
				char *working_directory,
				boolean landscape_flag,
				char *logo_filename )
{
	LATEX *latex;
	char full_path[ 128 ];

	latex = (LATEX *)calloc( 1, sizeof( LATEX ) );
	if ( !latex )
	{
		fprintf(	stderr,
				"ERROR in %s/%s(): memory allocation error.\n",
				__FILE__,
				__FUNCTION__ );
		exit( 1 );
	}

	if ( working_directory && *working_directory )
	{
		sprintf(full_path,
			"%s/%s",
			working_directory,
			tex_filename );
	}
	else
	{
		strcpy( full_path, tex_filename );
	}

	latex->output_stream = fopen( full_path, "w" );
	if ( !latex->output_stream )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): cannot open %s for write.\n",
			 __FILE__,
			 __FUNCTION__,
			 full_path );
		exit( 1 );
	}

	latex->table_list = list_new_list();
	latex->dvi_filename = dvi_filename;
	latex->tex_filename = tex_filename;
	latex->working_directory = working_directory;
	latex->landscape_flag = landscape_flag;
	latex->logo_filename = logo_filename;

	return latex;
} /* latex_new_latex() */

LATEX_TABLE *latex_new_latex_table( char *caption )
{
	LATEX_TABLE *latex_table;

	latex_table = (LATEX_TABLE *)calloc( 1, sizeof( LATEX_TABLE ) );
	if ( !latex_table )
	{
		fprintf(	stderr,
				"ERROR in %s/%s(): memory allocation error.\n",
				__FILE__,
				__FUNCTION__ );
		exit( 1 );
	}
	latex_table->caption = caption;
	return latex_table;
} /* latex_new_latex_table() */

LATEX_TABLE_HEADING *latex_new_latex_table_heading( void )
{
	LATEX_TABLE_HEADING *latex_table_heading;

	latex_table_heading =
		(LATEX_TABLE_HEADING *)
			calloc( 1, sizeof( LATEX_TABLE_HEADING ) );
	if ( !latex_table_heading )
	{
		fprintf(	stderr,
				"ERROR in %s/%s(): memory allocation error.\n",
				__FILE__,
				__FUNCTION__ );
		exit( 1 );
	}
	return latex_table_heading;
} /* latex_new_latex_table_heading() */

void latex_output_table_footer( FILE *output_stream )
{
	fprintf( output_stream,
		 "\n\\hline\n\\end{longtable}\n" );
}

void latex_output_document_footer( FILE *output_stream )
{
	fprintf( output_stream,
		 "\\end{document}\n" );
}

void latex_output_document_heading(	FILE *output_stream,
					boolean landscape_flag,
					boolean table_package_flag,
					char *logo_filename )
{
/*
	fprintf( output_stream,
		 "\\documentclass[letterpaper,12pt" );

	if ( landscape_flag )
	{
		fprintf( output_stream,
			 ",landscape" );
	}

	fprintf( output_stream,
		 "]{article}\n" );

	fprintf( output_stream, "\\usepackage[dvipdfmx]{graphicx}\n" );
	fprintf( output_stream, "\\usepackage{bmpsize}\n" );
	fprintf( output_stream, "\\usepackage{graphics}\n" );

	fprintf( output_stream,
"\\documentclass{letter}\n"
"\\usepackage{graphics}\n" );
*/

	fprintf( output_stream,
"\\documentclass{report}\n"
"\\usepackage{graphics}\n" );

	if ( landscape_flag )
	{
		fprintf( output_stream,
			 "\\special{landscape}\n" );
	}

	if ( table_package_flag )
	{
/*
		fprintf( output_stream,
			 "\\usepackage{longtable}\n"
			 "\\usepackage[  margin=2.5cm,"
                			 "vmargin={0pt,0.5cm},"
                			 "nohead]{geometry}\n" );
*/

		if ( WITH_CAPTION )
		{
			fprintf(
			output_stream,
			"\\usepackage[labelformat=empty]{caption}\n"
			"\\usepackage{longtable}\n"
			"\\usepackage[  margin=1.5cm,"
                			 "vmargin=1.5cm,"
                			 "nohead]{geometry}\n" );
		}
		else
		{
			fprintf(
			output_stream,
			 "\\usepackage{longtable}\n"
			 "\\usepackage[  margin=1.5cm,"
                			 "vmargin=1.5cm,"
                			 "nohead]{geometry}\n" );
		}
	}

	fprintf( output_stream,
		 "\\begin{document}\n" );

	if ( logo_filename && *logo_filename )
	{
		fprintf( output_stream,
"\\begin{center}\n"
"\\includegraphics{%s}\n"
"\\end{center}\n",
		 	 logo_filename );
	}

} /* latex_output_document_heading() */

void latex_output_table_heading(	FILE *output_stream,
					char *caption,
					LIST *heading_list )
{
	LATEX_TABLE_HEADING *latex_table_heading;
	int first_time;
	char buffer[ 128 ];

	if ( !WITH_CAPTION )
	{
		if ( caption && *caption )
		{
			fprintf(output_stream,
				"\\begin{center} { \\bf %s} \\end{center}\n",
				caption );
		}
	}

	fprintf( output_stream,
		 "\\begin{longtable}{|" );

	if ( !list_rewind( heading_list ) ) return;

	do {
		latex_table_heading = list_get_pointer( heading_list );

		if ( latex_table_heading->paragraph_size )
		{
			fprintf(output_stream,
				"p{%s}|",
				latex_table_heading->paragraph_size );
		}
		else
		if ( latex_table_heading->right_justified_flag )
			fprintf( output_stream, "r|" );
		else
			fprintf( output_stream, "l|" );

	} while( list_next( heading_list ) );

	fprintf( output_stream, "} \\hline\n" );

	if ( WITH_CAPTION )
	{
		if ( caption && *caption )
		{
			fprintf(	output_stream,
					"\\caption {%s} \\\\ \\hline\n",
					caption );
		}
	}

	list_rewind( heading_list );

	first_time = 1;
	do {
		latex_table_heading = list_get_pointer( heading_list );

		if ( first_time )
			first_time = 0;
		else
			fprintf( output_stream, " & " );

		if ( latex_table_heading->heading
		&&   *latex_table_heading->heading )
		{
			fprintf( output_stream,
				 "%s",
				 format_initial_capital(
					buffer,
				 	latex_table_heading->heading ) );
		}

	} while( list_next( heading_list ) );

	fprintf( output_stream, "\\\\ \\hline \\hline \\endhead\n" );

	fprintf(output_stream,
"\\hline \\multicolumn{2}{r}{\\textit{Continued on next page}} \\\\\n" );

 	fprintf( output_stream, "\\hline\n" );
	fprintf( output_stream, "\\endfoot\n" );
	fprintf( output_stream, "\\endlastfoot\n" );

} /* latex_output_table_heading() */

void latex_output_table_row_list(	FILE *output_stream,
					LIST *row_list )
{
	int first_time;
	char *column_data;
	LATEX_ROW *row;
	char buffer[ 1024 ];

	if ( !row_list || !list_rewind( row_list ) ) return;

	do {
		row = list_get_pointer( row_list );

		if ( !row->column_data_list
		||   !list_rewind( row->column_data_list ) )
		{
			continue;
		}

		first_time = 1;
		do {
			column_data = list_get_pointer( row->column_data_list );

			if ( first_time )
				first_time = 0;
			else
				fprintf( output_stream, " & " );

			if ( column_data )
			{
				timlib_strcpy( buffer, column_data, 1024 );

				search_replace_string(
					buffer,
					"#",
					"Number " );

				search_replace_string(
					buffer,
					"&",
					"\\&" );

/*
				search_replace_string(
					buffer,
					"$",
					"\\$" );
*/

				search_replace_string(
					buffer,
					"%",
					"\\%" );

				fprintf(output_stream,
					"%s",
					buffer );
			}

		} while( list_next( row->column_data_list ) );
		fprintf( output_stream, "\\\\\n" );
	} while( list_next( row_list ) );

} /* latex_output_table_row_list() */

LATEX_ROW *latex_new_latex_row( void )
{
	LATEX_ROW *row;

	row = (LATEX_ROW *)calloc( 1, sizeof( LATEX_ROW ) );
	if ( !row )
	{
		fprintf( stderr,
			 "ERROR in %s/%s(): memory allocation\n",
			 __FILE__,
			 __FUNCTION__ );
		exit( 1 );
	}
	row->column_data_list = list_new_list();
	return row;
} /* latex_new_latex_row() */

void latex_tex2pdf(	char *tex_filename,
			char *working_directory )
{
	char sys_string[ 1024 ];

	chdir( working_directory );

/*
	sprintf( sys_string,
		 "latex %s",
		 tex_filename );
fprintf( stderr, "%s\n", sys_string );
	system( sys_string );

	sprintf( sys_string,
		 "dvipdf %s",
		 dvi_filename );
fprintf( stderr, "%s\n", sys_string );
	system( sys_string );
*/

	sprintf( sys_string,
		 "pdflatex %s 1>&2",
		 tex_filename );

	/* Need to run it several times because header widths may be wrong. */
	/* ---------------------------------------------------------------- */
	system( sys_string );
	system( sys_string );
	system( sys_string );

} /* latex_tex2pdf() */

void latex_table_output(	FILE *output_stream,
				boolean landscape_flag,
				LIST *table_list,
				char *logo_filename )
{
	LATEX_TABLE *table;

	latex_output_document_heading(	output_stream,
					landscape_flag,
					1 /* table_package_flag */,
					logo_filename );

	if ( !list_rewind( table_list ) ) return;

	do {
		table = list_get_pointer( table_list );

		latex_output_table_heading(
					output_stream,
					table->caption,
					table->heading_list );

		latex_output_table_row_list(
					output_stream,
					table->row_list );

		latex_output_table_footer(
					output_stream );

	} while( list_next( table_list ) );

	latex_output_document_footer( output_stream );

} /* latex_table_output() */

