/*#define ID_Button1 501
#define ID_Button2 502

// Get game directory
char szGameExePath[MAX_PATH] = { 0 }, szGamePath[MAX_PATH] = { 0 }, arg[MAX_PATH] = { 0 };
void InjectDll(HANDLE hProcess, const char * szDllPath);
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
void getshardinfo();

MSG Komunikat;
LPSTR classname = "class name";

HFONT hFont = CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
	OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
	DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));

HWND * s_hwButtons = new HWND[3];
HWND * s_hwEdit = new HWND[1];
HWND * s_hwText = new HWND[5];

char sz_Login[128] = { 0 };
char sz_Password[128] = { 0 };*/