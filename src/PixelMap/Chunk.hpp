

#pragma once
#ifndef CHUNK_HPP
#define CHUNK_HPP

#include <string>

#include "Color.hpp"
#include "Image.hpp"
#include "DataValues.hpp"

#include "Globals.hpp"

#include "ChunkPrefs.hpp"

class Chunk
{
public:
	Chunk();
	Chunk(const Chunk & chunk);
	~Chunk();

	bool Load(const STRING & file);
	bool Load(BYTE * data, UINT size);
	bool isValid(){return valid;}
	void Invalid(){valid = false;}

	void CreateImage();

	Image * GetImage();

	BYTE Read(int x, int y, int z, BYTE * d);
	int GetHeight(int x, int y, int z = MAPZ, int minZ = 0);
	Color GetColor(int x, int y, int z);

	inline COORDS GetX(){return chunkX;}
	inline COORDS GetY(){return chunkY;}
	inline INT64 GetLastUpdate(){return lastUpdate;}

	// Count what is in the map
	struct Counter
	{
		Counter();
		Counter & operator=(const Counter &c);
		Counter & operator+=(const Counter &c);
		const Counter & operator+(const Counter &c) const;
		INT64 
		// Blocks
			block[DATAVALUES_AMOUNT],
		// Mobs
			chicken,
			sheep,
			zombie,
			pig,
			skeleton,
			creeper,
			cow,
			slime,
			pigzombie,
			ghast;
		unsigned int filesize;
	} amount;

	ChunkPrefs pref;

private:
	void Allocate();
	void Deallocate();

	bool ParseNBT(void *);

	BYTE * blocks;
	BYTE * blockData;
	BYTE * skyLight;
	BYTE * blockLight;
	BYTE * heightMap;

	COORDS chunkX, chunkY;
	INT64 lastUpdate;

	bool valid;
	
	Image * image;

	// Function passes
	void PassExclude(int & x, int & y, int & z, Color & color);
	void PassNormal(int & x, int & y, int & z, Color & color);
	void PassGray(int & x, int & y, int & z, Color & color);
	void PassColor(int & x, int & y, int & z, Color & color);
	void PassCave(int & x, int & y, int & z, Color & color);
	void PassBlend(int & x, int & y, int & z, Color & color);
	void PassHeight(int & x, int & y, int & z, Color & color);
	void PassNight(int & x, int & y, int & z, Color & color);
};

#endif // CHUNK_HPP