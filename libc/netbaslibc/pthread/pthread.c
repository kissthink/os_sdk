#include <pthread.h>
#include <sys/types.h>
#include <stddef.h>

#include <unistd.h>

/* For printf. REMOVE WHEN ALL IMPLEMENTED */
#include <stdio.h>

#define UNIMPL printf("%s() Not IMPLEMENTED\n",__FUNCTION__)


void pthread_cleanup_push(void (*routine)(void*),void* arg)
{
	UNIMPL;
}

void pthread_cleanup_pop(int execute)
{
	UNIMPL;
}

int pthread_create(pthread_t *thread,const pthread_attr_t* attr,void* (*startRoutine)(void*),void* args)
{
	
	pthread_t id;
	id = thread_create(startRoutine,  args,0x10000);
	if (thread)
	{
		*thread = id;
	}
	return id;
}

int pthread_join(pthread_t thread,void** valuePtr)
{
	UNIMPL;
}

int pthread_detach(pthread_t thread)
{
	UNIMPL;
}

int pthread_getattr_np(pthread_t thread,pthread_attr_t* attr)
{
	UNIMPL;
}

int pthread_sigmask(int how,const sigset_t* set,sigset_t* oset)
{
	UNIMPL;
}

int pthread_key_create(pthread_key_t* key,void (*destructor)(void*))
{
	UNIMPL;
}

int pthread_key_delete(pthread_key_t key)
{
	UNIMPL;
}

int pthread_setspecific(pthread_key_t key,const void* value)
{
	UNIMPL;
}

void* pthread_getspecific(pthread_key_t key)
{
	UNIMPL;
}

int pthread_attr_setstacksize(pthread_attr_t* attr,size_t stackSize)
{
	UNIMPL;
}

int pthread_kill(pthread_t thread,int sig)
{
	UNIMPL;
thread_exit(thread);

}

int pthread_mutexattr_init(pthread_mutexattr_t* attr)
{
	UNIMPL;
}

int pthread_mutexattr_destroy(pthread_mutexattr_t* attr)
{
	UNIMPL;
}

int pthread_mutexattr_gettype(const pthread_mutexattr_t* attr,int* type)
{
	UNIMPL;
}

int pthread_mutexattr_settype(pthread_mutexattr_t* attr,int type)
{
	UNIMPL;
}


	
void pthread_exit(void* retval)

{

UNIMPL;
tid_t id = getpid();
thread_exit(id);

}



int pthread_cancel(pthread_t thread)
{
thread_exit(thread);
}



pthread_t pthread_self(void)

{
	
return getpid();

}
