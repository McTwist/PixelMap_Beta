

// This file contains all dirty code
// Add all additions for portability here

#include "Utility.hpp"


STRING Port::GetEnv(const char * var)
{
	STRING env;
#ifdef WIN32
	size_t tLen = 0;
#ifdef UNICODE
	wchar_t path[80];
	wchar_t var2[256] = {0};
	int size = strlen(var)+1;
	mbstowcs_s(&tLen, var2, size, var, size);
	_wgetenv_s(&tLen, path, 80, var2);
#else
	char path[80];
	getenv_s(&tLen, path, 80, var);
#endif
	env = path;
#elif defined(UNIX)
	char * temp = getenv(var);
	env.assign(temp, temp+strlen(temp));
#endif
	return env;
}

STRING Port::MakeString(const char * text)
{
	STRING str;
	str.assign(text, text+strlen(text));
	return str;
}

// Replace character in string
std::wstring Port::Replace(std::wstring str, wchar_t from, wchar_t to)
{
	for (UINT i = 0; i < str.size(); ++i)
		if (str[i] == from)
			str[i] = to;
	return str;
}
std::string Port::Replace(std::string str, char from, char to)
{
	for (UINT i = 0; i < str.size(); ++i)
		if (str[i] == from)
			str[i] = to;
	return str;
}

// Replace a path to be correct
STRING Port::ReplacePath(STRING path)
{
#ifdef WIN32
#ifdef UNICODE
	path = Replace(path, L'/', L'\\');
#else
	path = Replace(path, '/', '\\');
#endif
#elif defined(UNIX)
#ifdef UNICODE
	path = Replace(path, L'\\', L'/');
#else
	path = Replace(path, '\\', '/');
#endif
#endif
	return path;
}

// Get file name from path
STRING Port::FileName(STRING file)
{
#ifdef WIN32
#ifdef UNICODE
	int pos = file.find_last_of(L"\\") + 1;
#else
	int pos = file.find_last_of("\\") + 1;
#endif
#elif defined(UNIX)
#ifdef UNICODE
	int pos = file.find_last_of(L"/") + 1;
#else
	int pos = file.find_last_of("/") + 1;
#endif
#endif
	return file.substr(pos);
}

#ifdef WIN32
#include <windows.h>
#endif

// Porting of sleep
void Port::Rest(UINT milli)
{
#ifdef WIN32
	Sleep(milli);
#elif defined(UNIX)
	usleep(milli * 1000);
#endif
}