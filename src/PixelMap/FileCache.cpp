

#include "FileCache.hpp"

using namespace std;

FileCache::FileCache(const STRING file)
{
	Open(file);
}

FileCache::~FileCache()
{
	Close();
}

bool FileCache::Open(const STRING f)
{
	file.assign(f.begin(), f.end());
	// Create file
	stream.open(f.c_str(), ios::out);
	if (!stream.is_open())
		return false;
	stream.close();
	stream.clear();
	// Open cache
	stream.open(f.c_str(), ios::in | ios::out | ios::binary);
	return stream.is_open();
}

bool FileCache::Write(const char *buf, UINT off, UINT size)
{
	if (!stream.is_open())
		return false;
	stream.seekp(off);
	stream.write(buf, size);
	return true;
}

bool FileCache::Read(char **buf, UINT off, UINT size)
{
	if (!stream.is_open())
		return false;
	stream.seekg(off);
	stream.read(*buf, size);
	return true;
}

void FileCache::Close()
{
	if (!stream.is_open())
		return;
	stream.close();
	remove(file.c_str());
}

// Image Cache
#include "Color.hpp"

ImageCache::ImageCache(UINT w, UINT h)
: width(w), height(h)
{}

ImageCache::ImageCache(UINT w, UINT h, const STRING f)
: width(w), height(h), FileCache(f)
{}

ImageCache::~ImageCache()
{}

bool ImageCache::Open(const STRING f)
{
	return FileCache::Open(f);
}

bool ImageCache::Write(const Color * colors, UINT x, UINT y, UINT amount)
{
	return FileCache::Write((const char *)colors, (x + (y * width)) * sizeof(Color), amount * sizeof(Color));
}

bool ImageCache::Read(Color ** colors, UINT x, UINT y, UINT amount)
{
	return FileCache::Read((char **)colors, (x + (y * width)) * sizeof(Color), amount * sizeof(Color));
}

void ImageCache::Close()
{
	FileCache::Close();
}