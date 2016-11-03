#include <stdio.h>
#include "appaserver.h"

int main( int argc, char **argv )
{
	APPASERVER *appaserver;
	char *folder_name;
	enum relation_type relation_type;

	if ( argc != 3 )
	{
		fprintf( stderr,
			 "Usage: %s folder m:1|1:m\n",
			 argv[ 0 ] );
		exit( 1 );
	}

	folder_name = argv[ 1 ];
	relation_type = (strcmp(argv[ 2 ], "m:1") == 0 ) ? mto1 : one2m;

	appaserver = appaserver_new_appaserver(
					"everglades",
					"99",
					folder_name,
					relation_type,
					prompt_screen );

	printf( "%s\n",
		appaserver_get_display_string( appaserver ) );
	exit( 0 );
}
