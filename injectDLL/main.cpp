#include <windows.h> 
#include <tlhelp32.h> 
#include <shlwapi.h> 
#include <conio.h> 
#include <stdio.h>
#include <string>
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
using namespace std;
#define WIN32_LEAN_AND_MEAN 
#define CREATE_THREAD_ACCESS (PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ)

DWORD getTargetProcessID(const wchar_t* targetProcName)
{
    // PROCESSENTRY32 is used to open and get information about a running process..
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    // We use a th32snapprocess to iterate through all running processes.
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    // Success check oon the snapshot tool.
    if (!hSnap) {
        throw "Snapshot tool failed to open";
    }

    // If a first process exist (there are running processes), iterate through
    // all running processes.
    DWORD ProcID = NULL;
    if (Process32First(hSnap, &entry)) {
        do
        {
            // If the current process entry is the target process, store its ID.
            if (!wcscmp(entry.szExeFile, targetProcName))
            {
                ProcID = entry.th32ProcessID;
            }
        } while (Process32Next(hSnap, &entry) && !ProcID);        // Move on to the next running process.
    }
    else {
        // If there was no first process, notify the user.
        throw "No running processes found";
    }

    return ProcID;
}

bool inject(const wchar_t* targetProcName, const char* dllName)
{
    try
    {
        // Get the process id of the target process.
        DWORD targetProcID = getTargetProcessID(targetProcName);
        if (!targetProcID) {
            throw "Target process Was not found";
        }

        // Get a static address of the LoadLibrary function as a thread-start-routine function.
        LPTHREAD_START_ROUTINE funcLoadLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA");
        if (!funcLoadLibrary) {
            throw "Failed to retrieve a static function pointer to `LoadLbraryA`";
        }

        // Open the target process.
        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, targetProcID);
        if (hProcess == INVALID_HANDLE_VALUE) {
            throw "Failed to open target process";
        }

        // Virtually allocate memory for the path of the dll in the target process.
        LPVOID pDllPathAddr = VirtualAllocEx(hProcess, 0, strlen(dllName) + 1, MEM_COMMIT, PAGE_READWRITE);
        if (!pDllPathAddr) {
            throw "Failed to allocate memory in the target process";
        }

        // Write the dll path to the target process using WPM.
        WriteProcessMemory(hProcess, pDllPathAddr, (LPVOID)dllName, strlen(dllName) + 1, NULL);

        // Create a remote thread in the target process with LoadLibrary to load our dll into the target process.
        HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, NULL, funcLoadLibrary, pDllPathAddr, NULL, NULL);
        if (!hRemoteThread || hRemoteThread == INVALID_HANDLE_VALUE) {
            throw "Failed to load dll into target process";
        }

        // Wait until the remote thread is done loading the dll.
        WaitForSingleObject(hRemoteThread, INFINITE);
    }
    catch (const char* err) {
        std::cout << "An erro occurred: " << err << std::endl;
        return false;
    }

    return true;
}

/*
Function to retrieve the ID of a running process.
Input:
    targetProcName - The exe file name of the target process.
Output: The process's ID.
*/
#define TARGET_PROC L"Notepad.exe"
#define DLL_NAME "../Debug/injectdll.dll"
int main()
{

    bool success = inject(TARGET_PROC, "D:\\Magshimim\\Magshimim - Assembly\\Lesson17\\injectDLL\\x64\\Release\\mydll.dll");

    std::cout << "Did the injecition succeded? " << success << std::endl;

    return 0;
}