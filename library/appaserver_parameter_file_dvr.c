#include <stdio.h>
#include <stdlib.h>
#include "timlib.h"
#include "appaserver_parameter_file.h"

int main( int argc, char **argv )
{
	APPASERVER_PARAMETER_FILE *s;

	s = new_appaserver_parameter_file();
	printf( "database = %s\n", s->database );
	printf( "user = %s\n", s->user );
	printf( "password = %s\n", s->password );
	printf( "flags = %s\n", s->flags );
	printf( "appaserver_mount_point = %s\n", s->appaserver_mount_point );
	return 0;
}

