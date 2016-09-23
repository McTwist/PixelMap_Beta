

#include "Color.hpp"

Color::Color()
{
	r = 0;
	g = 0;
	b = 0;
	a = 0;
}

Color::Color(const Color & c)
{
	r = c.r;
	g = c.g;
	b = c.b;
	a = c.a;
}

Color::Color(COLOR red, COLOR green, COLOR blue, COLOR alpha)
{
	r = red;
	g = green;
	b = blue;
	a = alpha;
}

Color::Color(const unsigned int & c)
{
	*this = c;
}

Color & Color::operator=(const Color & color)
{
	if (this == &color)
		return *this;
	r = color.r;
	g = color.g;
	b = color.b;
	a = color.a;
	return *this;
}

int Color::getInt()
{
	return (a << 24) | (r << 16) | (g << 8) | b;
}

Color & Color::operator=(const unsigned int & color)
{
	r = (color >> 16) & 0xFF;
	g = (color >> 8) & 0xFF;
	b = color & 0xFF;
	a = color >> 24;
	return *this;
}

Color & Color::operator+=(const Color & color)
{
	Blend(color, 128);
	return *this;
}

Color & Color::operator-=(const Color & color)
{
	Blend(color, 128);
	return *this;
}

Color & Color::operator+=(const unsigned int & color)
{
	r += (color >> 16) & 0xFF;
	g += (color >> 8) & 0xFF;
	b += color & 0xFF;
	a += color >> 24;
	return *this;
}

Color & Color::operator-=(const unsigned int & color)
{
	r -= (color >> 16) & 0xFF;
	g -= (color >> 8) & 0xFF;
	b -= color & 0xFF;
	a -= color >> 24;
	return *this;
}

const Color Color::operator+(const Color & color) const
{
	Color t = *this;
	t += color;
	return t;
}

const Color Color::operator-(const Color & color) const
{
	Color t = *this;
	t -= color;
	return t;
}

const Color Color::operator/(const int n) const
{
	Color t = *this;
	t.r /= n;
	t.g /= n;
	t.b /= n;
	t.a /= n;
	return t;
}

// Blend two colors
void Color::Blend(const Color & color, int h)
{
	Color temp;
	float alpha1 = (float)a / 255;
	float alpha2 = (float)color.a / 255;
	float preAlpha = alpha2*(1 - alpha1);
	float alpha = alpha1 + preAlpha;
	if (alpha != 0)
	{
		float preH = (float)h / 128;
		temp.r = COLOR((r*alpha1 + (color.r*preH)*preAlpha) / alpha);
		temp.g = COLOR((g*alpha1 + (color.g*preH)*preAlpha) / alpha);
		temp.b = COLOR((b*alpha1 + (color.b*preH)*preAlpha) / alpha);
		temp.a = COLOR(alpha*255);
	}
	else
	{
		temp.r = 0;
		temp.g = 0;
		temp.b = 0;
		temp.a = 0;
	}
	// Copy new color to color
	*this = temp;
}

// Interpolate two colors
void Color::Interpolate(const Color & color, float n)
{
	r = COLOR((1.0f-n)*(float)r + n*(float)color.r);
	g = COLOR((1.0f-n)*(float)g + n*(float)color.g);
	b = COLOR((1.0f-n)*(float)b + n*(float)color.b);
}

// Mix several colors
/*void Color::Mix(const int n, ... )
{
	if (n < 1)
		return;
	unsigned long long color = getInt();
	va_list args;
	va_start(args, n);
	for (int i = 0; i < n; ++i)
	{
		Color color = va_arg(args, Color);
		color += color.getInt();
	}
	va_end(args);
	*this = (unsigned int)(color / (n+1));
}*/