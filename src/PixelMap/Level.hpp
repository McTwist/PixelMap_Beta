

#pragma once
#ifndef LEVEL_HPP
#define LEVEL_HPP

#include <list>
#include <stack>

#include "Chunk.hpp"

enum Level_Error
{
	LVL_OK,
	LVL_ERR_NOWORLD,
	LVL_ERR_INVALIDWORLD,
	LVL_ERR_CANCEL,
	LVL_ERR_LISTING,
	LVL_ERR_NOCHUNKS,
	LVL_ERR_INVALIDCHUNKS,
	LVL_ERR_INVALIDSIZE,
	LVL_ERR_TOOLARGE,
	LVL_ERR_DUPLICATES,
	LVL_ERR_CACHE,
	LVL_ERR_SAVE
};

// Inherit from ChunkPrefs
struct LevelPrefs : public ChunkPrefs
{
	STRING log;
	STRING cache;
};

#include <sstream>

// Handling map
struct World
{
	World() : n(0), s(L"0") {};
	World & operator=(const World & map)
	{
		s = map.s;
		n = map.n;
		return *this;
	}
	World & operator=(const std::wstring & str)
	{
		s = str;
		n = _wtoi(s.c_str());
		return *this;
	}
	World & operator=(const int & w)
	{
		n = w;
		std::wstringstream str;
		str << n;
		s = str.str();
		return *this;
	}
	bool operator==(const std::wstring & str)
	{
		return s == str;
	}
	// Integer operators overload
	bool operator==(const int & w)
	{
		return n == w;
	}
	bool operator!=(const int & w)
	{
		return n != w;
	}
	bool operator>=(const int & w)
	{
		return n >= w;
	}
	bool operator<=(const int & w)
	{
		return n <= w;
	}
	bool operator>(const int & w)
	{
		return n > w;
	}
	bool operator<(const int & w)
	{
		return n < w;
	}
	int n;
	std::wstring s;
};

class Level
{
public:
	Level();
	~Level();

	void Abort();
	void Reset();

	int LoadLevel(const STRING & path);
	int LoadWorld(const STRING & path);

	int CreateImage(const STRING & file);

	void ClearChunks();

	// What state the loader are in
	enum State
	{
		INITIALIZE,
		DIRECTORY,
		PROCESSING,
		CREATEIMAGE,
		SAVING,
		FINALIZING
	} state;

	int blocks;

	struct LevelInfo
	{
		LevelInfo()
		{
			seed = 0;
			version = 0;
		}
		INT64 seed;
		int version;
		std::string name;
	} info;

	LevelPrefs prefs;
	Chunk::Counter amount;

private:
	// Single or multithreaded
	void LoadSingle();
	void LoadMultiple();

	std::stack<STRING> files;
	std::list<Chunk *> chunks;

	UINT total;
	int work,
		result;
	float done;
	bool saved;
	bool cancel;

public:
	inline UINT GetTotal() {return total;}
	inline int GetWork() {return work;}
	inline int GetResult() {return result;}
	inline float GetDone() {return done;}
	inline bool isSaved() {return saved;}
};

#endif // LEVEL_HPP