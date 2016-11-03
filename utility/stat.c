/* utility/stat.c		       			   */
/* ------------------------------------------------------- */
/* Freely available software: see Appaserver.org	   */
/* ------------------------------------------------------- */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>
#include <time.h>
#include "timlib.h"
#include "boolean.h"
 
struct stat s;
void do_stat( char *path, boolean is_ymd );
char *get_owner();
char *get_modified( time_t mtime, boolean is_ymd );
 
int main( int argc, char **argv )
{
        char instring[ 200 ];
	boolean is_ymd;
 
	is_ymd = ( strcmp( argv[ 0 ], "statymd.e" ) == 0 );

        if ( argc == 1 )
        {
                while( get_line( instring, stdin ) )
                        do_stat( instring, is_ymd );
        }
        else
        {
                while( --argc )
                        do_stat( *++argv, is_ymd );
        }
 
	return 0;
} /* main() */
 
 
void do_stat( char *path, boolean is_ymd )
{
        if ( stat( path, &s ) == -1 )
        {
                perror( path );
        }
        else
        {
                printf( ".filename %s\n", path );
                printf( ".owner %s\n", get_owner( s.st_uid ) );
                printf( ".modified %s\n", get_modified( s.st_mtime, is_ymd ) );
                printf( ".size %lu\n", s.st_size );
        }
 
} /* do_stat() */
 
 
char *get_owner( uid )
int uid;
{
        struct passwd *pw = getpwuid( uid );
        return ( pw ) ? pw->pw_name : "Unknown";
 
} /* get_owner() */
 
char *get_modified( time_t mtime, boolean is_ymd )
{
        static char ret_time[ 31 ];
        struct tm *tm = localtime( &mtime );
 
	if ( is_ymd )
        	strftime( ret_time, 30, "%F %H:%M", tm );
	else
        	strftime( ret_time, 30, "%d-%b-%Y %H:%M", tm );

        return ret_time;
 
} /* get_modified() */
