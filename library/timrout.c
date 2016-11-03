/* timroutines.c */
/* ------------- */

/* ------------------------
General library routines.

Tim Riley
--------------------------- */

#include "timrout.h"

#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdarg.h>
#include <malloc.h>



/*********************************************************************/
/*RIGHT()

Description:    This function will append 'string2' to the end of
                'string1'.  In addition, it will fill enough spaces to
                flush 'string2' to the right column.

Syntax:         printf("%s\n",right(string1,string2,scr_len));
                                or
                string = right(string1,string2,scr_len);

Parameters:     'string2' will be appended to 'string1'.
                'scr_len' (an integer) is the number of columns to the
                right the text will end.

Example:        char *string1 =
"                           Title of Report";

                char *string = right(string,"Page",77);
                printf("%s",string);

                            Title of Report                          Page

Note:           Right() works with Center() to produce report headings.

                fprintf(file1,"%s %d\n",right(center("Title of Report",132),
                                "Page",128),++page_num);

Note:           This function returns static memory.
*/



char *right( string, substr, scr_len)
char *string, *substr;
int scr_len;
{
        static char line[ 200 ];
        int str_len = strlen( substr );
        int x;

        strcpy(line,string);

        /* insert just enough spaces */
        for( x = strlen( string );x < scr_len - str_len; x++)
                line[x] = ' ';

        line[x] = '\0';
        strcat(line,substr);
        return line;

} /* right() */





/****************************************************************/
/*CENTER()

Description:    This function pads just enough spaces in front of
                a string to center the text.

Syntax:         printf("%s\n",center(string,scr_len));
                                  or
                char *heading = center(string,scr_len);

Parameters:     'scr_len' (an integer) is the number of columns the
                screen or report spans.  It is usually 80 or 132.

Example:        char *string = "Here is a string";
                string = center(string,80);
                printf("%s",string);

                           Here is a string

Note: This function returns static memory.
*/

char *center( string, scr_len )
char *string;
int scr_len;
{
        static char line[ 500 ];
        int x,y;
        int str_len_div_two = strlen( string ) / 2;
        int scr_len_div_two = scr_len / 2;

        /* fill lead with blanks */
        /* --------------------- */
        for (x = 0;x < scr_len_div_two - str_len_div_two; x++)
                line[x] = ' ';

        /* place string into line */
        /* ---------------------- */
        line[x] = '\0';
        strcat(line,string);
        return line;

} /* center() */







/********************************************************************/
/*GET_LINE()
        This function gets a line from the file passed to it.

        If successful it returns 1.
        If all done it returns 0.
*/


#define EOLN    10

int get_line( inline, infile )
char *inline;
FILE *infile;
{
        int in_char;

        *inline = '\0';
        while (1){
                in_char = fgetc(infile);

                if (in_char == EOF || in_char == EOLN){

                        if (in_char == EOF && *inline == '\0')
                                return 0;

                        *inline = '\0';
                        return 1;
                }
                *inline++ = in_char;
        }
} /* get_line */




/*************************************************************/
/*INSTR()

Description:    This function searches for a sub-string in a string
                and returns the position found.  If the sub-string is
                not found, it returns -1.

Syntax:         instr(substr,string,occurrence);

Parameters:     'substr' (a string) will be search for in 'string' starting
                at the 'occurrence' occurrence.

Example:        printf("%d\n",instr("/","12345/67",1);
                prints 5

                printf("%d\n",instr("/",123/456/77/88",2);
                prints 7

Note:           The return value is zero based, and occurrence is one based.
                In other words, if 'substr' is located at the beginning of
                'string', the return value is 0, not 1.  The 'occurrence'
                parameter must be at least 1.
*/

int instr(substr,string,occurrence)
char *substr,*string;
int occurrence;
{
        int x,found;
        int str_len_str = strlen(string);
        int str_len_sub = strlen(substr);

        for(x = 0,found = 0;x < str_len_str;x++)
                if (strncmp(&string[x],substr,str_len_sub) == 0)
                        if (++found == occurrence)
                                return x;
        return -1;
}






/****************************************************************/
/*TIM_SUBSTR()

Description:            This function returns a sub-string of the string
                        passed as a parameter.

Syntax:                 printf("%s\n",tim_substr(string,start,how_many));
                                              or
                        char *sub = tim_substr(string,start,how_many);

Parameters:             'string' is the string parameter.

                        'start' (an integer) is the offset (starting position)
                        of where to start extracting. Note: 'start' is zero
                        based which means zero refers to the start of the
                        string.

                        'how_many' (an integer) is the number of characters
                        to extract.

Example:                printf("%s\n",tim_substr('0123456789",3,5));
                        will print '34567'.

                        printf("%s\n",tim_substr('0123456789",0,5));
                        will print '01234'.

Note: Static memory is returned.
*/

char *tim_substr(string,start,how_many)
char *string;
int start,how_many;
{
        int x,cnt;
        int str_len = strlen( string );
        static char ret_str[ 100 ];

        for (x = start,cnt = 0;  x < str_len && cnt < how_many;cnt++,x++)
                ret_str[ cnt ] = string[ x ];

        ret_str[cnt] = '\0';
        return ret_str;

} /* tim_substr() */



FILE *open_file ( fname, mode )
char *fname, *mode;
{
        FILE *ret_file;

        ret_file = fopen ( fname, mode );
        if (!ret_file){
                fprintf(stderr, "File open error: %s\n", fname );
                exit(1);
        }
        return ret_file;

} /* open_file() */





/*************************************************************/
/*COLUMN()

Description:    This function returns a sub string in a string.

Syntax:         column( col_num, string, buffer);

Parameters:     'col_num' is the zero based column number to return.
                'string' is the search string.
                'buffer' is where the sub string is returned.

Example:        printf("%s\n",column(2,"zero one two three four",buffer);
                prints two

                printf("%s\n",column(1,"  The   Line of     Text", buffer);
                prints Line
*/

char *column( col_num, string, buffer)
int col_num;
char *string, *buffer;
{
        char *skip_blanks();
        char *skip_chars();
        int this_column;

        this_column = 0;
        *buffer = '\0';

        /* If "  abc def ghi" */
        /* ------------------ */
        if ( isspace( *string ) )
                string = skip_blanks( string );

        while( *string ){

                if (this_column++ == col_num){
                        get_str( string, buffer );
                        return buffer;
                }
                string = skip_chars( string );
                string = skip_blanks( string );
        }
        return buffer;
}

static get_str( string, buffer)
char *string, *buffer;
{
        while ( *string && !isspace( *string ) )
                *buffer++ = *string++;

        *buffer = '\0';

} /* get_str() */


static char *skip_blanks( string )
char *string;
{
        while ( *string && isspace( *string ) )
                string++;

        return string;

} /* skip_blanks() */


static char *skip_chars( string )
char *string;
{
        while ( *string && !isspace( *string ) )
                string++;

        return string;

} /* skip_chars() */

/* ------------------- */
/* End of Column Block */
/* ------------------- */


char *delete_str( string, start, num_chars)
char *string;
int start,num_chars;
{
        int far_str;

        for(far_str = start + num_chars;
                far_str <= strlen(string);far_str++,start++)
                        string[start] = string[far_str];
        return string;

} /* delete_str() */


char *insert_str(sub,string,pos)
char *sub,*string;
int pos;
{
        int x;
        char *temp = malloc(strlen(string) + strlen(sub) + 2);

        temp[0] = '\0';

        /* copy first part over */
        strncpy(temp,string,pos);
        temp[pos] = '\0';

        /* place sub in middle */
        strcat(temp,sub);

        /* copy rest of string */
        strcat(temp,&string[pos]);

        /* copy entire string back to original */
        strcpy(string,temp);

        free(temp);
        return string;

} /* insert_str() */



char *search_replace(search_str,replace_str,looking_string)
char *search_str,*replace_str,*looking_string;
{
        int here,len_search = strlen(search_str);

        while(1){
                if ((here = instr(search_str,looking_string,1)) == -1)
                        return looking_string;

                delete_str(looking_string,here,len_search);
                insert_str(replace_str,looking_string,here);
        }
} /* search_replace */



bye( msg )
char * msg;
{
        fprintf( stderr, "bye: %s%c\n", msg, 7 );
        exit( 1 );

} /* bye() */


char *draw_line(ch,num_chars,buffer)
char ch;
int num_chars;
char *buffer;
{
        int x;

        for (x = 0;x < num_chars;x++)
                buffer[x] = ch;

        buffer[x] = '\0';
        return buffer;

} /* draw_line() */

double int_div_int( int1, int2 )
int int1, int2;
{
        /* don't divide by zero */
        /* -------------------- */
        if ( !int2 ) return 0.0;

        return (double)( (double)int1 / (double)int2 );

} /* int_div_int() */

int perc_of( int1, int2 )
int int1, int2;
{
        /* don't divide by zero */
        /* -------------------- */
        if ( !int2 ) return 0;

        return (int) ( ( ( (double)int1 / (double)int2 ) * 100 ) + .5 );

} /* perc_of () */


char *pad_zero( num, len )
int num, len;
{
        static char ret_zero[ 10 ];
        char *ptr = ret_zero;

        sprintf( ret_zero, "%*d", len, num );

        while ( *ptr && isspace( *ptr ) )
                *ptr++ = '0';

        return ret_zero;

}  /* pad_zero() */



char *get_token( char **s )
{
        static char buffer[ 500 ];
        char *buf_ptr = buffer, *skip_space();

        *s = skip_space( *s );

        if ( !**s )
                return "";

        if ( ispunct( **s ) )
        {
                *buf_ptr = **s;
                buf_ptr++;
                (*s)++;
                *buf_ptr = '\0';
                return buffer;
        }

        while ( **s && isalnum( **s ) )
        {
                *buf_ptr = **s;
                buf_ptr++;
                (*s)++;
        }

        *buf_ptr = '\0';
        return buffer;

} /* get_token() */


static char *skip_space( char *s )
{
        while ( *s && isspace( *s ) ) s++;
        return s;

} /* skip_space() */


mail_tim( int output_also, char *fmt_str, ... )
{
        va_list args;
        char *sys_str = (char *)malloc( 300 );
        char *buffer = (char *)malloc( 300 );

        va_start( args, fmt_str );
        vsprintf( buffer, fmt_str, args );
        va_end( args );

        if ( output_also )
                fprintf( stderr, "%s\n", buffer );

        sprintf( sys_str, "echo \"%s\" | mail tim &", buffer );
        system( sys_str );

        free( sys_str );
        free( buffer );

} /* mail_tim() */


static char *Month[] =  {       "JAN",
                                "FEB",
                                "MAR",
                                "APR",
                                "MAY",
                                "JUN",
                                "JUL",
                                "AUG",
                                "SEP",
                                "OCT",
                                "NOV",
                                "DEC"};

char *OraDate2StdDate( char *ora_date )
{
        static char StdDate[ 9 ];
        char OraMonth[ 4 ], OraDay[ 3 ], OraYear[ 3 ];
        char *piece();
        int m;

        if ( !piece( '-', ora_date, 0, OraDay ) ) return "";
        if ( !piece( '-', ora_date, 1, OraMonth ) ) return "";
        if ( !piece( '-', ora_date, 2, OraYear ) ) return "";

        for( m = 0; m < 12; m++ )
                if ( strcmp( OraMonth, Month[ m ] ) == 0 )
                {
                        sprintf( StdDate, "%02d/%02d/%02d",
                                m + 1, atoi( OraDay ), atoi( OraYear ) );
                        return StdDate;
                }

        return "";

} /* OraDate2StdDate() */

char *ReplaceAllOraDateWithStdDate( char *in_str, char *RetStr )
{
        int m, here;
        char *StdDate;
        char OraDate[ 10 ];
        strcpy( RetStr, in_str );

        for( m = 0; m < 12; m++ )
                if ( ( here = instr( Month[ m ], RetStr, 1 ) ) != -1
                &&   here >= 3
                &&   RetStr[ here - 1 ] == '-'
                &&   RetStr[ here + 3 ] == '-'
                &&   isdigit( RetStr[ here - 2 ] )
                &&   isdigit( RetStr[ here - 3 ] )
                &&   isdigit( RetStr[ here + 4 ] )
                &&   isdigit( RetStr[ here + 5 ] ) )
                {
                        /* -------------------- */
                        /* Found an Oracle Date */
                        /* -------------------- */
                        /* Extract It */
                        /* ---------- */
                        sprintf( OraDate, "%.9s", RetStr + here - 3 );
                        StdDate = OraDate2StdDate( OraDate );
                        search_replace( OraDate, StdDate, RetStr );
                        m--;
                }

        return RetStr;

} /* ReplaceAllOraDateWithStdDate() */

