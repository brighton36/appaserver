#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

#ifndef SEMAPHORE_H

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

extern int errno;

#define SEMPERM 0600

int semaphore( key_t semkey );
void semaphore_wait( int semid );
void semaphore_signal( int semid );

#endif
