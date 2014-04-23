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
	static Launcher *instance;

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
	static HWND *ProgressBar;

	static HKEY hKey;

	static Region *R;
	static User *Logged;

	static int *CheckBox;

	static char *GameDirRegistryKeyPath;
	static char *GameDirRegistryKeyName;
	static char *TermsOfService;
	static char GameDir[MAX_PATH];
	static char *GameUrl;

	static bool SevenOrNewer;
public:
	Launcher();
	~Launcher();

	static void Initialize(HINSTANCE hInstance);

	static Launcher& Get();

	static void remove();

	static bool Pulse();

	static bool SignIn(char *login, char *password, char *server, char *region);
	static int StartGame(char *SecurityToken, char *UserId, char *Server, char *Region);

	static bool GetGameDirFromRegistry();

	static void getshardinfo();
	static void launcherinfo();

	inline static char *GetGameDir() { return GameDir; };

	inline static void SetMarqueeProgressBar(int ProgressBar, bool a) { SendMessage(Launcher::ProgressBar[ProgressBar], PBM_SETMARQUEE, a ? 1 : 0, 0); }
	inline static void SetRangeProgressBar(int ProgressBar, int From, int To) { SendMessage(Launcher::ProgressBar[ProgressBar], PBM_SETRANGE, 0, (LPARAM)MAKELONG(From, To)); }
	inline static void SetPosProgressBar(int ProgressBar, int Pos) { SendMessage(Launcher::ProgressBar[ProgressBar], PBM_SETPOS, (WPARAM)Pos, 0); }
	inline static void StartGameEnable(){ EnableWindow(Launcher::Button[2], true); }

	static LRESULT WINAPI WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI OptionsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

