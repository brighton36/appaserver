/* /usr2/lib/report.c 					  */
/* ------------------------------------------------------
These routines support the REPORT ADT.

Tim Riley
--------------------------------------------------------- */

#include <stdio.h>
#include <string.h>

#include "report.h"
#include "timlib.h"
#include "piece.h"

REPORT *init_report()
{
        char in_line[ 30 ];
        char buffer[ 10 ];
        FILE *fp;
        REPORT *ret_report = ( REPORT *)calloc( 1, sizeof( REPORT ) );

        if ( !ret_report )
                report_bye ("malloc failed in init_report()" );

        ret_report->num_header_lines = 0;
        ret_report->num_title_lines = 0;
        ret_report->curr_line = 0;
        ret_report->page_num = 0;
        ret_report->outfile = ( FILE *)0;
        ret_report->output_to_file = 0;
        ret_report->report_count = 0;
        ret_report->form_feed_ok = 1;
        ret_report->num_leading_spaces = 0;

	/* Get date */
	/* -------- */
        fp = popen( "date '+%D %T'", "r" );
        fgets( in_line, 30, fp );
	in_line[ strlen( in_line ) - 1 ] = '\0';
        pclose( fp );

        ret_report->date_stamp = strdup( piece( buffer, ' ', in_line, 0 ) );
        ret_report->time_stamp = strdup( piece( buffer, ' ', in_line, 1 ) );

        return ret_report;

} /* init_report() */


set_lines_per_page( REPORT *report, int lines_per_page )
{
        report->lines_per_page = lines_per_page;

} /* set_lines_per_page() */


report_free( REPORT *report )
{
	free( report );
	return 1;
} /* report_free() */

set_report_width( REPORT *report, int report_width )
{
        int x;
        char buffer[ 80 ];
        char *line_ptr = report->line_str;

        if ( report_width > MAX_REPORT_WIDTH )
        {
                sprintf( buffer, "max report width: %d\n", MAX_REPORT_WIDTH );
                report_bye( buffer );
        }

        /* Set underline */
        /* ------------- */
        for ( x = 0; x < report_width; x++ )
                *line_ptr++ = '-';

        *line_ptr = '\0';

        report->report_width = report_width;

} /* set_report_width() */



increment_count( REPORT *report )
{
        report->report_count++;
}


reset_page_num( REPORT *report )
{
        report->page_num = 0;

} /* reset_page_num() */


reset_title( REPORT *report )
{
        report->num_title_lines = 0;

} /* reset_title() */


set_title( REPORT *report, char *title, int is_centered )
{
        if ( report->num_title_lines == MAX_HEADER_LINES )
                report_bye( "Max title lines exceeded" );

        report->title_line[ report->num_title_lines ].title_line_str = title;
        report->title_line[ report->num_title_lines ].is_centered = is_centered;


        report->num_title_lines++;

} /* set_title() */


reset_header( REPORT *report)
{
        report->num_header_lines = 0;

} /* reset_header() */



set_header ( REPORT *report, char *header_line )
{
        if ( report->num_header_lines == MAX_HEADER_LINES )
                report_bye ("Max header lines exceeded");

        report->header_line[ report->num_header_lines++ ] = header_line;

} /* set_header() */


set_no_form_feed( REPORT *report )
{
        report->form_feed_ok = 0;

} /* set_no_form_feed() */


set_num_leading_spaces( REPORT *report, int num_spaces )
{
        report->num_leading_spaces = num_spaces;

} /* set_num_leading_spaces() */


open_report_pipe( REPORT *report, char *pipe_str )
{
        report->outfile = popen( pipe_str, "w" );

	if ( !report->outfile ) report_bye( "open_report_pipe failed" );

        report->page_num = 0;
        report->output_to_file = 0;

} /* open_report_pipe() */


open_report_outfile( REPORT *report, char *outfile_str )
{
        report->outfile = fopen( outfile_str, "w" );

	if ( !report->outfile ) report_bye( "open_report_outfile failed" );

        report->page_num = 0;
        report->output_to_file = 1;

} /* open_report_outfile() */


start_title_header( REPORT *report )
{

        output_title_no_ff( report );
        return output_header( report );

} /* start_title_header() */



output_title_ff( REPORT *report )
{
        output_form_feed( report );
        output_title_no_ff( report );
}


static output_title_center( REPORT *report, int x )
{
	char *date_stamp = get_date_stamp( report );

        if ( x == 0 )
                fprintf( report->outfile, "%s%s %4d\n",
		date_stamp,
                right( 
		   center(
			report->title_line[ x ].title_line_str,
                        report->report_width - strlen( date_stamp ) * 2 
			),
                   "Page",
                   report->report_width - ( 6 + strlen( date_stamp ) ) ),
                ++report->page_num );
        else
                fprintf( report->outfile, "%s\n",
                        center( report->title_line[ x ].title_line_str,
                                report->report_width ) );

} /* output_title_center() */


static output_title_left( REPORT *report, int x )
{
        if ( x == 0 )
                fprintf( report->outfile, "%s %4d\n",
                        right( report->title_line[ x ].title_line_str,
                        "Page",
                        report->report_width - 6 ),
                        ++report->page_num );
        else
                fprintf( report->outfile, "%s\n",
                        report->title_line[ x ].title_line_str );

} /* output_title_left() */


output_title_no_ff( REPORT *report )
{
        int x;

        if ( !report->outfile ) report->outfile = stdout;

        report->curr_line = 0;

        for( x = 0; x < report->num_title_lines; x++ )

                if ( report->title_line[ x ].is_centered )
                        output_title_center( report, x );
                else
                        output_title_left( report, x );

        fprintf( report->outfile, "\n");

        report->curr_line += ( report->num_title_lines + 1 );

} /* output_title_no_ff() */



group_next( REPORT *report, int num_group )
{
        int diff =      report->curr_line +
                        num_group -
                        report->lines_per_page;

        if ( diff >= 0 )
        {
                output_title_ff( report );
                output_header( report );
        }

	return 1;

} /* group_next() */



output_header( REPORT *report )
{
        int x,y;

        if ( !report->outfile ) report->outfile = stdout;

        for( x = 0; x < report->num_header_lines; x++ )
        {
                for ( y = 0; y < report->num_leading_spaces; y++ )
                        putc( ' ', report->outfile );

                fprintf( report->outfile, "%s\n", report->header_line[ x ] );
        }

        report->curr_line += report->num_header_lines;
	fflush( report->outfile );
        return output_line_str( report );

} /* output_header() */


output_line_str( REPORT *report )
{
        return output_line( report, report->line_str );

} /* output_line_str() */


output_line( REPORT *report, char *line )
{
        int x;

        if ( !report->outfile ) report->outfile = stdout;

        if ( report->curr_line >= report->lines_per_page - 1 )
        {
                /* Prevent orphans and widows */
                /* -------------------------- */
                if ( *line )
                {
                        output_title_ff( report );
                        output_header( report );
                }
        }


        for ( x = 0; x < report->num_leading_spaces; x++ )
                putc( ' ', report->outfile );

        fprintf( report->outfile, "%s\n", line );
	fflush( report->outfile );
        report->curr_line++;
	return 1;

} /* output_line() */



output_form_feed( REPORT *report )
{
        if ( !report->outfile ) report->outfile = stdout;

        if ( report->form_feed_ok )
                fprintf( report->outfile, "%c", 12 );

        report->curr_line = 0;

} /* output_form_feed() */


char *get_date_stamp( REPORT *report )
{
        return report->date_stamp;

} /* get_date_stamp() */


char *get_time_stamp( REPORT *report )
{
        return report->time_stamp;

} /* get_time_stamp() */


end_of_report( REPORT *report, int with_close )
{
        char buffer[ 100 ];

        output_line( report, "" );

        if ( report->report_count )
                sprintf( buffer, "Report Count: %d  ", report->report_count );
        else
                strcpy( buffer, "End of Report" );

        strcat( buffer, "         " );
        strcat( buffer, get_date_stamp( report ) );
        strcat( buffer, " " );
        strcat( buffer, get_time_stamp( report ) );

        output_line( report, buffer );

        if ( with_close )
	{
                if ( report->output_to_file )
		{
			if ( report->outfile != stdout )
                        	fclose( report->outfile );
		}
                else
		{
                        pclose( report->outfile );
		}
	}
	return 1;

} /* end_of_report() */


report_bye( char *s )
{
        fprintf( stderr, "%s\n", s );
        exit( 1 );
}
