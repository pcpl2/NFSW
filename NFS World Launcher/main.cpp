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
	Logger::Init("launcher.log");
	Launcher::Initialize(hInstance);
	while (Launcher::Pulse());
	return 1;
}