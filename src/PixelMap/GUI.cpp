
#ifdef WIN32

#include "GUI.hpp"

#include <sstream> // Progressbar

bool GUI_Creator::Initialize(HINSTANCE hi)
{
	hInstance = hi;
	hwnd = 0;
	render = false;
	isRender = false;
	memset(controls, 0, sizeof(controls));
	return true;
}

#define STATIC		0
#define EDIT		1
#define BUTTON		2
#define CHECKBOX	3
#define RADIO		4
#define FIELDSET	5
#define DROPDOWN	6
#define PROGRESS	7
#define TRACKBAR	8

void GUI_Creator::Create(HWND wnd)
{
	hwnd = wnd;
	font = CreateFontA(15, 
		0, 0, 0, FW_NORMAL,
		FALSE, FALSE, 0, ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, 
		DEFAULT_PITCH | FF_SWISS, 
		"Arial");

	// GUI
	Add(STATIC, 0, 0, 500, 500, GUI_STATIC); // Background
	// Type
	Add(RADIO, 25, 5, 65, 20, GUI_ALPHA, "Alpha");
	Add(RADIO, 100, 5, 65, 20, GUI_BETA, "Beta");
	Set(GUI_BETA, TRUE);
	// World
	//Add(DROPDOWN, 70, 30, 50, 120, GUI_WORLDFILE, "0");
	Add(DROPDOWN, 70, 30, 155, 120, GUI_WORLDFILE);
	Add(RADIO, 5, 30, 65, 20, GUI_WORLD, "World");
	Add(RADIO, 5, 55, 65, 20, GUI_CUSTOM, "Custom");
	Set(GUI_WORLD, TRUE);
	Add(EDIT, 70, 55, 155, 20, GUI_CUSTOMFILE);
	Enable(GUI_CUSTOMFILE, FALSE);
	// Mode
	Add(RADIO, 10, 85, 110, 20, GUI_NORMAL, "Normal");
	Add(RADIO, 10, 105, 90, 20, GUI_FREQ, "Heightmap");
	Add(RADIO, 10, 125, 100, 20, GUI_COLOR, "Height, color");
	Add(RADIO, 10, 145, 100, 20, GUI_GRAY, "Height, gray");
	Add(RADIO, 10, 165, 100, 20, GUI_SOLID, "Solid");
	Set(GUI_NORMAL, TRUE);
	Add(EDIT, 105, 105, 25, 20, GUI_FREQ_H, "4");
	Enable(GUI_FREQ_H, FALSE);

	Add(STATIC, 240, 5, 60, 20, GUI_STATIC, "Cores");
	Add(EDIT, 300, 5, 25, 20, GUI_THREADS, "1");
	Add(STATIC, 240, 30, 60, 20, GUI_STATIC, "Only");
	Add(EDIT, 300, 30, 80, 20, GUI_ONLY);
	Add(STATIC, 240, 55, 60, 20, GUI_STATIC, "Exclude");
	Add(EDIT, 300, 55, 80, 20, GUI_EXCLUDE);
	Add(STATIC, 240, 80, 60, 20, GUI_STATIC, "Rotation");
	Add(DROPDOWN, 300, 80, 50, 120, GUI_ROTATE, "0");
	Append(GUI_ROTATE, "90");
	Append(GUI_ROTATE, "180");
	Append(GUI_ROTATE, "270");
	Add(STATIC, 240, 105, 70, 20, GUI_STATIC, "Slice (0-127)");
	Add(EDIT, 314, 105, 32, 20, GUI_SLICE);
	Add(CHECKBOX, 350, 105, 50, 20, GUI_SLICE_SINGLE, "");

	Add(CHECKBOX, 240, 130, 100, 20, GUI_NORENDER, "Log only");
	Add(CHECKBOX, 240, 150, 100, 20, GUI_NETHER, "Nether");
	Add(CHECKBOX, 240, 170, 100, 20, GUI_NIGHT, "Night");
	Add(CHECKBOX, 240, 190, 100, 20, GUI_CAVEMODE, "Cavemode");
	Add(CHECKBOX, 240, 210, 130, 20, GUI_NOHEIGHTMAP, "Disable heightmap");
	Add(CHECKBOX, 240, 230, 100, 20, GUI_OPEN_FOLDER, "Open folder");
	Add(CHECKBOX, 240, 250, 100, 20, GUI_AUTO_CLOSE, "Auto-close");

	// Texture
	Add(STATIC, 5, 195, 40, 20, GUI_STATIC, "Texture");
	Add(EDIT, 50, 195, 175, 20, GUI_TEXTURE);
	// Cache
	Add(CHECKBOX, 5, 220, 60, 20, GUI_CACHE, "Cache");
	Add(EDIT, 70, 220, 155, 20, GUI_CACHEFILE);
	Enable(GUI_CACHEFILE, FALSE);
	// Log
	Add(CHECKBOX, 5, 245, 40, 20, GUI_LOG, "Log");
	Add(EDIT, 55, 245, 170, 20, GUI_LOGFILE);
	Enable(GUI_LOGFILE, FALSE);
	// Output
	Add(STATIC, 5, 270, 55, 20, GUI_OUTPUT, "Output");
	Add(EDIT, 55, 270, 170, 20, GUI_OUTPUT);

	Add(BUTTON, 10, 305, 100, 30, GUI_RENDER, "Render");
	Add(BUTTON, 120, 305, 100, 30, GUI_CANCEL, "Cancel");
	Enable(render ? GUI_RENDER : GUI_CANCEL, FALSE);

	Add(BUTTON, 350, 5, 40, 20, GUI_HELP, "Help");

	// Rendering
	Add(STATIC, 10, 380, 50, 20, GUI_STATIC, "State:");
	Add(STATIC, 60, 380, 130, 20, GUI_STATE, "Unknown");
	Add(STATIC, 10, 400, 100, 20, GUI_STATIC, "Time elapsed:");
	Add(STATIC, 110, 400, 70, 20, GUI_TIME, "0:00");
	Add(STATIC, 10, 420, 100, 20, GUI_STATIC, "Time left:");
	Add(STATIC, 110, 420, 70, 20, GUI_LEFT, "0:00");
	Add(STATIC, 200, 380, 90, 20, GUI_STATIC, "Total chunks:");
	Add(STATIC, 300, 380, 70, 20, GUI_TOTAL, "0");
	Add(STATIC, 200, 400, 90, 20, GUI_STATIC, "Queue:");
	Add(STATIC, 300, 400, 70, 20, GUI_WORK, "0");
	Add(STATIC, 200, 420, 90, 20, GUI_STATIC, "Finished:");
	Add(STATIC, 300, 420, 70, 20, GUI_RESULT, "0");

	Add(STATIC, 213, 353, 35, 20, GUI_PERCENT, "0%");
	Add(PROGRESS, 10, 350, 202, 21, GUI_BAR);

	Add(FIELDSET, 260, 275, 130, 100, GUI_MESSAGE, "");
}

// Each time something changes
void GUI_Creator::onChange(int id)
{
	BOOL t = FALSE;
	INT i = 0;
	switch (id)
	{
		case GUI_ONLY:
			{
				std::wstring str;
				if (Get(GUI_ONLY, str))
					Set(GUI_EXCLUDE, "");
			}
			break;
		case GUI_EXCLUDE:
			{
				std::wstring str;
				if (Get(GUI_EXCLUDE, str))
					Set(GUI_ONLY, "");
			}
			break;
		case GUI_SLICE:
			{
				std::wstring str;
				if (Get(GUI_SLICE, str) && str.size() == 0)
					break;
				i = _wtoi(str.c_str());
				if (i <= 0)
					Set(GUI_SLICE, "0");
				else if (i > 127)
					Set(GUI_SLICE, "127");
					//SendMessageA(controls[GUI_SLICE], WM_UNDO, 0, 0);
			}
			break;
		case GUI_THREADS:
			{
				std::wstring str;
				if (Get(GUI_THREADS, str) && str.size() == 0)
					break;
				i = _wtoi(str.c_str());
				if (i <= 0)
					Set(GUI_THREADS, "1");
				else if (i > 99)
					Set(GUI_THREADS, "99");
			}
		case GUI_NORMAL:
		case GUI_COLOR:
		case GUI_GRAY:
		case GUI_SOLID:
		case GUI_FREQ:
			Get(GUI_FREQ, t);
			Enable(GUI_FREQ_H, t);
			break;
		case GUI_CACHE:
			Get(GUI_CACHE, t);
			Enable(GUI_CACHEFILE, t);
			break;
		case GUI_LOG:
			Get(GUI_LOG, t);
			Enable(GUI_LOGFILE, t);
			break;
		case GUI_WORLD:
		case GUI_CUSTOM:
			Get(GUI_CUSTOM, t);
			Enable(GUI_CUSTOMFILE, t);
			Get(GUI_WORLD, t);
			Enable(GUI_WORLDFILE, t);
			break;
		case GUI_RENDER:
			SetRender(true);
			break;
		case GUI_CANCEL:
			SetRender(false);
			break;
		case GUI_HELP:
			ShellExecuteA(NULL, "open", "http://www.minecraftforum.net/viewtopic.php?f=1022&t=74419", NULL, NULL, SW_SHOWNORMAL);
			break;
	}
}

void GUI_Creator::Enable(int id, BOOL b)
{
	if (id < 0 || id >= GUI_AMOUNTS)
		return;
	EnableWindow(controls[id], b);
}

void GUI_Creator::Show(int id, BOOL b)
{
	if (id < 0 || id >= GUI_AMOUNTS)
		return;
	ShowWindow(controls[id], b ? SW_SHOW : SW_HIDE);
}

HWND GUI_Creator::Add(int type, int x, int y, int w, int h, int id, const char * label)
{
	static int lastType = 0;
	int special = 0;
	switch (type)
	{
		case RADIO:
			special = BS_AUTORADIOBUTTON;
			if (lastType != type)
				special |= WS_GROUP;
			break;
		case CHECKBOX:
			special = BS_AUTOCHECKBOX;
			break;
	}
	types[id] = lastType = type;
	switch (type)
	{
		case STATIC:
			controls[id] = CreateWindowA("static", label, WS_CHILD | WS_VISIBLE, x, y, w, h, hwnd, (HMENU)id, hInstance, NULL);
			SendMessageA(controls[id], WM_SETFONT, (WPARAM)font, (LPARAM)TRUE);
			return controls[id];
		case EDIT:
			controls[id] = CreateWindowA("edit", label, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, x, y, w, h, hwnd, (HMENU)id, hInstance, NULL);
			SendMessageA(controls[id], WM_SETFONT, (WPARAM)font, (LPARAM)TRUE);
			return controls[id];
		case BUTTON:
		case CHECKBOX:
		case RADIO:
			controls[id] = CreateWindowA("button", label, WS_CHILD | WS_VISIBLE | special, x, y, w, h, hwnd, (HMENU)id, hInstance, NULL);
			SendMessageA(controls[id], WM_SETFONT, (WPARAM)font, (LPARAM)TRUE);
			return controls[id];
		case FIELDSET:
			controls[id] = CreateWindowA("static", label, WS_CHILD | WS_VISIBLE | WS_BORDER, x, y, w, h, hwnd, (HMENU)id, hInstance, NULL);
			SendMessageA(controls[id], WM_SETFONT, (WPARAM)font, (LPARAM)TRUE);
			return controls[id];
		case DROPDOWN:
			controls[id] = CreateWindowA("combobox", label, WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_VSCROLL | WS_TABSTOP, x, y, w, h, hwnd, (HMENU)id, hInstance, NULL);
			SendMessageA(controls[id], WM_SETFONT, (WPARAM)font, (LPARAM)TRUE);
			SendMessageA(controls[id], CB_ADDSTRING, (WPARAM)0, (LPARAM)label);
			SendMessageA(controls[id], CB_SETCURSEL, (WPARAM)0, (LPARAM)0);
			return controls[id];
			// Net stuff below
#ifdef _NETSTUFF
		case PROGRESS:
			controls[id] = CreateWindowA("msctls_progress32", label, WS_CHILD | WS_VISIBLE | WS_BORDER, x, y, w, h, hwnd, (HMENU)id, hInstance, NULL);
			//SendMessageA(controls[id], PBM_SETRANGE, (WPARAM)0, (LPARAM)100);
			return controls[id];
		case TRACKBAR:
			controls[id] = CreateWindowA("msctls_trackbar32", label, WS_CHILD | WS_VISIBLE | /*TBS_BOTH | TBS_NOTICKS |*/ WS_TABSTOP, x, y, w, h, hwnd, (HMENU)id, hInstance, NULL);
			//SendMessageA(controls[id], TBM_SETRANGE, (WPARAM)0, (LPARAM)100);
			return controls[id];
#else
		case PROGRESS:
			controls[id] = CreateWindowA("static", label, WS_CHILD | WS_VISIBLE | WS_BORDER, x, y, w, h, hwnd, (HMENU)id, hInstance, NULL);
			//SendMessageA(controls[id], WM_SETFONT, (WPARAM)font, (LPARAM)TRUE);
			return controls[id];
#endif
		case -1:
			break;
			//controls[id] = CreateWindow("msctls_progress32");
			// msctls_progress32 // Progressbar
			// msctls_trackbar32 ,TBS_BOTH | TBS_NOTICKS | WS_TABSTOP // Trackbar slider
			// ComboBoxEx32, CBS_DROPDOWN | CBS_SORT | WS_VSCROLL | WS_TABSTOP // Dropdown list
	}
	return 0;
}

// Get checkbox/radio
BOOL GUI_Creator::Get(int id, int & i)
{
	switch (types[id])
	{
		case CHECKBOX:
		case RADIO:
			return i = SendMessageA(controls[id], BM_GETCHECK, (WPARAM)0, (LPARAM)0);
		case DROPDOWN:
			return i = SendMessageW(controls[id], CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
			break;
	}
	return i = 0;
}

// Set checkbox/radio
void GUI_Creator::Set(int id, const int i)
{
	switch (types[id])
	{
		case CHECKBOX:
		case RADIO:
			SendMessageA(controls[id], BM_SETCHECK, (WPARAM)i, (LPARAM)0);
			break;
		case DROPDOWN:
			SendMessageW(controls[id], CB_SETCURSEL, (WPARAM)i, (LPARAM)0);
			break;
	}
}

// Other way of getting a text
//size = GetWindowTextA(controls[id], temp, size);

// Get string
DWORD GUI_Creator::Get(int id, std::string & str)
{
	DWORD size = SendMessageA(controls[id], WM_GETTEXTLENGTH, (WPARAM)0, (LPARAM)0)+1;
	if (size <= 1)
		return 0;
	char * temp = new char[size];
	switch (types[id])
	{
		case STATIC:
		case EDIT:
		case BUTTON:
		case FIELDSET:
		case DROPDOWN:
#ifndef _NETSTUFF
		case PROGRESS:
#endif
			size = SendMessageA(controls[id], WM_GETTEXT, (WPARAM)size, (LPARAM)temp);
			break;
	}
	str.assign(temp, temp+size);
	delete[] temp;
	return size;
}

// Get wide string
DWORD GUI_Creator::Get(int id, std::wstring & str)
{
	DWORD size = SendMessageW(controls[id], WM_GETTEXTLENGTH, (WPARAM)0, (LPARAM)0)+1;
	if (size <= 1)
		return 0;
	wchar_t * temp = new wchar_t[size];
	switch (types[id])
	{
		case STATIC:
		case EDIT:
		case BUTTON:
		case FIELDSET:
		case DROPDOWN:
#ifndef _NETSTUFF
		case PROGRESS:
#endif
			size = SendMessageW(controls[id], WM_GETTEXT, (WPARAM)size, (LPARAM)temp);
			break;
	}
	str.assign(temp, temp+(size * sizeof(wchar_t)));
	delete[] temp;
	return size;
}

// Set string
void GUI_Creator::Set(int id, const std::string str)
{
	switch (types[id])
	{
		case STATIC:
		case EDIT:
		case BUTTON:
		case FIELDSET:
#ifndef _NETSTUFF
		case PROGRESS:
#endif
			SendMessageA(controls[id], WM_SETTEXT, (WPARAM)0, (LPARAM)str.c_str());
			break;
		case DROPDOWN:
			SendMessageA(controls[id], CB_ADDSTRING, (WPARAM)0, (LPARAM)str.c_str());
			break;
	}
}

// Get string
void GUI_Creator::Set(int id, const std::wstring str)
{
	switch (types[id])
	{
		case STATIC:
		case EDIT:
		case BUTTON:
		case FIELDSET:
#ifndef _NETSTUFF
		case PROGRESS:
#endif
			SendMessageW(controls[id], WM_SETTEXT, (WPARAM)0, (LPARAM)str.c_str());
			break;
		case DROPDOWN:
			SendMessageW(controls[id], CB_ADDSTRING, (WPARAM)0, (LPARAM)str.c_str());
			break;
	}
}

// Specifically for the progressbar
void GUI_Creator::Set(int p)
{
	std::string str;
	for (int i = 0; i < p / 2; ++i)
		str += '|';
	Set(GUI_BAR, str);
	std::stringstream ss;
	ss << p << '%';
	Set(GUI_PERCENT, ss.str());
}

// Append to dropbox
void GUI_Creator::Append(int id, const std::string str)
{
	SendMessageA(controls[id], CB_ADDSTRING, (WPARAM)0, (LPARAM)str.c_str());
}
void GUI_Creator::Append(int id, const std::string str, int value)
{
	LRESULT lr = SendMessageA(controls[id], CB_ADDSTRING, (WPARAM)0, (LPARAM)str.c_str());
	SendMessageA(controls[id], CB_SETITEMDATA, lr, (WPARAM)value);
}
void GUI_Creator::Append(int id, const std::string str, void * value)
{
	LRESULT lr = SendMessageA(controls[id], CB_ADDSTRING, (WPARAM)0, (LPARAM)str.c_str());
	SendMessageA(controls[id], CB_SETITEMDATA, lr, (WPARAM)value);
}

// Get Dropdown item
BOOL GUI_Creator::GetData(int id, int * value)
{
	int i = SendMessageA(controls[id], CB_GETCURSEL, 0, 0);
	if (i < 0)
		return false;
	*value = (int)SendMessageA(controls[id], CB_GETITEMDATA, (WPARAM)i, (LPARAM)0);
	return true;
}
BOOL GUI_Creator::GetData(int id, void ** value)
{
	int i = SendMessageA(controls[id], CB_GETCURSEL, 0, 0);
	if (i < 0)
		return false;
	*value = (void *)SendMessageA(controls[id], CB_GETITEMDATA, (WPARAM)i, (LPARAM)0);
	return true;
}

// Clear out the status information
void GUI_Creator::ClearStatus()
{
	Set(0); // Progress bar
	Set(GUI_STATE, "Unknown");
	Set(GUI_STATE, "Unknown");
	Set(GUI_TIME, "0:00");
	Set(GUI_LEFT, "0:00");
	Set(GUI_TOTAL, "0");
	Set(GUI_WORK, "0");
	Set(GUI_RESULT, "0");
}

// Check what to do
bool GUI_Creator::CheckRender()
{
	bool t = isRender;
	isRender = false;
	return t;
}

// Get if rendering
bool GUI_Creator::DoRender()
{
	return render;
}

void GUI_Creator::SetRender(bool b)
{
	if (b)
	{
		render = true;
		isRender = !isRender;
		Enable(GUI_RENDER, FALSE);
		Enable(GUI_CANCEL, TRUE);
	}
	else
	{
		render = false;
		isRender = !isRender;
		Enable(GUI_RENDER, TRUE);
		Enable(GUI_CANCEL, FALSE);
	}
}

#endif // WIN32