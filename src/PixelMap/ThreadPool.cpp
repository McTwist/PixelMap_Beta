

#include "ThreadPool.hpp"

#include "Utility.hpp"

// Scruct for storage thread information
struct Thread
{
	THREAD_FUNC function;
	HANDLE handle;
	ThreadPool * parent;
	SafeQueue<ThreadOrder *> orders;
	SafeQueue<ThreadResult *> results;
};

// Each thread processing
THREAD_FUNCTION(ThreadPoolWork, data)
{
	Thread * thread = (Thread *)data;

	ThreadPool * parent = thread->parent;

	ThreadOrder * next = 0;
	ThreadResult * result = 0;

	while (parent->StayAlive())
	{
		result = 0;
		next = thread->orders.Pop();
		// Idle
		if (next == 0)
		{
			Port::Rest(1);
			continue;
		}
		result = thread->function(next);

		// Wait for free space
		while (parent->StayAlive())
		{
			if (thread->results.Push(result))
			{
				parent->PushResult(NULL);
				break;
			}
			Port::Rest(1);
		}
	}

	parent->MarkThreadClosed();
	return 0;
}

ThreadPool::ThreadPool()
: threads(0), size(0)
{
	process = false;
	threads = NULL;
	handles = NULL;
	closed = 0;
	working = 0;
}
ThreadPool::~ThreadPool()
{
	Shutdown();
}

// Initialize a new pool
void ThreadPool::Initialize(int amount, THREAD_FUNC thread)
{
	// Idiotproof
	if (amount <= 0)
		return;
	if (threads)
		return;

	process = true;
	size = amount;
	threads = new Thread[amount];
	handles = new HANDLE[amount];
	for (int i = 0; i < amount; ++i)
	{
		threads[i].parent = this;
		threads[i].function = thread;
#ifdef WIN32
		handles[i] = threads[i].handle = CreateThread(NULL, 0, ThreadPoolWork, (void *)&threads[i], 0, 0);
#elif defined(UNIX)
		handles[i] = threads[i].handle = CreateThread(ThreadPoolWork, (void *)&threads[i]);
#endif
	}
}

// Shutdown and clear the pool
void ThreadPool::Shutdown()
{
	while (!Kill());
	if (threads)
	{
		delete[] threads;
		threads = NULL;
	}
#ifdef WIN32
	if (handles)
	{
		for (int i = 0; i < size; ++i)
		{
			CloseHandle(handles[i]);
		}
		delete[] handles;
		handles = NULL;
	}
#endif
}

// Check if thread should close
bool ThreadPool::StayAlive()
{
	return process;
}

// Kill all threads
bool ThreadPool::Kill()
{
	process = false;
	if (!handles)
		return true;
	return Check();
}

// Wait for threads to close
bool ThreadPool::Check()
{
#ifdef WIN32
	DWORD error = WaitForMultipleObjects(size, handles, TRUE, 1000);

	if (closed >= size)
		return true;

	return !(error == WAIT_TIMEOUT || error == WAIT_FAILED);
#elif defined(UNIX)
	for (int i = 0; i < size; ++i)
		WaitThread(handles[i]);
	return (closed >= size);
#endif
}

// Add more work to the threads
bool ThreadPool::PushWork(ThreadOrder * order)
{
	// Fill empty thread queues
	for (int i = 0; i < size; ++i)
	{
		if (threads[i].orders.Empty() && threads[i].orders.Push(order))
		{
			++working;
			return true;
		}
	}
	// Fill not empty thread queues
	SafeQueue<ThreadOrder *> * min = &threads[0].orders;
	int m = min->Amount();
	for (int i = 0; i < size; ++i)
	{
		int n = threads[i].orders.Amount();
		if (n < m && min->Full())
		{
			m = n;
			min = &threads[i].orders;
		}
	}
	bool t = min->Push(order);
	if (t)
		++working;
	return t;
}

// Check if the working queue is full
bool ThreadPool::FullWork()
{
	bool full = true;
	for (int i = 0; i < size; ++i)
		if (!threads[i].orders.Full())
			full = false;
	return full;
}

// Check if the working queue is empty
bool ThreadPool::EmptyWork()
{
	bool empty = true;
	for (int i = 0; i < size; ++i)
		if (!threads[i].orders.Empty())
			empty = false;
	return empty;
}

// Get work from queue
ThreadOrder * ThreadPool::PopWork()
{
	return NULL;
}

// Add result to queue
bool ThreadPool::PushResult(ThreadResult * result)
{
	--working;
	return false;
}

// Check if the result queue is empty
bool ThreadPool::EmptyResult()
{
	bool empty = true;
	for (int i = 0; i < size; ++i)
		if (!threads[i].results.Empty())
			empty = false;
	return empty;
}

// Get result from queue
ThreadResult * ThreadPool::PopResult()
{
	for (int i = 0; i < size; ++i)
		if (!threads[i].results.Empty())
			return threads[i].results.Pop();
	return NULL;
}

// Pool is working
bool ThreadPool::Working()
{
	return working > 0;
}

// Check if the pool is done with its work
bool ThreadPool::Progress()
{
	return closed < size;
}

// No work left
bool ThreadPool::NoMoreWork()
{
	return EmptyWork() && 
		Progress() && 
		!Working() && 
		EmptyResult();
}