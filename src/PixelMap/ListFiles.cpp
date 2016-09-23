

#include "ListFiles.hpp"

#include "Utility.hpp"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#elif defined(UNIX)
#include <sys/types.h>
#include <dirent.h>
#endif // WIN32 / UNIX

// A lot easier
#ifdef UNICODE
#define STRCMP wcscmp
#else
#define STRCMP strcmp
#endif

using namespace std;

FileSearch::FileSearch()
{
	exclude.push_back(STR("."));
	exclude.push_back(STR(".."));
	amount = 0;
	deep = true;
}

// List files
bool ListFiles(FileSearch * prefs)
{
	if (!prefs)
		return false;
	// Clear strings
	prefs->path.assign(prefs->path.c_str());

	stack<STRING> directories;
	STRING file;
	STRING path = prefs->path;
	directories.push(path);

	vector<STRING> * exclude = &prefs->exclude;

	UINT * amount = prefs->amount;

#ifdef WIN32
	HANDLE find = INVALID_HANDLE_VALUE;

	WIN32_FIND_DATA foundData = {0};

	while (!directories.empty())
	{
		path = directories.top();
		directories.pop();
		file = path + STR("\\*");
		file.assign(file.c_str());

		find = FindFirstFile(file.c_str(), &foundData);
		if (find == INVALID_HANDLE_VALUE)
			return false;

		do
		{
			if (amount && (*amount) == -1)
				return false;
			bool found = false;
			for (vector<STRING>::iterator i = exclude->begin(); i != exclude->end(); ++i)
			{
				if (STRCMP(foundData.cFileName, i->c_str()) == 0)
				{
					found = true;
					break;
				}
			}
			if (found)
				continue;
			
			if (prefs->deep && foundData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				directories.push(path + STR("\\") + foundData.cFileName);
			else
				prefs->files.push(path + STR("\\") + foundData.cFileName);
			if (amount)
				(*amount) = prefs->files.size();
		}
		while (FindNextFile(find, &foundData) != 0);

		FindClose(find);
		if (GetLastError() != ERROR_NO_MORE_FILES)
			return false;

		find = INVALID_HANDLE_VALUE;
	}
#elif defined(UNIX)
	DIR * dp = 0;
	dirent * dirp = 0;

	while (!directories.empty())
	{
		path = directories.top();
		directories.pop();
		file = path;

		dp = opendir(path.c_str());
		if (!dp)
			continue;

		while (dirp = readdir(dp))
		{
			if (amount && (*amount) == -1)
			{
				closedir(dp);
				return false;
			}
			bool found = false;
			for (vector<STRING>::iterator i = exclude->begin(); i != exclude->end(); ++i)
			{
				if (STRCMP(dirp->d_name, i->c_str()) == 0)
				{
					found = true;
					break;
				}
			}
			if (found)
				continue;
			if (prefs->deep && dirp->d_type == DT_DIR)
				directories.push(path + STR("/") + dirp->d_name);
			else if (dirp->d_type == DT_REG)
				prefs->files.push(path + STR("/") + dirp->d_name);
			// Update amount
			if (amount)
				(*amount) = prefs->files.size();
		}
		closedir(dp);
	}
#endif // WIN32 / UNIX

	return true;
}