#define CRYPTOPP_DEFAULT_NO_DLL
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

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

#include <md5.h>
#include <Base64.h>
#include <files.h>
#include <LzmaLib.h>

#include <HTTP.h>

#include <Utils.h>
#include <Downloader.h>
#include <Launcher.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define SAFE_DELETE(d) if(d) { delete d; d = 0; }