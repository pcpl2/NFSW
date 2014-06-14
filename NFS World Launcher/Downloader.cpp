#include <includes.h>

Downloader *Downloader::instance = 0;
std::vector<FileInfo> GFI;
short Downloader::FI_I = 0;

Downloader::Downloader()
{
	instance = this;
	Mutex = CreateMutex(NULL, FALSE, NULL);
	//new FileInfo*[100];
}

Downloader::~Downloader()
{
	if (Mutex)
	{
		CloseHandle(Mutex);
		Mutex = 0;
	}

}

int Progress_Func(void *ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded)
{
	//Debug("%d / %d", (int)NowDownloaded, (int)TotalToDownload);
	return 0;
}

void ClearTmp()
{
	char *DefaultTempPath = new char[MAX_PATH];
	char *TempPath = new char[MAX_PATH];
	GetTempPath(MAX_PATH, (LPSTR)DefaultTempPath);
	sprintf(TempPath, "%snfswl\\", DefaultTempPath);


	delete DefaultTempPath;
	delete TempPath;
}

void Downloader::Download(DownloadThread *DT)
{
	char *DefaultTempPath = new char[MAX_PATH];
	char *TempPath = new char[MAX_PATH];

	GetTempPath(MAX_PATH, (LPSTR)DefaultTempPath);

	sprintf(TempPath, "%snfswl\\", DefaultTempPath);
	Debug("%s\n%s\n", DefaultTempPath, TempPath);

	if (!Utils::FileExists(TempPath))
		CreateDirectory(TempPath, NULL);

	//Launcher::Get().SetMarqueeProgressBar(1, false);
	Launcher::Get().SetRangeProgressBar(1, 0, FI_I);

	Launcher::Get().SetPosProgressBar(0, 60);

	for (short s = 0; s < FI_I; s++)
	{
		Launcher::Get().SetPosProgressBar(1, s + 1);
		CURL *curl;
		FILE *fp;
		CURLcode Res;
		char *DatFile = new char[FILENAME_MAX];
		char *DatFileInTemp = new char[FILENAME_MAX];
		char *File = new char[FILENAME_MAX];
		char *TempPathFile = new char[MAX_PATH];
		char *Url = new char[256];
		size_t result;

		if (GFI[s].SetPackage)
			sprintf(TempPathFile, "%s%s\\", TempPath, GFI[s].Package);
		else
			sprintf(TempPathFile, "%s\\", TempPath);

		if (!Utils::FileExists(TempPathFile))
			CreateDirectory(TempPathFile, NULL);
		if (GFI[s].SetPackage)
			Debug("Download : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\nPackage : %s\n",
			GFI[s].Path, GFI[s].File, GFI[s].Section, GFI[s].ToSection, GFI[s].Offset, GFI[s].Lenght, GFI[s].Compressed, GFI[s].Package);
		else
			Debug("Download : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\nPackage : \n",
			GFI[s].Path, GFI[s].File, GFI[s].Section, GFI[s].ToSection, GFI[s].Offset, GFI[s].Lenght, GFI[s].Compressed);

		for (int val2 = GFI[s].Section; val2 <= GFI[s].ToSection; ++val2)
		{
			sprintf(DatFile, "section%d.dat", val2);
			Debug("%s", DatFile);

			if (GFI[s].SetPackage)
				sprintf(Url, "%s/%s/%s", DT->Url, GFI[s].Package, DatFile);
			else
				sprintf(Url, "%s/%s", DT->Url, DatFile);


			sprintf(DatFileInTemp, "%s%s", TempPathFile, DatFile);

			curl = curl_easy_init();

			if (!Utils::FileExists(DatFileInTemp))

			{
				fp = fopen(DatFileInTemp, "wb+");
				if (curl)
				{
					Debug("Downloading %s", Url);
					curl_easy_setopt(curl, CURLOPT_URL, Url);
					curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Utils::WriteDataCallback);
					curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
					curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
					curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Progress_Func);
					Res = curl_easy_perform(curl);

					if (Res != CURLE_OK)
						Error("curl_easy_perform() failed: %s\n", curl_easy_strerror(Res));

					curl_easy_cleanup(curl);
				}
				fclose(fp);

			}

		}

		if (strcmp(GFI[s].Path, "") != 0)
			sprintf(File, "%s\\%s\\%s", DT->Path, GFI[s].Path, GFI[s].File);
		else
			sprintf(File, "%s\\%s", DT->Path, GFI[s].File);

		char *Directory = new char[MAX_PATH];
		sprintf(Directory, "%s\\%s", DT->Path, GFI[s].Path);

		if (!Utils::FileExists(Directory))
			CreateDirectory(Directory, NULL);

		delete Directory;

		SizeT srcLen = GFI[s].Compressed - 13;
		SizeT destLen = GFI[s].Lenght;

		unsigned char Prop[5];
		unsigned char Arr[13];
		std::vector<unsigned char> inBuf1;
		std::vector<unsigned char> inBuf2;
		//unsigned char *LZMA;

		for (int x = GFI[s].Section; x <= GFI[s].ToSection; x++)
		{
			sprintf(DatFile, "section%d.dat", x);
			sprintf(DatFileInTemp, "%s%s", TempPathFile, DatFile);

			fp = fopen(DatFileInTemp, "rb");
			if (NULL != fp)

				Debug("Can open the file : %s", DatFile);

			else
				Debug("Could not open the file : %s", DatFile);

			fseek(fp, 0L, SEEK_END);
			long sz = ftell(fp);
			fseek(fp, 0L, SEEK_SET);
			/*
			unsigned char *FileData = new unsigned char[sz];
			result = fread(FileData, 1, sz, fp);
			*/
			std::vector<unsigned char> inBuf(sz);
			result = fread(&inBuf[0], 1, sz, fp);

			if (result != sz)
				Debug("Reading error");
			fclose(fp);

			for (int y = 0; y < sz; y++)

				inBuf1.push_back(inBuf.at(y));

			//memcpy(LZMA + sizeof(LZMA), FileData, sizeof(FileData));
			//delete FileData;
			//inBuf.clear();

		}
		delete DatFileInTemp;
		delete DatFile;

		for (int index = 0; index < 13; index++)
			Arr[index] = inBuf1.at(GFI[s].Offset + index);

		/*
		memcpy(&Arr, LZMA + GFI[s].Offset, 13);
		unsigned char *LZMA2 = new unsigned char[GFI[s].Offset + GFI[s].Compressed + 1];*/
		for (int index = GFI[s].Offset + 13; index < GFI[s].Offset + GFI[s].Compressed; index++)
			inBuf2.push_back(inBuf1.at(index));

		inBuf1.clear();
		//memcpy(&LZMA2, LZMA + GFI[s].Offset + 13, GFI[s].Offset + GFI[s].Compressed);

		memcpy(Prop, Arr, 5);

		unsigned char* Decomress = new unsigned char[destLen];

		if (inBuf2.size() == srcLen)
		{
			Debug("ok");
		}

		int ok = LzmaUncompress(Decomress, &destLen, &inBuf2[0], &srcLen, Prop, LZMA_PROPS_SIZE);
		if (ok > NULL)
		{
			Error("Error Uncompress");
		}

		inBuf2.clear();

		fp = fopen(File, "wb+");
		fwrite(Decomress, 1, destLen, fp);
		fclose(fp);

		delete Decomress;
		delete File;
		delete TempPathFile;
		delete Url;
	}
	delete DefaultTempPath;
	delete TempPath;
	free(DT->Url);
	free(DT->Path);
	delete DT;
	Launcher::Get().SetPosProgressBar(0, 70);
	Launcher::Get().StartGameEnable();
	ExitThread(0);
}

void Downloader::VerifyProcess(VerifyP *VP)
{
	char path[MAX_PATH];
	char path1[MAX_PATH];
	char FilePath[128];
	char ServerPath[128];
	bool tos = false;
	short tos_s = 0;

	if (VP->SetPackage)
		sprintf(ServerPath, "%s/%s/", VP->ServerUrl, VP->Package);
	else
		sprintf(ServerPath, "%s/", VP->ServerUrl);

	sprintf(FilePath, "%sindex.xml", ServerPath);
	char *IndexCharFile = Downloader::Get().GetIndexFile(FilePath);

	if (sizeof(IndexCharFile) < NULL)
		Error("Error retrieving the index.xml file");

	tinyxml2::XMLDocument indexFile;
	indexFile.Parse(IndexCharFile);
	free(IndexCharFile);

	sprintf(path, "%s\\Data", Launcher::GetGameDir());
	memcpy(path1, path, MAX_PATH);

	if (VP->SetPackage)
		sprintf(path1, "%s\\%s", path, VP->PackageDir);

	if (!Utils::FileExists(path1))
		CreateDirectory(path1, NULL);

	tinyxml2::XMLElement *ShardInfo = indexFile.FirstChildElement("index")->FirstChildElement("fileinfo");

	Launcher::Get().SetPosProgressBar(0, 10);

	for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
	{
		Utils::Timer t;
		t.start();
		char *path3 = { 0 };
		char path4[MAX_PATH] = { 0 };
		char *innerText = new char[MAX_PATH];
		char *File = new char[MAX_PATH];
		char *str = new char[10];
		char *Hash = new char[24];
		char *Hash1 = { 0 };
		char *HashSvr = { 0 };

		if (tos)
		{
			ChangeToSectionInFI(tos_s, atoi(ShardInfo->FirstChildElement("section")->GetText()));
			tos = false;
		}

		path3 = const_cast<char*>(ShardInfo->FirstChildElement("path")->GetText());
		sprintf(innerText, ShardInfo->FirstChildElement("file")->GetText());

		sscanf(path3, "%8s %s", str, path4);

		if (strcmp(path4, "") != 0)
			sprintf(File, "%s\\%s\\%s", path, path4, innerText);
		else
			sprintf(File, "%s\\%s", path, innerText);

		if (Utils::FileExists(File))
		{
			Hash1 = MD5File(File);
			std::string result_string = HEX2STR(Hash1);

			std::string thash = base64_encode(reinterpret_cast<const unsigned char*>(result_string.c_str()), result_string.length());
			HashSvr = const_cast<char*>(ShardInfo->FirstChildElement("hash")->GetText());
			Hash = const_cast<char*>(thash.c_str());

			if (strcmp(Hash, HashSvr) != 0)
			{
				Debug("(thread %d) FILE : %s Hash is not the same\nHash: %s\nHashSvr: %s", VP->Thread, innerText, Hash, HashSvr);
				tos_s = AddFileToFI(path4, innerText, atoi(ShardInfo->FirstChildElement("section")->GetText()),
					atoi(ShardInfo->FirstChildElement("offset")->GetText()),
					atoi(ShardInfo->FirstChildElement("length")->GetText()),
					atoi(ShardInfo->FirstChildElement("compressed")->GetText()),
					VP->SetPackage, VP->PackageSize, VP->Package,
					atoi(ShardInfo->FirstChildElement("section")->GetText()));
				tos = true;
			}
			else
			{
				Debug("(thread %d) FILE : %s Hash is the same\nHash: %s\nHashSvr: %s", VP->Thread, innerText, Hash, HashSvr);
			}
		}
		else
		{
			Debug("(thread %d) FILE : %s not found", VP->Thread, innerText);
			tos_s = AddFileToFI(path4, innerText, atoi(ShardInfo->FirstChildElement("section")->GetText()),
				atoi(ShardInfo->FirstChildElement("offset")->GetText()),
				atoi(ShardInfo->FirstChildElement("length")->GetText()),
				atoi(ShardInfo->FirstChildElement("compressed")->GetText()),
				VP->SetPackage, VP->PackageSize, VP->Package,
				atoi(ShardInfo->FirstChildElement("section")->GetText()));
			tos = true;
		}
		delete[] innerText;
		delete[] File;
		delete str;
		t.stop();
		Info("(thread %d) %dms", VP->Thread, t.elapsed());
	}
	ExitThread(0);
}

void Downloader::Verify(VerifyArgument *param)
{
	//en ; de ; es ; fr ; ru ; 
	Utils::Timer t;
//	Launcher::Get().SetMarqueeProgressBar(1, true);
	HANDLE *ThreadVP = new HANDLE[3];

	VerifyP *VP1 = new VerifyP;
	VerifyP *VP2 = new VerifyP;
	VerifyP *VP3 = new VerifyP;
	VerifyP *VP4 = new VerifyP;
	t.start();

	VP1->ServerUrl = new char[128];
	memcpy(VP1->ServerUrl, param->ServerPath, 128);
	VP1->SetPackage = false;
	VP1->Thread = 1;

	ThreadVP[0] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)VerifyProcess, VP1, 0, 0);
	
	Sleep(500);

	Launcher::Get().SetPosProgressBar(0, 10);
		
	VP2->ServerUrl = new char[128];
	memcpy(VP2->ServerUrl, param->ServerPath, 128);
	VP2->SetPackage = true;
	VP2->PackageSize = 3;
	VP2->Package = new char[VP2->PackageSize];
	memcpy(VP2->Package, param->Package, VP2->PackageSize);
	VP2->PackageDir = new char[MAX_PATH];
	strcpy(VP2->PackageDir, "Sound\\Speech\\");
	VP2->Thread = 2;

	ThreadVP[1] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)VerifyProcess, VP2, 0, 0);

	Sleep(500);

	Launcher::Get().SetPosProgressBar(0, 20);

	VP3->ServerUrl = new char[128];
	memcpy(VP3->ServerUrl, param->ServerPath, 128);
	VP3->SetPackage = true;
	VP3->PackageSize = 7;
	VP3->Package = new char[VP3->PackageSize];
	strcpy(VP3->Package, "Tracks");
	VP3->PackageDir = new char[MAX_PATH];
	strcpy(VP3->PackageDir, "Tracks\\");
	VP3->Thread = 3;

	ThreadVP[2] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)VerifyProcess, VP3, 0, 0);

	Sleep(500);

	Launcher::Get().SetPosProgressBar(0, 30);

	if (param->FullDownload)
	{
		VP4->ServerUrl = new char[128];
		memcpy(VP4->ServerUrl, param->ServerPath, 128);
		VP4->SetPackage = true;
		VP4->PackageSize = 11;
		VP4->Package = new char[VP4->PackageSize];
		strcpy(VP4->Package, "TracksHigh");
		VP4->PackageDir = new char[MAX_PATH];
		strcpy(VP4->PackageDir, "TracksHigh\\");
		VP4->Thread = 4;

		ThreadVP[3] = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)VerifyProcess, VP4, 0, 0);

		Sleep(500);

		Launcher::Get().SetPosProgressBar(0, 40);

		WaitForSingleObject(ThreadVP[3], INFINITE);

	}
	WaitForSingleObject(ThreadVP[2], INFINITE);
	WaitForSingleObject(ThreadVP[1], INFINITE);
	WaitForSingleObject(ThreadVP[0], INFINITE);

	for (short s = 0; s < FI_I; s++)
	{
		if (GFI[s].ToSection == 0)
		{
			GFI[s].ToSection = GFI[s].Section;
			if (GFI[s].SetPackage)
				Debug("Verify : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\nPackage : %s\n",
				GFI[s].Path, GFI[s].File, GFI[s].Section, GFI[s].ToSection, GFI[s].Offset, GFI[s].Lenght, GFI[s].Compressed, GFI[s].Package);
			else
				Debug("Verify : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\nPackage : \n",
				GFI[s].Path, GFI[s].File, GFI[s].Section, GFI[s].ToSection, GFI[s].Offset, GFI[s].Lenght, GFI[s].Compressed);
		}
		else
		{
			if (GFI[s].SetPackage)
				Debug("Verify : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\nPackage : %s\n",
				GFI[s].Path, GFI[s].File, GFI[s].Section, GFI[s].ToSection, GFI[s].Offset, GFI[s].Lenght, GFI[s].Compressed, GFI[s].Package);
			else
				Debug("Verify : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\nPackage : \n",
				GFI[s].Path, GFI[s].File, GFI[s].Section, GFI[s].ToSection, GFI[s].Offset, GFI[s].Lenght, GFI[s].Compressed);
		}
	}

	if (FI_I == 0)
	{
		Launcher::Get().SetPosProgressBar(0, 70);
		Launcher::Get().StartGameEnable();
	}
	else
	{
		DownloadThread *DT = new DownloadThread;
		
		DT->Path = (char *)malloc(MAX_PATH);
		sprintf(DT->Path, "%s\\Data", Launcher::GetGameDir());
		DT->Url = (char *)malloc(256);
		strcpy(DT->Url, param->ServerPath);

		Launcher::Get().SetPosProgressBar(0, 50);

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Downloader::Get().Download, (LPVOID)DT, 0, 0);
	}
	t.stop();
	Info("czas weryfikacji : %dms", t.elapsed());
	delete param->ServerPath;
	delete param->Package;
	delete param;
	ExitThread(0);
}

void Downloader::StartVerificationAndDownload(bool FullD, char *Package, char *ServerPath)
{
	VerifyArgument *Vparam = new VerifyArgument;
	Launcher::Get().SetRangeProgressBar(0, 0, 70);

	Vparam->ServerPath = new char[256];
	memcpy(Vparam->ServerPath, ServerPath, 256);
	Vparam->Package = new char[3];
	strcpy(Vparam->Package, Package);//en ; de ; es ; fr ; ru ; 
	Vparam->FullDownload = FullD;

	Info("Starting verify");
	Launcher::Get().SetPosProgressBar(0, 3);
	ThreadV = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Verify, Vparam, 0, 0);
}

char *Downloader::GetIndexFile(char *url)
{
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();

	Utils::BufferStruct output;
	output.buffer = NULL;
	output.size = 0;
	if (curl)
	{
		curl_easy_setopt(curl, CURLOPT_URL, url);
		Debug("Downloading %s", url);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Utils::WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&output);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Progress_Func);
		res = curl_easy_perform(curl);

		/* Check for errors */
		if (res != CURLE_OK)
			Error("curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));
		
		curl_easy_cleanup(curl);
	}

	while (output.buffer == 0)
	{
		Sleep(100);
	}
	Sleep(50);

	return output.buffer;
}

short Downloader::AddFileToFI(char *Path, char *File, int Section, int Offset, int Lenght, int Compressed, bool SetPackage, short PackageSize, char *Package, int ToSection)
{
	FileInfo FI;

	Downloader::Get().LockMutex();
	FI_I++;

	FI.Path = new char[MAX_PATH];
	memset(FI.Path, 0, MAX_PATH);
	strcpy(FI.Path, Path);
	FI.File = new char[MAX_PATH];
	memset(FI.File, 0, MAX_PATH);
	strcpy(FI.File, File);
	FI.Section = Section;
	FI.Offset = Offset;
	FI.Lenght = Lenght;
	FI.Compressed = Compressed;
	if (SetPackage)
	{
		FI.SetPackage = SetPackage;
		FI.Package = new char[PackageSize];
		memset(FI.Package, 0, PackageSize);
		strcpy(FI.Package, Package);
	}
	else
	{
		FI.SetPackage = SetPackage;
	}
	FI.ToSection = ToSection;

	GFI.push_back(FI);

	Downloader::Get().UnlockMutex();
 	return FI_I;
}

void Downloader::ChangeToSectionInFI(short i, int ToSection)
{
	Downloader::Get().LockMutex();

	GFI[i - 1].ToSection = ToSection;

	Downloader::Get().UnlockMutex();
}

void Downloader::LockMutex()
{
	WaitForSingleObject(Mutex, INFINITE);
}

void Downloader::UnlockMutex()
{
	ReleaseMutex(Mutex);
}

static Downloader * Get()
{
	static Downloader dl;
	return &dl;
}