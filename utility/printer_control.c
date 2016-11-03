/* utility/printer_control.c */
/* ------------------------- */
/* This program takes as input:
        1) application                  (command line)
        2) printer destination          (command line)
        3) a report                     (stdin)

   Is outputs: The report with the control codes necessary
               to control the printer.
   ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "name_arg.h"
#include "timlib.h"
#include "printer_control.h"

/* Global Variables */
/* ---------------- */
char *application;
char *printer;

int main( int argc, char **argv )
{

        int index;

        setup_command_line_argument( argc, argv );

        index = get_printer_index( application, printer );

        if( index == -1 )
        {
                fprintf( stderr,
                "This application (%s) isn't available for your printer.\n",
                         application );
                exit( 1 );
        }


        output_initialize_codes( index );
        pump_stdin_to_output();
        output_reset_codes( index );

	return 0;

} /* main() */


void output_reset_codes( int index )
{
        char *printer_reset_str;

        printer_reset_str = get_printer_reset_string( index );
        convert_tilda_to_esc( printer_reset_str );
        convert_carrot_to_control( printer_reset_str );
        printf( "%s", printer_reset_str );
        /* printf( "%c", 12 ); */

} /* output_reset_codes() */


void output_initialize_codes( int index )
{
        char *printer_init_str;

        printer_init_str = get_printer_init_string( index );
        convert_tilda_to_esc( printer_init_str );
        convert_carrot_to_control( printer_init_str );
        printf( "%s", printer_init_str );

} /* output_initialize_codes() */

void pump_stdin_to_output( void )
{
        char buffer[ 1024 ];

        while( get_line( buffer, stdin ) )
        {
                printf( "%s\r\n", buffer );
        }
}

void setup_command_line_argument( int argc, char **argv )
{
        NAME_ARG *arg = init_arg( argv[ 0 ] );
        int ticket;

        ticket = add_valid_option(arg, "printer");
        set_printer_from_table( arg, ticket );

        ticket = add_valid_option(arg, "application");
        set_application_from_table( arg, ticket );

        ins_all( arg, argc, argv);

        printer = fetch_arg(arg, "printer");
        application = fetch_arg(arg, "application");

} /* setup_command_line_argument() */

void set_application_from_table( NAME_ARG *arg, int ticket )
{
        int i;
        char *duplicate_table[ MAX_DUPLICATE_TABLE ];
        int number_in_duplicate_table = 0;


        for( i = 0; printer_table[ i ].application; i++ )
        {
                if ( !in_duplicate_table(     duplicate_table,
                                              number_in_duplicate_table,
                                              printer_table[ i ].application ) )

                {
                        add_valid_value( arg,
                                         ticket,
                                         printer_table[ i ].application );

                        add_to_duplicate_table(duplicate_table,
                                               &number_in_duplicate_table,
                                               printer_table[ i ].application );

                }
        }
} /* set_application_from_table() */

void set_printer_from_table( NAME_ARG *arg, int ticket )
{
        int i;
        char *duplicate_table[ MAX_DUPLICATE_TABLE ];
        int number_in_duplicate_table = 0;

        for( i = 0; printer_table[ i ].application; i++ )
        {
                if ( !in_duplicate_table(       duplicate_table,
                                                number_in_duplicate_table,
                                                printer_table[ i ].printer ) )
                {
                        add_valid_value(        arg,
                                                ticket,
                                                printer_table[ i ].printer );

                        add_to_duplicate_table( duplicate_table,
                                                &number_in_duplicate_table,
                                                printer_table[ i ].printer );
                }
        }

} /* set_printer_from_table() */



void add_to_duplicate_table(
			char **duplicate_table,
                        int *number_in_duplicate_table,
                        char *key_to_search )
{
        duplicate_table[ *number_in_duplicate_table ] = key_to_search;
        (*number_in_duplicate_table)++;

} /* add_to_duplicate_table() */



int in_duplicate_table( char **duplicate_table,
                        int number_in_duplicate_table,
                        char *key_to_search )
{
        int i;

        for( i = 0; i < number_in_duplicate_table; i++ )
        {
                /* If exists then return true */
                /* -------------------------- */
                if ( strcmp(    duplicate_table[ i ],
                                key_to_search ) == 0 )
                {
                        return 1;
                }
        }

        /* Not there, return false */
        /* ----------------------- */
        return 0;

} /* in_duplicate_table() */

void convert_tilda_to_esc( char *str )
{
        while( *str )   
	{
                if ( *str == '~' ) *str = 27;
                str++;
        }
}

int get_printer_index( char *application, char *printer )
{
        int i;

        for ( i = 0; printer_table[ i ].application; i++ )
        {
                if ( strcmp(    printer_table[ i ].application,
                                application ) == 0
                &&   strcmp(    printer_table[ i ].printer,
                                printer ) == 0 )
                {
                        return i;
                }
        }

        return -1;

} /* get_printer_index() */

char *get_printer_init_string( int index )
{

        if( index == -1 )
        {
                fprintf( stderr, "Invalid index: printer_init\n");
                exit( 1 );
        }

        return strdup( printer_table[ index ].printer_str );
        /* return ( printer_table[ index ].printer_str ); */
}


char *get_printer_reset_string( int index )
{
        if( index == -1 )
        {
                fprintf( stderr, "Invalid index: printer_reset\n" );
                exit( 1 );
        }

        return strdup( printer_table[ index ].reset_str );
        /* return ( printer_table[ index ].reset_str ); */
}


/* Assume s = "^18ABC" */
/* ------------------- */
void convert_carrot_to_control( char *s )
{
        int control_code;
        int control_code_string_length;

        while ( *s )
        {
                if ( *s == '^' )
                {
                        /* Assign the control_code by skipping the ^ */
                        /* ----------------------------------------- */
                        control_code = atoi( s + 1 );

                        /* Embed control code on top of carrot */
                        /* ----------------------------------- */
                        *s = control_code;

                        /* Find the length of the control code string */
                        /* ------------------------------------------ */
                        if ( control_code < 10 )
                                control_code_string_length = 1;
                        else
                                control_code_string_length = 2;

                        /* From: ^18ABC To: ^ABC */
                        /* --------------------- */
                        strcpy( s + 1, s + control_code_string_length + 1 );
                }
                s++;
        }
} /* convert_carrot_to_control() */
