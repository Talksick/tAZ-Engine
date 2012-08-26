//		Memory editing/trainer class
////		for C++ [POiZON v1.0Δ]
//////			by Tox1k
////		ryx.933industries.com
//		"good enough never is"




#include "stdafx.h"
#include "tAZ.h"
#include <Psapi.h>
#include <WindowsX.h>
#pragma comment( lib, "psapi" )
/*
	Constructor
*/
tAZTRAINER::tAZTRAINER()
{
	m_hProcess	= NULL;
	m_proc_id	= NULL;
	m_hWnd		= NULL;
	SavedDC		= NULL;
	dwMaxAddr	= NULL;
	BaseAddress = NULL;
	Attached	= false;
	ProcWName = "";
	ProcPName = L"";
}
/*
	Deconstructor
*/
tAZTRAINER::~tAZTRAINER()
{
	if(m_hProcess != NULL)
		CloseHandle(m_hProcess);

	m_hProcess	= NULL;
	m_proc_id	= NULL;
	m_hWnd		= NULL;
	SavedDC		= NULL;
	dwMaxAddr	= NULL;
	BaseAddress = NULL;
	Attached	= false;
	ProcWName = "";
	ProcPName = L"";
}

/*
    Function:	GetHandle()
       Input:	Window name, process name, if process is x64
 Description:	Gets a handle to specified window, if that fails then the process, then gets the base address
*/
byte tAZTRAINER::GetHandle(const char *WindowName, const wchar_t *ProcessName, bool ProcessIsx64)
{
	if(GetWindowHandle(WindowName) != SUCCESS)
		if(GetProcessHandle(ProcessName) != SUCCESS)
			return INVALID_PROC;

	ProcWName = WindowName;
	ProcPName = ProcessName;

	if(ProcessIsx64)
		BaseAddress = GetMod64BaseAddress(ProcessName);
	if(!ProcessIsx64)
		BaseAddress = GetMod32BaseAddress(ProcessName);

	if(BaseAddress != NULL)
	{
		Attached = true;
		return SUCCESS;
	}
	else return INVALID_PROC;
}

/*
    Function:	GetWindowHandle()
       Input:	Window name
 Description:	Gets a handle to specified window
*/
// IE: HANDLE hProcess = GetWindowHandle("Halo");
byte tAZTRAINER::GetWindowHandle(const char *WindowName)
{
	m_hWnd = FindWindowA(0, (LPCSTR)WindowName);					// findwindow, generic
	if(m_hWnd == NULL)
		return INVALID_WINDOW;											// return a null pointer
	GetWindowThreadProcessId(m_hWnd, &m_proc_id);					// get pid, generic
	if(m_proc_id == NULL)
		return INVALID_PROC;											// return a null pointer
	m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, m_proc_id);		// open with all access, generic
	if(m_hProcess == NULL)
		return INVALID_HANDLE;											// return a null pointer
	ProcWName = WindowName;
	return SUCCESS;
}

/*
    Function:	GetProcessHandle()
       Input:	Process name
 Description:	Gets a handle to specified window
*/
// IE: HANDLE hProcess = GetProcessHandle(L"haloce.exe");
byte tAZTRAINER::GetProcessHandle(const wchar_t *ProcessName)
{
	PROCESSENTRY32 entry = {0};
    entry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    if (Process32First(snapshot, &entry) == TRUE)
    {
        for(int z = 0; z < 10000, Process32Next(snapshot, &entry) == true; z++)
        {
            if (wcscmp(entry.szExeFile, ProcessName) == 0)
            {  
				ProcPName = ProcessName;
				m_hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, entry.th32ProcessID);
				return SUCCESS;
			}
        }
		return INVALID_PROC;
    }
}

/*
    Function:	GetMod64BaseAddress()
       Input:	Window name, process name
 Description:	Gets the base address of an x64 process
*/
DWORD_PTR tAZTRAINER::GetMod64BaseAddress(const wchar_t *ProcessName)
{
	HANDLE hSnap;
	PROCESSENTRY32 pe = {0};
	MODULEENTRY32 me = {0};
	HANDLE hMod;

    hSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
    if (hSnap==INVALID_HANDLE_VALUE)
         return 1;

    pe.dwSize=sizeof(pe);
    if (Process32First(hSnap, &pe))
		do {
			hMod=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE|TH32CS_SNAPMODULE32, pe.th32ProcessID);

			me.dwSize = sizeof(me);
			if (Module32First(hMod, &me))
				do{
					if(_wcsicmp(me.szModule, ProcessName) == 0)
					{
						CloseHandle(hMod);
						CloseHandle(hSnap);
						return (DWORD_PTR)me.modBaseAddr;
					}
				} while (Module32Next(hMod, &me));
		  } while (Process32Next(hSnap,&pe));
	if(hSnap != INVALID_HANDLE_VALUE)
		CloseHandle(hSnap);
	if(hMod != INVALID_HANDLE_VALUE)
		CloseHandle(hMod);
}

/*
    Function:	GetMod32BaseAddress()
       Input:	Window name, process name
 Description:	Gets the base address of an x32 process
*/
DWORD_PTR tAZTRAINER::GetMod32BaseAddress(const wchar_t *ProcessName)
{
	HANDLE hSnap;
	PROCESSENTRY32 pe = {0};
	MODULEENTRY32 me = {0};
	HANDLE hMod;

    hSnap=CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnap==INVALID_HANDLE_VALUE)
         return 1;

    pe.dwSize=sizeof(pe);
    if (Process32First(hSnap, &pe))
		do {
			hMod=CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pe.th32ProcessID);

			me.dwSize = sizeof(me);
			if (Module32First(hMod, &me))
				do{
					if(_wcsicmp(me.szModule, ProcessName) == 0)
					{
						CloseHandle(hMod);
						CloseHandle(hSnap);
						return (DWORD_PTR)me.modBaseAddr;
					}
				} while (Module32Next(hMod, &me));
		  } while (Process32Next(hSnap,&pe));
	if(hSnap != INVALID_HANDLE_VALUE)
		CloseHandle(hSnap);
	if(hMod != INVALID_HANDLE_VALUE)
		CloseHandle(hMod);
}

/*
    Function:	Bring2Front()
       Input:	None
 Description:	Sets game window to foreground
*/
byte tAZTRAINER::Bring2Front()
{
	if(m_hWnd == NULL)
		return INVALID_WINDOW;

	SetForegroundWindow(m_hWnd);
	return SUCCESS;
}

/*
    Function:	Send2Back()
       Input:	None
 Description:	Sets game window to background
*/
byte tAZTRAINER::Send2Back()
{
	if(m_hWnd == NULL)
		return INVALID_WINDOW;

	SetWindowPos(m_hWnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
	return SUCCESS;
}

/*
    Function:	_CloseHandle()
       Input:	None
 Description:	Closes and nulls game-handling variables
*/
byte tAZTRAINER::_CloseHandle()
{
	if(m_hProcess != NULL)
		CloseHandle(m_hProcess);
	
	m_hProcess	= NULL;
	m_proc_id	= NULL;
	m_hWnd		= NULL;

	ProcWName = "";
	ProcPName = L"";
	return SUCCESS;
}

/*
    Function:	WriteBytes()
       Input:	Address, byte array
 Description:	Writes specified bytes to memory location
*/
// IE: byte bitzy[] = {0x41};
//	   WriteBytes(hProcess, Address, bitzy)
byte tAZTRAINER::WriteBytes(DWORD_PTR Address, byte bytes[], int size)
{
	if(m_hProcess == NULL)
		return INVALID_HANDLE;

    DWORD db, d = PAGE_EXECUTE_READWRITE;
    if(VirtualProtectEx(m_hProcess, (LPVOID)Address, size, d, &db))						// Make sure we have readwrite access
        if(WriteProcessMemory(m_hProcess, (LPVOID)Address, (LPCVOID)bytes, size, NULL))	// write the value
            if(VirtualProtectEx(m_hProcess, (LPVOID)Address, size, db, &d))				// restore old access
                return SUCCESS;
    return MEM_PERMISSION;
}

/*
    Function:	WriteNops()
       Input:	Address, Size
 Description:	Writes nops to location
*/
byte tAZTRAINER::WriteNops(DWORD_PTR Address, int Size)
{	
	if(m_hProcess == NULL)
		return INVALID_HANDLE;

	byte *nops = new byte[Size];
	for(int z = 0; z < Size; z++)
		nops[z] = 0x90;

    DWORD db, d = PAGE_EXECUTE_READWRITE;
	if(VirtualProtectEx(m_hProcess, (LPVOID)Address, Size, d, &db))					// Make sure we have readwrite access
        if(WriteProcessMemory(m_hProcess, (LPVOID)Address, (LPCVOID)nops, Size, NULL))	// write the value
			if(VirtualProtectEx(m_hProcess, (LPVOID)Address, Size, db, &d))				// restore old access
			{
				delete[] nops;
				return SUCCESS;
			}
	delete[] nops;
	return MEM_PERMISSION;
}

/*
    Function:	WriteMem()
       Input:	Address, buffer
 Description:	Writes buffer to memory location
*/
byte tAZTRAINER::WriteMem(DWORD_PTR Address, LPVOID buffer, int size)
{
	if(m_hProcess == NULL)
		return INVALID_HANDLE;

    DWORD db, d = PAGE_EXECUTE_READWRITE;
    if(VirtualProtectEx(m_hProcess, (LPVOID)Address, size, d, &db))						// Make sure we have readwrite access
        if(WriteProcessMemory(m_hProcess, (LPVOID)Address, (LPCVOID)buffer, size, NULL))	// write the value
            if(VirtualProtectEx(m_hProcess, (LPVOID)Address, size, db, &d))				// restore old access
                return SUCCESS;
    return MEM_PERMISSION;
}

/*
    Function:	ReadRawMem()
       Input:	Handle, Address, Number of bytes to read
 Description:	Reads raw memory into lpvoid pointer
*/
LPVOID tAZTRAINER::ReadRawMem(DWORD_PTR Address, DWORD Bytes2Read)
{
	if(m_hProcess == NULL)
		return false;

	LPVOID buffer;
	DWORD dwOldProtect = NULL, d = PAGE_EXECUTE_READWRITE, db;
	if(!VirtualProtectEx(m_hProcess, (LPVOID)Address, Bytes2Read, d, &db))
		return ERROR;
		if(!ReadProcessMemory(m_hProcess, (LPCVOID)Address, &buffer, Bytes2Read, NULL))
			return ERROR;
			if(!VirtualProtectEx(m_hProcess, (LPVOID)Address, Bytes2Read, db, &d))
				return ERROR;

	return buffer;
}

/*
    Function:	ReadMemInt()
       Input:	Handle, Address, Number of bytes to read
 Description:	Reads memory as int
*/
int tAZTRAINER::ReadMemInt(DWORD_PTR Address)
{
	if(m_hProcess == NULL)
		return false;

	int buffer;
	DWORD dwOldProtect = NULL, d = PAGE_EXECUTE_READWRITE, db;
	if(!VirtualProtectEx(m_hProcess, (LPVOID)Address, sizeof(buffer), d, &db))
		return ERROR;
		if(!ReadProcessMemory(m_hProcess, (LPCVOID)Address, &buffer, sizeof(buffer), NULL))
			return ERROR;
			if(!VirtualProtectEx(m_hProcess, (LPVOID)Address, sizeof(buffer), db, &d))
				return ERROR;

	return buffer;
}

/*
    Function:	ReadMemFloat()
       Input:	Handle, Address, Number of bytes to read
 Description:	Reads memory as float
*/
float tAZTRAINER::ReadMemFloat(DWORD_PTR Address)
{
	if(m_hProcess == NULL)
		return false;

	float buffer;
	DWORD dwOldProtect = NULL, d = PAGE_EXECUTE_READWRITE, db;
	if(!VirtualProtectEx(m_hProcess, (LPVOID)Address, sizeof(buffer), d, &db))
		return ERROR;
		if(!ReadProcessMemory(m_hProcess, (LPCVOID)Address, &buffer, sizeof(buffer), NULL))
			return ERROR;
			if(!VirtualProtectEx(m_hProcess, (LPVOID)Address, sizeof(buffer), db, &d))
				return ERROR;

	return buffer;
}

/*
    Function:	ReadMemText()
       Input:	Handle, Address, Number of bytes to read
 Description:	Reads memory to an unsigned char array (NOT NULLED)
*/
byte* tAZTRAINER::ReadMemText(DWORD_PTR Address, int Size)
{
	if(m_hProcess == NULL)
		return false;
	DWORD_PTR memAddr = NULL;

	byte *buffer = new byte[Size];
	DWORD dwOldProtect = NULL, d = PAGE_EXECUTE_READWRITE, db;
	if(!VirtualProtectEx(m_hProcess, (LPVOID)Address, Size, d, &db))
		return ERROR;
	int z = 0;
	do {
		memAddr = Address + z;
		if(!ReadProcessMemory(m_hProcess, (LPCVOID)memAddr, &buffer[z], sizeof(byte), NULL))
			return ERROR;
		z++;
	} while(z < Size);
			if(!VirtualProtectEx(m_hProcess, (LPVOID)Address, Size, db, &d))
				return ERROR;

	return buffer;//(FixString((unsigned char*)buffer, Size));
}

/*
    Function:	ReadPointer()
       Input:	Offset, Address
 Description:	Reads address and multilevel pointers, returns address
*/
DWORD_PTR tAZTRAINER::ReadPointer(DWORD_PTR dpAddress, DWORD_PTR Offsets[], int OffCount)
{
	if(m_hProcess == NULL)
		return false;

	DWORD_PTR Address = dpAddress;
	DWORD_PTR buffer;

	for(int i = 0; i < OffCount; i++)
	{
		buffer = NULL;
		DWORD d = PAGE_EXECUTE_READWRITE, db = NULL;
		if(!VirtualProtectEx(m_hProcess, (LPVOID)Address, sizeof(buffer), d, &db))
			return ERROR;
			if(!ReadProcessMemory(m_hProcess, (LPCVOID)Address, &buffer, sizeof(buffer), NULL))
				return ERROR;
				if(!VirtualProtectEx(m_hProcess, (LPVOID)Address, sizeof(buffer), db, &d))
					return ERROR;
		Address = buffer + Offsets[i];
	}
	return Address;
}

/*
    Function:	EnableDebugPriv()
       Input:	None
 Description:	Enables debug priviliges on process, needs admin
*/
byte tAZTRAINER::EnableDebugPriv()
{
    HANDLE hToken;
    LUID sedebugnameValue;
    TOKEN_PRIVILEGES tkp;

    if (!OpenProcessToken(m_hProcess, TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return MEM_PERMISSION;
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
	{
        CloseHandle(hToken);
        return MEM_PERMISSION;
    }
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = sedebugnameValue;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges( hToken, FALSE, &tkp, sizeof tkp, NULL, NULL))
	{
        CloseHandle(hToken);
		return MEM_PERMISSION;
	}
	return SUCCESS;
} 

/*
    Function:	PatchFile()
       Input:	File name, bytes
 Description:	Finds address of bytes
*/
// IE: Patchdata Patch = {0x00000001, "\x24\x24\x24"};
//	  PatchFile("Halo.exe", Patch);
byte tAZTRAINER::PatchFile(const char *filename, PatchData Patch)
{
	long FileSize; // filesize checking
	filebuf* pbuf;

	fstream myfile (filename, ios::in|ios::out|ios::binary);
	pbuf = myfile.rdbuf();

	FileSize = pbuf->pubseekoff(0, ios_base::end);

	if(!myfile.is_open())
		return INVALID_FILE;

	if(myfile.fail())
		return FILE_ERROR;

	for(int i = 0; i < sizeof(Patch) / sizeof(PatchData); i++)
	{
		if(!myfile.seekp(Patch.offset, ios::beg))
			return FILE_ERROR;
		if(!myfile.write(Patch.patch, sizeof(Patch.patch)))
			return FILE_ERROR;
	}

	myfile.close();
	return SUCCESS;
}

/*
    Function:	MakeCave()
       Input:	Cave size, cave number
 Description:	Makes a cave. To set the cave bytes, use modifycave
*/
byte tAZTRAINER::MakeCave(DWORD CaveSize, int CaveNumber)
{
	if(m_hProcess == NULL)
		return INVALID_HANDLE;

	DWORD dwCaveAddress = NULL;
	dwCaveAddress = (DWORD)VirtualAllocEx(m_hProcess, NULL, CaveSize + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	pCaveSize[CaveNumber] = CaveSize;
	CaveAddy[CaveNumber] = dwCaveAddress;
	return SUCCESS;
}

/*
    Function:	ModifyCave()
       Input:	Bytes, cavenumber
 Description:	Modifies a codecave.
*/
byte tAZTRAINER::ModifyCave(byte bytes[], int CaveNumber)
{
	return WriteBytes(CaveAddy[CaveNumber], bytes, sizeof(bytes));
			
}

/*
    Function:	DeleteCave()
       Input:	Cave number
 Description:	Frees memory allocated by a cave.
*/
byte tAZTRAINER::DeleteCave(int CaveNumber)
{
	if(m_hProcess == NULL)
		return INVALID_HANDLE;

	if(VirtualFreeEx(m_hProcess, (LPVOID)CaveAddy[CaveNumber], pCaveSize[CaveNumber] + 5, MEM_DECOMMIT))
		return SUCCESS;
	return MEM_PERMISSION;
}

/*
    Function:	MakeJMP()
       Input:	Address to write JMP, cave number
 Description:	Sets a jmp to the specified cave. Make sure to MakeCave and ModifyCave first to avoid crashes.
*/
byte tAZTRAINER::MakeJMP(DWORD_PTR Address, int CaveNumber)
{
	byte *AddyJmp = (PBYTE)(Address + 1);

	byte JMP[] = {0xE9}; // opcode for jmp

	// Write our codecave before the jump to ensure we don't crash

	if(WriteBytes(CaveAddy[CaveNumber] + pCaveSize[CaveNumber], JMP, sizeof(JMP)) != SUCCESS)				// Write JMP at end of codecave
		return MEM_PERMISSION;
	if(WriteBytes(CaveAddy[CaveNumber] + pCaveSize[CaveNumber] + 1, AddyJmp, sizeof(AddyJmp)) != SUCCESS)	// Write offset for JMP at end of codecave
		return MEM_PERMISSION;

	if(WriteBytes(Address, JMP, sizeof(JMP)) != SUCCESS)													// Write the JMP command
		return MEM_PERMISSION;
	if(WriteBytes(Address + 1, AddyJmp, 4) != SUCCESS)														// Write the JMP offset
		return MEM_PERMISSION;

	return SUCCESS;
}

/*
    Function:	Cave()
       Input:	JMP address, cave bytes, cave number
 Description:	Writes cave and jmp
*/
byte tAZTRAINER::Cave(DWORD_PTR JMPAddress, byte bytes[], int CaveNumber)
{
	if(MakeCave(sizeof(bytes) / sizeof(byte), CaveNumber) == SUCCESS)
		if(ModifyCave(bytes, CaveNumber) == SUCCESS)
			if(MakeJMP(JMPAddress, CaveNumber) == SUCCESS)
				return SUCCESS;
	return MEM_PERMISSION;
}

/*
    Function:	CheckFile()
       Input:	File name
 Description:	Used by other functions to check if file exits
	  Errors:	returns false
*/
byte tAZTRAINER::CheckFile(const char *filename)
{
	fstream myfile (filename, ios::in | ios::app);
	if(myfile.is_open())
	{
		myfile.close();
		return SUCCESS;
	}
	else
		return INVALID_FILE;
}

/*
    Function:	GetLine()
       Input:	File name, line number
 Description:	Retrieves text at line number
*/
string tAZTRAINER::GetLine(const char *filename, int line)
{
	if(CheckFile(filename) != SUCCESS)
		return NULL;
	ifstream myfile (filename, ios::in | ios::beg);
	string buffer;

	for(int i = 0; i < line; i++)
		if(myfile.good())
			getline(myfile, buffer);
		else return NULL;

	buffer = "";									// reset buffer
	if(myfile.good())
		getline(myfile, buffer);
	else return NULL; 
		
	myfile.close();
	return buffer;
}

/*
    Function:	SetLine()
       Input:	File name, line number, buffer
 Description:	Sets text at line number
*/
byte tAZTRAINER::SetLine(const char *filename, string buffer, int line)
{
	if(CheckFile(filename) != SUCCESS)
		return INVALID_FILE;
	fstream myfile (filename, ios::in | ios::out | ios::beg);
	string temp;

	for(int i = 0; i < line; i++)
		if(myfile.good())
			getline(myfile, temp);
		else return FILE_ERROR;
	temp = ""; // idk why...
	myfile << buffer;
	myfile.close();
	return SUCCESS;
}

/*
    Function:	WriteiFile()
       Input:	File name, buffer
 Description:	Creates a new file and writes to it (warning, overwrites data)
*/
byte tAZTRAINER::WriteiFile(const char *filename, string buffer)
{
	ofstream myfile (filename, ios::out | ios::beg | ios::trunc);
	if(!myfile.is_open())
		return FILE_ERROR;
	myfile << buffer;
	myfile.close();
	return SUCCESS;
}

/*
    Function:	countSub
       Input:	Original string, substring to count
 Description:	Counts substrings in a string, don't ask why it's here. string comparison.
*/
int tAZTRAINER::countSub(string str, string sub)
{
    if (sub.length() == 0) 
		return NULL;
    int count = 0;
    for (size_t offset = str.find(sub); offset != std::string::npos;
	 offset = str.find(sub, offset + sub.length()))
    {
        ++count;
    }
    return count;
}

/*
    Function:	DrawBitDialog
       Input:	All automatic, don't worry about it.
 Description:	Gives your window a transparent background and custom shape.
*/
byte tAZTRAINER::DrawBitDialog(HWND winhandle,HDC &bmapDC,BITMAP &btmap,HRGN &retcombine)
{
	int x,y,startx=0;
	HRGN temp;
	COLORREF trans,tst;

	trans = GetPixel(bmapDC,0,0);

	retcombine=CreateRectRgn(0,0,0,0);
	temp = CreateRectRgn(0,0,0,0);

	for(y=0;y<=btmap.bmHeight-1;y++)
	{
		for(x=0;x<=btmap.bmWidth-1;x++)
		{
			tst=GetPixel(bmapDC,x,y);
			if(tst!=trans)
			{
				startx=x;
				while((x<=btmap.bmWidth)&&((GetPixel(bmapDC,x,y))!=trans))
				{x++;}
				x--;
				temp=CreateRectRgn(startx,y,x,y+1);
				CombineRgn(retcombine,retcombine,temp,RGN_OR);
			}
		}
	}	return SUCCESS;
}

/*
    Function:	WM_CREATEBitDlg
       Input:	HWND to your app, resource code to bitmap dialog
 Description:	Place in your win32 WM_CREATE call to form custom dialog. Then just replace the default createwindowex with your own calling this window HMENU.
*/
byte tAZTRAINER::WM_CREATEBitDlg(HWND hWnd, HMENU IDBBitResource)
{
			HDC hdc;
			static BITMAP bmap;

			HRGN combine;
			int GWL_HINSTANCE;
			hdc=GetDC(hWnd);
			iface = LoadBitmap((HINSTANCE)GetWindowLong(hWnd,GWL_HINSTANCE), MAKEINTRESOURCE(IDBBitResource));
			ifaceDC = CreateCompatibleDC(hdc);
			GetObject(iface,sizeof(bmap),&bmap);
			SavedDC = SaveDC(ifaceDC);
			SelectObject(ifaceDC,iface);
			ReleaseDC(hWnd,hdc);
			MoveWindow(hWnd,60,60,bmap.bmWidth,bmap.bmHeight,true);
			SendMessage(hWnd,WM_PAINT,0,0);

			DrawBitDialog(hWnd,ifaceDC,bmap,combine);

			SetWindowRgn(hWnd,combine,true);
			ShowWindow(hWnd,SW_SHOW);
			UpdateWindow(hWnd);
			return SUCCESS;
}

/*
    Function:	WM_DESTROYBitDlg
       Input:	None
 Description:	Place in your win32 WM_DESTROY call to destroy custom dialog.
*/
byte tAZTRAINER::WM_DESTROYBitDlg()
{
	RestoreDC(ifaceDC,SavedDC);
	DeleteBitmap(iface);
	return SUCCESS;
}

/*
    Function:	CreateBitDlgClass
       Input:	None.
 Description:	Call this OnInitInstance or on WINMAIN. Replaces the default window class. Uncomment the wclass.lpfnWndProc.
*/
WNDCLASSEX tAZTRAINER::CreateBitDlgClass()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	const char *MainClassName = "Trainer";
	WNDCLASSEX wclass;

	wclass.cbSize = sizeof(wclass);
	wclass.style = CS_HREDRAW | CS_VREDRAW;
//	wclass.lpfnWndProc = WndProc;				UNCOMMENT THIS IN WIN32 APP!
	wclass.cbClsExtra = 0;
	wclass.cbWndExtra = 0;
	wclass.hInstance = hInstance;
	wclass.hIcon = LoadIcon(NULL,IDI_APPLICATION);
	wclass.hCursor = LoadCursor(NULL,IDC_ARROW);
	wclass.hbrBackground = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
	wclass.lpszMenuName = NULL;
	wclass.lpszClassName = (LPCWSTR)MainClassName;
	wclass.hIconSm = LoadIcon(NULL,IDI_APPLICATION);
	RegisterClassEx(&wclass);

	return wclass;
}


HWND tAZTRAINER::CreateIconButton(HMENU ICONResource, int x, int y, int sizeX, int sizeY)
{
	HWND hWnd = FindWindow(NULL, NULL);
	HWND hWndButton = CreateWindowEx(NULL, L"BUTTON", L"BUTTON", BS_ICON|BS_FLAT|WS_VISIBLE|WS_CHILD, x, x, sizeX, sizeY, hWnd, ICONResource, GetModuleHandle(NULL), NULL);
	HANDLE hU = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(ICONResource), IMAGE_ICON, sizeX, sizeY, LR_DEFAULTCOLOR);
	SendMessage(hWndButton, BM_SETIMAGE, IMAGE_ICON, (LPARAM) hU);
	return hWndButton;
}
/*
HWND tAZTRAINER::CreateIconButton(HMENU BMPResource, int x, int y, int sizeX, int sizeY)
{
	HWND hWnd = FindWindow(NULL, NULL);
	HWND hWndButton = CreateWindowEx(NULL, L"BUTTON", L"BUTTON", BS_ICON|BS_FLAT|WS_VISIBLE|WS_CHILD, x, x, sizeX, sizeY, hWnd, BMPResource, GetModuleHandle(NULL), NULL);
	HANDLE hU = LoadImage(GetModuleHandle(NULL), MAKEINTRESOURCE(BMPResource), IMAGE_BITMAP, sizeX, sizeY, LR_DEFAULTCOLOR);
	SendMessage(hWndButton, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM) hU);
	return hWndButton;
}*/


/*
    Function:	SendKey()
       Input:	VK_KEY
 Description:	Sends VK_KEY keydown and keyup
*/
byte tAZTRAINER::SendKey(int VK_KEY)
{
	bool bExtended = false;
    KEYBDINPUT  kb = {0};
    INPUT       Input = {0};

    /* Generate a "key down" */
    if(bExtended == true)
		{ kb.dwFlags  = KEYEVENTF_EXTENDEDKEY; }
    kb.wVk  = VK_KEY;
    Input.type  = INPUT_KEYBOARD;
    Input.ki  = kb;
    SendInput(1, &Input, sizeof(Input));

    /* Generate a "key up" */
    ZeroMemory(&kb, sizeof(KEYBDINPUT));
    ZeroMemory(&Input, sizeof(INPUT));
    kb.dwFlags  =  KEYEVENTF_KEYUP;
    if (bExtended) { kb.dwFlags |= KEYEVENTF_EXTENDEDKEY; }
    kb.wVk = VK_KEY;
    Input.type = INPUT_KEYBOARD;
    Input.ki = kb;
    SendInput(1, &Input, sizeof(Input));

    return SUCCESS;
}

/*
    Function:	SpamKeyString
       Input:	String (no spaces), times to repeat, pause between key presses
 Description:	Sends word spam to keyboard. Can also be used to send a key string.
*/
byte tAZTRAINER::SpamKeyString(char *KeyString, int Repeat, int SpeedinMillisec, bool ReturnAtEndofString)
{
	int length;
	for(length = 0; KeyString[length] != NULL; length++){} // get size of string

	char *UKeyString = new char[length]; // create a buffer for uppercase
	for(int q = 0; q < length; q++)
		UKeyString[q] = toupper(KeyString[q]);

	char Caps[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
//	char Punc[] = {'.', ',', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '-', '+', '=', '{', '}', '[', ']', '|', '\',, '~', '<', '>', '?'
	for(int z = 0; z < Repeat; z++)
	{
		for(int i = 0; i < length; i++)
		{
			bool kaps = false;
			// loop to check for capital
			for(int z = 0; z < 26; z++)
				if(KeyString[i] == Caps[z])
					kaps = true;
			// turn on capslock
			if(kaps == true)
				SendKey(VK_CAPITAL);

			SendKey(UKeyString[i]);
			// turn off capslock
			if(kaps == true)
				SendKey(VK_CAPITAL);

			Sleep(SpeedinMillisec);
		}
		
		if(ReturnAtEndofString == true)
		{
			SendKey(VK_RETURN);
			Sleep(SpeedinMillisec);
		}
	}
	delete[] UKeyString;
	return SUCCESS;
}

byte tAZTRAINER::PressButton(int VK_KEY) 
{
	INPUT input[2]; 

    input[0].type			= INPUT_KEYBOARD; 
    input[0].ki.wVk			= 0; 
    input[0].ki.wScan		= VK_KEY; 
    input[0].ki.dwFlags		= KEYEVENTF_UNICODE ; 
    input[0].ki.time		= 0; 
    input[0].ki.dwExtraInfo = 0; 

    input[1].type			= INPUT_KEYBOARD; 
    input[1].ki.wVk			= 0; 
    input[1].ki.wScan		= VK_KEY; 
    input[1].ki.dwFlags		= KEYEVENTF_KEYUP  | KEYEVENTF_UNICODE ; 
    input[1].ki.time		= 0; 
    input[1].ki.dwExtraInfo = 0;

    SendMessage(m_hWnd, VK_KEY, WM_KEYDOWN, 0);
    PostMessage(m_hWnd, WM_CHAR, VK_KEY, 0);
	SendInput(2, input, sizeof(INPUT)); 
    SendMessage(m_hWnd, VK_KEY, WM_KEYUP, 0);
    return SUCCESS;
}

/*
    Function:	SendUnicodeChar()
       Input:	Hex number of char
 Description:	Sends a unicode character as a keypress
*/
bool tAZTRAINER::SendUnicodeChar(int hex_number) // ie 0x81e4 = 33252
{
	INPUT input[2]; 

    input[0].type			= INPUT_KEYBOARD; 
    input[0].ki.wVk			= 0; 
    input[0].ki.wScan		= hex_number; 
    input[0].ki.dwFlags		= KEYEVENTF_UNICODE ; 
    input[0].ki.time		= 0; 
    input[0].ki.dwExtraInfo = 0; 

    input[1].type			= INPUT_KEYBOARD; 
    input[1].ki.wVk			= 0; 
    input[1].ki.wScan		= hex_number; 
    input[1].ki.dwFlags		= KEYEVENTF_KEYUP  | KEYEVENTF_UNICODE ; 
    input[1].ki.time		= 0; 
    input[1].ki.dwExtraInfo = 0;

    int retval = SendInput(2, input, sizeof(INPUT)); 
    if(retval)
		return SUCCESS;
    else 
		return ERROR;
}

/*
    Function:	ModCursor
       Input:	x, y, if position will set from x and y, and if cursor is visible
 Description:	Just a simple cursor edit, for a project
*/
byte tAZTRAINER::ModCursor(int x, int y, bool SetPos, bool Visible)
{
	if(SetPos)
		SetCursorPos(x, y);
	if(Visible)
		ShowCursor(true);
	else
		ShowCursor(false);
	return SUCCESS;
}

void tAZTRAINER::LeftClick()
{  
	INPUT Input = {0};

	Input.type			= INPUT_MOUSE;
	Input.mi.dwFlags	= MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &Input, sizeof(INPUT));

	ZeroMemory(&Input,sizeof(INPUT));
	Input.type			= INPUT_MOUSE;
	Input.mi.dwFlags	= MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));
}

void tAZTRAINER::RightClick()
{  
	INPUT Input = {0};

	Input.type			= INPUT_MOUSE;
	Input.mi.dwFlags	= MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &Input, sizeof(INPUT));

	ZeroMemory(&Input,sizeof(INPUT));
	Input.type			= INPUT_MOUSE;
	Input.mi.dwFlags	= MOUSEEVENTF_RIGHTUP;
	SendInput(1, &Input, sizeof(INPUT));
}

/*
    Function:	ModMouse
       Input:	Left clicks, right clicks, pauses
 Description:	Sends left and right clicks
*/
byte tAZTRAINER::ModMouse(int LeftClicks, int RightClicks, int sleep)
{
	int repL = 0, repR = 0;
	do 
	{
		if(repL <= LeftClicks)
		{
			LeftClick();
			repL++;
		}
		if(repR <= LeftClicks)
		{
			RightClick();
			repR++;
		}
		Sleep(sleep);
	} while(repL != LeftClicks && repR != RightClicks);
	return SUCCESS;
}

/*
    Function:	IsKeyPressed
       Input:	Virtual key code
 Description:	Checks for keyboard press
*/
bool tAZTRAINER::IsKeyPressed(unsigned char VK_KEY)
{
	unsigned short Status = GetAsyncKeyState(VK_KEY);
	return (((Status & 0x8000) >> 15) == 1) || ((Status & 1) == 1);
}

/*
    Function:	ReadRegSub
       Input:	Key, value
 Description:	Reads registry subkey
	   Other:	Unfinished
*/
char* tAZTRAINER::RegReadSub(LPCTSTR RegKey, LPCTSTR Value)
{
	char *buffer		= new char[1024];	// buffer of 1024 char (max length)
	HKEY hKey;								// Declare a key to store the result
	DWORD buffersize	= sizeof(buffer) / sizeof(const char);
	if(RegOpenKeyEx(HKEY_CURRENT_USER, (LPCWSTR)RegKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return ERROR;

	RegQueryValueEx(hKey, (LPCWSTR)Value, NULL, NULL, (LPBYTE)buffer, &buffersize);

	RegCloseKey (hKey);
	return buffer;
}

/*
    Function:	InjectDll
       Input:	Dll name
 Description:	Injects Dll with CreateRemoteThread method
	   Other:	Untested
*/
HANDLE tAZTRAINER::InjectDLL(char *dllName)
{
    char buf[50]={0};
    LPVOID RemoteString, LoadLibAddy;

    if(!m_proc_id)
       return INVALID_HANDLE_VALUE;

    LoadLibAddy = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");

    RemoteString = (LPVOID)VirtualAllocEx(m_hProcess, NULL, strlen(dllName), MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    if(RemoteString)
		if(WriteProcessMemory(m_hProcess, (LPVOID)RemoteString, dllName, strlen(dllName), NULL))
			return CreateRemoteThread(m_hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddy, (LPVOID)RemoteString, NULL, NULL);
	return INVALID_HANDLE_VALUE;   
}

/*
    Function:	x2b
       Input:	Hex string (ie: 81e481e4), number of bytes to be read (size of hexstring / 2)
 Description:	Converts char hex string to bytes, for namehacks and byte sigs
*/
byte* tAZTRAINER::x2b(const char hexstring[], int numBytes)
{
    const char *pos = hexstring;
	byte val[32] = {0};
    size_t count = 0;

    for(count = 0; count < sizeof(val)/sizeof(val[0]); count++) {
        sscanf(pos, "%2hx", &val[count]);
        pos += 2 * sizeof(char);
    }

	return val;
}
