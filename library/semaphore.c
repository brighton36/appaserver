#include <stdlib.h>
#include <stdio.h>
#include "semaphore.h"

int semaphore( key_t key )
{
	int status = 0;
	int semid;

	if ( ( semid = semget( key, 1, SEMPERM|IPC_CREAT|IPC_EXCL ) ) == -1 )
	{
		if ( errno == EEXIST )
		{
			semid = semget( key, 1, 0 );
		}
		else
		{
		}
	}
	else
	{
		status = semctl( semid, 0, SETVAL, 1 );
	}

	if ( semid == -1 || status == -1 )
	{
		char msg[ 128 ];
		sprintf( msg, "%s() failed", __FUNCTION__ );
		perror( msg );
		return -1;
	}

	return semid;
}

void semaphore_wait( int semid )
{
	struct sembuf p_buf;

	p_buf.sem_num = 0;
	p_buf.sem_op = -1;
	p_buf.sem_flg = SEM_UNDO;

	if ( semop( semid, &p_buf, 1 ) == -1 )
	{
		char msg[ 128 ];
		sprintf( msg, "%s() failed", __FUNCTION__ );
		perror( msg );
		exit( 1 );
	}
}

void semaphore_signal( int semid )
{
	struct sembuf v_buf;

	v_buf.sem_num = 0;
	v_buf.sem_op = 1;
	v_buf.sem_flg = SEM_UNDO;

	if ( semop( semid, &v_buf, 1 ) == -1 )
	{
		char msg[ 128 ];
		sprintf( msg, "%s() failed", __FUNCTION__ );
		perror( msg );
		exit( 1 );
	}
}

