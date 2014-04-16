#include <includes.h>

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

HWND *Launcher::Window = new HWND[1];
HWND *Launcher::Button = new HWND[8];
HWND *Launcher::Edit = new HWND[1];
HWND *Launcher::Text = new HWND[5];
HWND *Launcher::Combo = new HWND[2];

int Launcher::region;

int *Launcher::CheckBox = new int[5];

char Login[128] = { 0 };
char Password[128] = { 0 };

Launcher::Region *Launcher::R = new Region[4];
Launcher::User *Launcher::Logged = new User[0];

HKEY Launcher::hKey = 0;

/*
		Links
		https://94.236.124.241/nfsw/Engine.svc/systeminfo
		https://94.236.124.241/nfsw/Engine.svc/launcherinfo
		http://world.needforspeed.com/SpeedAPI/ws/game/nfsw/server/status?locale=EU&shard=apex ???

*/

char *Launcher::GameDirRegistryKeyPath = "SOFTWARE\\Electronic Arts\\Need For Speed World";
char *Launcher::GameDirRegistryKeyName = "GameInstallDir";
char *Launcher::TermsOfService = "http://cdn.world.needforspeed.com/static/world/euala.txt"; //GetUserDefaultLangID _de ; _es ; _fr ; _pl ; _pt ; _ru ; _th ; _tr ; _zh ;  _zh_chs
char Launcher::GameDir[MAX_PATH] = { 0 };
char *Launcher::GameUrl = (char *)malloc(256);

void Launcher::Initialize(HINSTANCE hInstance)
{
	HFONT hFont = CreateFont(15, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, TEXT("Tahoma"));
	char *LanguageText[] = { "English", "Germany", "Spanish", "French", "Polish", "Russian", "Portuguese", "Thai", "Turkish", "Chinese", "Chinese_Simplified" };

	WNDCLASSEX wc;

	curl_global_init(CURL_GLOBAL_DEFAULT);

	/*if (!GetGameDirFromRegistry())
	{

	}*/
	
	/* 
	de 0x0C07(3079) Germany;
	es 0x2C0A(11274) Spanish; 
	fr 0x080c(2060) French; 
	pl 0x0415(1045) Polish;
	pt 0x0416(1046) Portuguese;
	ru 0x0419(1049) Russian;
	th 0x041E(1054) Thai;
	tr 0x041F(1055) Turkish; 
	zh 0x0C04(3076) Chinese;
	zh_chs 0x0004(4) Chinese Simplified;
	*/

	LANGID lang = GetUserDefaultUILanguage(); 

	GetGameDirFromRegistry();

	getshardinfo();
	launcherinfo();

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

	Window[0] = CreateWindowEx(WS_EX_CLIENTEDGE, "NFSWL_main", "Need For speed World Launcher c++", WS_SYSMENU | WS_MINIMIZEBOX,CW_USEDEFAULT, CW_USEDEFAULT, 800, 300, NULL, NULL, hInstance, NULL);
	Button[0] = CreateWindowEx(NULL, WC_BUTTON, "Login", WS_CHILD | WS_VISIBLE, 600, 200, 130, 30, Window[0], (HMENU)ID_Button1, hInstance, NULL);
	Button[1] = CreateWindowEx(NULL, WC_BUTTON, "Settings", WS_CHILD | WS_VISIBLE, 660, 20, 100, 30, Window[0], (HMENU)ID_Button2, hInstance, NULL);
	strcpy(Login, "Email");
	strcpy(Password, "Password");
	Text[0] = CreateWindowEx(NULL, WC_STATIC, "Email :", WS_VISIBLE | WS_CHILD | SS_SIMPLE, 20, 85, 80, 25, Window[0], NULL, hInstance, NULL);
	Edit[0] = CreateWindowEx(NULL, WC_EDIT, Login, WS_CHILD | WS_VISIBLE | WS_BORDER, 20, 100, 200, 25, Window[0], NULL, hInstance, NULL);
	Text[1] = CreateWindowEx(NULL, WC_STATIC, "Password :", WS_VISIBLE | WS_CHILD | SS_SIMPLE, 250, 85, 80, 25, Window[0], NULL, hInstance, NULL);
	Edit[1] = CreateWindowEx(NULL, WC_EDIT, Password, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_PASSWORD, 250, 100, 200, 25, Window[0], NULL, hInstance, NULL);
	Combo[0] = CreateWindowEx(NULL, WC_COMBOBOX, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN, 20, 20, 150, 200, Window[0], NULL, hInstance, NULL);

	Button[2] = CreateWindowEx(NULL, WC_BUTTON, "Start Game", WS_CHILD | WS_VISIBLE, 600, 200, 130, 30, Window[0], (HMENU)ID_Button3, hInstance, NULL);
	ShowWindow(Button[2], SW_HIDE);

	wc.lpfnWndProc = OptionsProc;
	wc.lpszClassName = "NFSWL_Options";
	RegisterClassEx(&wc);

	Window[1] = CreateWindowEx(WS_EX_CLIENTEDGE, "NFSWL_Options", "Options", WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, 400, 300, NULL, NULL, hInstance, NULL);
	Text[2] = CreateWindowEx(NULL, WC_STATIC, "Select a language :", WS_VISIBLE | WS_CHILD | SS_SIMPLE, 90, 20, 115, 25, Window[1], NULL, hInstance, NULL);
	Combo[1] = CreateWindowEx(NULL, WC_COMBOBOX, NULL, WS_CHILD | WS_VISIBLE | WS_BORDER | CBS_DROPDOWN, 90, 40, 200, 100, Window[1], NULL, hInstance, NULL);
	Button[3] = CreateWindowEx(NULL, WC_BUTTON, "Full Download", WS_CHILD | WS_VISIBLE | BS_CHECKBOX, 90, 70, 150, 30, Window[1], NULL, hInstance, NULL);
	Button[4] = CreateWindowEx(NULL, WC_BUTTON, "Save & close", WS_CHILD | WS_VISIBLE, 120, 220, 150, 30, Window[1], (HMENU)ID_Button4, hInstance, NULL);

	for (int i = 0; i < sizeof(R); i++)
	{
		char buffer[128];
		Debug("%s - %s", R[i].ShardName, R[i].Name);
		sprintf(buffer, "%s - %s", R[i].ShardName, R[i].Name);
		SendMessage(Combo[0], CB_ADDSTRING, 0, (LPARAM)buffer);
	}

	SendMessage(Combo[0], CB_SETCURSEL, (WPARAM)0, 0);

	for (int i = 0; i < 11; i++)
	{
		SendMessage(Combo[1], CB_ADDSTRING, 0, (LPARAM)LanguageText[i]);
	}

	SendMessage(Combo[1], CB_SETCURSEL, (WPARAM)0, 0);
	
	for (int i = 0; i < sizeof(Button); i++)
	{
		if (Button[i] == NULL)
			continue;

		SendMessage(Button[i], WM_SETFONT, WPARAM(hFont), TRUE);

		if (Text[i] == NULL)
			continue;

		SendMessage(Text[i], WM_SETFONT, WPARAM(hFont), TRUE);

		if (Edit[i] == NULL) 
			continue;

		SendMessage(Edit[i], WM_SETFONT, WPARAM(hFont), TRUE);

		if (Combo[i] == NULL)
			continue;

		SendMessage(Combo[i], WM_SETFONT, WPARAM(hFont), TRUE);
	}

	ShowWindow(Window[0], SW_SHOWNORMAL);
	
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

bool Launcher::SignIn(char *login, char *password, char *server, char *region)
{
	CURL *curl;
	CURLcode res;
	curl_slist *headers = NULL;
	curl = curl_easy_init();
	char postthis[256];
	char url[69];

	sprintf(postthis, "<Credentials xmlns=\"http://schemas.datacontract.org/2004/07/Victory.DataLayer.Serialization\"><Email>%s</Email><Password>%s</Password><Region>%s</Region></Credentials>", login, password, region);
	sprintf(url, "%s/User/AuthenticateUser2", server);
	Utils::BufferStruct output;
	output.buffer = NULL;
	output.size = 0;
	if (curl)
	{

		headers = curl_slist_append(headers, "Content-Type: text/xml");
		headers = curl_slist_append(headers, "charset=utf-8");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);

		curl_easy_setopt(curl, CURLOPT_URL, url);
		Debug("Connect to %s", url);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, (curl_off_t)-1);

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Utils::WriteMemoryCallback);

		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&output);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
		/* Check for errors */
		if (res != CURLE_OK)
		{
			Error("curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
			return false;
		}

		/* always cleanup */
		curl_easy_cleanup(curl);
	}
	else
	{
		MessageBox(NULL, "Cannot create curl", "NFSW - Error", MB_ICONERROR);
		return false;
	}

	while (output.buffer == 0)
	{
		Sleep(100);
	}
	Sleep(50);

	tinyxml2::XMLDocument doc;
	doc.Parse(output.buffer);
	//if ()
	//doc.FirstChildElement("EngineExceptionTrans")->FirstChildElement("ErrorCode")->ToText();
	memcpy(Logged[0].remoteUserId, doc.FirstChildElement("User")->FirstChildElement("remoteUserId")->GetText(), 12);
	memcpy(Logged[0].securityToken, doc.FirstChildElement("User")->FirstChildElement("securityToken")->GetText(), 50);
	memcpy(Logged[0].userId, doc.FirstChildElement("User")->FirstChildElement("userId")->GetText(), 10);

	free(output.buffer);
	return true;
}

int Launcher::StartGame(char *SecurityToken, char *UserId, char *Server, char *Region)
{
	char GameExePath[MAX_PATH] = { 0 }, GameDataPath[MAX_PATH] = { 0 }, Arg[MAX_PATH] = { 0 };
	sprintf(GameDataPath, "%s\\Data\\", Launcher::GameDir);
	sprintf(GameExePath, "%snfsw.exe", GameDataPath);
	sprintf(Arg, "\"%s\" %s %s %s %s", GameExePath, Region, Server, SecurityToken, UserId);

	// Create game process
	STARTUPINFO SiStartupInfo = { 0 };
	PROCESS_INFORMATION PiProcessInformation = { 0 };
	SiStartupInfo.cb = sizeof(SiStartupInfo);

	if (!CreateProcess(GameExePath, Arg, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, GameDataPath, &SiStartupInfo, &PiProcessInformation))
	{
		MessageBox(NULL, "Cannot create game process.", "NFSW - Error", MB_ICONERROR);
		return E_FAIL;
	}

	ResumeThread(PiProcessInformation.hThread);

	return S_OK;
}

void Launcher::getshardinfo()
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();

	Utils::BufferStruct output;
	output.buffer = NULL;
	output.size = 0;
	if (curl)
	{

		curl_easy_setopt(curl, CURLOPT_URL, "https://94.236.124.241/nfsw/Engine.svc/getshardinfo");
		Debug("Connect to https://94.236.124.241/nfsw/Engine.svc/getshardinfo");

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Utils::WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&output);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
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

	tinyxml2::XMLDocument doc;
	doc.Parse(output.buffer);
	free(output.buffer);
	tinyxml2::XMLElement *ShardInfo = doc.FirstChildElement("ArrayOfShardInfo")->FirstChildElement("ShardInfo");

	for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
	{
		int i = atoi(ShardInfo->FirstChildElement("RegionId")->GetText()) - 1;	
		R[i].Id = atoi(ShardInfo->FirstChildElement("RegionId")->GetText());
		memcpy(R[i].Name, ShardInfo->FirstChildElement("RegionName")->GetText(), 20);
		memcpy(R[i].ShardName, ShardInfo->FirstChildElement("ShardName")->GetText(), 20);
		memcpy(R[i].Url, ShardInfo->FirstChildElement("Url")->GetText(), 50);
	}
}

void Launcher::launcherinfo()
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();

	Utils::BufferStruct output;
	output.buffer = NULL;
	output.size = 0;
	if (curl)
	{

		curl_easy_setopt(curl, CURLOPT_URL, "https://94.236.124.241/nfsw/Engine.svc/launcherinfo");
		Debug("Connect to https://94.236.124.241/nfsw/Engine.svc/launcherinfo");

		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Utils::WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&output);

		/* Perform the request, res will get the return code */
		res = curl_easy_perform(curl);
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

	tinyxml2::XMLDocument doc;
	doc.Parse(output.buffer);
	free(output.buffer);
	tinyxml2::XMLElement *gameurl = doc.FirstChildElement("configuration")->FirstChildElement("cdn")->FirstChildElement("game");
	strcpy(Launcher::GameUrl, gameurl->GetText());
}

bool Launcher::GetGameDirFromRegistry()
{
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, GameDirRegistryKeyPath, NULL, KEY_READ, &Launcher::hKey) != ERROR_SUCCESS)
	{
		return false;
	}

	unsigned int uiType = REG_SZ;
	unsigned int uiSize = MAX_PATH;
	if (RegQueryValueEx(Launcher::hKey, GameDirRegistryKeyName, NULL, (LPDWORD)&uiType, (BYTE *)Launcher::GameDir, (LPDWORD)&uiSize) != ERROR_SUCCESS)
	{
		return false;
	}
	return true;
}

LRESULT CALLBACK Launcher::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Downloader D;
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
			EnableWindow(Combo[0], false);
			region = SendMessage(Combo[0], CB_GETCURSEL, 0, 0);
			GetWindowText(Edit[0], Login, sizeof(Login));
			GetWindowText(Edit[1], Password, sizeof(Password));
			if (Launcher::SignIn(Login, Password, R[region].Url, R[region].Name))
			{
				ShowWindow(Combo[0], SW_HIDE);
				ShowWindow(Edit[0], SW_HIDE);
				ShowWindow(Edit[1], SW_HIDE);
				ShowWindow(Text[1], SW_HIDE);
				ShowWindow(Text[0], SW_HIDE);
				ShowWindow(Button[0], SW_HIDE);
				ShowWindow(Button[1], SW_HIDE);
				ShowWindow(Button[2], SW_SHOW);
	//			D.StartVerificationAndDownload(true, "en", GameUrl);
			}
			break;
		case ID_Button2:
			D.StartVerificationAndDownload(true, "en", GameUrl); //debug
	//		ShowWindow(Window[1], SW_SHOW);
			break;
		case ID_Button3:
			Launcher::StartGame(Logged[0].securityToken, Logged[0].userId, R[region].Url, R[region].Name);
			DestroyWindow(hwnd);
			break;
		}

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
		/*switch (wParam)
		{

		} break;*/
		return 0;
	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return 0;
}

void Launcher::remove()
{
	delete[] R;
	delete[] Logged;
	free(GameUrl);
	DeleteObject(Window);
	/*HWND * Launcher::Window = new HWND[1];
HWND * Launcher::Button = new HWND[4];
HWND * Launcher::Edit = new HWND[1];
HWND * Launcher::Text = new HWND[5];
HWND * Launcher::Combo = new HWND[0];*/
}

/*
public void SetRegion(string serverUrl, string userId, string securityToken, int regionId)
{
string command = string.Format("/User/SetRegion?userId={0}&regionId={1}", (object) userId, (object) regionId);
string[][] extraHeaders = new string[4][]
{
new string[2]
{
"Content-Type",
"text/xml;charset=utf-8"
},
new string[2]
{
"Content-Length",
"0"
},
new string[2]
{
"userId",
userId
},
new string[2]
{
"securityToken",
securityToken
}
};
this.DoCall(serverUrl, command, (string[]) null, extraHeaders, RequestMethod.POST);
}
}



private void LoadWebStore(string remoteUserId, string webAuthKey)
{
this.remoteUserId = remoteUserId;
this.webAuthKey = webAuthKey;
if (!this.parentForm.PortalUp)
return;
string str = ShardManager.ShardUrl;
string[] strArray = str.Split(new char[1]
{
'/'
});
if (strArray.Length > 3)
str = strArray[2] + "/" + strArray[3];
this.wWebBrowser.DocumentCompleted += new WebBrowserDocumentCompletedEventHandler(this.HandleWebBrowserDocumentLoaded);
this.wWebBrowser.Navigate(new Uri(this.parentForm.PortalDomain + "/webkit/lp/store?locale=" + CultureInfo.CurrentCulture.Name.Replace('-', '_')), "", (byte[]) null, string.Format("userId: {0}{4}token: {1}{4}shard: {2}{4}worldserverurl: {3}{4}User-Agent: {5}", (object) remoteUserId, (object) webAuthKey, (object) ShardManager.ShardName, (object) str, (object) Environment.NewLine, (object) "Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 6.1; WOW64; Trident/4.0; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; InfoPath.2; MS-RTC LM 8; .NET4.0C; .NET4.0E)"));
this.wWebBrowser.Visible = true;
remoteUserId = string.Empty;
webAuthKey = string.Empty;
}


private string GetWebSecurityToken()
{
string xml = new WebServicesWrapper().DoCall(ShardManager.ShardUrl, "/security/generatewebtoken", (string[]) null, new string[4][]
{
new string[2]
{
"Content-Type",
"text/xml;charset=utf-8"
},
new string[2]
{
"Content-Length",
"0"
},
new string[2]
{
"userId",
this.mUserId
},
new string[2]
{
"securityToken",
this.mAuthKey
}
}, (string) null, RequestMethod.POST);
XmlDocument xmlDocument = new XmlDocument();
xmlDocument.LoadXml(xml);
return xmlDocument.InnerText;
}
*/