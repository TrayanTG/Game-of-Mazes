/*
OneLoneCoder.com - Command Line Game Engine
"Who needs a frame buffer?" - @Javidx9
The Original & Best :P
One Lone Coder License
~~~~~~~~~~~~~~~~~~~~~~
- This software is Copyright (C) 2018 Javidx9
- This is free software
- This software comes with absolutely no warranty
- The copyright holder is not liable or responsible for anything
  this software does or does not
- You use this software at your own risk
- You can distribute this software
- You can modify this software
- Redistribution of this software or a derivative of this software
  must attribute the Copyright holder named above, in a manner
  visible to the end user
License
~~~~~~~
One Lone Coder Console Game Engine  Copyright (C) 2018  Javidx9
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under certain conditions; See license for details.
Original works located at:
	https://www.github.com/onelonecoder
	https://www.onelonecoder.com
	https://www.youtube.com/javidx9
GNU GPLv3
	https://github.com/OneLoneCoder/videos/blob/master/LICENSE
From Javidx9 :)
~~~~~~~~~~~~~~~
Hello! Ultimately I don't care what you use this for. It's intended to be
educational, and perhaps to the oddly minded - a little bit of fun.
Please hack this, change it and use it in any way you see fit. You acknowledge
that I am not responsible for anything bad that happens as a result of
your actions. However this code is protected by GNU GPLv3, see the license in the
github repo. This means you must attribute me if you use it. You can view this
license here: https://github.com/OneLoneCoder/videos/blob/master/LICENSE
Cheers!
Background
~~~~~~~~~~
If you've seen any of my videos - I like to do things using the windows console. It's quick
and easy, and allows you to focus on just the code that matters - ideal when you're
experimenting. Thing is, I have to keep doing the same initialisation and display code
each time, so this class wraps that up.
Author
~~~~~~
Twitter: @javidx9	http://twitter.com/javidx9
Blog:				http://www.onelonecoder.com
YouTube:			http://www.youtube.com/javidx9
Videos:
~~~~~~
Original:				https://youtu.be/cWc0hgYwZyc
Added mouse support:	https://youtu.be/tdqc9hZhHxM
Beginners Guide:		https://youtu.be/u5BhrA8ED0o
Shout Outs!
~~~~~~~~~~~
Thanks to cool people who helped with testing, bug-finding and fixing!
wowLinh, JavaJack59, idkwid, kingtatgi, Return Null, CPP Guy, MaGetzUb
Last Updated: 02/07/2018
Usage:
~~~~~~
This class is abstract, so you must inherit from it. Override the OnUserCreate() function
with all the stuff you need for your application (for thready reasons it's best to do
this in this function and not your class constructor). Override the OnUserUpdate(float fElapsedTime)
function with the good stuff, it gives you the elapsed time since the last call so you
can modify your stuff dynamically. Both functions should return true, unless you need
the application to close.
	int main()
	{
		// Use olcConsoleGameEngine derived app
		OneLoneCoder_Example game;
		// Create a console with resolution 160x100 characters
		// Each character occupies 8x8 pixels
		game.ConstructConsole(160, 100, 8, 8);
		// Start the engine!
		game.Start();
		return 0;
	}
Input is also handled for you - interrogate the m_keys[] array with the virtual
keycode you want to know about. bPressed is set for the frame the key is pressed down
in, bHeld is set if the key is held down, bReleased is set for the frame the key
is released in. The same applies to mouse! m_mousePosX and Y can be used to get
the current cursor position, and m_mouse[1..5] returns the mouse buttons.
The draw routines treat characters like pixels. By default they are set to white solid
blocks - but you can draw any unicode character, using any of the colours listed below.
There may be bugs!
See my other videos for examples!
http://www.youtube.com/javidx9
Lots of programs to try:
http://www.github.com/OneLoneCoder/videos
Chat on the Discord server:
https://discord.gg/WhwHUMV
Be bored by Twitch:
http://www.twitch.tv/javidx9
*/

#pragma once
#pragma comment(lib, "winmm.lib")

#ifndef UNICODE
#error Please enable UNICODE for your compiler! VS: Project Properties -> General -> \
Character Set -> Use Unicode. Thanks! - Javidx9
#endif

#include <windows.h>

#include <iostream>
#include <chrono>
#include <vector>
#include <list>
#include <thread>
#include <atomic>
#include <condition_variable>

enum COLOUR
{
	FG_BLACK = 0x0000,
	FG_DARK_BLUE = 0x0001,
	FG_DARK_GREEN = 0x0002,
	FG_DARK_CYAN = 0x0003,
	FG_DARK_RED = 0x0004,
	FG_DARK_MAGENTA = 0x0005,
	FG_DARK_YELLOW = 0x0006,
	FG_GREY = 0x0007, // Thanks MS :-/
	FG_DARK_GREY = 0x0008,
	FG_BLUE = 0x0009,
	FG_GREEN = 0x000A,
	FG_CYAN = 0x000B,
	FG_RED = 0x000C,
	FG_MAGENTA = 0x000D,
	FG_YELLOW = 0x000E,
	FG_WHITE = 0x000F,
	BG_BLACK = 0x0000,
	BG_DARK_BLUE = 0x0010,
	BG_DARK_GREEN = 0x0020,
	BG_DARK_CYAN = 0x0030,
	BG_DARK_RED = 0x0040,
	BG_DARK_MAGENTA = 0x0050,
	BG_DARK_YELLOW = 0x0060,
	BG_GREY = 0x0070,
	BG_DARK_GREY = 0x0080,
	BG_BLUE = 0x0090,
	BG_GREEN = 0x00A0,
	BG_CYAN = 0x00B0,
	BG_RED = 0x00C0,
	BG_MAGENTA = 0x00D0,
	BG_YELLOW = 0x00E0,
	BG_WHITE = 0x00F0,
};

class olcConsoleGameEngine
{
public:
	olcConsoleGameEngine()
	{
		m_nScreenWidth = 80;
		m_nScreenHeight = 30;

		m_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		m_hConsoleIn = GetStdHandle(STD_INPUT_HANDLE);

		// Remove blinking underscore on console / cmd prompt
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(m_hConsole, &cursorInfo);
		cursorInfo.bVisible = false;
		SetConsoleCursorInfo(m_hConsole, &cursorInfo);
		// done

		std::memset(m_keyNewState, 0, 256 * sizeof(short));
		std::memset(m_keyOldState, 0, 256 * sizeof(short));
		std::memset(m_keys, 0, 256 * sizeof(sKeyState));
		m_mousePosX = 0;
		m_mousePosY = 0;

		m_sAppName = L"Game of Mazes";
	}

	int ConstructConsole(int width, int height, int fontw, int fonth)
	{
		if (m_hConsole == INVALID_HANDLE_VALUE)
			return Error(L"Bad Handle");

		m_nScreenWidth = width;
		m_nScreenHeight = height;

		// Update 13/09/2017 - It seems that the console behaves differently on some systems
		// and I'm unsure why this is. It could be to do with windows default settings, or
		// screen resolutions, or system languages. Unfortunately, MSDN does not offer much
		// by way of useful information, and so the resulting sequence is the reult of experiment
		// that seems to work in multiple cases.
		//
		// The problem seems to be that the SetConsoleXXX functions are somewhat circular and 
		// fail depending on the state of the current console properties, i.e. you can't set
		// the buffer size until you set the screen size, but you can't change the screen size
		// until the buffer size is correct. This coupled with a precise ordering of calls
		// makes this procedure seem a little mystical :-P. Thanks to wowLinh for helping - Jx9

		// Change console visual size to a minimum so ScreenBuffer can shrink
		// below the actual visual size
		m_rectWindow = { 0, 0, 1, 1 };
		SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow);

		// Set the size of the screen buffer
		COORD coord = { (short)m_nScreenWidth, (short)m_nScreenHeight };
		if (!SetConsoleScreenBufferSize(m_hConsole, coord))
			Error(L"SetConsoleScreenBufferSize");

		// Assign screen buffer to the console
		if (!SetConsoleActiveScreenBuffer(m_hConsole))
			return Error(L"SetConsoleActiveScreenBuffer");

		// Set the font size now that the screen buffer has been assigned to the console
		CONSOLE_FONT_INFOEX cfi;
		cfi.cbSize = sizeof(cfi);
		cfi.nFont = 0;
		cfi.dwFontSize.X = fontw;
		cfi.dwFontSize.Y = fonth;
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_NORMAL;

		/*	DWORD version = GetVersion();
			DWORD major = (DWORD)(LOBYTE(LOWORD(version)));
			DWORD minor = (DWORD)(HIBYTE(LOWORD(version)));*/

			//if ((major > 6) || ((major == 6) && (minor >= 2) && (minor < 4)))		
			//	wcscpy_s(cfi.FaceName, L"Raster"); // Windows 8 :(
			//else
			//	wcscpy_s(cfi.FaceName, L"Lucida Console"); // Everything else :P

			//wcscpy_s(cfi.FaceName, L"Liberation Mono");
		wcscpy_s(cfi.FaceName, L"Consolas");
		if (!SetCurrentConsoleFontEx(m_hConsole, false, &cfi))
			return Error(L"SetCurrentConsoleFontEx");

		// Get screen buffer info and check the maximum allowed window size. Return
		// error if exceeded, so user knows their dimensions/fontsize are too large
		CONSOLE_SCREEN_BUFFER_INFO csbi;
		if (!GetConsoleScreenBufferInfo(m_hConsole, &csbi))
			return Error(L"GetConsoleScreenBufferInfo");
		if (m_nScreenHeight > csbi.dwMaximumWindowSize.Y)
			return Error(L"Screen Height / Font Height Too Big");
		if (m_nScreenWidth > csbi.dwMaximumWindowSize.X)
			return Error(L"Screen Width / Font Width Too Big");

		// Set Physical Console Window Size
		m_rectWindow = { 0, 0, (short)m_nScreenWidth - 1, (short)m_nScreenHeight - 1 };
		if (!SetConsoleWindowInfo(m_hConsole, TRUE, &m_rectWindow))
			return Error(L"SetConsoleWindowInfo");

		// Set flags to allow mouse input		
		if (!SetConsoleMode(m_hConsoleIn, ENABLE_EXTENDED_FLAGS | ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT))
			return Error(L"SetConsoleMode");

		// Allocate memory for screen buffer
		m_bufScreen = new CHAR_INFO[m_nScreenWidth*m_nScreenHeight];
		memset(m_bufScreen, 0, sizeof(CHAR_INFO) * m_nScreenWidth * m_nScreenHeight);

		SetConsoleCtrlHandler((PHANDLER_ROUTINE)CloseHandler, TRUE);
		return 1;
	}

	virtual void Draw(int x, int y, short c = 0x2588, short col = 0x000F)
	{
		if (x >= 0 && x < m_nScreenWidth && y >= 0 && y < m_nScreenHeight)
		{
			m_bufScreen[y * m_nScreenWidth + x].Char.UnicodeChar = c;
			m_bufScreen[y * m_nScreenWidth + x].Attributes = col;
		}
	}

	void Fill(int x1, int y1, int x2, int y2, short c = 0x2588, short col = 0x000F)
	{
		Clip(x1, y1);
		Clip(x2, y2);
		for (int x = x1; x < x2; x++)
			for (int y = y1; y < y2; y++)
				Draw(x, y, c, col);
	}

	void DrawString(int x, int y, std::wstring c, short col = 0x000F)
	{
		for (size_t i = 0; i < c.size(); i++)
		{
			m_bufScreen[y * m_nScreenWidth + x + i].Char.UnicodeChar = c[i];
			m_bufScreen[y * m_nScreenWidth + x + i].Attributes = col;
		}
	}

	void Clip(int &x, int &y)
	{
		if (x < 0) x = 0;
		if (x >= m_nScreenWidth) x = m_nScreenWidth;
		if (y < 0) y = 0;
		if (y >= m_nScreenHeight) y = m_nScreenHeight;
	}

	~olcConsoleGameEngine()
	{
		SetConsoleActiveScreenBuffer(m_hOriginalConsole);
		delete[] m_bufScreen;
	}

public:
	void Start()
	{
		// Start the thread
		m_bAtomActive = true;
		std::thread t = std::thread(&olcConsoleGameEngine::GameThread, this);

		// Wait for thread to be exited
		t.join();
	}

	int ScreenWidth() { return m_nScreenWidth; }
	int ScreenHeight() { return m_nScreenHeight; }

private:
	void GameThread()
	{
		// Create user resources as part of this thread
		if (!OnUserCreate())
			m_bAtomActive = false;

		auto tp1 = std::chrono::system_clock::now();
		auto tp2 = std::chrono::system_clock::now();

		while (m_bAtomActive)
		{
			// Run as fast as possible
			while (m_bAtomActive)
			{
				// Handle Timing
				tp2 = std::chrono::system_clock::now();
				std::chrono::duration<float> elapsedTime = tp2 - tp1;
				tp1 = tp2;
				float fElapsedTime = elapsedTime.count();

				// Handle Keyboard Input
				for (int i = 0; i < 256; i++)
				{
					m_keyNewState[i] = GetAsyncKeyState(i);

					m_keys[i].bPressed = false;
					m_keys[i].bReleased = false;

					if (m_keyNewState[i] != m_keyOldState[i])
					{
						if (m_keyNewState[i] & 0x8000)
						{
							m_keys[i].bPressed = !m_keys[i].bHeld;
							m_keys[i].bHeld = true;
						}
						else
						{
							m_keys[i].bReleased = true;
							m_keys[i].bHeld = false;
						}
					}

					m_keyOldState[i] = m_keyNewState[i];
				}

				// Handle Mouse Input - Check for window events
				INPUT_RECORD inBuf[32];
				DWORD events = 0;
				GetNumberOfConsoleInputEvents(m_hConsoleIn, &events);
				if (events > 0)
					ReadConsoleInput(m_hConsoleIn, inBuf, events, &events);

				// Handle events - we only care about mouse clicks and movement
				// for now
				for (DWORD i = 0; i < events; i++)
				{
					switch (inBuf[i].EventType)
					{
					case FOCUS_EVENT:
					{
						m_bConsoleInFocus = inBuf[i].Event.FocusEvent.bSetFocus;
					}
					break;

					case MOUSE_EVENT:
					{
						switch (inBuf[i].Event.MouseEvent.dwEventFlags)
						{
						case MOUSE_MOVED:
						{
							m_mousePosX = inBuf[i].Event.MouseEvent.dwMousePosition.X;
							m_mousePosY = inBuf[i].Event.MouseEvent.dwMousePosition.Y;
						}
						break;

						case 0:
						{
							for (int m = 0; m < 5; m++)
								m_mouseNewState[m] = (inBuf[i].Event.MouseEvent.dwButtonState & (1 << m)) > 0;

						}
						break;

						default:
							break;
						}
					}
					break;

					default:
						break;
						// We don't care just at the moment
					}
				}

				for (int m = 0; m < 5; m++)
				{
					m_mouse[m].bPressed = false;
					m_mouse[m].bReleased = false;

					if (m_mouseNewState[m] != m_mouseOldState[m])
					{
						if (m_mouseNewState[m])
						{
							m_mouse[m].bPressed = true;
							m_mouse[m].bHeld = true;
						}
						else
						{
							m_mouse[m].bReleased = true;
							m_mouse[m].bHeld = false;
						}
					}

					m_mouseOldState[m] = m_mouseNewState[m];
				}


				// Handle Frame Update
				if (!OnUserUpdate(fElapsedTime))
					m_bAtomActive = false;

				// Update Title & Present Screen Buffer
				wchar_t s[256];
				swprintf_s(s, 256, L"%s - FPS: %3.0f", m_sAppName.c_str(), 1.0f / fElapsedTime);
				SetConsoleTitle(s);
				WriteConsoleOutput(m_hConsole, m_bufScreen, { (short)m_nScreenWidth, (short)m_nScreenHeight }, { 0,0 }, &m_rectWindow);
			}

			// Allow the user to free resources if they have overrided the destroy function
			if (OnUserDestroy())
			{
				// User has permitted destroy, so exit and clean up
				delete[] m_bufScreen;
				SetConsoleActiveScreenBuffer(m_hOriginalConsole);
				m_cvGameFinished.notify_one();
			}
			else
			{
				// User denied destroy for some reason, so continue running
				m_bAtomActive = true;
			}
		}
	}

public:
	// User MUST OVERRIDE THESE!!
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(float fElapsedTime) = 0;

	// Optional for clean up 
	virtual bool OnUserDestroy() { return true; }

protected:
	struct sKeyState
	{
		bool bPressed;
		bool bReleased;
		bool bHeld;
	} m_keys[256], m_mouse[5];

	int m_mousePosX;
	int m_mousePosY;

public:
	sKeyState GetKey(int nKeyID) { return m_keys[nKeyID]; }
	int GetMouseX() { return m_mousePosX; }
	int GetMouseY() { return m_mousePosY; }
	sKeyState GetMouse(int nMouseButtonID) { return m_mouse[nMouseButtonID]; }
	bool IsFocused() { return m_bConsoleInFocus; }

protected:
	int Error(const wchar_t *msg)
	{
		wchar_t buf[256];
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);
		SetConsoleActiveScreenBuffer(m_hOriginalConsole);
		wprintf(L"ERROR: %s\n\t%s\n", msg, buf);
		return 0;
	}

	static BOOL CloseHandler(DWORD evt)
	{
		// Note this gets called in a seperate OS thread, so it must
		// only exit when the game has finished cleaning up, or else
		// the process will be killed before OnUserDestroy() has finished
		if (evt == CTRL_CLOSE_EVENT)
		{
			m_bAtomActive = false;

			// Wait for thread to be exited
			std::unique_lock<std::mutex> ul(m_muxGame);
			m_cvGameFinished.wait(ul);
		}
		return true;
	}

protected:
	int m_nScreenWidth;
	int m_nScreenHeight;
	CHAR_INFO *m_bufScreen;
	std::wstring m_sAppName;
	HANDLE m_hOriginalConsole;
	CONSOLE_SCREEN_BUFFER_INFO m_OriginalConsoleInfo;
	HANDLE m_hConsole;
	HANDLE m_hConsoleIn;
	SMALL_RECT m_rectWindow;
	short m_keyOldState[256] = { 0 };
	short m_keyNewState[256] = { 0 };
	bool m_mouseOldState[5] = { 0 };
	bool m_mouseNewState[5] = { 0 };
	bool m_bConsoleInFocus = true;

	// These need to be static because of the OnDestroy call the OS may make. The OS
	// spawns a special thread just for that
	static std::atomic<bool> m_bAtomActive;
	static std::condition_variable m_cvGameFinished;
	static std::mutex m_muxGame;
};

// Define our static variables
std::atomic<bool> olcConsoleGameEngine::m_bAtomActive(false);
std::condition_variable olcConsoleGameEngine::m_cvGameFinished;
std::mutex olcConsoleGameEngine::m_muxGame;