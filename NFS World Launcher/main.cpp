#include <includes.h>

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
	Launcher::Initialize(hInstance);
	while (Launcher::Pulse());
	Launcher::remove();
	_CrtDumpMemoryLeaks();
	return 1;
}