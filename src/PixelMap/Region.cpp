

#include <fstream>

#include "Region.hpp"

#include "Convert.hpp"

const int BUFFER_SIZE = 4096;
const int SECTOR_BYTES = BUFFER_SIZE;

RegionReader::RegionReader()
: data(0), size(0)
{
	Clear();
}

RegionReader::~RegionReader()
{
	Clear();
}

// Clear input
void RegionReader::Clear()
{
	memset(offsets, 0, sizeof(ChunkOffset) * MAX_CHUNKS);
	while (!sectors.empty())
	{
		BYTE * d = sectors.back().data;
		delete[] d;
		sectors.pop_back();
	}
	if (data)
	{
		delete[] data;
		data = 0;
	}
	amount = 0;
}

// Load a region file
bool RegionReader::Load(const STRING file)
{
	Clear();
	using namespace std;
	ifstream stream;
	stream.open(file.c_str(), ios::in | ios::binary);
	// Unable to open file
	if (!stream.is_open())
		return false;

	// Read to buffer
	string temp;
	BYTE buffer[BUFFER_SIZE];
	while (stream.read((char *)buffer, BUFFER_SIZE))
		temp.append(buffer, buffer + BUFFER_SIZE);

	// Copy data
	int size = temp.size();
	if (data)
		delete[] data;
	data = new BYTE[size];
	memcpy(data, temp.c_str(), size);
	amount = size / SECTOR_BYTES;

	// Parsing
	ParseData();

	return true;
}

// Read through data
void RegionReader::ParseData()
{
	ParseHeader();
	ParseSectors();
}

// Get header data
void RegionReader::ParseHeader()
{
	// Each chunk
	for (int i = 0; i < MAX_CHUNKS; ++i)
	{
		int o = i << 2;
		BYTE t[sizeof(int)] = {0};
		memcpy(t+1, data + o, 3);
		offsets[i].sector_number = Convert<int>(t);
		offsets[i].sector_count = data[o + 3];
	}
}

// Get timestamp data
void RegionReader::ParseTimestamp()
{
	// Each chunk
	for (int i = 0; i < MAX_CHUNKS; ++i)
	{
		int o = (i << 2) + SECTOR_BYTES;
		BYTE t[sizeof(int)] = {0};
		offsets[i].timestamp = Convert<int>(data + o);
	}
}

// Get all chunks in sectors
void RegionReader::ParseSectors()
{
	// Go through sectors
	for (int i = 0; i < MAX_CHUNKS; ++i)
	{
		int offset = offsets[i].sector_number;
		if (offset == 0 || offset > amount)
			continue;
		BYTE s = offsets[i].sector_count;
		if (s == 0 || s > 256)
			continue;
		// Set new pointer
		BYTE * current = data + (offset * SECTOR_BYTES);
		Sector sector;
		sector.length = Convert<int>(current) - 1;
		if (sector.length <= 0 || sector.length > s * SECTOR_BYTES || sector.length > 1024 * 64)
			continue;
		sector.version = current[4];
		current += 5;
		// Only deflated compression
		if (sector.version != COMPRESSION_DEFLATE)
			continue;
		// Copy data
		sector.data = new BYTE[sector.length];
		memcpy(sector.data, current, sector.length);
		sectors.push_back(sector);
	}
}

// Get an indexed chunk
BYTE * RegionReader::GetChunk(UINT index, UINT & size)
{
	if (index >= sectors.size())
		return 0;
	size = sectors[index].length;
	return sectors[index].data;
}

// Get amounts of chunks
int RegionReader::GetAmountChunks()
{
	return sectors.size();
}