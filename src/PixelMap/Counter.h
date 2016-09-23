

// Made by McTwist
// Works in both Windows and Linux

#pragma once
#ifndef COUNTER_H
#define COUNTER_H
#ifdef WIN32
#include <windows.h>
#elif defined(UNIX)
#include <sys/time.h>
#endif // WIN32 / UNIX

class Counter
{
public:
	Counter();
	virtual ~Counter();
	virtual void Update();

	// Get
	float Get();
	float GetElapsed();
private:
	typedef void (Counter::*Call)();

	Call call;

	float counter,
		elapsed;

#ifdef WIN32
	// Slow, compatible
	void CallSlow();
	struct Old
	{
		unsigned long first;
	} oldCounter;

	// Fast, modern
	void CallFast();
	struct New
	{
		LARGE_INTEGER first,
			frequency;
	} newCounter;
#elif defined(UNIX)
	// Unix
	void CallUnix();
	timeval last;
#endif // WIN32 / UNIX
};

#endif // COUNTER_H