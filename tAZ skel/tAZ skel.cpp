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
#include "tAZ Engine\tAZ.h"
#include "tAZ Engine\tAZgfx.h"
#include "tAZ Engine\data\chiptune.h"
#include <windows.h>
#include <process.h>

#define WIN32_LEAN_AND_MEAN

tAZgfx trainer;				// our trainer
tAZTRAINER game;			// interacts with the game

void vHotKeys(void *nul);	// handles the key presses in a new thread

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps = {0};
	UINT_PTR mytimer;
    switch(msg)
    {
        case WM_CREATE:
			mytimer = SetTimer(hWnd, NULL, 30, NULL);
			_beginthread(vHotKeys, NULL, NULL);
			trainer.InitStarfield();
            break;
        case WM_PAINT:
				trainer.WM_PAINTStarfield(ps);
				EndPaint(hWnd, &ps);
                break;
        case WM_COMMAND:
            switch(LOWORD(wParam))
            break;
				case WM_TIMER:                                                                                                                              
					trainer.UpdateStarfield();

					trainer.PaintedButton(130, 80, L"LOAD", 0xf0, 0x80, 0x00, 4);
					trainer.PaintedButton(180, 80, L"NFO", 0xf0, 0x80, 0x00, 3);
					trainer.PaintedButton(230, 80, L"EXIT", 0xf0, 0x80, 0x00, 4);

					trainer.UpdateScroller(wcScroll, sizeof(wcScroll) / sizeof(wchar_t));
					trainer.ShadowedText(wcAscii, 60, 10, 0x0080f0, 3);
					trainer.ShadowedText(HotKeys, 150, 190, 0x0080f0, 2);

					RedrawWindow(trainer.hwnd, NULL, NULL, RDW_INVALIDATE);
					break;
                case WM_LBUTTONDOWN:
						if(trainer.IsOver(130, 80))
							trainer.LoadFile();
						if(trainer.IsOver(180, 80))
							MessageBox(hWnd, L"NFO system in progress", L"NFO", MB_OK);
						if(trainer.IsOver(230, 80))
							PostQuitMessage(0);
						else
							SendMessage(hWnd, WM_NCLBUTTONDOWN, HTCAPTION, lParam);
                        break;
                case WM_KEYDOWN:
                        if(wParam == VK_ESCAPE)
                            PostQuitMessage(0);
                        break;
        case WM_DESTROY:
			KillTimer(trainer.hwnd, mytimer);
            PostQuitMessage(0);
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    MSG msg = {0};

	HWND hwnd = trainer.InitializeDemoScene(WndProc);

	if(hwnd == INVALID_HANDLE_VALUE)
		return 0;

    while(GetMessage(&msg, NULL, 0, 0))
    {
        if(msg.message == WM_QUIT)
            break;
        else
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}



void vHotKeys(void *nul)
{
	// First play our song
	uFMOD_PlaySong(&chiptune,(void*)sizeof(chiptune),XM_MEMORY);
	trainer.uFmode = true;

	bool bOption1 = false;		// Our control boolean
	while(true)
	{
	// first check hotkeys
		// An example of a simple toggle hack
		if(game.IsKeyPressed(VK_NUMPAD1))
		{
			if(bOption1)
				bOption1 = false;
			else
				bOption1 = true;
			// Get a handle on the game
			if(!game.Attached)
				game.GetHandle("Halo 2", L"halo2.exe", false);
			game.WriteMem(game.BaseAddress + 0x51A628, &bOption1, 1);
			game._CloseHandle();
		}
	// Now check our music toggle
		if(game.IsKeyPressed(VK_NUMPAD0))
		{
			if(trainer.uFmode == false)
			{
				uFMOD_Resume();
				trainer.uFmode = true;
			}
			else
			{
				uFMOD_Pause();
				trainer.uFmode = false;
			}
		}
	Sleep(75);	// Sleep from 50 to 100 for good response time
	}
	_endthread();
}