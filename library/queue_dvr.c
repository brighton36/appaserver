/* queue_dvr.c */
/* ======================================================= */
/* This is the test driver program for the QUEUE ADT.	   */
/* Freely available software: see Appaserver.org	   */
/* ======================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "queue.h"

typedef struct
{
	char *key;
	char *data;
} RECORD;

RECORD *record_new( char *key, char *data );
void test1( void );
void test2( void );
void test3( void );

int main( void )
{
	printf( "test1()\n" );
	test1();
	printf( "\n" );

/*
	printf( "test2()\n" );
	test2();
	printf( "\n" );

	printf( "test3()\n" );
	test3();
	printf( "\n" );
*/
	return 0;
}

void test1()
{
	QUEUE *s = queue_init();
	char *item;

	enqueue( s, strdup( "hello" ) );
	enqueue( s, strdup( "there" ) );
	enqueue( s, strdup( "tim" ) );

	while ( ( item = dequeue( s ) ) )
	{
		printf( "%s\n", item );
		free( item );
	}
}

void test2()
{
	QUEUE *s = queue_init();

	enqueue( s, "tim" );
	enqueue( s, "there" );
	enqueue( s, "hello" );

	queue_display( s );
}

RECORD *record_new( char *key, char *data )
{
	RECORD *r = (RECORD *)calloc( 1, sizeof( RECORD ) );
	r->key = strdup( key );
	r->data = strdup( data );
	return r;
}

void test3( void )
{
	QUEUE *s = queue_init();
	RECORD *record;

	record = record_new( "key1", "data1" );
	enqueue( s, (char *)record );
	record = record_new( "key2", "data2" );
	enqueue( s, (char *)record );
	record = record_new( "key3", "data3" );
	enqueue( s, (char *)record );

	printf( "queue length = %d\n", queue_length( s ) );
	while ( ( record = (RECORD *)dequeue( s ) ) )
	{
		printf( "%s=%s\n", record->key, record->data );
		printf( "queue length = %d\n", queue_length( s ) );
		free( record );
	}
}

