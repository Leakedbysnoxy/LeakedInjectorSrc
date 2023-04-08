#pragma once
#include <TlHelp32.h>

using namespace std;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
PVOID get_dll_by_file(LPCSTR file_path)
{
	HANDLE h_dll = CreateFileA(file_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h_dll == INVALID_HANDLE_VALUE)
		return NULL;

	DWORD dll_file_sz = GetFileSize(h_dll, NULL);
	PVOID dll_buffer = VirtualAlloc(NULL, dll_file_sz, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (!ReadFile(h_dll, dll_buffer, dll_file_sz, NULL, FALSE) || *(PDWORD)dll_buffer != 9460301)
	{
		VirtualFree(dll_buffer, 0, MEM_RELEASE);
		goto exit;
	}

exit:
	CloseHandle(h_dll);
	return dll_buffer;
}

DWORD get_process_id_and_thread_id_by_window_class(LPCWSTR windowname, PDWORD p_thread_id)
{
	DWORD process_id = 0;
	while (!process_id)
	{
		*p_thread_id = GetWindowThreadProcessId(FindWindow(windowname, 0), &process_id); Sleep(20);
	} return process_id;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//DWORD get_process_id_by_name(PCSTR name)
//{
//	DWORD pid = 0;
//
//	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//	PROCESSENTRY32 process;
//	ZeroMemory(&process, sizeof(process));
//	process.dwSize = sizeof(process);
//
//	if (Process32First(snapshot, &process))
//	{
//		do
//		{
//			if (std::string(process.szExeFile) == std::string(name))
//			{
//				pid = process.th32ProcessID;
//				break;
//			}
//		} while (Process32Next(snapshot, &process));
//	}
//
//	CloseHandle(snapshot);
//	return pid;
//}
//
//std::string get_process_name_by_pid(DWORD process_id)
//{
//	PROCESSENTRY32 processInfo;
//	processInfo.dwSize = sizeof(processInfo);
//	HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
//	if (processesSnapshot == INVALID_HANDLE_VALUE)
//		return std::string();
//
//	for (BOOL bok = Process32First(processesSnapshot, &processInfo); bok; bok = Process32Next(processesSnapshot, &processInfo))
//	{
//		if (process_id == processInfo.th32ProcessID)
//		{
//			return processInfo.szExeFile;
//		}
//	}
//
//	CloseHandle(processesSnapshot);
//	return std::string();
//}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
