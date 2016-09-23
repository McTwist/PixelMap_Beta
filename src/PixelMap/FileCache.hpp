

#pragma once
#ifndef FILECACHE_HPP
#define FILECACHE_HPP

#include <fstream>

#include "Globals.hpp"

// File cache
class FileCache
{
public:
	FileCache() {}
	FileCache(const STRING file);
	virtual ~FileCache();
	bool Open(const STRING file);
	bool Write(const char * buf, UINT off, UINT size);
	bool Read(char ** buf, UINT off, UINT size);
	void Close();
private:
	std::fstream stream;
	std::string file;
};

struct Color;

// Image cache
class ImageCache : private FileCache
{
public:
	ImageCache(UINT width, UINT height);
	ImageCache(UINT width, UINT height, const STRING file);
	virtual ~ImageCache();
	bool Open(const STRING file);
	bool Write(const Color * colors, UINT x, UINT y, UINT amount);
	bool Read(Color ** colors, UINT x, UINT y, UINT amount);
	void Close();
private:
	UINT width, height;
};

#endif // FILECACHE_HPP