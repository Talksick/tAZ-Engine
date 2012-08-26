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

// Functions are stated as such:

/*
    Function:	The function name
       Input:	What you'll need to input
 Description:	Short description of what it does
	  Errors:	What the return will be if you have an error/invalid
*/

#pragma once
#include "..\stdafx.h"
#include <Windows.h>
#include <TlHelp32.h>	// GetProcessHandle
#include <string>		// ReadMemText
#include <fstream>		// Patcher and logfiles

using namespace std;

// error codes - return instead of bool
//define	ERROR				0x00 (predefined)
#define		SUCCESS				0x01
#define		INVALID_HANDLE		0xF0
#define		INVALID_WINDOW		0xF1
#define		INVALID_PROC		0xF2
#define		INVALID_ADDR		0xF3
#define		FILE_NOT_FOUND		0xF4
#define		FILE_ERROR			0xF5
#define		INVALID_FILE		0xF6
#define		MEM_PERMISSION		0xF7
#define		INITIALIZE_ERROR	0xFE
#define		EXCEPTION_ERROR		0xFF

#define byte unsigned char

/*
	  Struct:	PatchData
 Description:	Stores patch data
*/
struct PatchData 
	{
		DWORD_PTR offset;
		const char *patch;
	};

			
	static int SavedDC;				// Saved DCI			For BitDlg
	static HBITMAP iface;			// Saved bitmap				|
	static HDC ifaceDC;				// Saved HDC				V

/*
	   Class:	tAZTRAINER
 Description:	Simplified trainer/patcher class
*/
class tAZTRAINER
{
private:
	HANDLE		m_hProcess;		// Handle to proc
	DWORD		m_proc_id;		// PID
	HWND		m_hWnd;			// Window
	
	const char		*ProcWName;		// Game window name				(ie Halo)			Sets on GetWindowHandle
	const wchar_t	*ProcPName;		// Game process/exe name		(ie haloce.exe)		Sets on GetProcessHandle

	byte		DrawBitDialog(HWND winhandle,HDC &bmapDC,BITMAP &btmap,HRGN &retcombine);	 // called on custom bitmap
	
	void		RightClick();		// pretty self explanatory
									//			|
	void		LeftClick();		//			V

	DWORD_PTR	*CaveAddy;		// address of codecave (pointer to array)
	DWORD_PTR	*JumpAddy;		// address of jmp to codecave
	DWORD		*pCaveSize;		// Size of cave

//	unsigned long	 base;
	LPVOID		dwMaxAddr;

	bool		CompareData(char* szBytes, char* szMask, char* szMemDump, long offset);
	byte		MakeCave(DWORD CaveSize, int CaveNumber);
	byte		MakeJMP(DWORD_PTR Address, int CaveNumber);

public:
	bool Attached;
	DWORD_PTR	BaseAddress;

	tAZTRAINER();
	~tAZTRAINER();

	// Memory Editing related:
		// Process related:
		byte		GetHandle(const char *WindowName, const wchar_t *ProcessName, bool ProcessIsx64);
		byte		GetWindowHandle(const char *WindowName);
		byte		GetProcessHandle(const wchar_t *ProcessName);
		byte		EnableDebugPriv();
		DWORD_PTR   GetMod64BaseAddress(const wchar_t *ProcessName);
		DWORD_PTR	GetMod32BaseAddress(const wchar_t *ProcessName);
		byte		Bring2Front();
		byte		Send2Back();
		byte		_CloseHandle();

		// Readmem related:
		LPVOID		ReadRawMem(DWORD_PTR Address, DWORD Bytes2Read);
		int			ReadMemInt(DWORD_PTR Address);
		float		ReadMemFloat(DWORD_PTR Address);
		byte*		ReadMemText(DWORD_PTR Address, int Size);
		DWORD_PTR	ReadPointer(DWORD_PTR Address, DWORD_PTR Offsets[], int offCount);

		// Writemem related:
		byte		WriteMem(DWORD_PTR Address, LPVOID buffer, int size);
		byte		WriteBytes(DWORD_PTR Address, byte bytes[], int size);
		byte		WriteNops(DWORD_PTR Address, int Size);
		byte		ModifyCave(byte bytes[], int CaveNumber);
		byte		DeleteCave(int CaveNumber);
		byte		Cave(DWORD_PTR Address, byte bytes[], int CaveNumber);
		HANDLE		InjectDLL(char *dllName);

		// Patcher/File related:
		byte		PatchFile(const char *filename, PatchData Patch);
		byte		CheckFile(const char *filename);
		byte		WriteiFile(const char *filename, string buffer);
		byte		SetLine(const char *filename, string buffer, int line);
		string		GetLine(const char *filename, int line);
		int			countSub(string str, string sub);
		byte*		x2b(const char hexstring[], int numBytes);

		// Graphic related:
		byte		WM_CREATEBitDlg(HWND hWnd, HMENU IDBBitResource);
		byte		WM_DESTROYBitDlg();
		WNDCLASSEX	CreateBitDlgClass();	
		HWND		CreateIconButton(HMENU BMPResource, int x, int y, int sizeX, int sizeY);

		// Message related:
		byte		SendKey(int VK_KEY);
		byte		PressButton(int VK_KEY);
		bool		SendUnicodeChar(int hex_number);
		byte		SpamKeyString(char *KeyString, int Repeat, int SpeedinMillisec, bool ReturnAtEndofString);
		byte		ModCursor(int x, int y, bool SetPos, bool Visible);
		byte		ModMouse(int LeftClicks, int RightClicks, int sleep);
		bool		IsKeyPressed(unsigned char VK_KEY);

		// Registry
		char*		RegReadSub(LPCTSTR RegKey, LPCTSTR Value);
		byte		RegWriteSub();
};