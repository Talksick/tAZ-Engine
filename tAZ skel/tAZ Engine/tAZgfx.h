//		Memory editing/trainer class
////		for C++ [POiZON v1.0Δ]
//////			by Tox1k
////		ryx.933industries.com
//		"good enough never is"

/*
This file is part of tAZ-Engine.

    tAZ-Engine is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    tAZ-Engine is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with tAZ-Engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "..\stdafx.h"
#include <Windows.h>
#include <MMSystem.h>
#include <CommDlg.h>

#pragma once

#define byte unsigned char

// Set these for trainer size
#define screen_height	300
#define screen_width	400
#define dib_height		300
#define dib_width		400

// Our structs:
struct star
{
        int x;			// derp
        int y;			// derpity
		int layer;		// unused as of right now

		int speed;		// how fast it moves
		int colorR,
			colorG,
			colorB;
		int fadelength;	// how long it's trail is (long for prettiness <3)
};

class tAZgfx 
{
	private:
		RECT	DrawButton(int posX, int posY, LPCWSTR Text, int r, int g, int b, int Length, bool bOver);		// private functions draws the basic button
		void	AddPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char *dib);	// SetPixel() for dibs
		void	AddTransPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a, unsigned char *dib); // Transparency support

		byte *buffer;	// where our DIB will be saved to
		int cScroller;	// shift
		int xScroll;	// rise and fall
		bool sineDecreasing;
		int every3rd;	// control ticker rate
		int acounter;	// buffer to hold the xscroll

		// And now our DiB effects variables:
		star	starfield[30];		// our fast stars in the foreground
		star	background[300];	// slower ones :( but they're cool too
		int		moveback;			// will the background move?
		bool	bmoveback;			// yes or no?

		// And fonts:
		HFONT	DescriptionFont;	// Hotkeys font
		HFONT	ButtonFont;			// Those DIB buttons

	public:
		tAZgfx();
		~tAZgfx();

		HWND	hwnd;	// main trainer hwnd
		HDC		dc;		// main trainer dc
						// something to handle our DiB connections? device context? the world may never know :O
		
		bool uFmode;	// if ufmod is playing

		HWND	InitializeDemoScene(WNDPROC WndProc);	// Creates a window for our scene with a dib
		void	InitStarfield();						// kills kittens... i mean creates the starfield
		void	WM_PAINTStarfield(PAINTSTRUCT ps);		// we'll just place this here in wm_paint
		void	UpdateStarfield();						// place in a timer call or create a thread to handle it
		void	PaintedButton(int posX, int posY, LPCWSTR Text, int r, int g, int b, int TextLength);			// draws a button that detects if mouse is over it and changes colors
		bool	IsOver(int posX, int posY);			// Checks if mouse is over coords
		void	ShadowedText(LPCWSTR lpString, int x,int y, int TextColor, int ShadowOffset);	// drawtext fer scenies
		bool	OpenFile();
		void	UpdateScroller(wchar_t *Text, int length);

		bool	LoadFile();
};

// UFMOD stuff
#pragma comment(lib, "ufmod.lib")
#pragma comment(lib, "winmm.lib")

#ifdef __cplusplus
	extern "C" {
#endif

	HWAVEOUT *__stdcall uFMOD_PlaySong(void*, void*, int);
	#define uFMOD_StopSong() uFMOD_PlaySong(0, 0, 0)

	void __stdcall uFMOD_Pause();

	void __stdcall uFMOD_Resume();

#ifdef __cplusplus
	}
#endif

#define XM_RESOURCE       0
#define XM_MEMORY         1
#define XM_FILE           2
#define XM_NOLOOP         8
#define XM_SUSPENDED      16
#define uFMOD_MIN_VOL     0
#define uFMOD_MAX_VOL     25
#define uFMOD_DEFAULT_VOL uFMOD_MAX_VOL