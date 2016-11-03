/* cdmenu.c */
/* -------- */
/* ---------------------------------------------------------------------
This program allows you to change directories easily by selecting from a
subset of common directories located in $HOME/cdmenu.dat.

Tim Riley
------------------------------------------------------------------------ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"

#define MAX_LIST        1024
#define CDMENU_FILE     "cdmenu.dat"
#define SHELL_TO_FORK	"/bin/ksh"

main( argc, argv )
int argc;
char **argv;
{
        if ( argc == 2 )
        {
                if ( strcmp( argv[ 1 ], "-a" ) == 0 )
                {
                        put_current_directory_at_top();
                }
                else
                {
                        cdmenu( argv[ 1 ] );
                }
        }
        else
        {
                cdmenu( "" );
        }

        execl( SHELL_TO_FORK, *argv, (char *)0 );

} /* main */


put_current_directory_at_top()
{
        char pwd[ 100 ];
        char cdmenu_filename[ 100 ];
        FILE *f;

        f = popen( "pwd", "r" );
        fscanf( f, "%s", pwd );
        pclose( f );

        if ( directory_exists( pwd ) )
        {
                fprintf( stderr, "ERROR: %s already exists in \"~/%s\".\n",
                        pwd, CDMENU_FILE );
                return;
        }

        sprintf( cdmenu_filename, "%s/%s", getenv( "HOME" ), CDMENU_FILE );

	place_row_at_top_of_file( cdmenu_filename, pwd );
	printf( "Done\n" );

} /* put_current_directory_at_top() */


place_row_at_top_of_file( cdmenu_filename, pwd )
char *cdmenu_filename;
char *pwd;
{
	FILE *cdmenu_file;
        char *array[ MAX_LIST ];
	int i;

	/* Load menus from cdmenu file with no search string */
	/* ------------------------------------------------- */
        load_menus( array, "" );

        cdmenu_file = fopen( cdmenu_filename, "w" );
	if ( !cdmenu_file )
	{
		fprintf( stderr, "Can't open %s for write\n", cdmenu_filename );
		exit( 1 );
	}

        fprintf( cdmenu_file, "%s\n", pwd );

	/* Put the rest of the menus back */
	/* ------------------------------ */
	for( i = 0; array[ i ]; i++ )
		fprintf( cdmenu_file, "%s\n", array[ i ] );

        fclose( cdmenu_file );

} /* place_row_at_top_of_file() */


directory_exists( pwd )
char *pwd;
{
        char grep_cmd[ 200 ];
        int num_occurrences;
        FILE *p;

        sprintf( grep_cmd, "grep -c \"%s\" %s/%s",
                pwd, getenv( "HOME" ), CDMENU_FILE );

        p = popen( grep_cmd, "r" );
        fscanf( p, "%d", &num_occurrences );
        pclose( p );

        return num_occurrences;

} /* directory_exists() */




cdmenu( search_str )
char *search_str;
{
	char *get_pwd();
        char *array[ MAX_LIST ];
        char select_dir[80];
        int selection;

        load_menus( array, search_str );

        if ( ( selection = make_select(array) ) == 99 )
                strcpy( select_dir, get_pwd() );
        else
                strcpy( select_dir, array[ selection ] );

        free_array( array );
        chdir( array[ selection ] );

} /* cdmenu */



load_menus( array, search_str )
char **array;
char *search_str;
{
        FILE *file;
        char cdmenu_filename[ 100 ];
        char instring[100];
        int x;

        sprintf( cdmenu_filename, "%s/%s", getenv( "HOME" ), CDMENU_FILE );
        file = fopen( cdmenu_filename, "r" );

	/* If no cdmenu file */
	/* ----------------- */
	if ( !file )
	{
        	*array = (char *)0;
		return;
	}

        for (   x = 0;
                x < MAX_LIST - 1 && fgets( instring, 100, file );
                x++ )
        {
		/* Strip off trailing <CR> */
		/* ----------------------- */
		instring[ strlen( instring ) - 1 ] = '\0';

                if ( search_passed( instring, search_str ) )
                        *array++ = strdup( instring );
        }

        *array = (char *)0;
        fclose( file );

} /* load_menus */



search_passed( instring, search_str )
char *instring;
char *search_str;
{
        /* If no search string then passed */
        /* ------------------------------- */
        if ( !*search_str ) return 1;

        /* If search failed */
        /* ---------------- */
        if ( instr( search_str, instring, 1 ) == -1 )
                return 0;
        else
                return 1;

} /* search_passed() */





make_select( array )
char **array;
{
        int num_items;
        char sel_str[ 5 ];
        char **arr_ptr;
        int sel_num;
	char *get_pwd();

        while (1)
        {
                arr_ptr = array;
                num_items = 0;
                printf( "\n" );

                while ( *arr_ptr )
                        printf( "%2d) %s\n", ++num_items, *arr_ptr++ );

                printf("99) Stay Here: %s", get_pwd() );
                printf("\n\n");
                printf("Enter Selection: ");
                get_line( sel_str, stdin );
                sel_num = atoi( sel_str );

                if (sel_num == 99) return 99;

                if (sel_num > 0 && sel_num <= num_items)
                        return sel_num - 1;
        }

} /* make_select */

char *get_pwd()
{
	static char pwd[ 128 ];
	FILE *p;

	p = popen( "pwd", "r" );
	get_line( pwd, p );
	pclose( p );
	return pwd;

} /* get_pwd() */
	

free_array(array)
char **array;
{
        while (*array)
                free (*array++);
}

