

#include "Counter.h"

/*
 * Counter
 */

Counter::Counter()
: call(0), counter(0), elapsed(0)
{
#ifdef WIN32
	BOOL test = QueryPerformanceFrequency(&newCounter.frequency);
	// Fast
	if (test)
	{
		QueryPerformanceCounter(&newCounter.first);
		call = &Counter::CallFast;
	}
	// Slow
	else
	{
		oldCounter.first = GetTickCount();
		call = &Counter::CallSlow;
	}
#elif defined(UNIX)
	gettimeofday(&last, 0);
	call = &Counter::CallUnix;
	Update();
#endif
}

Counter::~Counter()
{
}

// Updates the counter
void Counter::Update()
{
	(this->*call)();
	counter += elapsed;
}

// Get elapsed milliseconds from first update
float Counter::Get()
{
	return counter;
}

// Get elapsed milliseconds from last update
float Counter::GetElapsed()
{
	return elapsed;
}

#ifdef WIN32

// Call slow counter
void Counter::CallSlow()
{
	DWORD currentCount = GetTickCount();
	elapsed = float(currentCount - oldCounter.first);
	oldCounter.first = currentCount;
}

// Call fast counter
void Counter::CallFast()
{
	LARGE_INTEGER hqElapsed;
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter(&currentCount);

	hqElapsed.QuadPart = currentCount.QuadPart - newCounter.first.QuadPart;

	elapsed = float(hqElapsed.QuadPart) / float(newCounter.frequency.QuadPart);

	newCounter.first = currentCount;
}

#elif defined(UNIX)

// Unix
void Counter::CallUnix()
{
	timeval now;
	
	gettimeofday(&now, 0);
	
	long seconds = now.tv_sec - last.tv_sec;
	long useconds = now.tv_usec - last.tv_usec;
	
	elapsed = (seconds + useconds/1000000.0);
	
	last = now;
}

#endif