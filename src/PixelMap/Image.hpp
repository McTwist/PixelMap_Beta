

#pragma once
#ifndef IMAGE_HPP

#include "Color.hpp"
#include "Globals.hpp"

#ifdef WIN32
#include "png.h"
#elif defined(UNIX)
#include <png.h>
#endif

struct ImageSave
{
public:
	ImageSave() : save(0) {};
	bool Prepare(STRING file, UINT width, UINT height);
	bool Row(Color * data);
	bool Close();
private:
	FILE * save;
	png_structp png;
	png_infop info;
};

struct Image
{
public:
	Image();
	Image(const Image & image);
	Image(UINT width, UINT height);
	~Image();

	UINT Width() {return width;}
	UINT Height() {return height;}

	bool Load(STRING file);
	bool Save(STRING file);

	// Save to image for each row
	bool PrepareSave(STRING file);
	bool SaveRow(Color * data);
	bool SaveRow(UINT y);
	bool CloseSave();

	inline void * GetRawData();
	void SetPixel(UINT x, UINT y, Color pixel);
	Color GetPixel(UINT x, UINT y);

	Color Average();
private:
	Color * data;
	UINT width, height;

	// When saving in rows
	ImageSave * rows;
};

#endif // IMAGE_HPP