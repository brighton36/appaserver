/* $APPASERVER_HOME/library/ajax.c			*/
/* ==================================================== */
/*                                                      */
/* Freely available software: see Appaserver.org	*/
/* ==================================================== */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <malloc.h>
#include "timlib.h"
#include "piece.h"
#include "ajax.h"

AJAX_RELATED_FOLDER *ajax_related_folder_new(
				FOLDER *display_folder )
{
	AJAX_RELATED_FOLDER *a;

	if ( ! ( a = calloc( 1, sizeof( AJAX_RELATED_FOLDER ) ) ) )
	{
		fprintf( stderr,
			 "ERROR in %s/%s()/%d: cannot allocate memory.\n",
			 __FILE__,
			 __FUNCTION__,
			 __LINE__ );
		exit( 1 );
	}

	a->display_folder = display_folder;

	a->ajax_mto1_related_folder_list =
		ajax_get_mto1_related_folder_list(
			display_folder->mto1_recursive_related_folder_list );

	a->ajax_drop_down_folder =
		ajax_get_drop_down_folder(
			a->ajax_mto1_related_folder_list );

	return a;

} /* ajax_related_folder_new() */

LIST *ajax_get_mto1_related_folder_list(
				LIST *mto1_recursive_related_folder_list )
{
	LIST *ajax_mto1_related_folder_list;

	ajax_mto1_related_folder_list = list_new();


fprintf( stderr, "%s/%s()/%d: got mto1_recursive_related_folder_list = (%s)\n",
__FILE__,
__FUNCTION__,
__LINE__,
related_folder_list_display(
		mto1_recursive_related_folder_list,
		mto1,
		'\n' ) );

	return ajax_mto1_related_folder_list;

} /* ajax_get_mto1_related_folder_list() */

FOLDER *ajax_get_drop_down_folder(
				LIST *ajax_mto1_related_folder_list )
{
	return (FOLDER *)0;

} /* ajax_get_drop_down_folder() */

