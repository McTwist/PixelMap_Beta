
#pragma once
#ifndef COLOR_HPP
#define COLOR_HPP

//#include <stdarg.h>

typedef unsigned char COLOR;

struct Color
{
	Color();
	Color(const Color & c);
	Color(COLOR red, COLOR green, COLOR blue, COLOR alpha = 255);
	Color(const unsigned int & color);

	Color & operator=(const Color & color);
	int getInt();
	Color & operator=(const unsigned int & color);
	Color & operator+=(const Color & color);
	Color & operator-=(const Color & color);
	Color & operator+=(const unsigned int & color);
	Color & operator-=(const unsigned int & color);
	const Color operator+(const Color & color) const;
	const Color operator-(const Color & color) const;
	const Color operator/(const int n) const;

	void Blend(const Color & color, int h);
	void Interpolate(const Color & color, float n);
	//void Mix(const int n, ... );

	COLOR r, g, b, a;
};

#endif // COLOR_HPP