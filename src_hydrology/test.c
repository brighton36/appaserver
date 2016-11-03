#include <stdio.h>

main()
{
	char input_line[ 128 ];
	char shef_alias_string[ 128 ], shef_code[ 128 ];
	char sys_string[ 1024 ];
	FILE *p;
	int i = 0;

	sprintf( sys_string,
		 "./shef_alias_list.sh %s %s", "everglades", "hydrology" );

fprintf( stderr, "%s\n", sys_string );

	p = popen( sys_string, "r" );
	while( get_line( input_line, p ) )
	{
		input_line[ strlen( input_line ) - 1 ] = '\0';
fprintf( stderr, "got = %s\n", input_line );
	}
	pclose( p );
}
