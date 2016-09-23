
// Dependies

#include <string>
#include <sstream>

#include <fstream>

#include "DataValues.hpp"
#include "ParseArguments.hpp"
#include "Computer.hpp"

#include "PixelMap.hpp"

#include "ListFiles.hpp"

// Parse a string to a list
std::list<int> str2list(const char * str, const int size)
{
	using namespace std;
	list<int> li;

	for (int i = 0, l = 0; true; ++i)
	{
		// Found space or end of line
		if (i >= size || str[i] == ' ')
		{
			string s;
			s.assign(str+l, str+i);
			li.push_back(atoi(s.c_str()));
			l = i+1;
			if (i >= size)
				break;
		}
	}

	return li;
}

// Parse a list to a string
std::string list2str(std::list<int> li)
{
	using namespace std;
	stringstream str;
	for (list<int>::iterator i = li.begin(); i != li.end(); ++i)
	{
		if (i == li.begin())
			str << (*i);
		else
			str << ' ' << (*i);
	}
	return str.str();
}

PixelMap::PixelMap()
: running(0), elapsed(0), level(0), 
processTime(0), nextDisplay(0), stageDisplay(1.0f / 30.0f), thread(0)
{
	level = new Level();
	go.folder = new STRING();
	go.save = new STRING();
	ResetArgs();
}

PixelMap::~PixelMap()
{
	delete go.save;
	delete go.folder;
	if (level)
		delete level;
}

// Initialize PixelMap
void PixelMap::Initialize()
{
	counter.Update();
	counter.Update();
	thread = 0;
}

// Shutdown various items
void PixelMap::Shutdown()
{
}

// Tells PixelMap to start
void PixelMap::Start()
{
	if (thread)
		return;
	go.running = true;
	go.error = LVL_OK;
	counter.Update();
	if (level)
		level->Reset();
#ifdef WIN32
	thread = CreateThread(NULL, 0, PixelMap::WorkingLevel, (void *)&go, 0, 0);
#elif defined(UNIX)
	thread = CreateThread(PixelMap::WorkingLevel, (void *)&go);
#endif
	if (thread <= 0)
		go.running = false;
}

void PixelMap::Stop()
{
	Abort();
	if (thread)
#ifdef WIN32
		WaitForSingleObject(thread, INFINITE);
#elif defined(UNIX)
		WaitThread(thread);
#endif
	if (level)
		level->ClearChunks();
	thread = 0;
}

void PixelMap::Abort()
{
	running = 0;
	elapsed = 0;
	processTime = 0;
	nextDisplay = 0;
	if (level)
		level->Abort();
}

// Get Error messages
std::string PixelMap::GetErrorMessage()
{
	if (!silent)
	{
		// None of these will be called if hwnd is not set. Good when closing the window.
		if (level->isSaved())
			return "The task was completed successfully.";
		else
		{
			std::string saveA;
			saveA.assign(save.begin(), save.end());
			switch (go.error)
			{
				// Canceled, ignore
				case LVL_ERR_CANCEL:
					break;
				case LVL_ERR_NOWORLD:
					return "No world folder found.";
					break;
				case LVL_ERR_LISTING:
					return "Unable to list files. Please try again.";
					break;
				case LVL_ERR_NOCHUNKS:
					return "Invalid world: No chunks found.";
					break;
				case LVL_ERR_INVALIDCHUNKS:
					return "Invalid world: Invalid chunk data.";
					break;
				case LVL_ERR_INVALIDSIZE:
					return "Invalid world: Invalid chunk coordinates.";
					break;
				case LVL_ERR_TOOLARGE:
					return "Reached allocation limit. Try again with cache parameter --cache.";
					break;
				case LVL_ERR_DUPLICATES:
					return "Duplicate chunks found. Unable to continue.";
					break;
				case LVL_ERR_CACHE:
					return "Unable to allocate cache file. Change directory to a privileged one.";
					break;
				case LVL_ERR_SAVE:
					return "Could not save " + saveA + ". Change directory to a privileged one.";
				case LVL_OK:
				default:
					return "An unknown error occured.\nPlease try again or send a note to the creator.";
			}
		}
	}
	return "";
}

// Command line arguments
void PixelMap::DoArgs(wchar_t ** commands, int nArgs)
{
	using namespace std;

	// Parameters
	ResetArgs();

	ParseArguments args = ParseArguments();

	UINT arguments[32] = {0};

	arguments[0] = args.AddParameter(L'w', PARAM_WSTRING); // World folder
	arguments[1] = args.AddParameter(L'f', PARAM_WSTRING); // Output file
	arguments[2] = args.AddParameter(L'l', PARAM_WSTRING); // Log file
	{
		wchar_t temp2[] = {
			'w', WATER, 
			'w', STATIONARYWATER, 
			'l', LAVA, 
			'l', STATIONARYLAVA, 
			0};
		arguments[3] = args.AddParameter(L'o', PARAM_INT | PARAM_LIST, temp2); // Only mode
		arguments[4] = args.AddParameter(L'x', PARAM_INT | PARAM_LIST, temp2); // Exclude mode
	}
	arguments[5] = args.AddParameter(L'n', PARAM_BOOL); // Night mode
	arguments[6] = args.AddParameter(L's', PARAM_INT | PARAM_LIST); // Slice mode
	arguments[7] = args.AddParameter(L'h', PARAM_STRING); // Height mode
	arguments[8] = args.AddParameter(L't', PARAM_INT); // Threads
	arguments[9] = args.AddParameter(L'c', PARAM_BOOL); // Cave mode
	arguments[14] = args.AddParameter(L'r', PARAM_INT); // Rotation mode
	arguments[10] = args.AddParameter(L"texture", PARAM_WSTRING); // Texture mode
	arguments[11] = args.AddParameter(L"no-render", PARAM_BOOL); // No render mode
	arguments[12] = args.AddParameter(L"nether", PARAM_BOOL); // Nether mode
	arguments[15] = args.AddParameter(L"no-gui", PARAM_BOOL); // No GUI mode
	arguments[16] = args.AddParameter(L"silent", PARAM_BOOL); // Silent mode
	arguments[17] = args.AddParameter(L"cache", PARAM_WSTRING); // Cache file
	arguments[18] = args.AddParameter(L"no-heightmap", PARAM_BOOL); // Disable heightmap
	arguments[19] = args.AddParameter(L"auto-close", PARAM_BOOL); // Closes application after rendering
	arguments[20] = args.AddParameter(L"open-folder", PARAM_BOOL); // Opens folder where saved
	arguments[21] = args.AddParameter(L"version", PARAM_STRING); // Set world version

	// Parse arguments
	args.PushArguments(commands, nArgs);

	// Version
	string t;
	if (args.GetAdvancedParameter(arguments[21])->Get(&t) && !t.empty())
	{
		// Make lowercase
		for (UINT i = 0; i < t.size(); ++i)
			t[i] = tolower(t[i]);
		// Alpha
		if (t.compare("alpha") == 0)
		{
			worldVersion = ALPHA;
		}
		else if (t.compare("beta") == 0)
		{
			worldVersion = BETA;
		}
	}

	// Get world
	wstring temp;
	if (args.GetParameter(0)->Get(&temp))
	{
		if (worldVersion == ALPHA)
		{
			if (temp.size())
				map = temp;
			if (map < 0 || map > 5)
				map = 1;
			if (map > 0 && map <= 5)
			{
				map.s = L"World" + map.s;
			}
		}
		else
		{
			map = temp;
		}
	}
	args.GetParameter(arguments[1])->Get(&saveFile);
	if (args.GetParameter(arguments[2])->Get(&logFile) && logFile.empty())
		logFile = L"log.txt";
	args.GetParameter(arguments[3])->Get(&only);
	args.GetParameter(arguments[4])->Get(&exclude);
	args.GetParameter(arguments[5])->Get(&night);
	if (args.GetParameter(arguments[6])->Get(&slice))
	{
		if (slice.size() > 2)
			slice.resize(2);
		if (slice.size() >= 1 && (slice.front() < 0 || slice.front() > 127))
		{
			slice.clear();
			slice.push_back(-1);
		}
	}
	// Height parameter
	{
		wstring temp;
		args.GetParameter(arguments[7])->Get(&temp);
		height = _wtoi(temp.c_str());
		if (height == 0)
		{
			wchar_t t = temp[0];
			switch (t)
			{
				// Grey
				case L'g':
					height = -1;
					break;
				// Color
				case L'c':
					height = -2;
					break;
				// Solid
				case L's':
					height = -3;
					break;
			}
		}
		else
		{
			// Limit
			if (height <= 1 || height > MAPZ / 8)
				height = 0;
		}
	}
	if (args.GetParameter(arguments[8])->Get(&threads) && threads == 0)
		// Automatic detection
		threads = Computer::GetLogicalCores();
	// Failproof
	if (threads < 1 || threads > 255)
		threads = 1;
	args.GetParameter(arguments[9])->Get(&caveMode);
	if (!args.GetParameter(arguments[14])->Get(&rotation))
		rotation = 0;
	args.GetAdvancedParameter(arguments[10])->Get(&texture);
	args.GetAdvancedParameter(arguments[11])->Get(&noRender);
	args.GetAdvancedParameter(arguments[12])->Get(&nether);
	args.GetAdvancedParameter(arguments[15])->Get(&nogui);
	args.GetAdvancedParameter(arguments[16])->Get(&silent);
	if (args.GetAdvancedParameter(arguments[17])->Get(&cache) && cache.empty())
	{
		// Default cache
		STRING str = GetTemp();
		cache.assign(str.begin(), str.end());
	}
	args.GetAdvancedParameter(arguments[18])->Get(&noHeightMap);
	args.GetAdvancedParameter(arguments[19])->Get(&autoClose);
	args.GetAdvancedParameter(arguments[20])->Get(&openFolder);

	if (nogui)
		autoClose = true;

	// Only one of them
	if (!only.empty())
		exclude.clear();

	PrepareMap();

	PrepareArgs();
}

#ifdef WIN32
// Go through all gui controls
void PixelMap::DoArgs(GUI_Creator * gui)
{
	if (!gui)
		return;
	using namespace std;

	// Parameters
	ResetArgs();

	BOOL b;

	if (gui->Get(GUI_ALPHA, b))
	{
		worldVersion = ALPHA;
	}
	else if (gui->Get(GUI_BETA, b))
	{
		worldVersion = BETA;
	}
	if (gui->Get(GUI_WORLD, b))
	{
		wstring temp;
		//gui->Get(GUI_WORLDFILE, temp);
		int i = 0;
		gui->GetData(GUI_WORLDFILE, &i);
		STRING temp2 = worlds[i];
		temp.assign(temp2.begin(), temp2.end());
		map = temp;
		if (worldVersion == ALPHA)
		{
			if (map < 0 || map > 5)
				map = 1;
			if (map > 0 && map <= 5)
			{
				map = L"World" + map.s;
			}
		}
	}
	else if (gui->Get(GUI_CUSTOM, b))
	{
		wstring temp;
		gui->Get(GUI_CUSTOMFILE, temp);
		map = temp;
	}

	// Savefile
	gui->Get(GUI_OUTPUT, saveFile);

	// Texture
	gui->Get(GUI_TEXTURE, texture);

	// Cache
	if (gui->Get(GUI_CACHE, b) && gui->Get(GUI_CACHEFILE, cache) && cache.empty())
	{
		// Default cache
		STRING str = GetTemp();
		cache.assign(str.begin(), str.end());
	}

	// Log
	if (gui->Get(GUI_LOG, b) && gui->Get(GUI_LOGFILE, logFile) == 0)
		logFile = L"log.txt";

	// Bools
	nether = !!gui->Get(GUI_NETHER, b);
	night = !!gui->Get(GUI_NIGHT, b);
	noRender = !!gui->Get(GUI_NORENDER, b);
	caveMode = !!gui->Get(GUI_CAVEMODE, b);
	autoClose = !!gui->Get(GUI_AUTO_CLOSE, b);
	openFolder = !!gui->Get(GUI_OPEN_FOLDER, b);
	noHeightMap = !!gui->Get(GUI_NOHEIGHTMAP, b);

	// Rotation
	{
		wstring temp;
		gui->Get(GUI_ROTATE, temp);
		rotation = _wtoi(temp.c_str());
	}

	// Modes
	// Normal
	if (gui->Get(GUI_NORMAL, b))
		height = 0;
	// Height
	else if (gui->Get(GUI_FREQ, b))
	{
		wstring temp;
		gui->Get(GUI_FREQ_H, temp);
		height = _wtoi(temp.c_str());
		// Limit
		if (height <= 1 || height > MAPZ / 8)
			height = 0;
	}
	// Gray
	else if (gui->Get(GUI_GRAY, b))
		height = -1;
	// Color
	else if (gui->Get(GUI_COLOR, b))
		height = -2;
	// Solid
	else if (gui->Get(GUI_SOLID, b))
		height = -3;

	// Threads
	{
		wstring temp;
		if (gui->Get(GUI_THREADS, temp))
			threads = _wtoi(temp.c_str());
		if (threads < 1)
			threads = 1;
	}

	// Slice
	{
		wstring temp;
		int tt = -1;
		if (gui->Get(GUI_SLICE, temp))
			tt = _wtoi(temp.c_str());
		if (tt < 0 || tt > MAPZ)
			tt = -1;
		slice.push_back(tt);
		gui->Get(GUI_SLICE_SINGLE, tt);
		slice.push_back(tt);
	}

	// Only and exclude, only one of them
	{
		std::string str;
		int size = 0;
		// Only
		if (size = gui->Get(GUI_ONLY, str))
		{
			only = str2list(str.c_str(), size);
		}
		// Exclude
		else if (size = gui->Get(GUI_ONLY, str))
		{
			exclude = str2list(str.c_str(), size);
		}
	}

	gui->Get(GUI_TEXTURE, texture);

	PrepareMap();

	PrepareArgs();
}

// Set the args for the GUI
void PixelMap::GetArgs(GUI_Creator * gui)
{
	if (!gui)
		return;
	using namespace std;

	gui->Set(GUI_ALPHA, worldVersion == ALPHA);
	gui->Set(GUI_BETA, worldVersion == BETA);

	bool worldfile = map.s.find_first_of(L'/') == -1 && map.s.find_first_of(L'\\') == -1;
	gui->Set(GUI_WORLD, worldfile);
	gui->Set(GUI_CUSTOM, !worldfile);
	gui->Enable(GUI_WORLDFILE, worldfile);
	gui->Enable(GUI_CUSTOMFILE, !worldfile);
	if (worldfile)
		gui->Set(GUI_WORLDFILE, map.s);
	else
		gui->Set(GUI_CUSTOMFILE, map.s);

	// Savefile
	gui->Set(GUI_OUTPUT, saveFile);

	// Texture
	gui->Set(GUI_TEXTURE, texture);

	// Cache
	if (!cache.empty())
	{
		gui->Set(GUI_CACHE, TRUE);
		gui->Enable(GUI_CACHEFILE, TRUE);
		gui->Set(GUI_CACHEFILE, cache);
	}

	// Log
	if (!logFile.empty())
	{
		gui->Set(GUI_LOG, TRUE);
		gui->Enable(GUI_LOGFILE, TRUE);
		gui->Set(GUI_LOGFILE, logFile);
	}

	// Bools
	gui->Set(GUI_NETHER, nether);
	gui->Set(GUI_NIGHT, night);
	gui->Set(GUI_NORENDER, noRender);
	gui->Set(GUI_CAVEMODE, caveMode);
	gui->Set(GUI_AUTO_CLOSE, autoClose);
	gui->Set(GUI_OPEN_FOLDER, openFolder);
	gui->Set(GUI_NOHEIGHTMAP, noHeightMap);

	// Rotation
	gui->Set(GUI_ROTATE, rotation / 90);

	gui->Set(GUI_NORMAL, FALSE);
	gui->Set(GUI_FREQ, FALSE);
	gui->Set(GUI_GRAY, FALSE);
	gui->Set(GUI_COLOR, FALSE);
	gui->Set(GUI_SOLID, FALSE);
	gui->Enable(GUI_FREQ_H, FALSE);
	// Modes
	switch (height)
	{
		case 0:
			gui->Set(GUI_NORMAL, TRUE);
			break;
		case -1:
			gui->Set(GUI_GRAY, TRUE);
			break;
		case -2:
			gui->Set(GUI_COLOR, TRUE);
			break;
		case -3:
			gui->Set(GUI_SOLID, TRUE);
			break;
		default:
			gui->Set(GUI_FREQ, TRUE);
			gui->Enable(GUI_FREQ_H, TRUE);
			stringstream str;
			str << height;
			gui->Set(GUI_FREQ_H, str.str());
	}

	// Threads
	{
		stringstream str;
		str << threads;
		gui->Set(GUI_THREADS, str.str());
	}

	// Slice
	{
		if (slice.size() > 0)
		{
			stringstream str;
			str << slice.front();
			gui->Set(GUI_SLICE, str.str());
		}
		if (slice.size() > 1)
		{
			gui->Set(GUI_SLICE_SINGLE, slice.back());
		}
	}

	// Init block colors
	if (!texture.empty())
		gui->Set(GUI_TEXTURE, texture);

	// Only
	if (only.size())
	{
		gui->Set(GUI_ONLY, list2str(only));
	}
	else if (exclude.size())
	{
		gui->Set(GUI_EXCLUDE, list2str(exclude));
	}
}

// Get worlds in world folder
void PixelMap::GetWorlds(GUI_Creator *gui)
{
	if (!gui)
		return;

	STRING home = GetSaveFolder();

	FileSearch search = FileSearch();
	search.path = home;
	search.deep = false;
	if (!ListFiles(&search))
		return;

	while (search.files.size())
	{
		STRING file = search.files.top();
		search.files.pop();
		Level lvl = Level();
		lvl.LoadLevel(file);
		if (lvl.info.seed)
		{
			int i = worlds.size();
			worlds.push_back(Port::FileName(file));
			gui->Append(GUI_WORLDFILE, lvl.info.name, i);
		}
	}
	gui->Set(GUI_WORLDFILE, 0);
}

#endif // WIN32

// Clear arguments for next use
void PixelMap::ResetArgs()
{
	slice.clear();
	height = 0;
	worldVersion = BETA;
	map = World();
	map.n = 1;
	map.s = L"World1";
	night = false;
	noRender = false;
	caveMode = false;
	nether = false;
	noHeightMap = false;
	threads = 1;
	rotation = 0;
	nogui = false;
	silent = false;
	autoClose = false;
	openFolder = false;
}

// Prepare arguments
void PixelMap::PrepareArgs()
{
	using namespace std;

	// Add parameters
	LevelPrefs prefs = LevelPrefs();

	// Init block colors
	if (texture.empty())
		DataValues::MakeColor();
	else
	{
		STRING t;
		t.assign(texture.begin(), texture.end());
		DataValues::MakeColor(t);
	}

	// Pick world version
	prefs.version = worldVersion;

	STRINGSTREAM str;
	// Slice
	if (slice.size() && slice.front() != -1)
	{
		prefs.z = (BYTE)slice.front();
		str << STR("-slice") << (int)prefs.z;
		if (slice.size() == 2 && slice.back() != 0)
		{
			prefs.flags |= CHUNKP_SINGLESLICE;
			str << STR("S");
		}
	}
	// Line heightmap
	if (height > 0)
	{
		prefs.h = (BYTE)height;
		str << STR("-height") << height;
	}
	// Gray heightmap
	else if (height == -1)
	{
		prefs.flags |= CHUNKP_GRAY;
		str << STR("-gray");
	}
	// Color heightmap
	else if (height == -2)
	{
		prefs.flags |= CHUNKP_COLOR;
		str << STR("-color");
	}
	// Solid heightmap
	else if (height == -3)
	{
		prefs.flags |= CHUNKP_SOLID;
		str << STR("-solid");
	}
	// Night
	if (night)
	{
		prefs.flags |= CHUNKP_NIGHT;
		str << STR("-night");
	}
	// Cavemode
	if (caveMode)
	{
		prefs.flags |= CHUNKP_CAVE;
		str << STR("-cave");
	}
	// Nether
	if (nether)
	{
		// Cave is only because nether is below surface
		prefs.flags |= CHUNKP_CAVE;
		str << STR("-nether");
		folder.append(STR("\\DIM-1"));
	}
	// Rotation
	if (rotation != 0)
	{
		prefs.rotation = rotation;
		str << STR("-rot") << rotation;
	}
	// No rendering
	if (noRender)
		prefs.flags |= CHUNKP_NORENDER;
	prefs.threads = (BYTE)threads;
	// Add log file
	prefs.log.assign(logFile.begin(), logFile.end());
	prefs.log = Port::ReplacePath(prefs.log);
	// Add cache folder
	prefs.cache.assign(cache.begin(), cache.end());
	// Use only these blocks
	if (!only.empty())
	{
		memset(prefs.block, false, DATAVALUES_AMOUNT);
		for (list<int>::iterator i = only.begin(); i != only.end(); ++i)
			if (*i >= 0 && *i < 256)
				prefs.block[*i] = true;
	}
	// Exclude these blocks
	else if (!exclude.empty())
	{
		for (list<int>::iterator i = exclude.begin(); i != exclude.end(); ++i)
			if (*i >= 0 && *i < 256)
				prefs.block[*i] = false;
	}
	// Special savefile
	if (saveFile.size() == 0)
	{
		save.assign(save.c_str());
		save.append(str.str() + STR(".png"));
	}
	// Disable heightmap
	if (noHeightMap)
		prefs.flags |= CHUNKP_NOHEIGHTMAP;

	// Validate folders
	folder = Port::ReplacePath(folder);
	save = Port::ReplacePath(save);

	level->prefs = prefs;
	
	go.level = level;
	go.folder->assign(folder.c_str());
	go.save->assign(save.c_str());

	counter.Update(); // Start here
}

// Prepare map path
void PixelMap::PrepareMap()
{
	using namespace std;

	// Define world folder
	STRING home = GetSaveFolder();
	if (worldVersion == ALPHA)
	{
		if (map.n != 0)
		{
			world.assign(map.s.begin(), map.s.end());
			folder = home + STR("\\") + world;
		}
		else
		{
			folder.assign(map.s.begin(), map.s.end());
		}
	}
	else if (worldVersion == BETA)
	{
		if (map.s.size())
		{
			if (map.s.find_first_of(L'/') == -1 && map.s.find_first_of(L'\\') == -1)
			{
				world.assign(map.s.begin(), map.s.end());
				folder = home + STR("\\") + world;
			}
			else
			{
				folder.assign(map.s.begin(), map.s.end());
			}
			folder.assign(folder.c_str());
			if (!nether)
				folder.append(STR("\\region"));
		}
		else
		{
			go.error = LVL_ERR_INVALIDWORLD;
		}
	}
	if (nether)
		folder.append(STR("\\DIM-1"));
	// Specified save file
	if (saveFile.size() > 0)
	{
		saveFile = Port::Replace(saveFile, L'/', L'\\');
		save.assign(saveFile.begin(), saveFile.end());
	}
	// Use world folder name
	else if (map.n == 0)
	{
		wstring temp = map.s;
		int num = temp.find_last_of(L'\\');
		if (num >= 0)
			temp = temp.substr(num+1);
		else
		{
			num = temp.find_last_of(L'/');
			if (num >= 0)
				temp = temp.substr(num+1);
		}
		save.assign(STR(".\\"));
		save.append(temp.begin(), temp.end());
	}
	// Default worlds
	else
	{
		save.assign(STR(".\\") + world);
	}
}

// Update latest results
bool PixelMap::Update()
{
	counter.Update();
	elapsed = counter.GetElapsed();
	running += elapsed;

	Level::State state = level->state;
	if (!processTime && state == Level::PROCESSING)
		processTime = running;

	return go.running;
}

// Check if the values needs to be displayed
bool PixelMap::Display()
{
	// Reduce displaying rate
	if (nextDisplay > running)
	{
		Port::Rest(1);
		return false;
	}
	else
	{
		nextDisplay = running + stageDisplay;
		return true;
	}
}

// Get status
int PixelMap::GetState()
{
	if (!level)
		return 0;
	return level->state;
}
int PixelMap::GetPercent()
{
	if (!level)
		return 0;
	return int(level->GetDone() * 100.0f);
}
int PixelMap::GetQueue()
{
	if (!level)
		return 0;
	return level->GetWork();
}
int PixelMap::GetFinished()
{
	if (!level)
		return 0;
	return level->GetResult();
}
int PixelMap::GetChunks()
{
	if (!level)
		return 0;
	return level->GetTotal();
}
int PixelMap::GetTime()
{
	return (int)running;
}
int PixelMap::GetTimeLeft()
{
	if (!level)
		return 0;
	float r = running - processTime;
	return (level->GetDone() == 0) ? 0 : int((r / level->GetDone()) - r);
}

std::string PixelMap::GetSave()
{
	return std::string(save.begin(), save.end());
}

bool PixelMap::NoGui()
{
	return nogui;
}

bool PixelMap::OpenFolder()
{
	return openFolder;
}

bool PixelMap::AutoClose()
{
	return autoClose;
}

// Get temporary path
STRING PixelMap::GetTemp()
{
#ifdef WIN32
	STRING tmp = Port::GetEnv("TEMP");
#elif defined(UNIX)
	STRING tmp = STR("/tmp");
#endif
	return tmp;
}

// Get save folder where minecraft store its saves
STRING PixelMap::GetSaveFolder()
{
#ifdef WIN32
	STRING appdata = Port::GetEnv("APPDATA");
#elif defined(UNIX)
	STRING appdata = Port::GetEnv("HOME");
#endif
	appdata.assign(appdata.c_str() + STR("\\.minecraft\\saves"));
	return appdata;
}

// Working thread
THREAD_FUNCTION(PixelMap::WorkingLevel, data)
{
	GoThread * go = (GoThread*)data;
	Level * level = go->level;
	if (level)
	{
		STRING folder = *go->folder;
		go->error = level->LoadWorld(folder);
		if (go->error != LVL_OK)
		{
			go->running = false;
			return 0;
		}
		STRING save = *go->save;
		go->error = level->CreateImage(save);
		if (go->error != LVL_OK)
		{
			go->running = false;
			return 0;
		}

		STRING w = Port::FileName(folder);
		std::string world;
		world.assign(w.begin(), w.end());

		// Save log
		if (level->prefs.logging)
		{
			using namespace std;

			ofstream file(level->prefs.log.c_str());
			if (file.is_open())
			{
				INT64 total = 0;
				for (int i = 0; i < DATAVALUES_AMOUNT; ++i)
					total += level->amount.block[i];

				int digits = 0;
				{
					INT64 t = level->amount.filesize;
					t /= 1000000;
					while (t > 0)
					{
						++digits;
						t /= 10;
					}
				}
				double size = level->amount.filesize;
				size /= 1000000;

				stringstream stream;
				stream.setf(ios_base::dec, ios::floatfield);
				stream.precision(2 + digits);
#define END "\n"
#define SEP(t) setSeparator(t)
				stream << world << END
					<< size << "MB" << END << END

					<< "Map surface: " << SEP(level->blocks) << END
					<< "Total blocks: " << SEP(total) << END << END

					<< "Air: " << SEP(level->amount.block[AIR]) << END
					<< "Stone: " << SEP(level->amount.block[STONE]) << END
					<< "Dirt: " << SEP(level->amount.block[GRASS] + level->amount.block[DIRT]) << END
					<< "Water: " << SEP(level->amount.block[WATER] + level->amount.block[STATIONARYWATER]) << END
					<< "Clay: " << SEP(level->amount.block[CLAY]) << END
					<< "Sand: " << SEP(level->amount.block[SAND]) << END
					<< "Gravel: " << SEP(level->amount.block[GRAVEL]) << END << END

					<< "Log: " << SEP(level->amount.block[LOG]) << END
					<< "Leaves: " << SEP(level->amount.block[LEAVES]) << END
					<< "Sapling: " << SEP(level->amount.block[SAPLING]) << END << END

					<< "Soil: " << SEP(level->amount.block[SOIL]) << END
					<< "Crops: " << SEP(level->amount.block[CROPS]) << END << END

					<< "Reed: " << SEP(level->amount.block[REED]) << END
					<< "Cactus: " << SEP(level->amount.block[CACTUS]) << END
					<< "Red Mushroom: " << SEP(level->amount.block[REDMUSHROOM]) << END
					<< "Brown Mushroom: " << SEP(level->amount.block[BROWNMUSHROOM]) << END
					<< "Red rose: " << SEP(level->amount.block[REDROSE]) << END
					<< "Yellow flower: " << SEP(level->amount.block[YELLOWFLOWER]) << END << END

					<< "Ice: " << SEP(level->amount.block[ICE]) << END
					<< "Snow: " << SEP(level->amount.block[SNOW]) << END
					<< "Snow Block: " << SEP(level->amount.block[SNOWBLOCK]) << END << END

					<< "Fire: " << SEP(level->amount.block[FIRE]) << END << END

					<< "Diamond Ore: " << SEP(level->amount.block[DIAMONDORE]) << END
					<< "Iron Ore: " << SEP(level->amount.block[IRONORE]) << END
					<< "Coal Ore: " << SEP(level->amount.block[COALORE]) << END
					<< "Gold Ore: " << SEP(level->amount.block[GOLDORE]) << END
					<< "Redstone: " << SEP(level->amount.block[REDSTONEORE] + level->amount.block[GLOWINGREDSTONEORE]) << END
					<< "Lapiz Lazuli Ore: " << SEP(level->amount.block[LAPIZLAZULIORE]) << END
					<< "Obsidian: " << SEP(level->amount.block[OBSIDIAN]) << END
					<< "Mossy Cobblestone: " << SEP(level->amount.block[MOSSYCOBBLESTONE]) << END
					<< "Bedrock (Adminium): " << SEP(level->amount.block[BEDROCK]) << END << END

					<< "Lava: " << SEP(level->amount.block[LAVA] + level->amount.block[STATIONARYLAVA]) << END << END

					<< "Cobblestone: " << SEP(level->amount.block[COBBLESTONE]) << END
					<< "Wood: " << SEP(level->amount.block[WOOD]) << END
					<< "Glass: " << SEP(level->amount.block[GLASS]) << END
					<< "Brick: " << SEP(level->amount.block[BRICK]) << END
					<< "Sandstone: " << SEP(level->amount.block[SANDSTONE]) << END
					<< "Cobblestone Stairs: " << SEP(level->amount.block[COBBLESTONESTAIRS]) << END
					<< "Wooden Stairs: " << SEP(level->amount.block[WOODENSTAIRS]) << END
					<< "Step: " << SEP(level->amount.block[SLAB_STONE]) << END
					<< "Double Step: " << SEP(level->amount.block[DOUBLESLAB_STONE]) << END << END

					<< "Torch: " << SEP(level->amount.block[TORCH]) << END
					<< "Wooden Door: " << SEP(level->amount.block[WOODENDOOR]) << END
					<< "Iron Door: " << SEP(level->amount.block[IRONDOOR]) << END
					<< "Chest: " << SEP(level->amount.block[CHEST]) << END
					<< "Bookcase: " << SEP(level->amount.block[BOOKSHELF]) << END
					<< "Workbench: " << SEP(level->amount.block[WORKBENCH]) << END
					<< "Furnace: " << SEP(level->amount.block[FURNACE] + level->amount.block[BURNINGFURNACE]) << END
					<< "Dispencer: " << SEP(level->amount.block[DISPENSER]) << END
					<< "Sign: " << SEP(level->amount.block[SIGNPOST] + level->amount.block[WALLSIGN]) << END
					<< "Ladder: " << SEP(level->amount.block[LADDER]) << END
					<< "Minecart Tracks: " << SEP(level->amount.block[MINECARTTRACK]) << END
					<< "Jukebox: " << SEP(level->amount.block[JUKEBOX]) << END
					<< "Note Block: " << SEP(level->amount.block[NOTEBLOCK]) << END
					<< "Fence: " << SEP(level->amount.block[FENCE]) << END
					<< "Wool: " << SEP(level->amount.block[WOOL]) << END << END

					<< "Lapiz Lazuli Block: " << SEP(level->amount.block[LAPIZLAZULIBLOCK]) << END
					<< "Diamond Block: " << SEP(level->amount.block[DIAMONDBLOCK]) << END
					<< "Gold Block: " << SEP(level->amount.block[GOLDBLOCK]) << END
					<< "Iron Block: " << SEP(level->amount.block[IRONBLOCK]) << END << END

					<< "Redstone Torch: " << SEP(level->amount.block[REDSTONETORCHON] + level->amount.block[REDSTONETORCHOFF]) << END
					<< "Redstone Wire: " << SEP(level->amount.block[REDSTONEWIRE]) << END
					<< "Lever: " << SEP(level->amount.block[LEVER]) << END
					<< "Stone Button: " << SEP(level->amount.block[STONEBUTTON]) << END
					<< "Stone Pressure Plates: " << SEP(level->amount.block[STONEPRESSUREPLATE]) << END
					<< "Wodden Pressure Plates: " << SEP(level->amount.block[WOODPRESSUREPLATE]) << END << END

					<< "TNT: " << SEP(level->amount.block[TNT]) << END
					<< "Sponge: " << SEP(level->amount.block[SPONGE]) << END
					<< "Cake: " << SEP(level->amount.block[CAKE]) << END << END

					<< "Mob Spawner: " << SEP(level->amount.block[MOBSPAWNER]) << END << END

					<< "Zombies: " << SEP(level->amount.zombie) << END
					<< "Creeper: " << SEP(level->amount.creeper) << END
					<< "Skeletons: " << SEP(level->amount.skeleton) << END
					<< "Slimes: " << SEP(level->amount.slime) << END
					<< "Pig Zombies: " << SEP(level->amount.pigzombie) << END
					<< "Ghast: " << SEP(level->amount.ghast) << END << END

					<< "Pigs: " << SEP(level->amount.pig) << END
					<< "Chickens: " << SEP(level->amount.chicken) << END
					<< "Cows: " << SEP(level->amount.cow) << END
					<< "Sheeps: " << SEP(level->amount.sheep);
				string str = stream.str();
				const char * c = str.c_str();

				file << c;

				file.close();
			}
		}
	}

	go->running = false;
	return 0;
}