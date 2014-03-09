#include <includes.h>

Downloader::Downloader() : StopFlag(false), ThreadV(0)
{

}

Downloader::~Downloader()
{


}

void Download(DownloadThread *DT)
{
	//http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client/section1075.dat nfsw.exe

	short i = 0;
	char *DefaultTempPath = new char[MAX_PATH];
	char *TempPath = new char[MAX_PATH];

	i = DT->i;

	GetTempPath(MAX_PATH, (LPSTR)DefaultTempPath);

	sprintf(TempPath, "%s\\nfswl\\", DefaultTempPath);
	Debug("%s\n%s\n", DefaultTempPath, TempPath);

	if (!Utils::FileExists(TempPath))
		CreateDirectory(TempPath, NULL);

	for (short s = 0; s < i; s++)
	{
		CURL *curl;
		FILE *fp;
		CURLcode res;
		char *DatFile = new char[FILENAME_MAX];
		char *DatFileInTemp = new char[FILENAME_MAX];
		char *File = new char[FILENAME_MAX];
		char Url[256];
		size_t result;

		sprintf(DatFile, "section%d.dat", DT->FI[s]->section);

		Debug("Download : \npath : %s\nfile : %s\nsection : %d\noffset : %d\nlength : %d\ncompressed : %d\nDatFile : %s\n", DT->FI[s]->path, DT->FI[s]->file, DT->FI[s]->section, DT->FI[s]->offset, DT->FI[s]->lenght, DT->FI[s]->compressed, DatFile);

		sprintf(Url, "http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client/%s", DatFile);
		sprintf(DatFileInTemp, "%s%s", TempPath, DatFile);
		curl = curl_easy_init();
		if (!Utils::FileExists(DatFileInTemp))
		{
			if (curl) 
			{
				fp = fopen(DatFileInTemp, "wb");
				curl_easy_setopt(curl, CURLOPT_URL, Url);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Utils::WriteDataCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
				res = curl_easy_perform(curl);
				/* always cleanup */
				curl_easy_cleanup(curl);
				fclose(fp);
			}
		}
		delete DatFile;
		if (strcmp(DT->FI[s]->path, "") != 0)
			sprintf(File, "%s\\%s\\%s", DT->path, DT->FI[s]->path, DT->FI[s]->file);
		else
			sprintf(File, "%s\\%s", DT->path, DT->FI[s]->file);

		SizeT srcLen = DT->FI[s]->compressed - 13;

		SizeT destLen = DT->FI[s]->lenght;

/*		unsigned char* Decomress = new unsigned char[srcLen];

		unsigned char* Lzma = new unsigned char[srcLen];

		fp = fopen(DatFileInTemp, "rb");

		result = fread(Lzma, 1, destLen, fp);

		if (result != destLen)
			Debug("Reading error");
		
		unsigned char prop[5] =
		{
			0
		};
		
		fclose(fp);
		int ok = LzmaUncompress(Decomress, &srcLen, Lzma, &destLen, prop, LZMA_PROPS_SIZE);*/
		delete DatFileInTemp;

		/*
		    public static string DecompressLZMA(byte[] compressedFile)
    {
      IntPtr srcLen = new IntPtr(compressedFile.Length - 13);
      byte[] src = new byte[srcLen.ToInt64()];
      IntPtr outPropsSize = new IntPtr(5);
      byte[] outProps = new byte[5];
      compressedFile.CopyTo((Array) src, 13);
      for (int index = 0; index < 5; ++index)
        outProps[index] = compressedFile[index];
      int length = 0;
      for (int index = 0; index < 8; ++index)
        length += (int) compressedFile[index + 5] << 8 * index;
      IntPtr destLen = new IntPtr(length);
      byte[] numArray = new byte[length];
      int errorCode = UnsafeNativeMethods.LzmaUncompress(numArray, ref destLen, src, ref srcLen, outProps, outPropsSize);
      if (errorCode != 0)
      {
        Downloader.mLogger.Fatal((object) ("Decompression returned " + (object) errorCode));
        throw new UncompressionException(errorCode, string.Format(ResourceWrapper.Instance.GetString("GameLauncher.LanguageStrings", "DOWNLOADER00002"), (object) errorCode));
      }
      else
        return new string(Encoding.UTF8.GetString(numArray).ToCharArray());
    }
		
		*/
	}


	/*
	char ** tablica = new char*[ilosc];
	for (uint32 i = 0; i < rozmiar; i++)
	{
	DajZadanieCurlowi(tablica[i]);
	}
	delete[] tablica;
	*/

}

void Verify(VerifyCommandArgument *param)
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


	// add http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client/TracksHigh/index.xml
	// add http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client/en/index.xml  //en ; de ; es ; fr ; ru ; 
	Downloader::SetVerifying(true);
	char ServerPath[128] = { 0 };
	char languagePackage[128] = { 0 };
	char PatchPath[128] = { 0 };

	Downloader D;
	char FilePath[128];

	char *path = new char[MAX_PATH];
	char *path1 = new char[MAX_PATH];

	if (param->Package > NULL)
		memcpy(languagePackage, param->Package, 128);
	if (languagePackage > NULL)
		sprintf(ServerPath, "%s/%s", param->ServerPath, languagePackage);

	//	sprintf(PatchPath, "%s", arg->PatchPath);
	//	Debug("Verify Starting %s", languagePackage);

	bool stopOnFail = param->StopOnFail;
	bool clearHashes = param->ClearHashes;
	int num1 = param->WriteHashes ? 1 : 0;
	bool downlload = param->Download;
	bool fullDownload = false;
	bool flag1 = false;

	sprintf(FilePath, "%sindex.xml", ServerPath);
	char * IndexCharFile = D.GetIndexFile(FilePath, false);
	tinyxml2::XMLDocument indexFile;
	indexFile.Parse(IndexCharFile);
	free(IndexCharFile);
	/*	if (indexFile = (tinyxml2::XMLDocument) NULL)
	{
	D.StopFlag = true;
	Error("Error retrieving the index.xml file");
	}else*/
	long num2 = atol(indexFile.FirstChildElement("index")->FirstChildElement("header")->FirstChildElement("length")->GetText());
	int  num3 = atoi(indexFile.FirstChildElement("index")->FirstChildElement("header")->FirstChildElement("firstcab")->GetText());
	int  num31 = atoi(indexFile.FirstChildElement("index")->FirstChildElement("header")->FirstChildElement("lastcab")->GetText());

	sprintf(path, "%s\\%s", Launcher::GetGameDir(), param->PatchPath);

	memcpy(path1, path, MAX_PATH);

	if (sizeof(languagePackage) == 2)
		sprintf(path1, "%s\\Sound\\Speech", path);
	else if (sizeof(languagePackage) > 0)
		sprintf(path1, "%s\\%s", path, languagePackage);

	if (!Utils::FileExists(path1))
		fullDownload = true;
	if (!Utils::FileExists(path))
		CreateDirectory(path, NULL);
	delete path1;
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

	int num7 = 1;
	int val2 = 1;
	bool flag2 = false;

	tinyxml2::XMLElement* ShardInfo = indexFile.FirstChildElement("index")->FirstChildElement("fileinfo");

	FileInfo **FI = (FileInfo**)malloc(sizeof(FileInfo*) * 100);

	short i = 0;

	for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
	{
		USING_NAMESPACE(CryptoPP)
		USING_NAMESPACE(Weak1)

		char *path3 = 0;
		char path4[MAX_PATH] = { 0 };
		char *innerText = new char[MAX_PATH];
		char *File = new char[MAX_PATH];
		char *str = new char[10];
		char *Hash = 0;
		char *HashSvr = 0;
		MD5 md5;
		std::string HashOutput;

		path3 = const_cast<char*>(ShardInfo->FirstChildElement("path")->GetText());
		sprintf(innerText, ShardInfo->FirstChildElement("file")->GetText());

		if (PatchPath == NULL)
		{
			//int length = strcspn(path3, "/") + 1; //path3.IndexOf("/");
			//path3 = length < 0 ? patchPath : path3.Replace(path3.Substring(0, length), patchPath);
		}
		sscanf(path3, "%8s %s", str, path4);

		if (strcmp(path4, "") != 0)
			sprintf(File, "%s\\%s\\%s", path, path4, innerText);
		else
			sprintf(File, "%s\\%s", path, innerText);

		if(Utils::FileExists(File))
		{
			FileSource f(File, true, new HashFilter(md5, new Base64Encoder(new StringSink(HashOutput))));
			Hash = const_cast<char*>(HashOutput.c_str());
			Hash[strlen(Hash) - 1] = '\0';
			HashSvr = const_cast<char*>(ShardInfo->FirstChildElement("hash")->GetText());

			if (strcmp(Hash, HashSvr) != 0)
			{
				Debug("FILE : %s Hash nie jest taki sam\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
				i++;
				if (!FI)
				{	
					FI[0] = (FileInfo*)malloc(sizeof(FileInfo));
					FI[0]->path = (char *)malloc(MAX_PATH);
					memset(FI[0]->path, 0, MAX_PATH);
					strcpy(FI[0]->path, path4);
					FI[0]->file = (char *)malloc(MAX_PATH);
					memset(FI[0]->file, 0, MAX_PATH);
					strcpy(FI[0]->file, innerText);
					FI[0]->section = atoi(ShardInfo->FirstChildElement("section")->GetText());
					FI[0]->offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
					FI[0]->lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
					FI[0]->compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
				}
				else
				{
					FI[i - 1] = (FileInfo*)malloc(sizeof(FileInfo));
					FI[i - 1]->path = (char *)malloc(MAX_PATH);
					memset(FI[i - 1]->path, 0, MAX_PATH);
					strcpy(FI[i - 1]->path, path4);
					FI[i - 1]->file = (char *)malloc(MAX_PATH);
					memset(FI[i - 1]->file, 0, MAX_PATH);
					strcpy(FI[i - 1]->file, innerText);
					FI[i - 1]->section = atoi(ShardInfo->FirstChildElement("section")->GetText());
					FI[i - 1]->offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
					FI[i - 1]->lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
					FI[i - 1]->compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
				}
			}
			else
			{
				Debug("FILE : %s Hash jest taki sam\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
			}
		}
		else
		{
			i++;
			Debug("FILE : %s Brak pliku", innerText);
			if (!FI)
			{
				FI[0] = (FileInfo*)malloc(sizeof(FileInfo));
				FI[0]->path = (char *)malloc(MAX_PATH);
				memset(FI[0]->path, 0, MAX_PATH);
				strcpy(FI[0]->path, path4);
				FI[0]->file = (char *)malloc(MAX_PATH);
				memset(FI[0]->file, 0, MAX_PATH);
				strcpy(FI[0]->file, innerText);
				FI[0]->section = atoi(ShardInfo->FirstChildElement("section")->GetText());
				FI[0]->offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
				FI[0]->lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
				FI[0]->compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
			}
			else
			{
				FI[i - 1] = (FileInfo*)malloc(sizeof(FileInfo));
				FI[i - 1]->path = (char *)malloc(MAX_PATH);
				memset(FI[i - 1]->path, 0, MAX_PATH);
				strcpy(FI[i - 1]->path, path4);
				FI[i - 1]->file = (char *)malloc(MAX_PATH);
				memset(FI[i - 1]->file, 0, MAX_PATH);
				strcpy(FI[i - 1]->file, innerText);
				FI[i - 1]->section = atoi(ShardInfo->FirstChildElement("section")->GetText());
				FI[i - 1]->offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
				FI[i - 1]->lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
				FI[i - 1]->compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
			}
		}

		delete[] innerText;
		delete[] File;
		delete[] str;
	}

	for (short s = 0; s < i; s++)
	{
		Debug("Verify : \npath : %s\nfile : %s\nsection : %d\noffset : %d\nlength : %d\ncompressed : %d\n", FI[s]->path, FI[s]->file, FI[s]->section, FI[s]->offset, FI[s]->lenght, FI[s]->compressed);
	}

	DownloadThread *DT = new DownloadThread;

	DT->path = (char *)malloc(MAX_PATH);
	strcpy(DT->path, path);
	DT->i = i;
	DT->FI = (FileInfo**)malloc(sizeof(FileInfo)* 100);
	memcpy(&DT->FI, &FI, sizeof(sizeof(FileInfo)* 100));

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Download, (LPVOID)DT, 0, 0);

	delete path;
	delete param;
}

void Downloader::StartVerificationAndDownload()
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

	VerifyCommandArgument *Vparam = new VerifyCommandArgument;

	Vparam->ServerPath = "http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client";
	Vparam->PatchPath = "Data";
	Vparam->Package = NULL;
	Vparam->StopOnFail = false;
	Vparam->ClearHashes = false;
	Vparam->WriteHashes = false;
	Vparam->Download = true;

	Info("Starting verify");
	StopFlag = false;
	ThreadV = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Verify, Vparam, 0, 0);

	time_t timeraw = time(NULL);
	struct tm * Timeinfo = localtime(&timeraw);

	//sprintf(DownloaderStartTime, "%d/%d/%d %d:%d:%d", Timeinfo->tm_mon, Timeinfo->tm_mday, Timeinfo->tm_year, Timeinfo->tm_hour, Timeinfo->tm_min, Timeinfo->tm_sec);
	//DownloadManager.ServerPath = parameters.ServerPath;
	//DownloadCommandArgument downloadCommandArgument = new DownloadCommandArgument(parameters.Package, parameters.PatchPath);
	//ThreadD = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Download, (LPVOID)&downloadCommandArgument, 0, 0);
}

char *Downloader::GetIndexFile(char * url, bool useCache)
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

		return output.buffer;
	}

	return NULL;
}
