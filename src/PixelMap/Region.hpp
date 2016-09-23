
// Region Reader
// Made by McTwist

#pragma once
#ifndef REGION_HPP
#define REGION_HPP

#include <vector>

#include "Globals.hpp"

const int MAX_CHUNKS = 32 * 32;

enum CompressionType
{
	COMPRESSION_UNKNOWN,
	COMPRESSION_GZIP,
	COMPRESSION_DEFLATE
};

class RegionReader
{
public:
	RegionReader();
	~RegionReader();

	void Clear();

	bool Load(const STRING file);
	BYTE * GetChunk(UINT index, UINT & size);
	int GetAmountChunks();

private:
	struct ChunkOffset
	{
		int sector_number;
		BYTE sector_count;
		int timestamp;
	} offsets[MAX_CHUNKS];
	struct Sector
	{
		int length;
		BYTE version;
		BYTE * data;
	};
	std::vector<Sector> sectors;

	void ParseData();
	void ParseHeader();
	void ParseTimestamp();
	void ParseSectors();

	BYTE * data;
	int size;
	int amount;
};

#endif // REGION_HPP