#include <pthread.h>
#include <sys/types.h>
#include <stddef.h>

/* For printf. REMOVE WHEN ALL IMPLEMENTED */
#include <stdio.h>

int pthread_cond_init(pthread_cond_t* cond, pthread_condattr_t* cond_attr)
{
	/* cond_attr is ignored */
	*cond=PTHREAD_COND_INITIALIZER;
	return 0;
}

int pthread_cond_signal(pthread_cond_t* cond)
{
//	printf("pthread_cond_signal(%#X)\n", cond);
	/* Temporary measure for now. */
	//printf("pthread_cond_signal called 0x%x\n", cond);
	*cond=PTHREAD_COND_SIGNALLED;
	return 0;
}

int pthread_cond_broadcast(pthread_cond_t* cond)
{
	printf("pthread_cond_broadcast\n");
	return 0;
}

int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex)
{
	pthread_mutex_unlock(mutex);
	while (*cond != PTHREAD_COND_SIGNALLED){
		//printf("pthread_cond_wait sleep 0x%x\n",cond);
		usleep(500000);
	}

	*cond=PTHREAD_COND_INITIALIZER;
	pthread_mutex_lock(mutex);
	return 0;
}

int pthread_cond_destroy(pthread_cond_t* cond)
{
	printf("pthread_cond_destroy(%#X)\n", cond);
	return 0;
}

int pthread_cond_timedwait(pthread_cond_t* cond, pthread_mutex_t* mutex, const struct timespec* abstime)
{
	printf("pthread_cond_timedwait\n");
	return 0;
}

