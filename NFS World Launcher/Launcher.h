#pragma once

enum Language{
	English,
	Germany,
	Spanish,
	French,
	Polish,
	Russian,
	Portuguese,
	Thai,
	Turkish,
	Chinese,
	Chinese_Simplified,
}; 

class Launcher
{
private:
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

	#define ID_Button1 501
	#define ID_Button2 502
	#define ID_Button3 503
	#define ID_Button4 504

	static HWND *Window;

	static HWND *Button;
	static HWND *Edit;
	static HWND *Text;
	static HWND *Combo;

	static Region *R;
	static User *Logged;

	static int *CheckBox;

	static int region;

	static char *GameDirRegistryKeyPath;
	static char *GameDirRegistryKeyName;
	static char *TermsOfService;
	static char GameDir[MAX_PATH];
	static char *GameUrl;

	static HKEY hKey;
public:

	static void Initialize(HINSTANCE hInstance);

	static void remove();

	static bool Pulse();

	static bool SignIn(char *login, char *password, char *server, char *region);

	static int StartGame(char *login, char *password, char *url, char *region);

	static bool GetGameDirFromRegistry();

	static void getshardinfo();
	static void launcherinfo();

	inline static char *GetGameDir() { return GameDir; };

	static LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OptionsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

