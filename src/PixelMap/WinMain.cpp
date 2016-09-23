/****************************************************

    PixelMap v0.85

	By McTwist

	Built for Windows

	This source is free to edit and distribute.
	Please give McTwist credits for it.
	McTwist takes no responsibility for computer
	failure, damage to the computer nor the
	content(s) within it.
	This program is distributed as-is with no
	warranty what-so-ever.

	How to use at:
	http://www.minecraftforum.net/viewtopic.php?f=1022&t=74419

****************************************************/

#define PIXELMAP_VERSION "0.8"

#ifdef WIN32

#include <windows.h>

#include <string>
#include <sstream>

#include "PixelMap.hpp"

#include "GUI.hpp"

#ifdef _DEBUG
#define MEMTRACKER
#endif

// Memory tracker
#ifdef MEMTRACKER
#include "MemTracker.h"
MemTracker * memoryTracker = 0;
#endif

// Defined functions
LRESULT CALLBACK WinProc(HWND, UINT, WPARAM, LPARAM);

// GUI Holder
GUI_Creator gui;

// Main
int WINAPI WinMain(
	HINSTANCE hInstance, 
	HINSTANCE hPrevInstance, 
	LPSTR lpCmdLine, 
	int nShowCmd)
{
#ifdef MEMTRACKER
	memoryTracker = new MemTracker();
	{
#endif

	PixelMap * pxlmp = new PixelMap();

	// Get arguments
	int nArgs = 0;
	LPWSTR * commands = CommandLineToArgvW(GetCommandLineW(), & nArgs);
	if (nArgs > 1)
		pxlmp->DoArgs(commands, nArgs);

	HWND hwnd = 0;

	if (!pxlmp->NoGui())
	{
		gui.Initialize(hInstance);
		if (nArgs > 1)
			gui.SetRender(true);

		char szClassName[] = {"PixelMap"};

		WNDCLASSEXA wClass = {0};

		wClass.cbSize = sizeof(WNDCLASSEX);
		wClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
		wClass.hCursor = (HCURSOR) LoadCursor(0, IDC_ARROW);
		wClass.hIcon = (HICON) 0;
		wClass.hIconSm = 0;
		wClass.hInstance = hInstance;
		wClass.lpfnWndProc = WinProc;
		wClass.lpszClassName = szClassName;
		wClass.lpszMenuName = 0;
		wClass.style = 0;

		RegisterClassExA(& wClass);

		hwnd = CreateWindowA(szClassName,
			"PixelMap",
			WS_OVERLAPPED | WS_MINIMIZEBOX | WS_SYSMENU,
			50, 50,
			400, 470,
			NULL,
			NULL,
			hInstance,
			0);

		ShowWindow(hwnd, SW_SHOW);
		UpdateWindow(hwnd);

		pxlmp->GetWorlds(&gui);
	}

	bool isRunning = true;
	MSG msg;
	memset(&msg, 0, sizeof(MSG));

	using namespace std;

	pxlmp->Initialize();
	// Update GUI with arguments
	if (!pxlmp->NoGui() && nArgs > 1)
		pxlmp->GetArgs(&gui);
	// Autostart
	if (pxlmp->NoGui() || gui.CheckRender())
		pxlmp->Start();

	while (isRunning)
	{
		while (PeekMessage(& msg, 0, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				isRunning = FALSE;

			TranslateMessage(& msg);
			DispatchMessage(& msg);
		}

		if (!isRunning)
		{
			pxlmp->Stop();
			break;
		}

		// GUI
		if (!pxlmp->NoGui())
		{
			bool check = gui.CheckRender();
			bool render = gui.DoRender();
			// Button pressed and not rendering
			if (check && render)
			{
				gui.ClearStatus();
				pxlmp->DoArgs(&gui);
				pxlmp->Start();
			}
			// Button pressed and rendering
			else if (check && !render)
			{
				gui.Set(100);
				pxlmp->Stop();
				continue;
			}
			// Not rendering
			else if (!render)
			{
				Sleep(1);
				continue;
			}
		}

		// Check mapper status
		if (!pxlmp->Update())
		{
			string str = pxlmp->GetErrorMessage();
			if (pxlmp->NoGui())
			{
				if (!str.empty())
					MessageBoxA(0, str.c_str(), "PixelMap", MB_OK);
			}
			else
			{
				if (pxlmp->OpenFolder())
				{
					string output = pxlmp->GetSave();
					int last = output.find_last_of('\\');
					if (last != -1)
						output.assign(output.begin(), output.begin()+last);
					ShellExecuteA(NULL, "open", output.c_str(), NULL, NULL, SW_SHOWNORMAL);
				}
				gui.Set(GUI_MESSAGE, str);
				gui.Set(GUI_STATE, "Done");
				gui.SetRender(false);
			}
			pxlmp->Stop();
			if (pxlmp->AutoClose())
				isRunning = false;
			continue;
		}

		// Take it easy when there is no gui
		if (pxlmp->NoGui())
		{
			Sleep(10);
			continue;
		}

		if (!pxlmp->Display())
			continue;

		Level::State nState = (Level::State)pxlmp->GetState();

		// Avoid displaying anything when there is nothing done
		if (nState > Level::DIRECTORY)
		{
			stringstream sWork;
			sWork << pxlmp->GetQueue();
			stringstream sResult;
			sResult << pxlmp->GetFinished();

			gui.Set(GUI_WORK, setSeparator(sWork.str()));
			gui.Set(GUI_RESULT, setSeparator(sResult.str()));
			gui.Set(pxlmp->GetPercent());
		}

		// Total chunks
		stringstream sTotal;
		sTotal << pxlmp->GetChunks();
		gui.Set(GUI_TOTAL, setSeparator(sTotal.str()));

		// State
		string sState;
		switch (nState)
		{
			case Level::INITIALIZE:
				sState = "Initializing";
				break;
			case Level::DIRECTORY:
				sState = "Listing folders";
				break;
			case Level::PROCESSING:
				sState = "Process chunks";
				break;
			case Level::CREATEIMAGE:
				sState = "Creating image";
				break;
			case Level::SAVING:
				sState = "Saving image";
				break;
			case Level::FINALIZING:
				sState = "Closing down";
				break;
		}
		gui.Set(GUI_STATE, sState);

		// Elapsed
		stringstream sTime;
		{
			int nTime = pxlmp->GetTime();
			int seconds = nTime % 60;
			int minutes = int(nTime / 60.0f);
			sTime << minutes << ":";
			if (seconds <= 9)
				sTime << '0' << seconds;
			else
				sTime << seconds;
		}
		gui.Set(GUI_TIME, sTime.str());

		// Left
		if (nState == Level::PROCESSING)
		{
			int nLeft = pxlmp->GetTimeLeft();
			if (nLeft >= 0)
			{
				int seconds = nLeft % 60;
				int minutes = int(nLeft / 60.0f);
				stringstream sLeft;
				sLeft << minutes << ":";
				if (seconds <= 9)
					sLeft << '0' << seconds;
				else
					sLeft << seconds;
				gui.Set(GUI_LEFT, sLeft.str());
			}
		}
	}
	gui.Set(100); // Set to done

	pxlmp->Shutdown();

	if (!pxlmp->NoGui())
		UnregisterClassA("PixelMap", hInstance);

	delete pxlmp;

#ifdef MEMTRACKER
	}
	delete memoryTracker;
#endif

	return EXIT_SUCCESS;
}

// WinProc
LRESULT CALLBACK WinProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch(msg)
	{
		case WM_CREATE:
			gui.Create(hwnd);
			break;
		case WM_COMMAND:
			gui.onChange(LOWORD(wp));
			break;
		case WM_DESTROY:
			PostQuitMessage(0);
			break;

		default:
			return DefWindowProc(hwnd, msg, wp, lp);
	}

	return 0;
}

#endif // WIN32