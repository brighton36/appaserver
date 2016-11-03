#include <stdio.h>

main()
{
	printf( "next sequence for /tmp/tim.dat is (%d)\n",
		get_next_sequence( "/tmp/tim.dat" ) );
}

int get_next_sequence( char *path_filename )
{
	FILE *p;
	char buffer[ 1024 ];
	char current_sequence[ 16 ];
	int next_sequence;

	sprintf( 
	buffer, 
	"ls -dt %s* | egrep \"#[0-9][0-9]*$\" | head -1 | sed 's/^.*#//",
	 path_filename );

	p = popen( buffer, "r" );
	if ( fgets( current_sequence, 15, p ) )
		next_sequence = atoi( current_sequence ) + 1;
	else
		next_sequence = 0;

	pclose( p );
	return next_sequence;

} /* get_next_sequence() */

