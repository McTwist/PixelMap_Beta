
#pragma once
#ifndef PIXELMAP_HPP
#define PIXELMAP_HPP

#include <string>
#include <vector>
#include "Utility.hpp"

#include "Globals.hpp"
#include "Level.hpp"

#ifdef WIN32
#include "GUI.hpp"
#endif

#include "Counter.h"

#ifdef UNIX
#include "parallel.h"
typedef THREAD HANDLE;
#endif

struct GoThread
{
	GoThread()
	{
		memset(this, 0, sizeof(GoThread));
	}
	Level * level;
	STRING * folder;
	STRING * save;
	int error;
	bool running;
};

class PixelMap
{
public:
	PixelMap();
	~PixelMap();
	void Initialize();
	void Shutdown();
	void Start();
	void Stop();
	void Abort();
	std::string GetErrorMessage();
	void DoArgs(wchar_t ** commands, int nArgs);
#ifdef WIN32
	void DoArgs(GUI_Creator * gui);
	void GetArgs(GUI_Creator * gui);
	void GetWorlds(GUI_Creator * gui);
#endif
	bool Update();
	bool Display();

	int GetState();
	int GetPercent();
	int GetQueue();
	int GetFinished();
	int GetChunks();
	int GetTime();
	int GetTimeLeft();
	bool NoGui();
	bool OpenFolder();
	bool AutoClose();
	std::string GetSave();
private:

	float elapsed;
	float running;

	Counter counter;

	Level * level;

	GoThread go;
	HANDLE thread;
	static THREAD_FUNCTION(WorkingLevel, data);

	float processTime;
	float nextDisplay;
	float stageDisplay;

	// Flags
	bool silent;
	bool nogui;
	bool autoClose;
	bool openFolder;

	// Parameters
	MAP_VERSION worldVersion;
	int height;
	std::list<int> slice;
	World map;
	std::wstring saveFile;
	std::list<int> only;
	std::list<int> exclude;
	bool night;
	bool noRender;
	bool caveMode;
	bool nether;
	bool noHeightMap;
	std::wstring logFile;
	int threads;
	std::wstring texture;
	int rotation;
	std::wstring cache;
	// Temporary variables
	STRING world;
	STRING folder;
	STRING save;
	std::vector<STRING> worlds;

	void ResetArgs();
	void PrepareArgs();
	void PrepareMap();

	// Variables
	STRING GetTemp();
	STRING GetSaveFolder();
};

#endif // PIXELMAP_HPP