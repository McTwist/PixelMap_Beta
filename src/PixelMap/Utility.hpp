

#pragma once
#ifndef UTILITY_HPP
#define UTILITY_HPP

#include <string>
#include <sstream>

#include "Globals.hpp"

// Namespace for various porting abilities
namespace Port
{
	STRING GetEnv(const char * var);
	STRING MakeString(const char * text);
	std::wstring Replace(std::wstring str, wchar_t from, wchar_t to);
	std::string Replace(std::string str, char from, char to);
	STRING ReplacePath(STRING path);
	STRING FileName(STRING file);
	void Rest(UINT milli);
}

#define STR(str) Port::MakeString(str)

// Chops thousand
inline std::string chopThousand(INT64 & number)
{
	std::stringstream str;
	int remainder = number % 1000;
	number /= 1000;
	str << (number == 0 ? remainder : (remainder < 0 ? -remainder : remainder));
	std::string more = str.str();
	while (more.size() < 3 && number)
		more = "0" + more;
	return more;
}

// Chops thousand
inline std::string chopThousand(int & number)
{
	std::stringstream str;
	int remainder = number % 1000;
	number /= 1000;
	str << (number == 0 ? remainder : (remainder < 0 ? -remainder : remainder));
	std::string more = str.str();
	while (more.size() < 3 && number)
		more = "0" + more;
	return more;
}

// Sets a separator for a number
inline std::string setSeparator(long long number, char c = ',')
{
	std::string temp;

	temp = chopThousand(number);
	
	while (number)
		temp = chopThousand(number) + c + temp;

	return temp;
}

// Sets a separator for a number
inline std::string setSeparator(int number, char c = ',')
{
	std::string temp;

	temp = chopThousand(number);
	
	while (number)
		temp = chopThousand(number) + c + temp;

	return temp;
}

// Sets a separator for a string
inline std::string setSeparator(std::string str, char c = ',')
{
	return setSeparator(atoi(str.c_str()), c);
}

#endif // UTILITY_HPP