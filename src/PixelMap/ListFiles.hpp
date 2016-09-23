

#pragma once
#ifndef LISTFILES_HPP
#define LISTFILES_HPP

#include "Globals.hpp"

#include <string.h>
#include <vector>
#include <stack>

struct FileSearch
{
	FileSearch();
	STRING path;
	std::stack<STRING> files;
	std::vector<STRING> exclude;
	UINT * amount;
	bool deep;
};

// List files
bool ListFiles(FileSearch * prefs);

#endif // LISTFILES_HPP