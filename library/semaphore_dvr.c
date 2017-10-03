/* $APPASERVER_HOME/library/semaphore_dvr.c		*/
/* ---------------------------------------------------- */
/* Freely available software: see Appaserver.org	*/
/* ---------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "semaphore.h"

#define SEMAPHORE_TEST_KEY1		1
#define SEMAPHORE_TEST_KEY2		2
#define SEMAPHORE_TEST_KEY3		3
#define SEMAPHORE_TEST_KEY4		4
#define SEMAPHORE_TEST_KEY5		5
#define SEMAPHORE_TEST_KEY6		6
#define SEMAPHORE_TEST_KEY7		7
#define SEMAPHORE_TEST_KEY8		8
#define SEMAPHORE_TEST_KEY9		9
#define SEMAPHORE_TEST_KEY65536		65536

int main( void )
{
	/* key_t key = SEMAPHORE_TEST_KEY1; */
	key_t key = SEMAPHORE_TEST_KEY65536;
	int semid, pid = getpid();

	if ( ( semid = semaphore( key ) ) < 0 )
	{
		fprintf( stderr, "Error: %s failed.\n", __FILE__ );
		exit( 1 );
	}

	printf( "Process %d with semid = %d before critical section.\n",
		pid, semid);

	semaphore_wait( semid );
	printf( "Process %d inside critical section.\n", pid );
	sleep( 5 );
	semaphore_signal( semid );
	printf( "Process %d outside critical section.\n", pid );

	return 0;
}

#ifdef NOT_DEFINED
int critical_function( key_t key );

int main( void )
{
	key_t key = 1;
	int semid;

	if ( fork() == 0 )
		semid = critical_function( key );
	else
	if ( fork() == 0 )
		semid = critical_function( key );
	else
	if ( fork() == 0 )
	{
		semid = critical_function( key );
		semctl( semid, 0, IPC_RMID, 1 );
		printf( "Semaphore %d freed.\n", semid );
	}

	return 0;
}

int critical_function( key_t key )
{
	int semid, pid = getpid();

	if ( ( semid = semaphore( key ) ) < 0 ) exit( 1 );

	printf( "Process %d before critical section.\n", pid );
	semaphore_wait( semid );
	printf( "Process %d inside critical section.\n", pid );
	sleep( 5 );
	semaphore_signal( semid );
	printf( "Process %d outside critical section.\n", pid );
	return semid;

}
#endif
