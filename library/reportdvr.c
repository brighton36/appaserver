/* invest2:$ROOT/library/tim/reportdvr.c */
/* ------------------------------------- */

/* ---------------------------------- */
/* Test driver program for REPORT ADT */
/*                                    */
/* To compile: make reportdvr.e       */
/*                                    */
/* Tim Riley                          */
/* ---------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include "report.h"
#include "using.h"

char *fmt_header =
"~sssssssss ~SSSSSS ~SSSSSSS ~ssssssssssssssssssssssss ~ssssssssssssssssssssss";


char *fmt_line =
"~sssssssss ~DDDDDD ~FFFF.FF ~ssssssssssssssssssssssss ~ssssssssssssssssssssss";


void test_report_2();

int main()
{
	/* test_report_1(); */
	test_report_2();
	return 0;
}
	
void test_report_2()
{
        char head_line_buffer1[ 100 ];
        char head_line_buffer2[ 100 ];
        char report_line_buffer1[ 100 ];

        int x;

        REPORT *report = init_report();

	set_page_number( report, 5 );
	set_supress_end_of_report( report );
	set_suppress_date( report );
	set_page_number_to_footer( report, "Test footer" );

        set_report_width( report, 80 );
        set_lines_per_page( report, 54 );
        set_title( report, "Report Title Line 1", CENTER );
        set_title( report, "Report Title Line 2", CENTER );

        set_header( report, using (     head_line_buffer1,
                                        fmt_header,
                                        "",
                                        "Integer",
                                        "Float",
                                        "","" ) );

        set_header( report, using (     head_line_buffer2,
                                        fmt_header,
                                        "Call Date",
                                        "Amount",
                                        "Amount",
                                        "Other Information",
                                        "Additional Details" ) );


        open_report_outfile( report, "reportdvr.out" );
        start_title_header( report );

        for ( x = 0; x < 600; x++ )
        {
                output_line( report, using(     report_line_buffer1,
                                                fmt_line,
                                                "01-JAN-92",
                                                x,
                                                12.4,
                                                "Info, Info, Info, Info",
                                                "Detail, Detail, Detail" ) );
                increment_count( report );
        }
        end_of_report( report, WITH_CLOSE );

        printf( "reportdvr.out created\n" );
}

#ifdef NOT_DEFINED
test_report_1()
{
        char head_line_buffer1[ 100 ];
        char head_line_buffer2[ 100 ];
        char report_line_buffer1[ 100 ];

        int x;

        REPORT *report = init_report();

        set_report_width( report, 80 );
        set_lines_per_page( report, 54 );
        set_title( report, "Report Title Line 1", CENTER );
        set_title( report, "Report Title Line 2", CENTER );

        set_header( report, using (     head_line_buffer1,
                                        fmt_header,
                                        "",
                                        "Integer",
                                        "Float",
                                        "","" ) );

        set_header( report, using (     head_line_buffer2,
                                        fmt_header,
                                        "Call Date",
                                        "Amount",
                                        "Amount",
                                        "Other Information",
                                        "Additional Details" ) );


        open_report_outfile( report, "reportdvr.out" );
        start_title_header( report );

        for ( x = 0; x < 600; x++ )
        {
                output_line( report, using(     report_line_buffer1,
                                                fmt_line,
                                                "01-JAN-92",
                                                x,
                                                12.4,
                                                "Info, Info, Info, Info",
                                                "Detail, Detail, Detail" ) );
                increment_count( report );
        }
        end_of_report( report, WITH_CLOSE );

        printf( "reportdvr.out created\n" );
}
#endif

