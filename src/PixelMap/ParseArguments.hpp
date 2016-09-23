// ========
// Name :: ParseArguments
// Usage :: An argument parser to make it easier to
// parse arguments
// ========

#pragma once
#ifndef PARSEARGUMENTS_HPP
#define PARSEARGUMENTS_HPP

#include "Globals.hpp"

#include <vector>
#include <list>

#define PARAM_DEFAULT -1

// Param types
#define PARAM_BOOL 0
#define PARAM_INT 1
#define PARAM_STRING 2
#define PARAM_WSTRING 4
#define PARAM_LIST 8

union SCHAR
{
	char c;
	const char * s;
	wchar_t wc;
	const wchar_t * ws;
};

class ParseArguments;

// Parameter
struct Parameter
{
	Parameter();
	Parameter(const Parameter & a);
	// Assign
	Parameter & operator=(const Parameter & a);
	Parameter & operator=(const bool & a);
	Parameter & operator=(const int & a);
	Parameter & operator=(const std::string & a);
	Parameter & operator=(const std::wstring & a);
	Parameter & operator=(const std::list<int> & a);
	Parameter & operator=(const std::list<std::string> & a);
	Parameter & operator=(const std::list<std::wstring> a);
	// Get
	bool Get(bool * a);
	bool Get(int * a);
	bool Get(std::string * a);
	bool Get(std::wstring * a);
	bool Get(std::list<int> * a);
	bool Get(std::list<std::string> * a);
	bool Get(std::list<std::wstring> * a);

	// Exceptions
	std::string eStr;
	std::wstring eWstr;

private:
	friend class ParseArguments;

	// Data
	bool boolean;
	int integer;
	std::string str;
	std::wstring wstr;
	std::list<int> nlist;
	std::list<std::string> slist;
	std::list<std::wstring> wlist;

	// Type
	int type;
	// Name
	SCHAR name;

	bool exist;
};

class ParseArguments
{
public:
	~ParseArguments();
	unsigned int AddParameter(char p, int type = PARAM_BOOL, const char * l = 0);
	unsigned int AddParameter(wchar_t p, int type = PARAM_BOOL, const wchar_t * l = 0);
	unsigned int AddParameter(const char * p, int type = PARAM_BOOL, const char * l = 0);
	unsigned int AddParameter(const wchar_t * p, int type = PARAM_BOOL, const wchar_t * l = 0);

	Parameter * GetParameter(unsigned int i);
	Parameter * GetAdvancedParameter(unsigned int i);

	void PushArguments(char ** args, const int argc);
	void PushArguments(wchar_t ** args, const int argc);
private:
	std::vector<Parameter *> params; // Normal
	std::vector<Parameter *> aparams; // Advanced
};

#endif // PARSEARGUMENTS_HPP