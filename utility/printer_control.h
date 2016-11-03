/* utility/printer_control.h */
/* ------------------------- */

#ifndef PRINTER_CONTROL_H
#define PRINTER_CONTROL_H

#define MAX_DUPLICATE_TABLE     100

struct
{
        char *application;
        char *printer;
        char *printer_str;
        char *reset_str;
} printer_table[]=
        {
                /* Key */
                /* --- */
               {"210report",
                "hp",
                        /* Control codes */
                        /* ------------- */
                        "~E~&l1O~(0U~(s0p18.75h6v0s0b6T~&k6.00H~&l8D",
                        "~E"},
                /* Key */
                /* --- */
               {"134report",
                "hp",
                        /* Control codes */
                        /* ------------- */
                        "~E~&k2S",
                        "~E"},
                /* Key */
                /* --- */
               {"80report",
                "hp",
                        /* Control codes */
                        /* ------------- */
                        "~E",
                        "~E"},
                /* Delimiter */
                /* --------- */
               {(char *)0,
                "",
                        "",
                        ""}
        };


/* Operations */
/* ---------- */
void output_reset_codes( int index );
void output_initialize_codes( int index );
void pump_stdin_to_output( void );
void setup_command_line_argument( int argc, char **argv );
void set_application_from_table( NAME_ARG *arg, int ticket );
void set_printer_from_table( NAME_ARG *arg, int ticket );
void convert_tilda_to_esc( char *str );
int get_printer_index( char *application, char *printer );
char *get_printer_init_string( int index );
char *get_printer_reset_string( int index );
void convert_carrot_to_control( char *s );
void add_to_duplicate_table(
			char **duplicate_table,
                        int *number_in_duplicate_table,
                        char *key_to_search );
int in_duplicate_table( char **duplicate_table,
                        int number_in_duplicate_table,
                        char *key_to_search );
#endif
