

#pragma once
#ifndef THREADPOOL_HPP

#ifdef WIN32
#include <windows.h>
#elif defined(UNIX)
#include "parallel.h"
typedef THREAD HANDLE;
#endif

#include "SafeQueue.hpp"

// Standard thread structs that has to be inherited
// Tip: It is recommended to allocate in the main 
// thread and then send it to the thread
struct ThreadOrder
{
};

struct ThreadResult
{
};

typedef ThreadResult * (*THREAD_FUNC)(ThreadOrder * order);

struct Thread;

class ThreadPool
{
public:
	ThreadPool();
	~ThreadPool();

	void Initialize(int amount, THREAD_FUNC thread);
	void Shutdown();

	bool StayAlive();
	bool Kill();

	bool Check();

	bool PushWork(ThreadOrder * order);
	bool FullWork();
	bool EmptyWork();
	ThreadOrder * PopWork();
	bool PushResult(ThreadResult * result);
	bool EmptyResult();
	ThreadResult * PopResult();

	bool Working();
	bool Progress();
	bool NoMoreWork();

	inline void MarkThreadClosed() {++closed;}
	inline int GetClosedThreads() {return closed;}
private:
	Thread * threads;
	HANDLE * handles;
	int size;
	bool process;
	int closed,
		working;
};

#endif // THREADPOOL_HPP