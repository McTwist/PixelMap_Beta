/****************************************************

    PixelMap v0.85

	By McTwist

	Built for Linux

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

#include <string.h>
#include <cstdlib>
#include <iostream>
#include <sstream>
#include <sys/ioctl.h>

#include "PixelMap.hpp"

using namespace std;

// Progress class to show information on screen
class Progress
{
public:
	Progress() : percent(0), seconds(0) {}

	// Draw to screen
	void Draw()
	{
		int width = GetConsoleWidth();
		int percentWidth = width - (2 + 4 + 12); // [] + 100% + 0h0m0s
		cout << '\r'; // Reset line
		stringstream str;
		// Bar
		char bar[256] = {0};
		for (int i = 0; i < percentWidth; ++i)
		{
			int r = int(i * (100.0f / percentWidth));
			bar[i] = (r <= percent) ? '=' : ' ';
		}
		str << "[" << bar << "]";
		// Percent
		if (percent < 10)
			str << "  ";
		else if (percent < 100)
			str << " ";
		str << percent << '%';
		// Time
		int s = seconds % 60;
		int m = int(seconds / 60.0f);
		int h = int(m / 60.0f);
		m = m % 60;
		str << ' ' << h << 'h' << m << 'm' << s << 's';
		string st = str.str();
		while (st.size() < width)
			st.append(" ");
		cout << st.c_str();
		// Flush output
		fflush(stdout);
	}

	void SetPercent(int p)
	{
		percent = p;
	}
	void SetSeconds(int s)
	{
		seconds = s;
	}
private:
	// Get current console width
	int GetConsoleWidth()
	{
		winsize w;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
		return w.ws_col;
	}

	unsigned int percent;
	unsigned int seconds;
};

// Start the program
int main(int argc, char * argv[])
{
	if (argc <= 1)
	{
		cout << "PixelMap: This program requires parameters." << endl << "Check the guide for more information." << endl;
		return 0;
	}
	PixelMap * pxlmp = new PixelMap();

	// Convert to wide char
	wchar_t ** wCmd = new wchar_t*[argc];
	for (int i = 0; i < argc; ++i)
	{
		int size = strlen(argv[i])+1;
		wCmd[i] = new wchar_t[size];
		mbstowcs(wCmd[i], argv[i], size);
	}

	// Push arguments
	pxlmp->DoArgs(wCmd, argc);

	// Start PixelMap
	pxlmp->Initialize();
	pxlmp->Start();

	Progress bar = Progress();
	Level::State lastState = Level::INITIALIZE;

	while (pxlmp->Update())
	{
		if (pxlmp->NoGui())
		{
			usleep(100000);
			continue;
		}
		Level::State nState = (Level::State)pxlmp->GetState();

		// Show state
		if (nState != lastState)
		{
			switch (nState)
			{
				case Level::INITIALIZE:
					cout << "Initializing";
					break;
				case Level::DIRECTORY:
					cout << endl << "Listing folders" << endl;
					break;
				case Level::PROCESSING:
					cout << endl << "Process chunks" << endl;
					break;
				case Level::CREATEIMAGE:
					cout << endl << "Creating image" << endl;
					break;
				case Level::SAVING:
					cout << endl << "Saving image" << endl;
					break;
				case Level::FINALIZING:
					cout << endl << "Closing down";
					break;
			}
			lastState = nState;
		}

		// Avoid console spamming
		if (!pxlmp->Display())
			continue;

		// Only when processing chunks
		if (nState == Level::PROCESSING)
		{
			bar.SetSeconds(pxlmp->GetTimeLeft());
		}
		else
		{
			bar.SetSeconds(0);
		}

		// Avoid displaying anything when there is nothing done
		if (nState == Level::DIRECTORY)
		{
			cout << '\r' << "Chunks: " << pxlmp->GetChunks();
		}
		else if (nState >= Level::PROCESSING)
		{
			bar.SetPercent(pxlmp->GetPercent());
			bar.Draw();
		}
	}

	if (!pxlmp->NoGui())
	{
		cout << endl << pxlmp->GetErrorMessage() << endl;

		// Display executed time
		{
			int seconds = pxlmp->GetTime();
			// Time
			int s = seconds % 60;
			int m = int(seconds / 60.0f);
			int h = int(m / 60.0f);
			m = m % 60;
			cout << "Time: " << h << 'h' << m << 'm' << s << 's' << endl;;
		}
	}

	// Close PixelMap
	pxlmp->Stop();
	pxlmp->Shutdown();

	// Clean up
	for (int i = 0; i < 0; ++i)
		delete[] wCmd[i];
	delete[] wCmd;
	delete pxlmp;

	return 0;
}