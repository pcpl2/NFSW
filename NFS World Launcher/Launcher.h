#pragma once

struct BufferStruct
{
	char * buffer;
	size_t size;
};

struct Region
{
	int Id;
	char Name[20];
	char ShardName[20];
	char Url[50];
};

class Launcher
{
private:
	#define ID_Button1 501
	#define ID_Button2 502
	#define ID_Button3 503
	#define ID_Button4 504

	static HWND * s_hwWindow;

	static HWND * s_hwButtons;
	static HWND * s_hwEdit;
	static HWND * s_hwText;
	static HWND * s_hwCombo;

	static Region * R;

	static int region;

	static void InjectDll(HANDLE hProcess, const char * szDllPath);
public:

	static void Initialize(HINSTANCE hInstance);

	static bool Pulse();

	static int StartGame(char *login, char *password, char *url, char *region);

	static void getshardinfo();

	static LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI RegionProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	static LRESULT WINAPI OptionsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);


//	static HWND GetWindow() { return s_hwWindow; }
};

