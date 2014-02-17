#include <includes.h>
/*
HANDLE Downloader::Thread;
bool Downloader::Verifying;
char *Downloader::DownloaderStartTime;
*/

Downloader::Downloader() : StopFlag(false)
{

}

Downloader::~Downloader()
{


}

void Verify(VerifyCommandArgument param)
{
	/*
	ServerPath  http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client
	Package 
	PatchPath Data
	StopOnFail false
	ClearHashes false
	WriteHashes false
	Download true
	*/

	Downloader::SetVerifying(true);
	char *ServerPath = NULL, *languagePackage = NULL, *PatchPath = NULL;

	sprintf(ServerPath, "%s", param.ServerPath);
	sprintf(languagePackage, "%s", param.Package);
	if (languagePackage > NULL)
		sprintf(ServerPath, "%s/%s", ServerPath, languagePackage);

	sprintf(PatchPath, "%s", param.PatchPath);
	Info("Verify Starting %s", languagePackage);

	bool stopOnFail = param.StopOnFail;
	bool clearHashes = param.ClearHashes;
	int num1 = param.WriteHashes ? 1 : 0;
	bool downlload = param.Download;
	bool fullDownload = false;
	bool flag1 = false;

	Downloader D;
	char *FilePath = NULL;
	sprintf(FilePath, "%s/index.xml", ServerPath);
	tinyxml2::XMLDocument indexFile = &D.GetIndexFile(FilePath, false);
	/*	if (indexFile = (tinyxml2::XMLDocument) NULL)
		{
			D.StopFlag = true;
			Error("Error retrieving the index.xml file");
		}else*/
	long num2 = atol(indexFile.FirstChildElement("index")->FirstChildElement("header")->FirstChildElement("length")->GetText());
	int  num3 = atoi(indexFile.FirstChildElement("index")->FirstChildElement("header")->FirstChildElement("firstcab")->GetText());
	int  num31 = atoi(indexFile.FirstChildElement("index")->FirstChildElement("header")->FirstChildElement("lastcab")->GetText());

	//XmlNodeList xmlNodeList2 = indexFile.SelectNodes("/index/fileinfo");

	char path[MAX_PATH] = { 0 };
	char path1[MAX_PATH] = { 0 };

	sprintf(path, "%s//%s", Launcher::GetGameDir(), param.PatchPath);

	memcpy(path1, path, MAX_PATH);

	if (sizeof(languagePackage) == 2)
		sprintf(path1, "%s\\Sound\\Speech", path);
	else if (sizeof(languagePackage) > 0)
		sprintf(path1, "%s\\%s", path, languagePackage);

	if (!Utils::FileExists(path1))
		fullDownload = true;
	if (!Utils::FileExists(path))
		CreateDirectory(path, NULL);

	/*
	          string path1 = Path.Combine(Environment.CurrentDirectory, patchPath);
          string path2 = path1;
          if (languagePackage.Length == 2)
            path2 = path1 + "\\Sound\\Speech";
          else if (languagePackage.Length > 0)
            path2 = path1 + (object) '\\' + languagePackage;
          if (!Directory.Exists(path2))
          {
            this.mTelemetry.Call(string.Format("full_download_{0}", (object) languagePackage));
            fullDownload = true;
          }
          if (!Directory.Exists(path1))
            Directory.CreateDirectory(path1);
          HashManager.Instance.Clear();
          HashManager.Instance.Start(indexFile, patchPath, languagePackage + ".hsh", this.mHashThreads);
          if (!string.IsNullOrEmpty(languagePackage))
            languagePackage = "/" + languagePackage;

	*/
	if (!languagePackage == NULL)
		sprintf(languagePackage, "/%s", languagePackage);

	long num4 = 0L;
	long num5 = 0L;
	long num6 = 0L;

	int num7 = 1;
	int val2 = 1;
	bool flag2 = false;

	tinyxml2::XMLElement* ShardInfo = indexFile.FirstChildElement("index")->FirstChildElement("fileinfo");

	for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
	{
		char *path3 = { 0 };
		char *innerText = { 0 };
		sprintf(path3, ShardInfo->FirstChildElement("path")->GetText());
		sprintf(innerText, ShardInfo->FirstChildElement("file")->GetText());
		if (PatchPath == NULL)
		{
			int length = strcspn(path3, "/") + 1; //path3.IndexOf("/");
			//path3 = length < 0 ? patchPath : path3.Replace(path3.Substring(0, length), patchPath);
		}
	}
}

void Downloader::StartVerificationAndDownload(VerifyCommandArgument parameters)
{
	Info("Starting verify");
	StopFlag = false;
	Thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Verify, (LPVOID)&parameters, 0, 0);
	if (!parameters.Download)
		return;
	time_t timeraw = time(NULL);
	struct tm * pTimeinfo = localtime(&timeraw);

	sprintf(DownloaderStartTime, "%d/%d/%d %d:%d:%d", pTimeinfo->tm_mon, pTimeinfo->tm_mday, pTimeinfo->tm_year, pTimeinfo->tm_hour, pTimeinfo->tm_min, pTimeinfo->tm_sec);
	//DownloadManager.ServerPath = parameters.ServerPath;
	//DownloadCommandArgument downloadCommandArgument = new DownloadCommandArgument(parameters.Package, parameters.PatchPath);
	//Thread = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Download, (LPVOID)&downloadCommandArgument, 0, 0);
}

void Downloader::VerificationAndDownload()
{
	/*
	ServerPath  http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client
	Package 
	PatchPath Data
	StopOnFail false
	ClearHashes false
	WriteHashes false
	Download true
	*/
	VerifyCommandArgument Vparam;

	Vparam.ServerPath = "http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client";
	Vparam.PatchPath = "Data";
	Vparam.StopOnFail = false;
	Vparam.ClearHashes = false;
	Vparam.WriteHashes = false;
	Vparam.Download = true;

	StartVerificationAndDownload(Vparam);

}

char * Downloader::GetHash(char * File)
{
	USING_NAMESPACE(CryptoPP)
	MD5 hash;
	char * buffer = new char;
	std::string output;
	FileSource f(File, true, new HashFilter(hash,new Base64Encoder(new StringSink(output))));
	strcpy(buffer, output.c_str());
	//strrchr(buffer, '\n');
	return buffer;
}

tinyxml2::XMLDocument Downloader::GetIndexFile(char * url, bool useCache)
{

	if (useCache)
	{
		Info("Getting Index.xml from cache");
		//return Downloader.mIndexCached; 
	}
	else
	{
		Debug("Downloading %s", url);
		CURL *curl;
		CURLcode res;
		curl = curl_easy_init();

		struct BufferStruct output;
		output.buffer = NULL;
		output.size = 0;
		if (curl)
		{

			curl_easy_setopt(curl, CURLOPT_URL, url);
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

		Debug("%s", output.buffer);

		tinyxml2::XMLDocument doc;
		doc.Parse(output.buffer);
		return &doc;
	}

	return &(tinyxml2::XMLDocument) NULL;
}
