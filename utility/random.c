/* utility/random.c */
/* ---------------- */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

long my_random( int up_to );

int main( int argc, char **argv )
{
	int i;
	int up_to;
	int how_many = 1;
	long my_random();

	if ( argc < 2 || argc > 3)
	{
		fprintf(	stderr,
				"Usage: %s up_to [how_many]\n",
				argv[ 0 ] );
		exit( 1 );
	}

	up_to = atoi( argv[ 1 ] );
	if ( argc == 3 )
	{
		how_many = atoi( argv[ 2 ] );
	}

	srandom( time( 0 ) );
	for( i = 0; i < how_many; i++ )
	{
		printf( "%ld\n", my_random( up_to ) );
	}
	return 0;
}

long my_random( int up_to )
{
	double random_double;

	random_double = (double)random() / (double)RAND_MAX;
	return (long)((double)up_to * random_double) + 1;
/*
	double r_num;
	long time();
	double drand48();

	r_num = drand48() * (double)time(0);
	r_num = r_num - (int)r_num;
	return ((long)(r_num * up_to) + 1);
*/
}

