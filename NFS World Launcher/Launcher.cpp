#include <includes.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

int nFreeInternetId = 0;

HWND * Launcher::s_hwWindow = new HWND[1];
HWND * Launcher::s_hwButtons = new HWND[4];
HWND * Launcher::s_hwEdit = new HWND[1];
HWND * Launcher::s_hwText = new HWND[5];
HWND * Launcher::s_hwCombo = new HWND[0];

int Launcher::region;

char sz_Login[128] = { 0 };
char sz_Password[128] = { 0 };
char szGameExePath[MAX_PATH] = { 0 }, szGamePath[MAX_PATH] = { 0 }, arg[MAX_PATH] = { 0 };

Region * Launcher::R = new Region[4];

void Launcher::Initialize(HINSTANCE hInstance)
{
	HFONT hFont = CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));

	WNDCLASSEX wc;

	getshardinfo();

	memset(&wc, 0, sizeof(WNDCLASSEX));

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION); //static_cast<HICON>(LoadImage(hInstance,MAKEINTRESOURCE(IDI_ANATWND),IMAGE_ICON,32,32,LR_DEFAULTSIZE));
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = "NFSWL_main";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	RegisterClassEx(&wc);

	wc.lpfnWndProc = RegionProc;
	wc.lpszClassName = "NFSWL_region";
	RegisterClassEx(&wc);

	//_______________________________________________________________________________ main window

	s_hwWindow[0] = CreateWindowEx(WS_EX_CLIENTEDGE, "NFSWL_main", "Need For speed World Launcher c++", WS_SYSMENU | WS_MINIMIZEBOX,CW_USEDEFAULT, CW_USEDEFAULT, 800, 300, NULL, NULL, hInstance, NULL);
	s_hwButtons[0] = CreateWindowEx(NULL, WC_BUTTON, "Login and Start", WS_CHILD | WS_VISIBLE, 600, 200, 130, 30, s_hwWindow[0], (HMENU)ID_Button1, hInstance, NULL);
	s_hwButtons[1] = CreateWindowEx(NULL, WC_BUTTON, "Settings", WS_CHILD | WS_VISIBLE, 660, 20, 100, 30, s_hwWindow[0], (HMENU)ID_Button2, hInstance, NULL);
	strcpy(sz_Login, "Email");
	strcpy(sz_Password, "Password");
	s_hwText[0] = CreateWindowEx(NULL, WC_STATIC, "Email :", WS_VISIBLE | WS_CHILD | SS_SIMPLE, 20, 35, 80, 25, s_hwWindow[0], NULL, hInstance, NULL);
	s_hwEdit[0] = CreateWindowEx(NULL, WC_EDIT, sz_Login, WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 50, 200, 25, s_hwWindow[0], NULL, hInstance, NULL);
	s_hwText[1] = CreateWindowEx(NULL, WC_STATIC, "Password :", WS_VISIBLE | WS_CHILD | SS_SIMPLE, 250, 35, 80, 25, s_hwWindow[0], 0, hInstance, 0);
	s_hwEdit[1] = CreateWindowEx(NULL, WC_EDIT, sz_Password, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD, 250, 50, 200, 25, s_hwWindow[0], NULL, hInstance, NULL);



	//_______________________________________________________________________________ select region

	s_hwWindow[1] = CreateWindowEx(WS_EX_CLIENTEDGE, "NFSWL_region", "Need For speed World Launcher c++", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 400, 250, NULL, NULL, hInstance, NULL);
	s_hwCombo[0] = CreateWindowEx(NULL, WC_COMBOBOX, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN, 110, 80, 150, 200, s_hwWindow[1], NULL, hInstance, NULL);
	s_hwButtons[2] = CreateWindowEx(NULL, WC_BUTTON, "Select", WS_CHILD | WS_VISIBLE, 260, 150, 100, 30, s_hwWindow[1], (HMENU)ID_Button3, hInstance, NULL);
	s_hwButtons[3] = CreateWindowEx(NULL, WC_BUTTON, "Exit", WS_CHILD | WS_VISIBLE, 20, 150, 100, 30, s_hwWindow[1], (HMENU)ID_Button4, hInstance, NULL);

	for (int i = 0; i < sizeof(R); i++)
	{
		
		char buffer[128];
		Debug("%s - %s", R[i].ShardName, R[i].Name);
		sprintf(buffer, "%s - %s", R[i].ShardName, R[i].Name);

		SendMessage(s_hwCombo[0], CB_ADDSTRING, 0, (LPARAM)buffer);
	}

	SendMessage(s_hwCombo[0], CB_SETCURSEL, (WPARAM)0, 0);
	


	for (int i = 0; i < sizeof(s_hwButtons); i++)
	{
		if (s_hwButtons[i] == NULL)
			continue;

		SendMessage(s_hwButtons[i], WM_SETFONT, WPARAM(hFont), TRUE);

		if (s_hwText[i] == NULL)
			continue;

		SendMessage(s_hwText[i], WM_SETFONT, WPARAM(hFont), TRUE);

		if (s_hwEdit[i] == NULL) 
			continue;

		SendMessage(s_hwEdit[i], WM_SETFONT, WPARAM(hFont), TRUE);

		if (s_hwCombo[i] == NULL)
			continue;

		SendMessage(s_hwCombo[i], WM_SETFONT, WPARAM(hFont), TRUE);
	}

	ShowWindow(s_hwWindow[1], SW_SHOWNORMAL);
	
}

bool Launcher::Pulse()
{
	MSG msg;

	while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	if (msg.message == WM_QUIT)
		return false;

	Sleep(5);

	return true;
}

int Launcher::StartGame(char *login, char *password, char *server, char *region)
{
	HKEY hKey = 0;
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, "SOFTWARE\\Electronic Arts\\Need For Speed World", NULL, KEY_READ, &hKey) != ERROR_SUCCESS)
	{
		MessageBox(NULL, "Cannot find game executable file! #001", "NFSW - Error", MB_ICONERROR);
		return E_FAIL;
	}

	unsigned int uiType = REG_SZ;
	unsigned int uiSize = MAX_PATH;
	if (RegQueryValueEx(hKey, "GameInstallDir", NULL, (LPDWORD)&uiType, (BYTE *)szGamePath, (LPDWORD)&uiSize) != ERROR_SUCCESS)
	{
		MessageBox(NULL, "Cannot find game executable file! #002", "NFSW - Error", MB_ICONERROR);
		return E_FAIL;
	}


	CURL *curl;
	CURLcode res;
	struct curl_slist *headers = NULL;
	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();
	char postthis[256];
	char url[69];

	sprintf(postthis, "<Credentials xmlns=\"http://schemas.datacontract.org/2004/07/Victory.DataLayer.Serialization\"><Email>%s</Email><Password>%s</Password><Region>%s</Region></Credentials>", login, password, region);
	sprintf(url, "%s/User/AuthenticateUser2", server);
	BufferStruct output;
	output.buffer = NULL;
	output.size = 0;
	if (curl)
	{

		headers = curl_slist_append(headers, "Content-Type: text/xml");
		headers = curl_slist_append(headers, "charset=utf-8");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);

		curl_easy_setopt(curl, CURLOPT_URL, url);
		Info("Connect to %s", url);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)-1);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Utils::WriteMemoryCallback);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&output);

		Debug("2");
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		Debug("3");
		/* Check for errors */
		if (res != CURLE_OK)
			Error("curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));


		/* always cleanup */
		curl_easy_cleanup(curl);
	}

	while (output.buffer == 0)
	{
		Sleep(100);
	}
	Sleep(50);

	Debug("%s", output.buffer);

	tinyxml2::XMLDocument doc;
	doc.Parse(output.buffer);
	tinyxml2::XMLElement* securityTokenElement = doc.FirstChildElement("User")->FirstChildElement("securityToken");
	tinyxml2::XMLElement* userIdElement = doc.FirstChildElement("User")->FirstChildElement("userId");


	strcat(szGamePath, "\\Data\\");
	sprintf(szGameExePath, "%snfsw.exe", szGamePath);
	sprintf(arg, "\"%s\" %s %s %s %s", szGameExePath, region, server, securityTokenElement->GetText(), userIdElement->GetText());

	// Create game process
	STARTUPINFO siStartupInfo = { 0 };
	PROCESS_INFORMATION piProcessInformation = { 0 };
	siStartupInfo.cb = sizeof(siStartupInfo);

	if (!CreateProcess(szGameExePath, arg, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, szGamePath, &siStartupInfo, &piProcessInformation))
	{
		MessageBox(NULL, "Cannot create game process.", "NFSW - Error", MB_ICONERROR);
		return E_FAIL;
	}

/*	// Inject dll
	char szPath[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szPath, MAX_PATH);
	for (unsigned int i = strlen(szPath); i > 0; i--)
	{
		if (szPath[i] == '\\')
		{
			szPath[i] = '\0';
			break;
		}
	}
#ifdef DEBUG
	strcat(szPath, "\\core_d.dll");
#else
	strcat(szPath, "\\core.dll");
#endif
	InjectDll(piProcessInformation.hProcess, szPath);*/

	// Restore process thread
	ResumeThread(piProcessInformation.hThread);

	return S_OK;
}

void Launcher::getshardinfo()
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();

	struct BufferStruct output;
	output.buffer = NULL;
	output.size = 0;
	if (curl)
	{

		curl_easy_setopt(curl, CURLOPT_URL, "https://94.236.124.241/nfsw/Engine.svc/getshardinfo");
		Info("Connect to https://94.236.124.241/nfsw/Engine.svc/getshardinfo");

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Utils::WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&output);

		Debug("2");
		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		Debug("3");
		/* Check for errors */
		if (res != CURLE_OK)
			Error("curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));


		/* always cleanup */
		curl_easy_cleanup(curl);
	}

	while (output.buffer == 0)
	{
		Sleep(100);
	}
	Sleep(50);

	Debug("%s", output.buffer);

	tinyxml2::XMLDocument doc;
	doc.Parse(output.buffer);
	tinyxml2::XMLElement* ShardInfo = doc.FirstChildElement("ArrayOfShardInfo")->FirstChildElement("ShardInfo");

	int i = 0;
	for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
	{

		Debug("%s", ShardInfo->FirstChildElement("RegionId")->GetText());
		Debug("%s", ShardInfo->FirstChildElement("RegionName")->GetText());
		Debug("%s", ShardInfo->FirstChildElement("ShardName")->GetText());
		Debug("%s", ShardInfo->FirstChildElement("Url")->GetText());
		
		R[i].Id = atoi(ShardInfo->FirstChildElement("RegionId")->GetText());
		memcpy(R[i].Name, ShardInfo->FirstChildElement("RegionName")->GetText(), 20);
		memcpy(R[i].ShardName, ShardInfo->FirstChildElement("ShardName")->GetText(), 20);
		memcpy(R[i].Url, ShardInfo->FirstChildElement("Url")->GetText(), 50);

		Debug("in struct[%d] : %d", i, R[i].Id);
		Debug("in struct[%d] : %s", i, R[i].Name);
		Debug("in struct[%d] : %s", i, R[i].ShardName);
		Debug("in struct[%d] : %s", i, R[i].Url);

		i++;
	}

	Debug("in struct[%d] : %d", 3, R[3].Id);
	Debug("in struct[%d] : %s", 3, R[3].Name);
	Debug("in struct[%d] : %s", 3, R[3].ShardName);
	Debug("in struct[%d] : %s", 3, R[3].Url);
}

void Launcher::InjectDll(HANDLE hProcess, const char * szDllPath) //TODO: Error handling
{
	size_t sLibPathLen = (strlen(szDllPath) + 1);
	SIZE_T bytesWritten = 0;

	void * pRmtLibraryPath = VirtualAllocEx(hProcess, NULL, sLibPathLen, MEM_COMMIT, PAGE_READWRITE);
	WriteProcessMemory(hProcess, pRmtLibraryPath, (void*)szDllPath, sLibPathLen, &bytesWritten);
	HMODULE hKernel32 = GetModuleHandle("Kernel32");
	FARPROC pfnLoadLibraryA = GetProcAddress(hKernel32, "LoadLibraryA");
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pfnLoadLibraryA, pRmtLibraryPath, 0, NULL);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	VirtualFreeEx(hProcess, pRmtLibraryPath, sizeof(pRmtLibraryPath), MEM_RELEASE);
}

LRESULT CALLBACK Launcher::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
		break;
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CTLCOLORSTATIC:
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case ID_Button1:
			GetWindowText(s_hwEdit[0], sz_Login, sizeof(sz_Login));
			GetWindowText(s_hwEdit[1], sz_Password, sizeof(sz_Password));
			Launcher::StartGame(sz_Login, sz_Password, R[region].Url, R[region].Name);
			//DestroyWindow(hwnd);
			break;
		}

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK Launcher::RegionProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_CTLCOLORSTATIC:
		break;

	case WM_COMMAND:
		switch (wParam)
		{
		case ID_Button3:
			region = SendMessage(s_hwCombo[0], CB_GETCURSEL, 0, 0);
			ShowWindow(hwnd, SW_HIDE);
			ShowWindow(s_hwWindow[0], SW_SHOW);
			break;
		case ID_Button4:
			DestroyWindow(hwnd);
			break;
		} break;


		return 0;

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

LRESULT CALLBACK Launcher::OptionsProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
	{
		ShowWindow(hwnd, SW_HIDE);
		return 0;
	} break;

	case WM_CTLCOLORSTATIC:
		break;

	case WM_COMMAND:
		switch (wParam)
		{

		} break;
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

