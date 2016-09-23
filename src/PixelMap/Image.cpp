

#include "Image.hpp"

// Library
#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "libpng14d")
#else // _DEBUG
#pragma comment(lib, "libpng14")
#endif // _DEBUG
#endif // WIN32

// Prepare image to be written
bool ImageSave::Prepare(STRING file, UINT width, UINT height)
{
	if (save)
		return false;
	// Open file
#ifdef WIN32
#ifdef UNICODE
	_wfopen_s(&save, file.c_str(), L"wb");
#else
	fopen_s(&save, file.c_str(), "wb");
#endif
#elif defined(UNIX)
#ifdef UNICODE
	save = _wfopen(file.c_str(), L"wb");
#else
	save = fopen(file.c_str(), "wb");
#endif
#endif
	if (!save)
		return false;
	// Create file
	png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png)
	{
		fclose(save);
		save = 0;
		return false;
	}

	// Create header
	info = png_create_info_struct(png);
	if (!info)
	{
		fclose(save);
		return false;
	}
	if (setjmp(png_jmpbuf(png)))
	{
		png_destroy_info_struct(png, &info);
		png_destroy_write_struct(&png, NULL);
		fclose(save);
		save = 0;
		return false;
	}

	// Init io buffer
	png_init_io(png, save);
	if (setjmp(png_jmpbuf(png)))
	{
		png_destroy_info_struct(png, &info);
		png_destroy_write_struct(&png, NULL);
		fclose(save);
		save = 0;
		return false;
	}

	// Set IHDR
	png_set_IHDR(png, info, width, height,
		8, PNG_COLOR_TYPE_RGB_ALPHA, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	// Write header to file
	png_write_info(png, info);
	if (setjmp(png_jmpbuf(png)))
	{
		png_write_end(png, NULL);
		png_destroy_info_struct(png, &info);
		png_destroy_write_struct(&png, NULL);
		fclose(save);
		save = 0;
		return false;
	}
	return true;
}

// Append row to image
bool ImageSave::Row(Color * data)
{
	if (!save)
		return false;
	png_write_row(png, (png_byte*)data);
	return true;
}

// Close image file
bool ImageSave::Close()
{
	if (!save)
		return false;
	if (setjmp(png_jmpbuf(png)))
	{
		png_write_end(png, NULL);
		png_destroy_info_struct(png, &info);
		png_destroy_write_struct(&png, NULL);
		fclose(save);
		save = 0;
		return false;
	}

	// Destroy and close
	png_write_end(png, NULL);
	png_destroy_info_struct(png, &info);
	png_destroy_write_struct(&png, NULL);
	fclose(save);
	save = 0;
	return true;
}

// Standard size
Image::Image()
: data(0), rows(0)
{
	width = 16;
	height = 16;
	data = new Color[width * height];
}

// Copy constructor
Image::Image(const Image & image)
: data(0), rows(0)
{
	width = image.width;
	height = image.height;
	int size = width * height * sizeof(Color);
	memcpy(data, image.data, size);
}

// Create with size
Image::Image(UINT w, UINT h)
: data(0), rows(0)
{
	width = w;
	height = h;
	data = new Color[w * h];
}

Image::~Image()
{
	if (data)
		delete[] data;
}

// Get raw data
void * Image::GetRawData()
{
	return (void *)data;
}

// Set a pixel to the image
void Image::SetPixel(UINT x, UINT y, Color pixel)
{
	if (x < width && y < height)
		data[x + y*width] = pixel;
}

// Get a pixel from the image
Color Image::GetPixel(UINT x, UINT y)
{
	return (x < width && y < height) ? data[x + y*width] : Color();
}

// Load from file
bool Image::Load(STRING file)
{
	if (data)
	{
		delete[] data;
		data = 0;
	}
	BYTE header[8] = {0};

	FILE * load = 0;
#ifdef WIN32
#ifdef UNICODE
	_wfopen_s(&load, file.c_str(), L"rb");
#else
	fopen_s(&load, file.c_str(), "rb");
#endif
#elif defined(UNIX)
#ifdef UNICODE
	load = _wfopen(file.c_str(), L"rb");
#else
	load = fopen(file.c_str(), "rb");
#endif
#endif
	if (!load)
		return false;

	fread(header, 1, 8, load);
	if (png_sig_cmp(header, 0, 8))
	{
		fclose(load);
		return false;
	}

	// Create png
	png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (!png)
	{
		fclose(load);
		return false;
	}

	// Create header
	png_infop info = png_create_info_struct(png);
	if (!info)
	{
		png_destroy_read_struct(&png, NULL, NULL);
		fclose(load);
		return false;
	}

	if (setjmp(png_jmpbuf(png)))
	{
		png_destroy_info_struct(png, &info);
		png_destroy_read_struct(&png, NULL, NULL);
		fclose(load);
		return false;
	}

	// Prepare
	png_init_io(png, load);
	png_set_sig_bytes(png, 8);

	png_read_info(png, info);

	width = info->width;
	height = info->height;
	BYTE color_type = info->color_type;

	png_read_update_info(png, info);

	// Alocate
	png_bytep * _rows = new png_bytep[sizeof(png_bytep) * height];
	for (UINT i = 0; i < height; ++i)
		_rows[i] = new png_byte[info->rowbytes];

	// Read
	png_read_image(png, _rows);

	png_read_end(png, NULL);
	png_destroy_info_struct(png, &info);
	png_destroy_read_struct(&png, NULL, NULL);
	fclose(load);

	// Convert file to proper data
	data = new Color[width * height];
	memset(data, 0, width * height);

	for (UINT y = 0; y < height; ++y)
	{
		// RGBA
		if (color_type | PNG_COLOR_MASK_ALPHA)
			memcpy(data + (y * width), _rows[y], width * 4);
		// RGB
		else
		{
			for (UINT x = 0, a = 0; x < width; x += 4, a += 3)
			{
				memcpy(data + (y * width) + x, _rows[y] + a, 3);
			}
		}
		delete[] _rows[y];
	}
	delete[] _rows;

	return true;
}

// Save to file
bool Image::Save(STRING file)
{
	ImageSave row = ImageSave();
	if (!row.Prepare(file, width, height))
		return false;
	
	for (unsigned int i = 0; i < height; ++i)
		row.Row(data + i*width);

	return row.Close();
}

// Prepare image to be saved
bool Image::PrepareSave(STRING file)
{
	if (rows)
		return false;
	rows = new ImageSave();
	if (!rows->Prepare(file, width, height))
	{
		delete rows;
		rows = 0;
		return false;
	}
	return true;
}

// Append row to image
bool Image::SaveRow(Color * d)
{
	if (!rows)
		return false;
	return rows->Row(d);
}

// Save current row (Process each row to be saved)
bool Image::SaveRow(UINT y)
{
	if (!rows)
		return false;
	if (y >= height)
		return false;
	return rows->Row(data + y*width);
}

// Close saving
bool Image::CloseSave()
{
	if (!rows)
		return false;
	bool t = rows->Close();
	delete rows;
	rows = 0;
	return t;
}


// Get average color from image
Color Image::Average()
{
	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;
	int size = (width * height);

	for (int i = 0; i < size; ++i)
	{
		r += data[i].r;
		g += data[i].g;
		b += data[i].b;
		a += data[i].a;
	}

	r /= size;
	g /= size;
	b /= size;
	a /= size;

	return Color((BYTE)r, (BYTE)g, (BYTE)b, (BYTE)a);
}