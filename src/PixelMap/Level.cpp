

#include <stdlib.h>

#include "Region.hpp"
#include "nbt.hpp"

#include "Level.hpp"
#include "ListFiles.hpp"

#include "ThreadPool.hpp"

#include "Utility.hpp"
#include "Computer.hpp"

using namespace std;

// Thread structs
struct ThreadString : public ThreadOrder
{
	STRING file;
	ChunkPrefs prefs;
	Chunk * chunk;
};
struct ThreadChunk : public ThreadResult
{
};

// Working thread for the pool
ThreadResult * ThreadPoolWork(ThreadOrder * order)
{
	ThreadString * next = (ThreadString *)order;
	ChunkPrefs prefs = next->prefs;
	Chunk * chunk = next->chunk;
	chunk->pref = prefs;
	// Alpha
	if (prefs.version == ALPHA)
	{
		chunk->Load(next->file);
	}
	// Beta
	else if (prefs.version == BETA)
	{
		chunk->Load((BYTE *)next->file.c_str(), next->file.size());
	}
	if (chunk->isValid())
	{
		chunk->CreateImage();
		return (ThreadResult *)1;
	}
	return (ThreadResult *)0;
}

// Constructor
Level::Level()
{
	Reset();
}

// Destructor
Level::~Level()
{
	ClearChunks();
}

// Abort
void Level::Abort()
{
	cancel = true;
	if (state == DIRECTORY)
		total = -1;
}

// Reset everything
void Level::Reset()
{
	total = 0;
	work = 0;
	result = 0;
	state = INITIALIZE;
	done = 0;
	saved = false;
	cancel = false;

	blocks = 0;
}

// Clear all chunks
void Level::ClearChunks()
{
	while (!chunks.empty())
	{
		Chunk * t = chunks.back();
		delete t;
		chunks.pop_back();
	}
}

// Load level information
int Level::LoadLevel(const STRING & path)
{
	STRING file;
	file.assign(path.c_str());
	STRING name = Port::FileName(file);
	file.append(Port::ReplacePath(STR("\\level.dat")));
	NBT_Reader read = NBT_Reader();
	read.Load(file);

	bool valid = false;

	NBTTag tag = read.ReadData(); // Read first compound
	while ((tag = read.ReadData()) != TAG_Unknown)
	{
		const NBTData * data = read.GetData();
		if (!data)
			continue;
		if (!valid)
		{
			if (data->tag != TAG_Compound)
				continue;
			if (!data->name)
				continue;
			if (!StringCheck(data->name, "Data"))
				continue;
			valid = true;
		}
		switch (data->tag)
		{
			case TAG_Int:
				if (StringCheck(data->name, "version"))
				{
					info.version = data->i;
				}
				break;
			case TAG_Long:
				if (StringCheck(data->name, "RandomSeed"))
				{
					info.seed = data->l;
				}
				break;
			case TAG_String:
				if (StringCheck(data->name, "LevelName"))
				{
					info.name.assign((char *)data->bas, (char *)data->bas+data->size);
				}
				break;
		}
	}
	// Alpha
	if (info.name.empty())
		info.name = name;
	return valid;
}

// Load a world folder
int Level::LoadWorld(const STRING & p)
{
	if (cancel)
		return LVL_ERR_CANCEL;
	Reset();
	state = DIRECTORY;
	STRING path = p;

	STRING search;
	{
		char t[] = {"*"};
		search.assign(t, t+strlen(t));
	}

	// Find files
	FileSearch fs = FileSearch();
	fs.path = path;
	fs.amount = &total;
	fs.exclude.push_back(STR("DIM-1"));
	if (!ListFiles(&fs))
		return LVL_ERR_LISTING;
	files = fs.files;

	if (cancel)
		return LVL_ERR_CANCEL;
	total = files.size();
	work = 0;
	result = 0;
	done = 0;
	prefs.logging = !prefs.log.empty();
	if (prefs.version == BETA)
		total *= 1024;
	state = PROCESSING;

	// Limit threads
	if (prefs.threads > files.size())
		prefs.threads = files.size();
	// More threads than the computer can handle is no good for you
	unsigned long i = 0;
	if (prefs.threads > (i = Computer::GetLogicalCores()))
		prefs.threads = i & 0xFF;
	// Multithreaded program
	if (prefs.threads >= 2)
	{
		LoadMultiple();
	}
	else
	{
		LoadSingle();
	}

	// Fastest way to clear a stack
	while (!files.empty())
		files.pop();

	if (cancel)
		return LVL_ERR_CANCEL;

	return LVL_OK;
}

// Load singlethreaded
void Level::LoadSingle()
{
	// Go through all files
	Chunk * temp = 0;
	while (files.empty() == false && cancel == false)
	{
		if (prefs.version == ALPHA)
		{
			temp = new Chunk();
			// Load chunk
			temp->pref = prefs;
			temp->Load(files.top());
			files.pop();
			// Everything is fine
			if (temp->isValid())
			{
				temp->CreateImage();
				chunks.push_back(temp);
			}
			else
			{
				// Display correct total chunks
				--total;
				// Evade memory leak
				delete temp;
			}
			// Do some aftermath
			result = chunks.size();
			work = total - result;
			done = ((float)result/(float)total);
		}
		else if (prefs.version == BETA)
		{
			RegionReader region = RegionReader();
			region.Load(files.top());
			files.pop();
			// Reduce amount
			total -= 1024 - region.GetAmountChunks();
			// Go through region
			for (int i = 0; i < region.GetAmountChunks() && cancel == false; ++i)
			{
				temp = new Chunk();
				// Load chunk
				temp->pref = prefs;
				UINT size = 0;
				BYTE * data = region.GetChunk(i, size);
				temp->Load(data, size);
				// Everything is fine
				if (temp->isValid())
				{
					temp->CreateImage();
					chunks.push_back(temp);
				}
				else
				{
					// Display correct total chunks
					--total;
					// Evade memory leak
					delete temp;
				}
				// Do some aftermath
				result = chunks.size();
				work = total - result;
				done = ((float)result/(float)total);
			}
		}
	}
}

// Load with multithreaded
void Level::LoadMultiple()
{
	ThreadPool * pool = new ThreadPool();
	pool->Initialize(prefs.threads, ThreadPoolWork);

	stack<ThreadString *> strings;

	// Preparing region object
	RegionReader region = RegionReader();
	int current = 0;
	bool useRegion = (prefs.version == BETA);

	int n = 0;
	// Start swimming
	while (
		(!files.empty() || 
		pool->Progress()) &&
		!cancel)
	{
		// Get latest result
		if (!pool->EmptyResult())
		{
			ThreadChunk * chnk = (ThreadChunk *)pool->PopResult();
			if (chnk != 0)
			{
				++result;
			}
			else
			{
				--total;
			}
		}
		// Push work
		if (useRegion)
		{
			if (current < region.GetAmountChunks() && !pool->FullWork())
			{
				UINT size = 0;
				BYTE * data = region.GetChunk(current++, size);
				ThreadString * str = new ThreadString();
				str->file.assign(data, data+size);
				str->prefs = prefs;
				chunks.push_back(str->chunk = new Chunk());
				pool->PushWork(str);
				strings.push(str);
				++n;
			}
			if (files.empty() && region.GetAmountChunks() == current)
				useRegion = false;
			else if (!files.empty() && current >= region.GetAmountChunks())
			{
				region.Load(files.top());
				files.pop();
				current = 0;
				// Reduce amount
				total -= 1024 - region.GetAmountChunks();
			}
		}
		else if (!files.empty())
		{
			if (!pool->FullWork())
			{
				ThreadString * str = new ThreadString();
				str->file = files.top();
				str->prefs = prefs;
				chunks.push_back(str->chunk = new Chunk());
				if (pool->PushWork(str))
				{
					files.pop();
					strings.push(str);
					++n;
				}
				// Evade memory spikes
				else
				{
					delete str->chunk;
					delete str;
				}
			}
		}
		// No more work
		else if (pool->NoMoreWork())
		{
			// Wait for pool to close
			if (pool->Kill())
				break;
			else
				continue;
		}
		work = (total - files.size()) - result;
		done = ((float)result/(float)total);
		Port::Rest(0);
	}
	int amount = pool->GetClosedThreads();

	if (cancel)
		while (!pool->Kill());

#ifdef WIN32
	if (pool->GetClosedThreads() < prefs.threads)
		MessageBoxA(0, "Some threads were not released.", "PixelMap", MB_OK);
#endif

	// Clean up after you
	while (!strings.empty())
	{
		ThreadString * t = strings.top();
		strings.pop();
		if (t)
			delete t;
	}
	pool->Shutdown();
	delete pool;
}

#include <math.h>

#include "Globals.hpp"

#include "FileCache.hpp"

// Only way to do this due of how vector works
/*int amountDuplicates = 0;

bool compare_chunks(Chunk * chnk1, Chunk * chnk2)
{
	if (chnk1 == 0)
		return false;
	if (chnk2 == 0)
		return true;

	if (chnk1->GetX() > chnk2->GetX())
		return true;
	if (chnk1->GetX() < chnk2->GetX())
		return false;
	if (chnk1->GetY() < chnk2->GetY())
		return true;
	if (chnk1->GetY() > chnk2->GetY())
		return false;
	++amountDuplicates;
	return false;
}*/

// Create image saved in file
int Level::CreateImage(const STRING & file)
{
	if (cancel)
		return LVL_ERR_CANCEL;
	state = CREATEIMAGE;
	if (chunks.empty())
		return LVL_ERR_NOCHUNKS;
	INT64 minX = MAPCOORD;
	INT64 minY = MAPCOORD;
	INT64 maxX = -MAPCOORD;
	INT64 maxY = -MAPCOORD;

	blocks = chunks.size() << 8;
	bool doRender = !(prefs.flags & CHUNKP_NORENDER);

	// Calculate size
	for (list<Chunk *>::iterator i = chunks.begin(); i != chunks.end(); ++i)
	{
		Chunk * chunk = (*i);
		if (!chunk->isValid())
			continue;
		if (doRender)
		{
			COORDS x = chunk->GetX();
			COORDS y = chunk->GetY();
			if (x < minX)
				minX = x;
			if (y < minY)
				minY = y;
			if (x > maxX)
				maxX = x;
			if (y > maxY)
				maxY = y;
		}

		// And add to amount while we are processing
		amount += chunk->amount;
	}

	if (cancel)
		return LVL_ERR_CANCEL;

	if (doRender)
	{
		// No valid chunks?
		if (minX == MAPCOORD && minY == MAPCOORD &&
			maxX == -MAPCOORD && maxY == -MAPCOORD)
			return LVL_ERR_INVALIDCHUNKS;

		prefs.rotation = abs((prefs.rotation / 90) * 90) % 360;
		float rad = (2 * 3.14159265f * prefs.rotation) / 360;

		UINT
			maxWidth = (UINT)(maxX - minX),
			maxHeight = (UINT)(maxY - minY);

		UINT
			width = (maxWidth + 1) * MAPX, 
			height = (maxHeight + 1) * MAPY;

		// Foolproof
		if (width == 0 || height == 0)
			return LVL_ERR_INVALIDSIZE;

		// Rotate
		{
			float rCos = cos(rad);
			float rSin = sin(rad);
			rCos = (rCos < 0) ? -rCos : rCos;
			rSin = (rSin < 0) ? -rSin : rSin;
			UINT newWidth = UINT(width * rCos + height * rSin);
			UINT newHeight = UINT(height * rCos + width * rSin);
			width = newWidth;
			height = newHeight;
		}

		// Foolproof
		if (width == 0 || height == 0)
			return LVL_ERR_INVALIDSIZE;

		bool useCache = !prefs.cache.empty();
		ImageCache cache(height, width);
		Image * image = 0;

		if (!useCache)
		{
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif
			// No more than 1 GiB
			if (width * height * sizeof(Color) > min(1073741824, Computer::GetAvailableMemory()))
				return LVL_ERR_TOOLARGE;
			try
			{
				image = new Image(height, width);
				if (image == 0)
					throw LVL_ERR_TOOLARGE;
			}
			catch (Level_Error error)
			{
				return error;
			}
		}
		else
		{
			STRING cacheFile = prefs.cache;
			cacheFile.append(STR("\\cache.bin"));
			cacheFile = Port::ReplacePath(cacheFile);

			if (!cache.Open(cacheFile))
				return LVL_ERR_CACHE;
		}
		// Sort list
		//chunks.sort(compare_chunks);
		// Found duplicates
		//if (amountDuplicates > 0)
		//	return LVL_ERR_DUPLICATES;

		// Create image
		int n = 0;
		for (list<Chunk *>::iterator i = chunks.begin(); i != chunks.end(); ++i, ++n)
		{
			Chunk * chunk = (*i);
			if (!chunk->isValid())
				continue;

			Image * img = chunk->GetImage();
			// One more try
			if (!img)
			{
				chunk->pref = prefs;
				chunk->CreateImage();
				img = chunk->GetImage();
				if (!img)
					continue;
			}

			UINT x = 0;
			UINT y = 0;
			switch (prefs.rotation)
			{
				case 90:
					x = (width - (UINT(chunk->GetY() - minY) * (MAPY))) - MAPY;
					y = UINT(chunk->GetX() - minX) * MAPX;
					break;
				case 180:
					x = (width - (UINT(chunk->GetX() - minX) * (MAPX))) - MAPX;
					y = (height - (UINT(chunk->GetY() - minY) * (MAPY))) - MAPY;
					break;
				case 270:
					x = UINT(chunk->GetY() - minY) * MAPY;
					y = (height - (UINT(chunk->GetX() - minX) * (MAPX))) - MAPX;
					break;
				case 0:
				default:
					x = UINT(chunk->GetX() - minX) * MAPX;
					y = UINT(chunk->GetY() - minY) * MAPY;
			}

			Color chnk[MAPX][MAPY] = {0};

			// Write pixel for pixel
			for (UINT X = 0; X < MAPX; ++X)
			{
				for (UINT Y = 0; Y < MAPY; ++Y)
				{
					UINT XX = 0;
					UINT YY = 0;
					switch (prefs.rotation)
					{
						case 90:
							XX = Y;
							YY = (MAPX - 1) - X;
							break;
						case 180:
							XX = (MAPX - 1) - X;
							YY = (MAPY - 1) - Y;
							break;
						case 270:
							XX = (MAPY - 1) - Y;
							YY = X;
							break;
						case 0:
						default:
							XX = X;
							YY = Y;
					}
					if (!useCache)
					{
						image->SetPixel((height - (y + Y)) - 1, (x + X), img->GetPixel(YY, XX));
					}
					else
					{
						Color * current = (*chnk) + ((MAPY - (Y+1)) + (MAPY * X));
						Color c = img->GetPixel(YY, XX);
						memcpy(current, &c, sizeof(Color));
					}
				}
			}
			if (useCache)
			{
				for (UINT X = 0; X < MAPX; ++X)
				{
					cache.Write(chnk[X], (height - y) - MAPY, (x + X), MAPY);
				}
			}

			done = ((float)n/(float)total);
		}
		done = 1;

		// No render mode
		if (prefs.flags & CHUNKP_NORENDER)
		{
			saved = true;
			if (!useCache)
			{
				delete image;
			}
			else
			{
				cache.Close();
			}
			state = FINALIZING;
			return LVL_OK;
		}
		// Save image
		state = SAVING;

		// Normal save of image
		if (!useCache)
		{
			if (image->PrepareSave(file))
			{
				for (UINT i = 0; i < width; ++i)
				{
					if (!image->SaveRow(i))
						break;
					done = (float)i/(float)width;
				}
				saved = image->CloseSave();
			}
			done = 1;
			delete image;
		}
		// Save from cache
		else
		{
			ImageSave sav;
			if (sav.Prepare(file, height, width))
			{
				Color * line = new Color[height];
				for (UINT i = 0; i < width; ++i)
				{
					cache.Read(&line, 0, i, height);
					if (!sav.Row(line))
						break;
					done = (float)i/(float)width;
				}
				delete[] line;
				saved = sav.Close();
			}
			cache.Close();
			done = 1;
		}
	}
	else
	{
		saved = true;
	}
	state = FINALIZING;
	return saved ? LVL_OK : LVL_ERR_SAVE;
}