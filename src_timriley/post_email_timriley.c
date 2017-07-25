/* ---------------------------------------------------	*/
/* src_timriley/post_email_timriley.c			*/
/* ---------------------------------------------------	*/
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------	*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timlib.h"
#include "dictionary.h"
#include "document.h"
#include "post2dictionary.h"

#define TIMRILEY_EMAIL_ADDRESS	"timriley@appahost.com"
#define RICK_DOCUMENT_ROOT	"/var/www/appahost"
#define SUGAR_DOCUMENT_ROOT	"/home/timriley/public_html/appahost"
#define APPAHOST_DOCUMENT_ROOT	"/var/www/html/appahost_com"

int main( void )
{
	DICTIONARY *dictionary = {0};
	char *reason;
	char *email_address;
	char *message;
	char sys_string[ 1024 ];
	FILE *output_pipe;
	char *node_name;
	char *document_root;
	char output_process[ 128 ];

	dictionary = post2dictionary(
				stdin,
				(char *)0 /* appaserver_data_directory */,
				(char *)0 /* session */ );

	reason = dictionary_get( dictionary, "reason" );
	email_address = dictionary_get( dictionary, "email_address" );
	message = dictionary_get( dictionary, "message" );

	sprintf( sys_string,
		 "mailx -s \"Appahost message about %s from %s\" %s",
		 reason,
		 email_address,
		 TIMRILEY_EMAIL_ADDRESS );

	output_pipe = popen( sys_string, "w" );
	fprintf( output_pipe, "%s\n", message );
	pclose( output_pipe );

	document_output_content_type();

	node_name = pipe2string( "uname -n" );

	if ( strcmp( node_name, "rick" ) == 0 )
		document_root = RICK_DOCUMENT_ROOT;
	else
	if ( strcmp( node_name, "sugar.he.net" ) == 0 )
		document_root = SUGAR_DOCUMENT_ROOT;
	else
		document_root = APPAHOST_DOCUMENT_ROOT;

	sprintf( output_process, "cat %s/message_sent.html", document_root );
	system( output_process );

	exit( 0 );

} /* main() */

