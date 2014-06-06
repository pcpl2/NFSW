#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#include <Windows.h>
#include <stdio.h>
#include <curl\curl.h>
#include <tinyxml2.h>
#include <Logger.h>
#include <iostream>
#include <string.h>
#include <commctrl.h>
#include <time.h> 
#include <Shobjidl.h>
#include <vector>

#include <LzmaLib.h>

#include <hex.h>
#include <base64.h>
#include <md5.h>

#include <Utils.h>
#include <Downloader.h>
#include <Launcher.h>
#ifdef DEBUG
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif
#define SAFE_DELETE(d) if(d) { delete d; d = 0; }