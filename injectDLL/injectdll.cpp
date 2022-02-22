#include "windows.h"
#define DLL_EXPORT
#include "injectdll.h"
#include "stdio.h"

extern "C"
{
	DECLDIR void mess() {
		MessageBoxA(NULL, "HELLO THERE", "From Notepad", NULL);
	}
}
BOOL APIENTRY DllMain(HANDLE hModule, // Handle to DLL module
	DWORD ul_reason_for_call,
	LPVOID lpReserved) // Reserved
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// A process is loading the DLL.
		mess();
		break;
	case DLL_THREAD_ATTACH:
		// A process is creating a new thread.
		mess();
		break;
	case DLL_THREAD_DETACH:
		// A thread exits normally.
		mess();
		break;
	case DLL_PROCESS_DETACH:
		mess();
		// A process unloads the DLL.
		break;
	}
	return TRUE;
}