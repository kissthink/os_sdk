#include <semaphore.h>

/* For printf. REMOVE WHEN ALL IMPLEMENTED */
#include <stdio.h>

#define UNIMPL printf(__func__)

int sem_init(sem_t* sem,int pshared,unsigned int value)
{
	UNIMPL;
}

int sem_wait(sem_t* sem)
{
	UNIMPL;
}

int sem_destroy(sem_t* sem)
{
	UNIMPL;
}

int sem_post(sem_t* sem)
{
	UNIMPL;
}

int sem_getvalue(sem_t* sem,int* value)
{
	UNIMPL;
}