
// NBT Parser
// Made by McTwist

#include "nbt.hpp"

#ifdef WIN32
#include "zlib.h"
#elif defined(UNIX)
#include <zlib.h>
#endif

// Library
#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "zlib1d")
#else // _DEBUG
#pragma comment(lib, "zlib1")
#endif // _DEBUG
#endif // WIN32

#define SAFE_MODE 0

#include "Convert.hpp"

NBTData::NBTData()
: tag(TAG_Unknown), name(0), size(0), l(0)
{}

NBTData::~NBTData()
{}

NBT_Reader::NBT_Reader()
{
	data = 0;
	pointer = 0;
}

NBT_Reader::~NBT_Reader()
{
	while (RemoveTag());
	if (data)
		delete[] data;
}

const UINT BUFFER_SIZE = 1024;

// Load and decompress NBT file
bool NBT_Reader::Load(const STRING file)
{
	gzFile zload = 0;
#ifdef UNICODE
	std::string sFile(file.begin(), file.end());
	zload = gzopen(sFile.c_str(), "rb");
#else
	zload = gzopen(file.c_str(), "rb");
#endif
	BYTE buffer[BUFFER_SIZE] = {0};
	std::string load;
	int a = -1;
	do
	{
		a = gzread(zload, buffer, BUFFER_SIZE);
		if (a > 0)
			load.append(buffer, buffer+a);
	}
	while (a > 0);
	gzclose(zload);
	// Invalid
	if (load[0] != 10)
		return false;
	int size = load.size();
	data = new BYTE[size];
	memcpy(data, load.c_str(), size);
	pointer = data;
	return true;
}

const UINT CHUNK_INFLATE_MAX = 1024;

// Load from a string
bool NBT_Reader::Load(BYTE * d, UINT length)
{
	z_stream stream;

	BYTE buffer[BUFFER_SIZE] = {0};

	// Prepare stream
	stream.zalloc = 0;
	stream.zfree = 0;
	stream.opaque = 0;

	stream.next_in = d;
	stream.avail_in = length;
	stream.next_out = buffer;
	stream.avail_out = BUFFER_SIZE;

	std::string load;
	int ret = Z_OK;
	inflateInit(&stream);
	do
	{
		stream.avail_out = BUFFER_SIZE;
		stream.next_out = buffer;
		ret = inflate(&stream, Z_NO_FLUSH);
		int s = (stream.avail_out == 0) ? CHUNK_INFLATE_MAX : CHUNK_INFLATE_MAX - stream.avail_out;
		load.append(buffer, buffer+s);
	}
	while (stream.avail_out == 0 && ret != Z_STREAM_END);
	inflateEnd(&stream);
	// Invalid
	if (load[0] != 10)
		return false;
	int size = load.size();
	data = new BYTE[size];
	memcpy(data, load.c_str(), size);

	pointer = data;
	return true;
}

// Create tag and push to top
NBTData * NBT_Reader::CreateTag()
{
	NBTData * tag = new NBTData();
	tags.push(tag);
	return tag;
}

// Remove top tag
bool NBT_Reader::RemoveTag()
{
	if (tags.empty())
		return false;
	NBTData * tag = tags.top();
	tags.pop();
	switch (tag->tag)
	{
		case TAG_ByteArray:
#if SAFE_MODE < 2
			break;
#endif
		case TAG_String:
#if SAFE_MODE >= 1
			delete[] tag->bas;
#endif
			break;
		case TAG_Compound:
			delete tag->next;
			break;
	}
#if SAFE_MODE >= 1
	if (tag->name)
		delete[] tag->name;
#endif
	delete tag;
	return true;
}

// Read next tag in data stream
NBTTag NBT_Reader::ReadData()
{
	if (!data)
		return TAG_Unknown;
	// Remove non-list tags and empty lists
	if (!tags.empty())
	{
		NBTData * last = GetData();
		if (last->tag <= TAG_String)
			RemoveTag();
		last = GetData();
		if (last->tag == TAG_List && last->size <= 0)
			RemoveTag();
	}
	
	// Check for list
	NBTData * last = GetData();
	bool isList = last && last->tag == TAG_List;
	
	NBTData * tagdata = CreateTag();
	
	if (isList)
	{
		--last->size;
		tagdata->tag = last->list;
	}
	else
	{
		BYTE type = 0;
		// Get type
		ReadByte(&type);
		tagdata->tag = (NBTTag)type;
		// Get name
		if (tagdata->tag != TAG_End)
			ReadString(&tagdata->name);
	}
	
	// Read tag
	switch (tagdata->tag)
	{
		case TAG_End:
			RemoveTag();
			RemoveTag();
			if (tags.empty())
				return TAG_Unknown;
			return TAG_End;
			break;
		case TAG_Byte:
			tagdata->size = ReadByte(&tagdata->b);
			break;
		case TAG_Short:
			tagdata->size = ReadShort(&tagdata->s);
			break;
		case TAG_Int:
			tagdata->size = ReadInt(&tagdata->i);
			break;
		case TAG_Long:
			tagdata->size = ReadLong(&tagdata->l);
			break;
		case TAG_Float:
			tagdata->size = ReadFloat(&tagdata->f);
			break;
		case TAG_Double:
			tagdata->size = ReadDouble(&tagdata->d);
			break;
		case TAG_ByteArray:
			tagdata->size = ReadByteArray(&tagdata->bas);
			break;
		case TAG_String:
			tagdata->size = ReadString(&tagdata->bas);
			break;
		case TAG_List:
			tagdata->size = ReadList(&tagdata->list);
			break;
		case TAG_Compound:
			ReadCompound();
			break;
	}
	return tagdata->tag;
}

// Inefficient skipping of data
NBTTag NBT_Reader::SkipData()
{
	if (!data)
		return TAG_Unknown;
	NBTTag tag = TAG_Unknown;
	UINT pop = tags.size();
	do
	{
		tag = ReadData();
	}
	while (pop < tags.size());
	return tag;
}

// Get latest recieved data
NBTData * NBT_Reader::GetData()
{
	if (tags.empty())
		return 0;
	return tags.top();
}

// Read a byte
int NBT_Reader::ReadByte(BYTE * p)
{
	*p = Convert<BYTE>(pointer);
	pointer += 1;
	return 1;
}

// Read a short
int NBT_Reader::ReadShort(short * p)
{
	*p = Convert<short>(pointer);
	pointer += 2;
	return 2;
}

// Read an integer
int NBT_Reader::ReadInt(int * p)
{
	*p = Convert<int>(pointer);
	pointer += 4;
	return 4;
}

// Read a 64 integer
int NBT_Reader::ReadLong(INT64 * p)
{
	*p = Convert<INT64>(pointer);
	pointer += 8;
	return 8;
}

// Read a float
int NBT_Reader::ReadFloat(float * p)
{
	*p = Convert<float>(pointer);
	pointer += 4;
	return 4;
}

// Read a double
int NBT_Reader::ReadDouble(double * p)
{
	*p = Convert<double>(pointer);
	pointer += 8;
	return 8;
}

// Read a byte array
int NBT_Reader::ReadByteArray(BYTE ** p)
{
	int length = 0;
	ReadInt(&length);
#if SAFE_MODE >= 2
	*p = new BYTE[length];
	memcpy(*p, pointer, length);
#else
	*p = pointer;
#endif
	pointer += length;
	return length;
}

// Read a string
int NBT_Reader::ReadString(BYTE ** p)
{
	short length = 0;
	ReadShort(&length);
#if SAFE_MODE >= 1
	*p = new BYTE[length+1];
	memcpy(*p, pointer, length);
	(*p)[length] = 0;
#else
	*p = pointer;
#endif
	pointer += length;
	return length;
}

// Read a list
int NBT_Reader::ReadList(NBTTag * type)
{
	BYTE t = 0;
	ReadByte(&t);
	*type = (NBTTag)t;
	int length = 0;
	ReadInt(&length);
	return length;
}

// Do nothing with a compound
void NBT_Reader::ReadCompound()
{
}