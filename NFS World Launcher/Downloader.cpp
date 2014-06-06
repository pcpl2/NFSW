#include <includes.h>

Downloader *Downloader::instance = 0;

Downloader::Downloader()
{
	instance = this;
}

Downloader::~Downloader()
{


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
	short i = DT->i;
	char *DefaultTempPath = new char[MAX_PATH];
	char *TempPath = new char[MAX_PATH];

	GetTempPath(MAX_PATH, (LPSTR)DefaultTempPath);

	sprintf(TempPath, "%snfswl\\", DefaultTempPath);
	Debug("%s\n%s\n", DefaultTempPath, TempPath);

	if (!Utils::FileExists(TempPath))
		CreateDirectory(TempPath, NULL);

	//Launcher::Get().SetMarqueeProgressBar(1, false);
	Launcher::Get().SetRangeProgressBar(1, 0, i);

	Launcher::Get().SetPosProgressBar(0, 60);

	for (short s = 0; s < i; s++)
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

		if (DT->FI[s]->SetPackage)
			sprintf(TempPathFile, "%s%s\\", TempPath, DT->FI[s]->Package);
		else
			sprintf(TempPathFile, "%s\\", TempPath);

		if (!Utils::FileExists(TempPathFile))
			CreateDirectory(TempPathFile, NULL);
		if (DT->FI[s]->SetPackage)
			Debug("Download : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\nPackage : %s\n",
				DT->FI[s]->Path, DT->FI[s]->File, DT->FI[s]->Section, DT->FI[s]->ToSection, DT->FI[s]->Offset, DT->FI[s]->Lenght, DT->FI[s]->Compressed, DT->FI[s]->Package);
		else
			Debug("Download : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\nPackage : \n",
				DT->FI[s]->Path, DT->FI[s]->File, DT->FI[s]->Section, DT->FI[s]->ToSection, DT->FI[s]->Offset, DT->FI[s]->Lenght, DT->FI[s]->Compressed);

		for (int val2 = DT->FI[s]->Section; val2 <= DT->FI[s]->ToSection; ++val2)
		{
			sprintf(DatFile, "section%d.dat", val2);
			Debug("%s", DatFile);

			if (DT->FI[s]->SetPackage)
				sprintf(Url, "%s/%s/%s", DT->Url, DT->FI[s]->Package, DatFile);
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

		if (strcmp(DT->FI[s]->Path, "") != 0)
			sprintf(File, "%s\\%s\\%s", DT->Path, DT->FI[s]->Path, DT->FI[s]->File);
		else
			sprintf(File, "%s\\%s", DT->Path, DT->FI[s]->File);

		char *Directory = new char[MAX_PATH];
		sprintf(Directory, "%s\\%s", DT->Path, DT->FI[s]->Path);

		if (!Utils::FileExists(Directory))
			CreateDirectory(Directory, NULL);

		delete Directory;

		SizeT srcLen = DT->FI[s]->Compressed - 13;
		SizeT destLen = DT->FI[s]->Lenght;

		unsigned char Prop[5];
		unsigned char Arr[13];
		std::vector<unsigned char> inBuf1;
		std::vector<unsigned char> inBuf2;
		//unsigned char *LZMA;

		for (int x = DT->FI[s]->Section; x <= DT->FI[s]->ToSection; x++)
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
			Arr[index] = inBuf1.at(DT->FI[s]->Offset + index);

		/*
		memcpy(&Arr, LZMA + DT->FI[s]->Offset, 13);
		unsigned char *LZMA2 = new unsigned char[DT->FI[s]->Offset + DT->FI[s]->Compressed + 1];*/
		for (int index = DT->FI[s]->Offset + 13; index < DT->FI[s]->Offset + DT->FI[s]->Compressed; index++)
			inBuf2.push_back(inBuf1.at(index));

		inBuf1.clear();
		//memcpy(&LZMA2, LZMA + DT->FI[s]->Offset + 13, DT->FI[s]->Offset + DT->FI[s]->Compressed);

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
	free(DT->FI);
	delete DT;
	Launcher::Get().SetPosProgressBar(0, 70);
	Launcher::Get().StartGameEnable();
	ExitThread(0);
}

void Downloader::Verify(VerifyArgument *param)
{
	//en ; de ; es ; fr ; ru ; 

//	Launcher::Get().SetMarqueeProgressBar(1, true);

	char *languagePackage = new char[3];
	char *path = new char[MAX_PATH];
	char *path1 = new char[MAX_PATH];
	char FilePath[128];
	char ServerPath[128];
	bool FullDownload = param->FullDownload;
	bool tos = false;
	short i = 0;
	FileInfo **FI = (FileInfo**)malloc(sizeof(FileInfo*)* 100);

	sprintf(ServerPath, "%s/", param->ServerPath);

	if (sizeof(param->Package) > NULL)
		memcpy(languagePackage, param->Package, 3);

	sprintf(FilePath, "%sindex.xml", ServerPath);
	char *IndexCharFile = Downloader::Get().GetIndexFile(FilePath);

	if (sizeof(IndexCharFile) < NULL)
		Error("Error retrieving the index.xml file");

	tinyxml2::XMLDocument indexFile;
	indexFile.Parse(IndexCharFile);
	free(IndexCharFile);

	sprintf(path, "%s\\Data", Launcher::GetGameDir());
	memcpy(path1, path, MAX_PATH);

	if (sizeof(languagePackage) > NULL)
		sprintf(path1, "%s\\%s", path, languagePackage);

	if (!Utils::FileExists(path1))
		CreateDirectory(path1, NULL);

	tinyxml2::XMLElement *ShardInfo = indexFile.FirstChildElement("index")->FirstChildElement("fileinfo");
	
	Launcher::Get().SetPosProgressBar(0, 10);
	
	for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
	{
		Utils::Timer t;
		t.start();
		char *path3 = new char;
		char path4[MAX_PATH] = { 0 };
		char *innerText = new char[MAX_PATH];
		char *File = new char[MAX_PATH];
		char *str = new char[10];
		char *Hash = new char[24];
		char *Hash1 = new char;
		char *Hash2 = new char;
		char *Hash3 = new char;
		char *HashSvr = new char;

		if (tos)
		{
			FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
			tos = false;
		}

		path3 = const_cast<char*>(ShardInfo->FirstChildElement("path")->GetText());
		sprintf(innerText, ShardInfo->FirstChildElement("file")->GetText());

		sscanf(path3, "%8s %s", str, path4);

		if (strcmp(path4, "") != 0)
			sprintf(File, "%s\\%s\\%s", path, path4, innerText);
		else
			sprintf(File, "%s\\%s", path, innerText);

		if(Utils::FileExists(File))
		{
			Hash1 = MD5File(File);
			std::string result_string = HEX2STR(Hash1);

			std::string thash = base64_encode(reinterpret_cast<const unsigned char*>(result_string.c_str()), result_string.length());
			HashSvr = const_cast<char*>(ShardInfo->FirstChildElement("hash")->GetText());
			Hash = const_cast<char*>(thash.c_str());

			if (strcmp(Hash, HashSvr) != 0)
			{
				Debug("FILE : %s Hash is not the same\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
				i++;
				if (!FI)
				{	
					FI[0] = (FileInfo*)malloc(sizeof(FileInfo));
					FI[0]->Path = (char *)malloc(MAX_PATH);
					memset(FI[0]->Path, 0, MAX_PATH);
					strcpy(FI[0]->Path, path4);
					FI[0]->File = (char *)malloc(MAX_PATH);
					memset(FI[0]->File, 0, MAX_PATH);
					strcpy(FI[0]->File, innerText);
					FI[0]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
					FI[0]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
					FI[0]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
					FI[0]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
					FI[0]->SetPackage = false;
					FI[0]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
					tos = true;
				}
				else
				{
					FI[i - 1] = (FileInfo*)malloc(sizeof(FileInfo));
					FI[i - 1]->Path = (char *)malloc(MAX_PATH);
					memset(FI[i - 1]->Path, 0, MAX_PATH);
					strcpy(FI[i - 1]->Path, path4);
					FI[i - 1]->File = (char *)malloc(MAX_PATH);
					memset(FI[i - 1]->File, 0, MAX_PATH);
					strcpy(FI[i - 1]->File, innerText);
					FI[i - 1]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
					FI[i - 1]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
					FI[i - 1]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
					FI[i - 1]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
					FI[i - 1]->SetPackage = false;
					FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
					tos = true;
				}
			}
			else
				Debug("FILE : %s Hash is the same\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
		}
		else
		{
			i++;
			Debug("FILE : %s not found", innerText);
			if (!FI)
			{
				FI[0] = (FileInfo*)malloc(sizeof(FileInfo));
				FI[0]->Path = (char *)malloc(MAX_PATH);
				memset(FI[0]->Path, 0, MAX_PATH);
				strcpy(FI[0]->Path, path4);
				FI[0]->File = (char *)malloc(MAX_PATH);
				memset(FI[0]->File, 0, MAX_PATH);
				strcpy(FI[0]->File, innerText);
				FI[0]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
				FI[0]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
				FI[0]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
				FI[0]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
				FI[0]->SetPackage = false;
				FI[0]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
				tos = true;
			}
			else
			{
				FI[i - 1] = (FileInfo*)malloc(sizeof(FileInfo));
				FI[i - 1]->Path = (char *)malloc(MAX_PATH);
				memset(FI[i - 1]->Path, 0, MAX_PATH);
				strcpy(FI[i - 1]->Path, path4);
				FI[i - 1]->File = (char *)malloc(MAX_PATH);
				memset(FI[i - 1]->File, 0, MAX_PATH);
				strcpy(FI[i - 1]->File, innerText);
				FI[i - 1]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
				FI[i - 1]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
				FI[i - 1]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
				FI[i - 1]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
				FI[i - 1]->SetPackage = false;
				FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
				tos = true;
			}
		}
		delete[] innerText;
		delete[] File;
		delete[] str;
		t.stop();
		DWORD t1 = t.elapsed();
		Info("%dms", t1);
	}

	tos = false;

	if (sizeof(languagePackage) > NULL)
		sprintf(ServerPath, "%s/%s", param->ServerPath, languagePackage);

	sprintf(FilePath, "%s/index.xml", ServerPath);
	IndexCharFile = Downloader::Get().GetIndexFile(FilePath);

	if (sizeof(IndexCharFile) < NULL)
		Error("Error retrieving the index.xml file");

	indexFile.Parse(IndexCharFile);
	free(IndexCharFile);

	if (sizeof(languagePackage) > NULL)
		sprintf(path1, "%s\\Sound\\Speech", path);

	if (!Utils::FileExists(path1))
		CreateDirectory(path1, NULL);

	ShardInfo = indexFile.FirstChildElement("index")->FirstChildElement("fileinfo");

	Launcher::Get().SetPosProgressBar(0, 20);

	for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
	{
		char *path3 = new char;
		char path4[MAX_PATH] = { 0 };
		char *innerText = new char[MAX_PATH];
		char *File = new char[MAX_PATH];
		char *str = new char[10];
		char *Hash = new char[24];
		char *Hash1 = new char;
		char *Hash2 = new char;
		char *Hash3 = new char;
		char *HashSvr = new char;

		if (tos)
		{
			FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
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
				Debug("FILE : %s Hash is not the same\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
				i++;
				if (!FI)
				{
					FI[0] = (FileInfo*)malloc(sizeof(FileInfo));
					FI[0]->Path = (char *)malloc(MAX_PATH);
					memset(FI[0]->Path, 0, MAX_PATH);
					strcpy(FI[0]->Path, path4);
					FI[0]->File = (char *)malloc(MAX_PATH);
					memset(FI[0]->File, 0, MAX_PATH);
					strcpy(FI[0]->File, innerText);
					FI[0]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
					FI[0]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
					FI[0]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
					FI[0]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
					FI[0]->SetPackage = true;
					FI[0]->Package = (char *)malloc(3);
					memset(FI[0]->Package, 0, 3);
					strcpy(FI[0]->Package, languagePackage);
					FI[0]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
					tos = true;
				}
				else
				{
					FI[i - 1] = (FileInfo*)malloc(sizeof(FileInfo));
					FI[i - 1]->Path = (char *)malloc(MAX_PATH);
					memset(FI[i - 1]->Path, 0, MAX_PATH);
					strcpy(FI[i - 1]->Path, path4);
					FI[i - 1]->File = (char *)malloc(MAX_PATH);
					memset(FI[i - 1]->File, 0, MAX_PATH);
					strcpy(FI[i - 1]->File, innerText);
					FI[i - 1]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
					FI[i - 1]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
					FI[i - 1]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
					FI[i - 1]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
					FI[i - 1]->SetPackage = true;
					FI[i - 1]->Package = (char *)malloc(3);
					memset(FI[i - 1]->Package, 0, 3);
					strcpy(FI[i - 1]->Package, languagePackage);
					FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
					tos = true;
				}
			}
			else
				Debug("FILE : %s Hash is the same\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
		}
		else
		{
			i++;
			Debug("FILE : %s not found", innerText);
			if (!FI)
			{
				FI[0] = (FileInfo*)malloc(sizeof(FileInfo));
				FI[0]->Path = (char *)malloc(MAX_PATH);
				memset(FI[0]->Path, 0, MAX_PATH);
				strcpy(FI[0]->Path, path4);
				FI[0]->File = (char *)malloc(MAX_PATH);
				memset(FI[0]->File, 0, MAX_PATH);
				strcpy(FI[0]->File, innerText);
				FI[0]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
				FI[0]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
				FI[0]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
				FI[0]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
				FI[0]->SetPackage = true;
				FI[0]->Package = (char *)malloc(3);
				memset(FI[0]->Package, 0, 3);
				strcpy(FI[0]->Package, languagePackage);
				FI[0]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
				tos = true;
			}
			else
			{
				FI[i - 1] = (FileInfo*)malloc(sizeof(FileInfo));
				FI[i - 1]->Path = (char *)malloc(MAX_PATH);
				memset(FI[i - 1]->Path, 0, MAX_PATH);
				strcpy(FI[i - 1]->Path, path4);
				FI[i - 1]->File = (char *)malloc(MAX_PATH);
				memset(FI[i - 1]->File, 0, MAX_PATH);
				strcpy(FI[i - 1]->File, innerText);
				FI[i - 1]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
				FI[i - 1]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
				FI[i - 1]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
				FI[i - 1]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
				FI[i - 1]->SetPackage = true;
				FI[i - 1]->Package = (char *)malloc(3);
				memset(FI[i - 1]->Package, 0, 3);
				strcpy(FI[i - 1]->Package, languagePackage);
				FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
				tos = true;
			}
		}

		delete[] innerText;
		delete[] File;
		delete[] str;
	}
	
	tos = false;
	
	sprintf(ServerPath, "%s/Tracks", param->ServerPath);
	sprintf(FilePath, "%s/index.xml", ServerPath);
	IndexCharFile = Downloader::Get().GetIndexFile(FilePath);

	if (sizeof(IndexCharFile) < NULL)
		Error("Error retrieving the index.xml file");

	indexFile.Parse(IndexCharFile);
	free(IndexCharFile);

	sprintf(path1, "%s\\Tracks\\", path);

	if (!Utils::FileExists(path1))
		CreateDirectory(path1, NULL);

	ShardInfo = indexFile.FirstChildElement("index")->FirstChildElement("fileinfo");

	Launcher::Get().SetPosProgressBar(0, 30);

	for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
	{
		char *path3 = new char;
		char path4[MAX_PATH] = { 0 };
		char *innerText = new char[MAX_PATH];
		char *File = new char[MAX_PATH];
		char *str = new char[10];
		char *Hash = new char[24];
		char *Hash1 = new char;
		char *Hash2 = new char;
		char *Hash3 = new char;
		char *HashSvr = new char;

		if (tos)
		{
			FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
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
				Debug("FILE : %s Hash is not the same\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
				i++;
				if (!FI)
				{
					FI[0] = (FileInfo*)malloc(sizeof(FileInfo));
					FI[0]->Path = (char *)malloc(MAX_PATH);
					memset(FI[0]->Path, 0, MAX_PATH);
					strcpy(FI[0]->Path, path4);
					FI[0]->File = (char *)malloc(MAX_PATH);
					memset(FI[0]->File, 0, MAX_PATH);
					strcpy(FI[0]->File, innerText);
					FI[0]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
					FI[0]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
					FI[0]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
					FI[0]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
					FI[0]->SetPackage = true;
					FI[0]->Package = (char *)malloc(7);
					memset(FI[0]->Package, 0, 7);
					sprintf(FI[0]->Package, "Tracks");
					FI[0]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
					tos = true;
				}
				else
				{
					FI[i - 1] = (FileInfo*)malloc(sizeof(FileInfo));
					FI[i - 1]->Path = (char *)malloc(MAX_PATH);
					memset(FI[i - 1]->Path, 0, MAX_PATH);
					strcpy(FI[i - 1]->Path, path4);
					FI[i - 1]->File = (char *)malloc(MAX_PATH);
					memset(FI[i - 1]->File, 0, MAX_PATH);
					strcpy(FI[i - 1]->File, innerText);
					FI[i - 1]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
					FI[i - 1]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
					FI[i - 1]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
					FI[i - 1]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
					FI[i - 1]->SetPackage = true;
					FI[i - 1]->Package = (char *)malloc(7);
					memset(FI[i - 1]->Package, 0, 7);
					sprintf(FI[i - 1]->Package, "Tracks");
					FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
					tos = true;
				}
			}
			else
				Debug("FILE : %s Hash is the same\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
		}
		else
		{
			i++;
			Debug("FILE : %s not found", innerText);
			if (!FI)
			{
				FI[0] = (FileInfo*)malloc(sizeof(FileInfo));
				FI[0]->Path = (char *)malloc(MAX_PATH);
				memset(FI[0]->Path, 0, MAX_PATH);
				strcpy(FI[0]->Path, path4);
				FI[0]->File = (char *)malloc(MAX_PATH);
				memset(FI[0]->File, 0, MAX_PATH);
				strcpy(FI[0]->File, innerText);
				FI[0]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
				FI[0]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
				FI[0]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
				FI[0]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
				FI[0]->SetPackage = true;
				FI[0]->Package = (char *)malloc(7);
				memset(FI[0]->Package, 0, 7);
				sprintf(FI[0]->Package, "Tracks");
				FI[0]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
				tos = true;
			}
			else
			{
				FI[i - 1] = (FileInfo*)malloc(sizeof(FileInfo));
				FI[i - 1]->Path = (char *)malloc(MAX_PATH);
				memset(FI[i - 1]->Path, 0, MAX_PATH);
				strcpy(FI[i - 1]->Path, path4);
				FI[i - 1]->File = (char *)malloc(MAX_PATH);
				memset(FI[i - 1]->File, 0, MAX_PATH);
				strcpy(FI[i - 1]->File, innerText);
				FI[i - 1]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
				FI[i - 1]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
				FI[i - 1]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
				FI[i - 1]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
				FI[i - 1]->SetPackage = true;
				FI[i - 1]->Package = (char *)malloc(7);
				memset(FI[i - 1]->Package, 0, 7);
				sprintf(FI[i - 1]->Package, "Tracks");
				FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
				tos = true;
			}
		}
		delete[] innerText;
		delete[] File;
		delete[] str;
	}

	tos = false;

	if (FullDownload)
	{
		sprintf(ServerPath, "%s/TracksHigh", param->ServerPath);
		sprintf(FilePath, "%s/index.xml", ServerPath);
		IndexCharFile = Downloader::Get().GetIndexFile(FilePath);

		if (sizeof(IndexCharFile) < NULL)
			Error("Error retrieving the index.xml file");

		indexFile.Parse(IndexCharFile);
		free(IndexCharFile);

		sprintf(path1, "%s\\TracksHigh\\", path);

		if (!Utils::FileExists(path1))
			CreateDirectory(path1, NULL);

		ShardInfo = indexFile.FirstChildElement("index")->FirstChildElement("fileinfo");

		Launcher::Get().SetPosProgressBar(0, 40);

		for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
		{
			char *path3 = new char;
			char path4[MAX_PATH] = { 0 };
			char *innerText = new char[MAX_PATH];
			char *File = new char[MAX_PATH];
			char *str = new char[10];
			char *Hash = new char[24];
			char *Hash1 = new char;
			char *Hash2 = new char;
			char *Hash3 = new char;
			char *HashSvr = new char;

			if (tos)
			{
				FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
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
					Debug("FILE : %s Hash is not the same\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
					i++;
					if (!FI)
					{
						FI[0] = (FileInfo*)malloc(sizeof(FileInfo));
						FI[0]->Path = (char *)malloc(MAX_PATH);
						memset(FI[0]->Path, 0, MAX_PATH);
						strcpy(FI[0]->Path, path4);
						FI[0]->File = (char *)malloc(MAX_PATH);
						memset(FI[0]->File, 0, MAX_PATH);
						strcpy(FI[0]->File, innerText);
						FI[0]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
						FI[0]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
						FI[0]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
						FI[0]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
						FI[0]->SetPackage = true;
						FI[0]->SetPackage = true;
						FI[0]->Package = (char *)malloc(11);
						memset(FI[0]->Package, 0, 11);
						sprintf(FI[0]->Package, "TracksHigh");
						FI[0]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
						tos = true;
					}
					else
					{
						FI[i - 1] = (FileInfo*)malloc(sizeof(FileInfo));
						FI[i - 1]->Path = (char *)malloc(MAX_PATH);
						memset(FI[i - 1]->Path, 0, MAX_PATH);
						strcpy(FI[i - 1]->Path, path4);
						FI[i - 1]->File = (char *)malloc(MAX_PATH);
						memset(FI[i - 1]->File, 0, MAX_PATH);
						strcpy(FI[i - 1]->File, innerText);
						FI[i - 1]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
						FI[i - 1]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
						FI[i - 1]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
						FI[i - 1]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
						FI[i - 1]->SetPackage = true;
						FI[i - 1]->Package = (char *)malloc(11);
						memset(FI[i - 1]->Package, 0, 11);
						sprintf(FI[i - 1]->Package, "TracksHigh");
						FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
						tos = true;
					}
				}
				else
					Debug("FILE : %s Hash is the same\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
			}
			else
			{
				i++;
				Debug("FILE : %s not found", innerText);
				if (!FI)
				{
					FI[0] = (FileInfo*)malloc(sizeof(FileInfo));
					FI[0]->Path = (char *)malloc(MAX_PATH);
					memset(FI[0]->Path, 0, MAX_PATH);
					strcpy(FI[0]->Path, path4);
					FI[0]->File = (char *)malloc(MAX_PATH);
					memset(FI[0]->File, 0, MAX_PATH);
					strcpy(FI[0]->File, innerText);
					FI[0]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
					FI[0]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
					FI[0]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
					FI[0]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
					FI[0]->SetPackage = true;
					FI[0]->Package = (char *)malloc(11);
					memset(FI[0]->Package, 0, 11);
					sprintf(FI[0]->Package, "TracksHigh");
					FI[0]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
					tos = true;
				}
				else
				{
					FI[i - 1] = (FileInfo*)malloc(sizeof(FileInfo));
					FI[i - 1]->Path = (char *)malloc(MAX_PATH);
					memset(FI[i - 1]->Path, 0, MAX_PATH);
					strcpy(FI[i - 1]->Path, path4);
					FI[i - 1]->File = (char *)malloc(MAX_PATH);
					memset(FI[i - 1]->File, 0, MAX_PATH);
					strcpy(FI[i - 1]->File, innerText);
					FI[i - 1]->Section = atoi(ShardInfo->FirstChildElement("section")->GetText());
					FI[i - 1]->Offset = atoi(ShardInfo->FirstChildElement("offset")->GetText());
					FI[i - 1]->Lenght = atoi(ShardInfo->FirstChildElement("length")->GetText());
					FI[i - 1]->Compressed = atoi(ShardInfo->FirstChildElement("compressed")->GetText());
					FI[i - 1]->SetPackage = true;
					FI[i - 1]->Package = (char *)malloc(11);
					memset(FI[i - 1]->Package, 0, 11);
					sprintf(FI[i - 1]->Package, "TracksHigh");
					FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
					tos = true;
				}
			}
			delete[] innerText;
			delete[] File;
			delete[] str;
		}
	}
	
	for (short s = 0; s < i; s++)
	{
		if (FI[s]->ToSection == 0)
		{
			FI[s]->ToSection = FI[s]->Section;
			if (FI[s]->SetPackage)
				Debug("Verify : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\nPackage : %s\n",
					FI[s]->Path, FI[s]->File, FI[s]->Section, FI[s]->ToSection, FI[s]->Offset, FI[s]->Lenght, FI[s]->Compressed, FI[s]->Package);
			else
				Debug("Verify : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\nPackage : \n",
					FI[s]->Path, FI[s]->File, FI[s]->Section, FI[s]->ToSection, FI[s]->Offset, FI[s]->Lenght, FI[s]->Compressed);
		}
		else
		{
			if (FI[s]->SetPackage)
				Debug("Verify : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\nPackage : %s\n",
					FI[s]->Path, FI[s]->File, FI[s]->Section, FI[s]->ToSection, FI[s]->Offset, FI[s]->Lenght, FI[s]->Compressed, FI[s]->Package);
			else
				Debug("Verify : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\nPackage : \n",
					FI[s]->Path, FI[s]->File, FI[s]->Section, FI[s]->ToSection, FI[s]->Offset, FI[s]->Lenght, FI[s]->Compressed);
		}
	}

	if (i == 0)
	{
		Launcher::Get().SetPosProgressBar(0, 70);
		Launcher::Get().StartGameEnable();
	}
	else
	{
		DownloadThread *DT = new DownloadThread;

		DT->Path = (char *)malloc(MAX_PATH);
		strcpy(DT->Path, path);
		DT->i = i;
		DT->FI = (FileInfo**)malloc(sizeof(FileInfo)* 100);
		memcpy(&DT->FI, &FI, sizeof(sizeof(FileInfo)* 100));
		DT->Url = (char *)malloc(256);
		strcpy(DT->Url, param->ServerPath);

		Launcher::Get().SetPosProgressBar(0, 50);

		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Downloader::Get().Download, (LPVOID)DT, 0, 0);
	}
	delete path1;
	delete path;
	delete param->ServerPath;
	delete param->Package;
	delete param;
	delete[] languagePackage;
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

Downloader& Downloader::Get()
{
	return *instance;
}