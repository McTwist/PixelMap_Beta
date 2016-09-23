

#pragma once
#ifndef DEFINITIONS_HPP
#define DEFINITIONS_HPP

#include <string.h>
#include <string>

// Type definitions
typedef unsigned char BYTE;
typedef unsigned int UINT;
typedef long long INT64;
typedef int COORDS;

// Thread function
#ifdef WIN32
#define THREAD_FUNCTION(func, arg) DWORD WINAPI func(PVOID arg)
#elif defined(UNIX)
#define THREAD_FUNCTION(func, arg) void * func(void * arg)
#endif

#ifdef UNIX
#include <stdlib.h>
// Windows only -> Linux compatible function
inline int _wtoi(const wchar_t * wch)
{
	std::wstring wstr;
	wstr.assign(wch);
	std::string str;
	str.assign(wstr.begin(), wstr.end());
	return atoi(str.c_str());
}
#endif // UNIX

// Unicode
#ifdef UNICODE

typedef std::wstring STRING;
typedef std::wstringstream STRINGSTREAM;

// ANSI
#else // UNICODE

typedef std::string STRING;
typedef std::stringstream STRINGSTREAM;

#endif // UNICODE

#endif // DEFINITIONS_HPP