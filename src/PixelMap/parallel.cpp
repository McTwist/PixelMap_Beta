
#include <sys/wait.h>

#include <stdlib.h> // exit()

#include "parallel.h"

PROCESS CreateProcess(int (*func)(void *), void * data)
{
	if (!func)
		return 0;
	pid_t id = fork();
	// Error
	if (id < 0)
		return 0;
	// Child
	if (id == 0)
	{
		func(data);
		exit(0);
	}
	return (PROCESS)id;
}

THREAD CreateThread(void * (func)(void *), void * data)
{
	if (!func)
		return 0;
	pthread_t id;
	int res = pthread_create(&id, 0, func, data);
	// Error
	if (res < 0)
		return 0;
	return (THREAD)id;
}

void WaitProcess(PROCESS id)
{
	int status = 0;
	waitpid(id, &status, 0);
}

void WaitThread(THREAD id)
{
	void * status = 0;
	pthread_join(id, &status);
}