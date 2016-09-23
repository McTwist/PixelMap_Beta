

#pragma once
#ifndef CHUNKPREFS_HPP

#include <string.h>

#include "DataValues.hpp"

// Map size
#define MAPX 16
#define MAPY 16
#define MAPZ 128
const int MAXBLOCKS = MAPX * MAPY * MAPZ;
#define MAPCOORD 10000000

// Preferences
#define CHUNKP_NIGHT 1
#define CHUNKP_NORENDER 2
#define CHUNKP_CAVE 4
#define CHUNKP_COLOR 8
#define CHUNKP_GRAY 16
#define CHUNKP_SOLID 32
#define CHUNKP_NOHEIGHTMAP 64
#define CHUNKP_SINGLESLICE 128

// Map version
// Note: The old ones are only there if there will be a backcompatibility
enum MAP_VERSION
{
	UNKNOWN,
	CLASSIC,
	INVDEV,
	ALPHA,
	BETA
};

// Preferences for a chunk
struct ChunkPrefs
{
	ChunkPrefs()
		: version(UNKNOWN), flags(0), z(MAPZ), h(0), threads(0), logging(false), rotation(0)
	{
		memset(block, true, DATAVALUES_AMOUNT);
	}
	ChunkPrefs & operator=(const ChunkPrefs & pref)
	{
		memcpy(this, &pref, sizeof(ChunkPrefs));
		return *this;
	}
	MAP_VERSION version;
	int flags;
	BYTE z;
	BYTE h;
	BYTE threads;
	bool block[DATAVALUES_AMOUNT];
	bool logging;
	int rotation;
};

#endif // CHUNKPREFS_HPP