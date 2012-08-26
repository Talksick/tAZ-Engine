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

#include "stdafx.h"
#include "tAZgfx.h"

tAZgfx::tAZgfx()
{
	cScroller = dib_width;
	xScroll = 0;
	sineDecreasing = false;
	every3rd = 0;
	acounter = 0;
	ButtonFont = CreateFont(8,1,0,0,500,0,0,0,DEFAULT_CHARSET,0,0,0,0,TEXT("Terminal"));
	DescriptionFont = CreateFont(2, 1, 0, 0, 500, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, TEXT("Terminal"));
}

tAZgfx::~tAZgfx()
{
	DeleteObject(DescriptionFont);
	DeleteObject(ButtonFont);
	ReleaseDC(hwnd, dc);
}

/*
    Function:	InitializeDemoScene()
       Input:	WndProc of win32 app
 Description:	Creates a scene-style window and resets starfield
*/
HWND tAZgfx::InitializeDemoScene(WNDPROC WndProc)
{
	// first let's reset everything:

	// make sure starfield is empty
	ZeroMemory(&background, sizeof(star) * 300);
	ZeroMemory(&starfield, sizeof(star) * 30);

	// make sure the background don't move
	moveback = 0;
	bmoveback = false;

	HINSTANCE hInstance = GetModuleHandle(NULL);

	// now make our window
	WNDCLASS wc = {0};
	wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"POiZON v1.0";	// poison... tox1k... haha?
    wc.hbrBackground = CreateSolidBrush(0x000000);

	RegisterClass(&wc);

	// and now for the window
	hwnd = CreateWindow(wc.lpszClassName, L"POiZON v1.0", WS_POPUP,  (GetSystemMetrics(SM_CXSCREEN)-screen_width)/2, (GetSystemMetrics(SM_CYSCREEN)-screen_height)/2, screen_width, screen_height, NULL, NULL, hInstance, NULL);
	
	if(hwnd == INVALID_HANDLE_VALUE)
		return false;

	UnregisterClass(L"POiZON v1.0", hInstance);
	// Now that we have a blocky window, make the DiB engine part: (dammit deviated, your i's are rubbing off on me :P)
	BITMAPINFO DIBplane = {0};
	DIBplane.bmiHeader.biSize = 40;
    DIBplane.bmiHeader.biWidth = dib_width;
    DIBplane.bmiHeader.biHeight = dib_height;
    DIBplane.bmiHeader.biPlanes = 1;
    DIBplane.bmiHeader.biBitCount = 32;
	
	dc = CreateCompatibleDC(GetDC(hwnd));
    SelectObject(dc, CreateDIBSection(dc, &DIBplane, 0, (void**)&buffer, 0, 0)); 
    SetBkMode(dc, TRANSPARENT);
    SetTextColor(dc, 0xffffff);
	ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
	SetWindowText(hwnd, L"H2V Flair");
	return hwnd;
}

/*
    Function:	InitStarfield()
       Input:	ixnay faggots
 Description:	Randomizes starfield speed, color, and position
*/
void tAZgfx::InitStarfield()
{
	// set up the foreground stars, because they're the coolest (duh)
	for(int i = 0; i < 30; i++)
	{
		starfield[i].x = rand() % (dib_width - 1) + 1;	// redundancy to check if n00bs are leeching
		starfield[i].y = rand() % (dib_height - 1) + 1; // again, more of the same
		
				starfield[i].speed		= (rand() % 3) + 1;										// speed to something interesting (adjust at will)
				starfield[i].colorR		= (rand() % 0x1F) + 0xD0 - (-1 * starfield[i].speed);	// very little color variation, idk why I even bother
				starfield[i].colorB		= (rand() % 0x1F) + 0xD0 - (-1 * starfield[i].speed);	//					|
				starfield[i].colorG		= (rand() % 0x1F) + 0xD0 - (-1 * starfield[i].speed);	//					V
				starfield[i].fadelength	= (rand() % 3) + 13;
	}
	// and now the background (my there's a lot!)
	for(int s = 0; s < 300; s++)
	{
		background[s].x = rand() % (dib_width - 1) + 1;		// more of the same
		background[s].y = rand() % (dib_height - 1) + 1;	// randomizing both so we have constellations (the human mind makes shapes out of orderless objects, tell me if you see scorpius, that's my sign)
	}       
}

/*
    Function:	WM_PAINTStarfield()
       Input:	wndproc paintstruct
 Description:	Repaints the window with the dib
*/
void tAZgfx::WM_PAINTStarfield(PAINTSTRUCT ps)
{
	BitBlt(BeginPaint(hwnd, &ps), 0 /* x offset */, 10 /* y offset*/, dib_width, dib_height, dc, 0, 0, SRCCOPY);
}

/*
    Function:	UpdateStarfield()
       Input:	noooone
 Description:	Orbits background starfield every once in a while and moves normal stars
*/
void tAZgfx::UpdateStarfield()
{
	moveback++;										// tick the timer
	memset(buffer, 0, dib_width * dib_height * 4);	// set background to black (00000000) x 100000000000000 or something like that
	bmoveback = false;								// background doesn't move each time, if it is time it will be set later

	for(int i = 0; i < 300; i++)					// move the background
	{
		if(moveback > 5)							// every 5 ticks it moves
		{
			moveback = 0;		// reset it
			bmoveback = true;	// we're moving!
		}
		if(bmoveback)
		{
			background[i].x += 1;				// move one pixel D:
			if(background[i].x > dib_width)		// stay within the screen, dammit!
			{
				background[i].y = rand() % (dib_height - 1) + 1;	// reset our shitz
				background[i].x = 1;								// dis one loopz
			}
		}
		AddPixel(background[i].x, background[i].y, 100, 100, 100, buffer);
	}
	// now set our foreground (who's on first)
	for(int i = 0; i < 30; i++)
	{
		starfield[i].x -= starfield[i].speed;
			if(starfield[i].x-- < 0)	// check if the next move is into the -bad- negative catergory
			{
					starfield[i].y = rand() % (dib_height - 1) + 1;	// reshet it
					starfield[i].x = dib_width - 15;				// tail length ends up being ~ 15
			}
			AddPixel(starfield[i].x, starfield[i].y, starfield[i].colorR, starfield[i].colorG, starfield[i].colorB, buffer);
			// and now an overly complex way of generating a fading contrail!
			for(int r = 0; r < starfield[i].fadelength; r++)
			{
				// decrease each color by 0F for each pixel away from the star they are
				int tempR, tempG, tempB;
				tempR = starfield[i].colorR - (r * 0x0F);
				if(starfield[i].colorR - (r * 0x0F) <= 0) // error checking? in my app? D:
					tempR = 0;
				tempG = starfield[i].colorG - (r * 0x0F);
				if(starfield[i].colorG - (r * 0x0F) <= 0) // more of the same
					tempG = 0;
				tempB = starfield[i].colorB - (r * 0x0F);
				if(starfield[i].colorB - (r * 0x0F) <= 0) // more of the more of the same
					tempB = 0;
				AddPixel(starfield[i].x + r, starfield[i].y, tempR, tempG, tempB, buffer);
			}
	}
	// what's on second
}	// I don't know
	// THIRD BASE!
		// speaking of third base...
		// this summer is looking really good.

/*
    Function:	PaintedButton()
       Input:	X and Y coordinates relative to dialog, text, color, textlength
 Description:	Creates a scene-style button that changes color on hover
*/
void tAZgfx::PaintedButton(int posX, int posY, LPCWSTR Text, int r, int g, int b, int TextLength)
{
	bool isButtonOver = IsOver(posX, posY);
	DrawButton(posX, posY, Text, r, g, b, TextLength, isButtonOver);
}

/*
    Function:	IsOver()
       Input:	X and Y coordinates relative to dialog
 Description:	Returns true if mouse is over
*/
bool tAZgfx::IsOver(int posX, int posY)
{
	int sizeX = 40, sizeY = 20;
	POINT MousePos;
	RECT myWindow;
	GetWindowRect(hwnd, &myWindow);
	GetCursorPos(&MousePos);

	if((MousePos.x - myWindow.left) >= posX && (MousePos.x - myWindow.left) <= posX + sizeX)
		if((MousePos.y - myWindow.top) - posY - 10 >= posY && (MousePos.y - myWindow.top) - posY - 10 <= posY + sizeY)
			return true;
	return false;
}

/*
    Function:	ShadowedText()
       Input:	X and Y coordinates relative to dialog, text, color, HDC, offset for shadow, size of font
 Description:	Draws shadowed text for ASCII art and descriptions
*/
void tAZgfx::ShadowedText(LPCWSTR lpString, int x,int y, int TextColor, int ShadowOffset)
{
	SelectObject(dc, DescriptionFont);
	RECT rect;			// where our text shall be drawn
	rect.left = x;		// blah
	rect.top = y;		// derp
	SetTextColor(dc, 0x333333);	// calling this 20 times each 25 ms probably uses a lot of resources. oh well.
	DrawText(dc, lpString, -1, &rect, DT_NOCLIP);
	rect.left -= ShadowOffset;
	rect.top -= ShadowOffset;
	SetTextColor(dc, TextColor);
	DrawText(dc, lpString, -1, &rect, DT_NOCLIP);
}

/*
    Function:	UpdateScroller()
       Input:	Text, size of text
 Description:	Draws scissoring scroller along bottom of trainer. Call in a timer proc
*/
void tAZgfx::UpdateScroller(wchar_t Text[], int length)
{
	SetTextColor(dc, 0x0080f0);
	RECT scroller = {0};
	every3rd++;
	acounter = xScroll; // store our position
	if(every3rd > 2)
		{
			if(xScroll > 15)
				sineDecreasing = true;
			if(xScroll < 0)
				sineDecreasing = false;
			if(!sineDecreasing)
				xScroll++;
			if(sineDecreasing)
				xScroll--;	
		}

	for(int i = 0; i < length; i++)
	{
		scroller.top = dib_height - 20;
		scroller.top -= xScroll;
		// sine wave time!
		if(cScroller <= -1 * length * 6)
			cScroller = dib_width;
		if(i % 2 == 0) // because I can't figure out a sine wave, we'll just bump every other letter
			scroller.top -= xScroll / 6;
		if(i % 2 != 0) // opposite, bump down
			scroller.top += xScroll / 6;
		scroller.left = cScroller + i * 6; // space text
		DrawText(dc, &Text[i], 1, &scroller, DT_NOCLIP);
	}
	xScroll = acounter;
	if(sineDecreasing)
		xScroll--;
	if(!sineDecreasing)
		xScroll++;
	if(every3rd > 4)
		every3rd = 0;
	cScroller--;
}

/*
    Function:	DrawButton()
       Input:	X and Y coordinates, text, color
 Description:	Private function, called on PaintedButton
*/
RECT tAZgfx::DrawButton(int posX, int posY, LPCWSTR Text, int r, int g, int b, int Length, bool bOver)
{
	int sizeY = 20, sizeX = 40;
	RECT rectButton;
	int tempX, tempY;
	
	tempX = posX;
	tempY = posY + 40;
	rectButton.top = posY * 2 + 5;
	rectButton.left = posX + (sizeX / 2) - Length * 3;
	
	if(bOver)
	{
		r += 175;
		g += 175;
		b += 175;
	}

	// fill button
	for(int currX = 0; currX < sizeX; currX++)
	{
		for(int currY = 0; currY < sizeY; currY++) // lol curry
		{
			AddPixel(tempX + currX, tempY + currY, r, g, b, buffer);
		}
		if(r > 1)
			r--;
		if(g > 1)
			g--;
		if(b > 1)
			b--;
	}

	for(int bX = 0; bX < sizeX; bX++) // top border
		AddPixel(tempX + bX, tempY, 0xFF, 0xFF, 0xFF, buffer);
	for(int ebX = 0; ebX <= sizeX; ebX++) // bottom border
		AddPixel(tempX + ebX, tempY + sizeY, 0xFF, 0xFF, 0xFF, buffer);
	for(int bY = 0; bY < sizeY; bY++) // left border
		AddPixel(tempX, tempY + bY, 0xFF, 0xFF, 0xFF, buffer);
	for(int bY = 0; bY < sizeY; bY++) // right border
		AddPixel(tempX + sizeX, tempY + bY, 0xFF, 0xFF, 0xFF, buffer);

	SetTextColor(dc, 0xFFFFFF);
	SelectObject(dc, ButtonFont);
	DrawText(dc, Text, -1, &rectButton, DT_NOCLIP);
	return rectButton;
}

/*
    Function:	AddPixel()
       Input:	Position, color, buffer for dib
 Description:	Essentially SetPixel() but for dibs and less resource heavy
*/
void tAZgfx::AddPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char *dib)
{
        static int location = 0;
        location = (y * dib_width) + x;
        dib[location * 4] = b;
        dib[location * 4 + 1] = g;
        dib[location * 4 + 2] = r;
	//	dib[location * 4 + 3] = a; // alpha for transparency
}

/*
	Function:	AddTransPixel()
       Input:	Position, color, transparency (0-255), buffer for dib
 Description:	AddPixel with transparent support
*/
void tAZgfx::AddTransPixel(int x, int y, unsigned char r, unsigned char g, unsigned char b, unsigned char a, unsigned char *dib)
{
        static int location = 0;
        location = (y * dib_width) + x;
        dib[location * 4] = b;
        dib[location * 4 + 1] = g;
        dib[location * 4 + 2] = r;
		dib[location * 4 + 3] = a; // alpha for transparency
}

/*
    Function:	LoadFile()
       Input:	None
 Description:	Creates game process
*/
bool tAZgfx::LoadFile()
{
	OPENFILENAME ofn = {0};
	bool bResult = false;
	char gamePath[256] = {0};
	ofn.lStructSize = sizeof(ofn);
	ofn.lpstrFile = (LPWSTR)gamePath;
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(gamePath);
	ofn.lpstrFilter = L"*.exe\0*.EXE\0";
	ofn.lpstrTitle = L"Select game:";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
                    
	if(GetOpenFileName(&ofn))
	{
		PROCESS_INFORMATION processInformation = {0};
		STARTUPINFO startupInfo = {0};
		bResult = CreateProcess(ofn.lpstrFile, NULL, NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &startupInfo, &processInformation);
	}
	return bResult;
}