

#pragma once
#ifndef GUI_H
#define GUI_H
#include <windows.h>
#include <string>

enum GUI_CONTROLS
{
	GUI_STATIC,
	GUI_NIGHT,
	GUI_NORENDER,
	GUI_CAVEMODE,
	GUI_NOHEIGHTMAP,
	GUI_NORMAL,
	GUI_FREQ,
	GUI_COLOR,
	GUI_GRAY,
	GUI_SOLID,
	GUI_FREQ_H,
	GUI_ROTATE,
	GUI_NETHER,
	GUI_THREADS,
	// Type
	GUI_ALPHA,
	GUI_BETA,
	// World
	GUI_WORLD,
	GUI_WORLDFILE,
	GUI_CUSTOM,
	GUI_CUSTOMFILE,
	
	GUI_ONLY,
	GUI_EXCLUDE,
	GUI_SLICE,
	GUI_SLICE_SINGLE,
	// Texture
	GUI_TEXTURE,
	// Output
	GUI_OUTPUT,
	GUI_OPEN_FOLDER,
	// Log
	GUI_LOG,
	GUI_LOGFILE,
	// Cache
	GUI_CACHE,
	GUI_CACHEFILE,
	GUI_OTHER,
	GUI_AUTO_CLOSE,
	GUI_RENDER,
	GUI_CANCEL,
	GUI_HELP,
	// Rendering
	GUI_WORK,
	GUI_RESULT,
	GUI_PERCENT,
	GUI_STATE,
	GUI_TIME,
	GUI_LEFT,
	GUI_TOTAL,
	GUI_BAR,
	GUI_MESSAGE,
	GUI_AMOUNTS
};

class GUI_Creator
{
public:
	bool Initialize(HINSTANCE);
	void Create(HWND);
	void onChange(int id);
	void Enable(int id, BOOL);
	void Show(int id, BOOL);
	BOOL Get(int id, int &);
	void Set(int id, const int);
	DWORD Get(int id, std::string &);
	DWORD Get(int id, std::wstring &);
	void Set(int id, const std::string);
	void Set(int id, const std::wstring);
	void Set(int);
	void Append(int id, const std::string);
	void Append(int id, const std::string, int);
	void Append(int id, const std::string, void *);
	BOOL GetData(int id, int * value);
	BOOL GetData(int id, void ** value);
	void ClearStatus();

	bool CheckRender();
	bool DoRender();
	void SetRender(bool);
private:
	HWND hwnd;
	HINSTANCE hInstance;
	HWND controls[GUI_AMOUNTS];
	int types[GUI_AMOUNTS];

	bool render;
	bool isRender;
	HFONT font;

	HWND Add(int type, int x, int y, int w, int h, int id, const char * label = NULL);
};

#endif // GUI_H