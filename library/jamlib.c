/* c:\roommate\lib\jamlib.c */
/* ------------------------ */

#include <stdio.h>
#include "smdefs.h"
#include "smerror.h"
#include "smuprdb.h"
#ifdef JAMPI
#include "smpiinit.h"
#endif
#ifdef JAMRW
#include "rwdefs.h"
#endif
#ifdef SM_RTL
#include "smrtl.h"
#endif
#ifdef _WINDOWS
#include <dos.h>
#endif

#include <smglobs.h>
#include <smuprapi.h>
#include "timlib.h"

/* These routines are linked to jam.exe and jamdev.exe */
/* --------------------------------------------------- */

disconnect_database()
{
	dm_dbms( "close_all_connections" );

	/* Return JAM success */
	/* ------------------ */
	return 0;

} /* disconnect_database() */

close_until_screen( char *this_screen, char *stop_looking_screen )
{
	char compare_screen_up_case[ 20 ];
	char this_screen_up_case[ 20 ];
	char stop_looking_screen_up_case[ 20 ];
	char *sm_pinquire(), *sm_pinquire_results;

	strcpy( this_screen_up_case, up_string( this_screen ) );
	strcpy( stop_looking_screen_up_case, up_string( stop_looking_screen ) );

	while( 1 )
	{
		sm_pinquire_results = sm_pinquire( SP_NAME );

		if ( !sm_pinquire_results )
		{
			sm_err_reset(
				"close_until_screen: ran out of screens" );
			return -1;
		}       

		strcpy( compare_screen_up_case,
			up_string( sm_pinquire_results ) );

		if ( strcmp( compare_screen_up_case, this_screen_up_case ) == 0)
		{
			return 1;
		}
		else
		if ( strcmp( compare_screen_up_case, 
			     stop_looking_screen_up_case ) == 0 )
		{
			return 0;
		}

		/* Close this screen */
		/* ----------------- */
		jm_exit();
	} 

} /* close_until_screen() */


connect_oracle()
{
	if ( dm_dbms( "engine oracle" ) != 0 )
	{
		sm_message_box( "engine oracle failed", 
				"", 0, "" );
		exit( 1 );
	}

	/* 
		USER DEFINITION: 
				USERID
				AT NETWORK PREFIX
				HOSTNAME
				SYSTEM-ID
		Note: Double colons are required to prevent JAM from
		doing colon expansion on the names.
	*/			
	
	if ( dm_dbms( "declare c connection				 		       for user 'invlink@T::invest1::oracle'	 		       password 'investalink'" ) != 0 )
	{
		sm_message_box( "oracle connect failed", 
				"", 0, "" );
		exit( 1 );
	}


	/* Return JAM success */
	/* ------------------ */
	return 0;

} /* connect_oracle() */


connect_btrieve()
{
  char x[5];


	if ( dm_dbms( "engine odbc" ) != 0 )
	{
		sm_message_box( "engine  odbc failed", 
				"", 0, "" );
		exit( 1 );
	}

	if(dm_dbms("declare c connection for database   'INVLINKB'                                    datasource 'INVESTALINK_BTRIEVE'" ))
	{
		sm_message_box( "btrieve connect failed", 
				"", 0, "" );
		exit( 1 );
	}



	/* Return JAM success */
	/* ------------------ */
	return 0;

} /* connect_btrieve() */


connect_jdb()
{
	if ( dm_dbms( "engine jdb" ) != 0 )
	{
		sm_message_box( "engine jdb failed", 
				"", 0, "" );
		return 0;
	}

	if ( dm_dbms( "declare c connection	 		       for database 'ROOMMATE'" ) != 0 )
	{
		sm_message_box( "jdb connect failed", 
				"", 0, "" );
		return 0;
	}

	return 1;

} /* connect_jdb() */
