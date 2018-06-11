#include "SYSTEM.h"
#include <io.h>
#include <fcntl.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	AllocConsole();
	HANDLE OsFileHandle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int iFileDescriptor = _open_osfhandle((long)OsFileHandle_out, _O_TEXT);
	FILE* hf_out = _fdopen(iFileDescriptor, "w");
	setvbuf(hf_out, NULL, _IONBF, 0);
	*stdout = *hf_out;

	HANDLE OsFileHandle_in = GetStdHandle(STD_INPUT_HANDLE);
	iFileDescriptor = _open_osfhandle((long)OsFileHandle_in, _O_TEXT);
	FILE* hf_in = _fdopen(iFileDescriptor, "r");
	setvbuf(hf_in, NULL, _IONBF, 0);
	*stdin = *hf_in;

	System* pGame = new System();
	if(!pGame)
		return 0;

	if(!pGame->Init())
		return 0;

	pGame->MainLoop();
	pGame->Shutdown();
	delete pGame;
	pGame = NULL;

	return 0;
}