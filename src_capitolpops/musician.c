/* src_capitolpops/musician.c						*/
/* -------------------------------------------------------------------- */
/* This is the appaserver musician ADT.					*/
/*									*/
/* Freely available software: see Appaserver.org			*/
/* -------------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "capitolpops.h"
#include "musician.h"
#include "timlib.h"
#include "piece.h"
#include "appaserver_library.h"
#include "appaserver_error.h"

MUSICIAN *musician_new( char *full_name )
{
	MUSICIAN *m;

	if ( ! ( m = (MUSICIAN *)calloc( 1, sizeof( MUSICIAN ) ) ) )
	{
		fprintf(stderr,
			"ERROR: cannot allocate a new musician.\n" );
		exit( 1 );
	}

	m->full_name = full_name;
	return m;

} /* musician_new() */

MUSICIAN *musician_fetch( char *musician_full_name )
{
	static LIST *musician_list = {0};
	MUSICIAN *musician;

	if ( !musician_list )
	{
		musician_list = musician_get_list();
	}

	if ( !list_rewind( musician_list ) ) return (MUSICIAN *)0;

	do {
		musician = list_get( musician_list );

		if (	timlib_strcmp(
			musician->full_name,
			musician_full_name ) == 0 )
		{
			return musician;
		}
	} while( list_next( musician_list ) );

	return (MUSICIAN *)0;

} /* musician_fetch() */

LIST *musician_get_list( void )
{
	char sys_string[ 1024 ];
	char piece_buffer[ 256 ];
	char *select;
	char input_buffer[ 1024 ];
	LIST *musician_list;
	MUSICIAN *musician;
	FILE *input_pipe;

	select =
"full_name,primary_instrument,email_address,street_address,city,state_code,zip_code,home_phone,cell_phone";

	sprintf( sys_string,
		 "get_folder_data	application=%s			"
		 "			select=%s			"
		 "			folder=musician			",
		 APPLICATION,
		 select );

	input_pipe = popen( sys_string, "r" );

	musician_list = list_new();

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		musician = musician_new( strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		musician->primary_instrument = strdup( piece_buffer );
	
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		musician->email_address = strdup( piece_buffer );
	
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		musician->street_address = strdup( piece_buffer );
	
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
		musician->city = strdup( piece_buffer );
	
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
		musician->state_code = strdup( piece_buffer );
	
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
		musician->zip_code = strdup( piece_buffer );
	
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 7 );
		musician->home_phone = strdup( piece_buffer );
	
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 8 );
		musician->cell_phone = strdup( piece_buffer );

		list_append_pointer( musician_list, musician );
	}

	pclose( input_pipe );
	return musician_list;

} /* musician_get_list() */

LIST *musician_get_band_member_list(	char **email_address,
					char *member_full_name )
{
	MUSICIAN *member;
	char sys_string[ 1024 ];
	char input_buffer[ 512 ];
	char piece_buffer[ 128 ];
	char *select;
	char *from;
	char *where;
	FILE *input_pipe;
	LIST *member_list = list_new();

	if ( ! ( member = musician_fetch( member_full_name ) ) )
	{
		return (LIST *)0;
	}

	*email_address = strdup( member->email_address );

	select =
"band_member.full_name,musician.primary_instrument,entity.email_address,musician.home_phone,musician.cell_phone,band_member.contact_information_private_yn";

	from = "band_member,musician,entity";

	where =
"band_member.full_name = musician.full_name and band_member.street_address = musician.street_address and musician.full_name = entity.full_name and musician.street_address = entity.street_address";

	sprintf( sys_string,
		 "echo \"	select %s			"
		 "		from %s				"
		 "		where %s			"
		 "		order by musician.full_name;\"	|"
		 "sql.e '^' 2>>%s				 ",
		 select,
		 from,
		 where,
		 appaserver_error_get_filename( APPLICATION ) );

	input_pipe = popen( sys_string, "r" );

	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		member = musician_new( strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		member->primary_instrument = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		member->email_address = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		member->home_phone = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
		member->cell_phone = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
		member->work_phone = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 6 );
		member->contact_information_private_yn = strdup( piece_buffer );

		list_append_pointer( member_list, member );
	}

	pclose( input_pipe );
	return member_list;

} /* musician_get_band_member_list() */

LIST *musician_get_substitute_musician_list(	char **email_address,
						char *member_full_name )
{
	MUSICIAN *musician;

	if ( ! ( musician =
			musician_fetch(
				member_full_name ) ) )
	{
		fprintf( stderr,
"ERROR in %s/%s()/%d: cannot get musician record for (%s).\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__,
			 member_full_name );
		exit( 1 );
	}

	if ( !*musician->primary_instrument ) return (LIST *)0;

	*email_address = musician->email_address;

	return musician_get_substitution_substitute_musician_list(
			musician->primary_instrument );

} /* musician_get_substitute_musician_list() */

LIST *musician_get_substitution_substitute_musician_list(
			char *member_primary_instrument )
{
	MUSICIAN *musician;
	char sys_string[ 1024 ];
	char input_buffer[ 512 ];
	char piece_buffer[ 128 ];
	char select[ 128 ];
	char *from;
	char where[ 1024 ];
	FILE *input_pipe;
	LIST *substitute_musician_list = list_new();
	char *band_member_subquery;

	sprintf( select,
"%s.full_name,email_address,home_phone,cell_phone,work_phone,substitute_concert_only_yn",
		 "substitute" );

	from = "substitute,musician";

	band_member_subquery =
	"musician.full_name not in ( select full_name from band_member );";

	sprintf( where,
		 "substitute.full_name = musician.full_name		"
		 "and ( primary_instrument = '%s' or			"
		 "exists (						"
		 "	select *					"
		 "	from secondary_instrument			"
		 "	where instrument = '%s'				"
		 "	  and %s.full_name = %s.full_name ) ) 		"
		 "and %s						",
		 member_primary_instrument,
		 member_primary_instrument,
		 "musician",
		 "secondary_instrument",
		 band_member_subquery );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\"			 |"
		 "sql.e '^' 2>>%s					  ",
		 select,
		 from,
		 where,
		 appaserver_error_get_filename( APPLICATION ) );

/*
{
char msg[ 1024 ];
sprintf( msg, "%s/%s()/%d: got sys_string = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
sys_string );
m2( "capitolpops", msg );
}
*/
	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 0 );
		musician = musician_new( strdup( piece_buffer ) );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 1 );
		musician->email_address = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 2 );
		musician->home_phone = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 3 );
		musician->cell_phone = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 4 );
		musician->work_phone = strdup( piece_buffer );

		piece( piece_buffer, FOLDER_DATA_DELIMITER, input_buffer, 5 );
		musician->substitute_concert_only_yn = strdup( piece_buffer );

		list_append_pointer( substitute_musician_list, musician );
	}

	pclose( input_pipe );
	return substitute_musician_list;

} /* musician_get_substitution_substitute_musician_list() */

LIST *musician_get_instrument_band_member_email_address_list(
			char *primary_instrument )
{
	char sys_string[ 1024 ];
	char input_buffer[ 512 ];
	char *select;
	char *from;
	char where[ 512 ];
	FILE *input_pipe;
	LIST *email_address_list = list_new();

	select = "email_address";
	from = "musician,band_member";

	sprintf( where,
		 "primary_instrument = '%s' and				"
		 "email_address is not null and				"
		 "band_member.full_name = musician.full_name		",
		 primary_instrument );

	sprintf( sys_string,
		 "echo \"select %s from %s where %s;\"			 |"
		 "sql.e '^' 2>>%s					  ",
		 select,
		 from,
		 where,
		 appaserver_error_get_filename( APPLICATION ) );

	input_pipe = popen( sys_string, "r" );
	while( get_line( input_buffer, input_pipe ) )
	{
		list_append_pointer(	email_address_list,
					strdup( input_buffer ) );
	}
	pclose( input_pipe );
	return email_address_list;

} /* musician_get_instrument_band_member_email_address_list() */

