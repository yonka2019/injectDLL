#include "windows.h"
extern "C" __declspec(dllimport) void mess();
// Other option would be-
// #include "mydll.h"
int main()
{
	mess();
	return 0;
}