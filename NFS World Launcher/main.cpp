#include <includes.h>

Launcher *launcher = 0;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdLine)
{
	HANDLE hMutex = CreateMutex(NULL, FALSE, "Jestem Zajebisty");
	if (hMutex && GetLastError() != 0)
	{
		MessageBox(NULL, "Ta aplikacja jest ju¿ uruchomiona.",
			"Project1", MB_ICONEXCLAMATION);
		PostQuitMessage(0);
		return 0;
	}
	// Debug stuffs
#ifdef DEBUG
	AllocConsole();
	freopen("CONOUT$", "w", stdout);
	freopen("CONOUT$", "w", stderr);
	freopen("CONIN$", "r", stdin);
#endif
	Logger::Init("launcher.log");
	launcher = new Launcher;
	//Launcher::Initialize(hInstance);
	launcher->Initialize(hInstance);
	while (launcher->Pulse());
	//Launcher::remove();
	SAFE_DELETE(launcher);
	_CrtDumpMemoryLeaks();
	return 1;
}