/* timroutines.h */
/* ------------- */

#ifndef ROUTINES_H
#define ROUTINES_H

/* ---------------- */
/* General Routines */
/*                  */
/* Tim Riley        */
/* ---------------- */

#define USING_MAX_BUFFER        512

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <time.h>

/* Function Prototypes For General Routines */
/* ---------------------------------------- */
char *piece_punct(      char *  /* string */,
                        int     /* fld_num */);

char *center(           char *  /* string */,
                        int     /* scr_len */);

char *right(            char *  /* string */,
                        char *  /* substr */,
                        int     /* scr_len */ );

int get_line(           char *  /* inline */,
                        FILE *  /* infile */);

int instr(              char *  /* substr */,
                        char *  /* string */,
                        int     /* occurrence */);

char *tim_substr(       char *  /* string */,
                        int     /* start */,
                        int     /* how_many */);

FILE *open_file(        char *  /* fname */,
                        char *  /* mode */ );

FILE *open_pipe(        char *  /* pname */,
                        char *  /* mode */ );

char *column(           int     /* col_num */,
                        char *  /* string */,
                        char *  /* buffer */ );

char *delete_str(       char *  /* string */,
                        int     /* start */,
                        int     /* num_chars */ );

char *insert_str(       char *  /* sub */,
                        char *  /* string */,
                        int     /* pos */);

char *search_replace(   char *  /* search_str */,
                        char *  /* replace_str */,
                        char *  /* looking_string */ );

bye(                    char *  /* msg */ );


char *draw_line(        char    /* ch */,
                        int     /* num_chars */,
                        char *  /* buffer */ );

double int_div_int(     int, int );
int perc_of(            int, int );

char *pad_zero(         int, int );

char *get_token(        char ** );
mail_tim(               int     /* output_also */,
                        char *, ... );
char *OraDate2StdDate(  char *  /* OraDate */ );
char *ReplaceAllOraDateWithStdDate(
                        char *, /* in_str */
                        char *  /* RetStr */ );

#endif /* not defined ROUTINES_H */


