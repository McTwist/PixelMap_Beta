

#pragma once
#ifndef CONVERT_H
#define CONVERT_H

// Check if CPU little endian
inline bool LittleEndian()
{
	static char save = 0;
	if (save > 0)
	{
		return save - 1 ? true : false;
	}
	union
	{
		int i;
		char c[4];
	}t;
	t.i = 1;
	return (save = (t.c[0] == 1) + 1) == 2 ? true : false;
}

// Convert data in data stream to big endian data
template<typename T>
inline T Convert(BYTE * data)
{
	int size = sizeof(T);
	T ret = 0;
	// Check if little endian and therefor swap to big endian
	if (size > 1 && LittleEndian())
	{
		BYTE * n = new BYTE[size];
		for (int i = 0; i < size; ++i)
			n[i] = data[(size - 1) - i];

		memcpy(&ret, n, size);
		delete[] n;
	}
	else
	{
		memcpy(&ret, data, size);
	}
	return ret;
}

#endif // CONVERT_H