//// Watch my video: https://www.youtube.com/watch?v=W6HpX85ICh8
//#include <iostream>
//#include <Windows.h>
//#include <TlHelp32.h>
//
//void display_usage(const char* argv0_)
//{
//	std::cout << "Usage: " << argv0_ << " <DLL File> <[-c <Program Path To Execute>] | [-p <Process Name>] | [-w <Window Title>]>" << std::endl;
//}
//
//DWORD get_process_id_by_creation(char* program_path_, HANDLE* creation_handle_)
//{
//	// initalize variables
//	STARTUPINFO startup_information;
//	PROCESS_INFORMATION process_information;
//	memset(&startup_information, 0, sizeof(startup_information));
//	memset(&process_information, 0, sizeof(process_information));
//
//	startup_information.cb = sizeof(startup_information);
//
//	// create process
//	if (CreateProcess(NULL, program_path_, 0, 0, false, CREATE_SUSPENDED, 0, 0, &startup_information, &process_information))
//	{
//		*creation_handle_ = process_information.hThread;
//		return process_information.dwProcessId;
//	}
//
//	return NULL;
//}
//
//DWORD get_process_id_by_process_name(const char* process_name_)
//{
//	PROCESSENTRY32 process_entry = { sizeof(PROCESSENTRY32) };
//	HANDLE processes_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
//
//	// loop through all process to find one that matches the process_name_
//	if (Process32First(processes_snapshot, &process_entry))
//	{
//		do
//		{
//			if (strcmp(process_entry.szExeFile, process_name_) == 0)
//			{
//				CloseHandle(processes_snapshot);
//				return process_entry.th32ProcessID;
//			}
//		} while (Process32Next(processes_snapshot, &process_entry));
//	}
//
//	CloseHandle(processes_snapshot);
//	return NULL;
//}
//
//DWORD get_process_id_by_window_title(const char* window_title_)
//{
//	// get a handle to window using the window name
//	HWND window_handle = FindWindow(NULL, window_title_);
//	if (window_handle == NULL)
//	{
//		return NULL;
//	}
//
//	// return the process id of the window handle we found
//	DWORD process_id;
//	GetWindowThreadProcessId(window_handle, &process_id);
//	return process_id;
//}
//
//bool inject_dll(DWORD process_id_, const char* dll_file_)
//{
//	// get the full path of the dll file
//	TCHAR full_dll_path[MAX_PATH];
//	GetFullPathName(dll_file_, MAX_PATH, full_dll_path, NULL);
//
//	// get the function LoadLibraryA
//	LPVOID load_library = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");
//	if (load_library == NULL)
//	{
//		return false;
//	}
//
//	// open the process
//	HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, false, process_id_);
//	if (process_handle == NULL)
//	{
//		return false;
//	}
//
//	// allocate space to write the dll location
//	LPVOID dll_parameter_address = VirtualAllocEx(process_handle, 0, strlen(full_dll_path), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
//	if (dll_parameter_address == NULL)
//	{
//		CloseHandle(process_handle);
//		return false;
//	}
//
//	// write the dll location to the space we previously allocated
//	BOOL wrote_memory = WriteProcessMemory(process_handle, dll_parameter_address, full_dll_path, strlen(full_dll_path), NULL);
//	if (wrote_memory == false)
//	{
//		CloseHandle(process_handle);
//		return false;
//	}
//
//	// launch the dll using LoadLibraryA
//	HANDLE dll_thread_handle = CreateRemoteThread(process_handle, 0, 0, (LPTHREAD_START_ROUTINE)load_library, dll_parameter_address, 0, 0);
//	if (dll_thread_handle == NULL)
//	{
//		CloseHandle(process_handle);
//		return false;
//	}
//
//	CloseHandle(dll_thread_handle);
//	CloseHandle(process_handle);
//	return true;
//}
//
//int main(int argc_, char** argv_)
//{
//	// ensure proper usage
//	if (argc_ != 4 || strlen(argv_[2]) != 2)
//	{
//		display_usage(argv_[0]);
//		return -1;
//	}
//
//	// obtain the process ID
//	DWORD process_id = NULL;
//	HANDLE creation_handle = NULL;
//	switch (argv_[2][1])
//	{
//	case 'C':
//	case 'c':
//		process_id = get_process_id_by_creation(argv_[3], &creation_handle);
//		break;
//	case 'P':
//	case 'p':
//		process_id = get_process_id_by_process_name(argv_[3]);
//		break;
//	case 'W':
//	case 'w':
//		process_id = get_process_id_by_window_title(argv_[3]);
//		break;
//	default:
//		display_usage(argv_[0]);
//		return -2;
//	}
//
//	// inject the dll
//	std::cout << "Obtained Process ID: " << process_id << std::endl;
//	std::cout << "Injection: " << (inject_dll(process_id, argv_[1]) ? "Success" : "Failure") << std::endl;
//
//	// if we created the process, resume it
//	if (creation_handle)
//	{
//		ResumeThread(creation_handle);
//	}
//
//	return 0;
//}