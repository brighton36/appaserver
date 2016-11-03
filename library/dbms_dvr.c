/* g:\library\tim\dbms_dvr.c */
/* ------------------------- */
/* DBMS test driver program  */
/* ------------------------- */
#include <stdio.h>

main()
{
	dbms_init();
	dbms_append( "select first_name from rep" );
	dbms_append( " where 1 = 1" );
	dbms_send_to_dbms_out();
	dbms_execute();
}

dm_dbms( char *stmt )
{
	printf( "dm_dbms: %s\n", stmt );
	return 0;
}

sm_err_reset( char *text )
{
	printf( "sm_err_reset: %s\n", text );
	return 0;

} /* sm_err_reset() */
