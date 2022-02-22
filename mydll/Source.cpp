#include "windows.h"
extern "C" __declspec(dllimport) void showMessageBox();
// Other option would be-
// #include "mydll.h"
int main()
{
	showMessageBox();
	return 0;
}