// ========
// Name :: SafeQueue
// Usage :: Queueing data to a threadpool without getting 
// any race conditions
// ========

#pragma once
#ifndef SAFE_QUEUE_HPP

#include <string.h>

#define SAFE_QUEUE_SIZE 256

template<typename T>
struct SafeQueue
{
	// Constructors that does the heavy job
	SafeQueue(unsigned int ssize = SAFE_QUEUE_SIZE)
	: size(ssize), amount(0), read(0), write(0)
	{
		queue = new T[size];
		memset(queue, 0, sizeof(T) * size);
	}
	~SafeQueue()
	{
		delete[] queue;
	}

	// Resize queue (ONLY IF THERE IS NO PROCESSING ON IT)
	void Resize(unsigned int ssize)
	{
		read = write = 0;
		size = ssize;
		T * q = new T[size];
		memset(q, 0, sizeof(T) * size);
		delete[] queue;
		queue = q;
	}

	// Push to the queue if enough place
	bool Push(const T & obj)
	{
		int nextElement = (write + 1) % size;
		if (nextElement != read)
		{
			queue[write] = obj;
			write = nextElement;
			return true;
		}
		else
			return false;
	}
	// Pop from the queue
	T Pop()
	{
		if (read == write)
			return NULL;

		int nextElement = (read + 1) % size;
		T obj = queue[read];
		read = nextElement;
		return obj;
	}
	// Check if the queue is empty
	bool Empty()
	{
		return read == write;
	}
	// Check if the queue is full
	bool Full()
	{
		int nextElement = (write + 1) % size;
		return nextElement == read;
	}
	// Get amount in queue
	int Amount()
	{
		return (read > write) ? (size - read) + write : write - read;
	}

private:
	T * queue;
	int size;
	int amount;
	int read;
	int write;
};

#endif // SAFE_QUEUE_HPP