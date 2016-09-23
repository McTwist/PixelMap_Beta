
#pragma once
#ifndef THREAD_H
#define THREAD_H

#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

typedef pid_t PROCESS;
typedef pthread_t THREAD;

PROCESS CreateProcess(int (*func)(void *), void * data);
THREAD CreateThread(void * (*func)(void *), void * data);

void WaitProcess(PROCESS id = -1);
void WaitThread(THREAD id = -1);

#endif // THREAD_H