
// NBT Parser
// Made by McTwist

#pragma once
#ifndef NBT_HPP
#define NBT_HPP

#include <stack>

#include "Globals.hpp"

enum NBTTag
{
	TAG_End,
	TAG_Byte,
	TAG_Short,
	TAG_Int,
	TAG_Long,
	TAG_Float,
	TAG_Double,
	TAG_ByteArray,
	TAG_String,
	TAG_List,
	TAG_Compound,
	TAG_Unknown = -1
};

struct NBTData
{
	NBTData();
	~NBTData();
	NBTTag tag;
	BYTE * name;
	int size;
	union
	{
		BYTE b;
		short s;
		int i;
		INT64 l;
		float f;
		double d;
		BYTE * bas;
		NBTTag list;
		NBTData * next;
	};
};

class NBT_Reader
{
public:
	NBT_Reader();
	~NBT_Reader();
	bool Load(const STRING file);
	bool Load(BYTE * data, UINT length);
	NBTTag ReadData();
	NBTTag SkipData();
	
	NBTData * GetData();
	
private:
	NBTData * CreateTag();
	bool RemoveTag();
	
	int ReadByte(BYTE * b);
	int ReadShort(short *);
	int ReadInt(int *);
	int ReadLong(INT64 *);
	int ReadFloat(float *);
	int ReadDouble(double *);
	int ReadByteArray(BYTE **);
	int ReadString(BYTE **);
	int ReadList(NBTTag *);
	void ReadCompound();
	
	BYTE * data;
	BYTE * pointer;
	std::stack<NBTData *> tags;
};

// Compare two strings of characters
inline bool StringCheck(const BYTE * str, const BYTE * str2, const UINT size)
{
	if (size == 0)
		return true;

	// Two versions. The first faster than the second
//#define STRINGCHECK_MEMORY
#ifndef STRINGCHECK_MEMORY
	for (UINT i = 0; i < size; ++i)
	{
		if (str[i] != str2[i])
			return false;
	}
	return true;
#else // STRINGCHECK_MEMORY
	return memcmp(str, str2, size) == 0;
#endif // STRINGCHECK_MEMORY
}

// Easier way to compare two strings of characters
inline bool StringCheck(const BYTE * str, const char * str2)
{
	UINT size = strlen(str2);
	if (size == 0)
		return false;
	return StringCheck(str, (BYTE *)str2, size);
}

#endif // NBT_HPP