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

struct User
{
	char remoteUserId[12];
	char securityToken[50];
	char userId[10];
};

class Launcher
{
private:
	#define ID_Button1 501
	#define ID_Button2 502
	#define ID_Button3 503
	#define ID_Button4 504

	static HWND * Window;

	static HWND * Buttons;
	static HWND * Edit;
	static HWND * Text;
	static HWND * Combo;

	static Region * R;
	static User * Logged;

	static int region;

	static char *GameDirRegistryKeyPath;
	static char *GameDirRegistryKeyName;
	static char *TermsOfService;
	static char GameDir[MAX_PATH];

	static HKEY hKey;
public:

	static void Initialize(HINSTANCE hInstance);

	static bool Pulse();

	static bool Login(char *login, char *password, char *server, char *region);

	static int StartGame(char *login, char *password, char *url, char *region);

	static bool GetGameDirFromRegistry();

	static void getshardinfo();

	inline static char* GetGameDir() { return GameDir; };

	static LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI DownloadProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OptionsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

