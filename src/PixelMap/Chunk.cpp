
#include <fstream>
#include <stdarg.h>
#include <stdlib.h>

#include "Chunk.hpp"

#include "nbt.hpp"

Chunk::Chunk()
{
	blocks = skyLight = blockLight = heightMap = 0;

	image = 0;
	lastUpdate = 0;

	chunkX = chunkY = 0;
	valid = false;
}

Chunk::Chunk(const Chunk & chunk)
{
	blocks = chunk.blocks;
	skyLight = chunk.skyLight;
	blockLight = chunk.blockLight;
	heightMap = chunk.heightMap;

	image = chunk.image;
	lastUpdate = chunk.lastUpdate;

	chunkX = chunk.chunkX;
	chunkY = chunk.chunkY;
	valid = chunk.valid;
}

Chunk::~Chunk()
{
	if (image)
		delete image;
	Deallocate();
}

#include <fstream>

// Load from file (below 1.3)
bool Chunk::Load(const STRING & file)
{
	valid = false;
	Allocate();
	chunkX = chunkY = 0;

	// Get file size
	{
		using namespace std;
		fstream size(file.c_str(), ios::in | ios::binary);
		if (size.is_open())
		{
			size.seekg(0, ios::end);
			amount.filesize = size.tellg();
			size.close();
		}
	}
	NBT_Reader read = NBT_Reader();
	if (!read.Load(file))
	{
		valid = false;
		amount.filesize = 0;
		return false;
	}

	return ParseNBT((void *)&read);
}

// Load from string (1.3 and above)
bool Chunk::Load(BYTE * data, UINT size)
{
	valid = false;
	Allocate();
	chunkX = chunkY = 0;

	// Get chunk size
	amount.filesize = size;

	NBT_Reader read = NBT_Reader();
	if (!read.Load(data, size))
	{
		valid = false;
		amount.filesize = 0;
		return false;
	}

	return ParseNBT((void *)&read);
}


// Parse NBT
bool Chunk::ParseNBT(void * p)
{
	if (!p)
		return false;
	NBT_Reader * read = (NBT_Reader *)p;

	NBTTag tag = read->ReadData(); // Read first compound
	while ((tag = read->ReadData()) != TAG_Unknown)
	{
		const NBTData * data = read->GetData();
		if (!data)
			continue;
		if (!valid)
		{
			if (data->tag != TAG_Compound)
				continue;
			if (!data->name)
				continue;
			if (!StringCheck(data->name, "Level"))
				continue;
			valid = true;
		}
		switch (data->tag)
		{
			case TAG_Int:
				if (StringCheck(data->name, "xPos"))
				{
					chunkX = data->i;
				}
				else if (StringCheck(data->name, "zPos"))
				{
					chunkY = data->i;
				}
				break;
			case TAG_Long:
				// Check for compatibility
				if (StringCheck(data->name, "LastUpdate"))
				{
					lastUpdate = data->l;
				}
				break;
			case TAG_ByteArray:
				// Blocks
				if (StringCheck(data->name, "Blocks"))
				{
					if (data->size < MAXBLOCKS)
						continue;
					// Get blocks
					if (pref.logging)
					{
						for (int i = 0; i < MAXBLOCKS; ++i)
						{
							++amount.block[blocks[i] = data->bas[i]];
						}
					}
					else
					{
						memcpy(blocks, data->bas, MAXBLOCKS);
					}
					continue;
				}
				// Data
				if (StringCheck(data->name, "Data"))
				{
					if (data->size < MAXBLOCKS / 2)
						continue;
					// Get data
					for (int x = 0; x < MAPX; ++x)
					{
						for (int y = 0; y < MAPY; ++y)
						{
							for (int z = 0; z < MAPZ; ++z)
							{
								int half = x*64 + y*64*16 + int((float)z*0.5f);
								BYTE l = 0;
								if (z % 2 == 0)
									l = (int)data->bas[half] % 16;
								else
									l = (int)data->bas[half] / 16;
								// Add light
								blockData[x*MAPZ + y*MAPZ*MAPX + z] = l;
							}
						}
					}
					continue;
				}
				// Heightmap
				if (StringCheck(data->name, "HeightMap"))
				{
					if (data->size < MAPX * MAPY)
						continue;
					memcpy(heightMap, data->bas, MAPX * MAPY);
					continue;
				}
				// Skylight
				if (StringCheck(data->name, "SkyLight"))
				{
					if (data->size < MAXBLOCKS / 2)
						continue;
					// Get light
					for (int x = 0; x < MAPX; ++x)
					{
						for (int y = 0; y < MAPY; ++y)
						{
							for (int z = 0; z < MAPZ; ++z)
							{
								int half = x*64 + y*64*16 + int((float)z*0.5f);
								BYTE l = 0;
								if (z % 2 == 0)
									l = (int)data->bas[half] % 16;
								else
									l = (int)data->bas[half] / 16;
								// Add light
								skyLight[x*MAPZ + y*MAPZ*MAPX + z] = l;
							}
						}
					}
					continue;
				}
				// Blocklight
				if (StringCheck(data->name, "BlockLight"))
				{
					if (data->size < MAXBLOCKS / 2)
						continue;
					// Get second light
					for (int x = 0; x < MAPX; ++x)
					{
						for (int y = 0; y < MAPY; ++y)
						{
							for (int z = 0; z < MAPZ; ++z)
							{
								int half = x*64 + y*64*16 + int((float)z*0.5f);
								BYTE l = 0;
								if (z % 2 == 0)
									l = (int)data->bas[half] % 16;
								else
									l = (int)data->bas[half] / 16;
								// Add light
								blockLight[x*MAPZ + y*MAPZ*MAPX + z] = l;
							}
						}
					}
					continue;
				}
				break;
			case TAG_String:
				if (pref.logging)
				{
					if (StringCheck(data->name, "id"))
					{
						// Sheep
						if (StringCheck(data->bas, "Sheep"))
						{
							++amount.sheep;
							continue;
						}

						// Zombie
						if (StringCheck(data->bas, "Zombie"))
						{
							++amount.zombie;
							continue;
						}

						// Pig
						if (StringCheck(data->bas, "Pig"))
						{
							++amount.pig;
							continue;
						}

						// Skeleton
						if (StringCheck(data->bas, "Skeleton"))
						{
							++amount.skeleton;
							continue;
						}

						// Creeper
						if (StringCheck(data->bas, "Creeper"))
						{
							++amount.creeper;
							continue;
						}

						// Cow
						if (StringCheck(data->bas, "Cow"))
						{
							++amount.cow;
							continue;
						}

						// Slime
						if (StringCheck(data->bas, "Slime"))
						{
							++amount.slime;
							continue;
						}
					}
				}
				break;
		}
	}

	// Invalid
	if (!valid)
	{
		amount.filesize = 0;
		return false;
	}
	
	// 10 000 000
	if (abs(chunkX) > MAPCOORD || abs(chunkY) > MAPCOORD)
	{
		amount.filesize = 0;
		valid = false;
		return false;
	}

	return true;
}

// Create image from chunk data
void Chunk::CreateImage()
{
	if (!blocks)
		return;
	if (pref.flags & CHUNKP_NORENDER)
	{
		Deallocate();
		return;
	}
	// Prepare variables
	int startz = pref.z;
	int min = 0;
	bool single = !!(pref.flags & CHUNKP_SINGLESLICE);
	if (single)
		min = startz;
	if (!image)
		image = new Image(MAPX, MAPY);

	for (int x = 0; x < MAPX; ++x)
	{
		for (int y = 0; y < MAPY; ++y)
		{
			Color color(0, 0, 0);
			int z = GetHeight(x, y, startz, min);
			color = GetColor(x, y, z);

			if (!single)
			{
				PassExclude(x, y, z, color);
				
				// Cave mode
				if (pref.flags & CHUNKP_CAVE)
					PassCave(x, y, z, color);

				// Gray mode
				if (pref.flags & CHUNKP_GRAY)
				{
					PassGray(x, y, z, color);
					image->SetPixel(x, y, color);
					continue;
				}
				// Color mode
				else if (pref.flags & CHUNKP_COLOR)
				{
					PassColor(x, y, z, color);
					image->SetPixel(x, y, color);
					continue;
				}

				PassBlend(x, y, z, color);

				// Normal mode
				if ((pref.flags & CHUNKP_SOLID) == 0)
					PassNormal(x, y, z, color);

				// Night mode
				if (pref.flags & CHUNKP_NIGHT)
				{
					int tz = z+1;
					PassNight(x, y, tz, color);
				}

				PassHeight(x, y, z, color);
			}

			image->SetPixel(x, y, color);
		}
	}

	Deallocate();
}

Image * Chunk::GetImage()
{
	return image;
}

// Read from chunk
BYTE Chunk::Read(int x, int y, int z, BYTE * d)
{
	if (x < 0 || x >= MAPX) return 0;
	if (y < 0 || y >= MAPY) return 0;
	if (z < 0 || z >= MAPZ) return 0;
	return d[x*MAPZ + y*MAPZ*MAPX + z];
}

// Get highest point
int Chunk::GetHeight(int x, int y, int Z, int minZ)
{
	if (x < 0 || x >= MAPX) return 0;
	if (y < 0 || y >= MAPY) return 0;
	if ((pref.flags & CHUNKP_NOHEIGHTMAP) == 0 && Z > (heightMap[x * MAPY + y]))
		Z = (heightMap[x * MAPY + y]);
	for (int z = Z; z >= minZ; --z)
		if (Read(x, y, z, blocks) != AIR || z <= minZ)
			return z;
	return minZ;
}

// Private functions

void Chunk::Allocate()
{
	if (!blocks)
	{
		blocks = new BYTE[MAXBLOCKS];
		blockData = new BYTE[MAXBLOCKS];
		skyLight = new BYTE[MAXBLOCKS];
		blockLight = new BYTE[MAXBLOCKS];
		heightMap = new BYTE[MAPX * MAPY];
	}

	// Clear them out
	memset(blocks, 0, sizeof(BYTE) * MAXBLOCKS);
	memset(skyLight, 0, sizeof(BYTE) * MAXBLOCKS);
	memset(blockLight, 0, sizeof(BYTE) * MAXBLOCKS);
	memset(heightMap, 0, sizeof(BYTE) * MAPX * MAPY);
}

void Chunk::Deallocate()
{
	if (blocks)
	{
		delete[] blocks;
		delete[] blockData;
		delete[] skyLight;
		delete[] blockLight;
		delete[] heightMap;
		blocks = skyLight = blockLight = heightMap = 0;
	}
}

// Get color from a block
Color Chunk::GetColor(int x, int y, int z)
{
	BYTE n = Read(x, y, z, blocks);
	// Special block
	switch (n)
	{
		case WOOL:
			n = WOOLCOLOR + Read(x, y, z, blockData);
			break;
		case LOG:
			n = LOGCOLOR + Read(x, y, z, blockData) % 3;
			break;
		case LEAVES:
			n = LEAFCOLOR + Read(x, y, z, blockData) % 3;
			break;
		case SLAB_STONE:
			n = SLABCOLOR + Read(x, y, z, blockData) % 4;
			break;
	}
	return DataValues::GetColor(n);
}

// Exclude blocks
void Chunk::PassExclude(int & x, int & y, int & z, Color & color)
{
	if (z < 0)
		return;
	BYTE n = 0;
	do
	{
		n = Read(x, y, z, blocks);
	}
	while (!pref.block[n] && --z >= 0);
	color = (z < 0) ? 0 : GetColor(x, y, z);
}

// Normal height rendering
void Chunk::PassNormal(int & x, int & y, int & z, Color & color)
{
	if (z < 0)
		return;
	Color temp(0, 0, 0, 127);
	temp.Blend(color, z + 127);
	color = temp;
}

// Gray height rendering
void Chunk::PassGray(int & x, int & y, int & z, Color & color)
{
	if (z < 0)
		return;
	BYTE g = (BYTE)z << 1;
	color = Color(g, g, g, (z > 0 ? 255 : 0));
}

// Color heightmap
void Chunk::PassColor(int & x, int & y, int & z, Color & color)
{
	if (z < 0)
		return;
	static unsigned int gradient[] = 
		{
			0xFF7F00FF,
			0xFF0000FF,
			0xFF00FFFF,
			0xFF00FF00,
			0xFFFFFF00,
			0xFFFF0000
		};

	int t = (((z + 1) << 1) - 1);
	float step = 256 / 5.0f;
	int bin = int(t / step);
	float norm = (t - bin*step) / step;

	Color block1 = gradient[bin];
	Color block2 = gradient[bin+1];
	block1.Interpolate(block2, norm);
	color = block1;
}

// Find next cave
void Chunk::PassCave(int & x, int & y, int & z, Color & color)
{
	if (z < 0)
		return;
	bool a = true;
	Color c;
	COLOR high = 0;
	while ((c.a < 255 || a) && z >= 0)
	{
		if (c.a > high)
			high = c.a;
		--z;
		c = GetColor(x, y, z);
		// Found a cave
		if (high == 255 && c.a < 255)
			a = false;
	}
	color = c;
}

// Blend transparent blocks
void Chunk::PassBlend(int & x, int & y, int & z, Color & color)
{
	if (z < 0)
		return;
	Color block(color);
	Color temp(color);
	int zz = z; // We rather not want to go deeper

	while (temp.a < 255 && --zz >= 0)
	{
		temp = GetColor(x, y, zz);
		// Invisible
		if (temp.a == 0)
			continue;
		block.Blend(temp, 127);
		// Solid
		if (temp.a == 255)
			block.a = 255; // Make solid to prevent little transparency
	}

	color = (zz < 0) ? 0 : block;
}

// Put a line frequently
void Chunk::PassHeight(int & x, int & y, int & z, Color & color)
{
	if (z < 0)
		return;
	// Height line
	if (pref.h > 0 && z % pref.h == 0)
	{
		Color temp(0, 0, 0, 128);
		temp.Blend(color, 160);
		color = temp;
	}
}

#include <math.h> // pow

// Put night on it
void Chunk::PassNight(int & x, int & y, int & z, Color & color)
{
	if (z < 0)
		return;
	BYTE bLight = Read(x, y, z, blockLight);
	float light = pow(0.9f, 15 - bLight);
	color.Interpolate(Color(0, 0, 0), 1 - light);
}