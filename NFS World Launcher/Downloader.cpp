#include <includes.h>

Downloader::Downloader()
{

}

Downloader::~Downloader()
{


}

int Progress_Func(void* ptr, double TotalToDownload, double NowDownloaded, double TotalToUpload, double NowUploaded)
{
	Debug("%d / %d", (int)NowDownloaded, (int)TotalToDownload);
	return 0;
}

void Download(DownloadThread *DT)
{
	short i = DT->i;
	char *DefaultTempPath = new char[MAX_PATH];
	char *TempPath = new char[MAX_PATH];

	GetTempPath(MAX_PATH, (LPSTR)DefaultTempPath);

	sprintf(TempPath, "%snfswl\\", DefaultTempPath);
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
		char *TempPathFile = new char[MAX_PATH];
		char Url[256];
		size_t result;
		int num7;

		sprintf(TempPathFile, "%s%i\\", TempPath, DT->FI[s]->Offset);
		if (!Utils::FileExists(TempPathFile))
			CreateDirectory(TempPathFile, NULL);

		Debug("Download : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\n", DT->FI[s]->Path, DT->FI[s]->File, DT->FI[s]->Section, DT->FI[s]->ToSection, DT->FI[s]->Offset, DT->FI[s]->Lenght, DT->FI[s]->Compressed);
		
		num7 = DT->FI[s]->ToSection;
		int val2 = DT->FI[s]->Section;

		if (DT->FI[s]->Offset == 0)
			--num7;

		for (; val2 <= num7; ++val2)
		{
			sprintf(DatFile, "section%d.dat", val2);
			Debug("section%d.dat", val2);

			if (DT->FI[s]->LanguagePackage)
			{
				sprintf(Url, "%s/%s/%s", DT->Url, DT->Package, DatFile);
			}
			else
			{
				sprintf(Url, "%s/%s", DT->Url, DatFile);
			}

			sprintf(DatFileInTemp, "%s%s", TempPathFile, DatFile);

			curl = curl_easy_init();

			fp = fopen(DatFileInTemp, "wb+");
			if (curl)
			{
				Debug("Downloading %s", Url);
				curl_easy_setopt(curl, CURLOPT_URL, Url);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Utils::WriteDataCallback);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
				curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
				curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Progress_Func);
				res = curl_easy_perform(curl);

				if (res != CURLE_OK)
					Error("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
				/* always cleanup */
				curl_easy_cleanup(curl);
					
			}
		}
		fclose(fp);
		delete DatFile;

		if (strcmp(DT->FI[s]->Path, "") != 0)
			sprintf(File, "%s\\%s\\%s", DT->Path, DT->FI[s]->Path, DT->FI[s]->File);
		else
			sprintf(File, "%s\\%s", DT->Path, DT->FI[s]->File);

		SizeT srcLen = DT->FI[s]->Compressed - 13;
		SizeT destLen = DT->FI[s]->Lenght;

//		for (int x = DT->FI[s]->Section; x <= num7; x++)
//		{
/*			fp = fopen(DatFileInTemp, "rb");
			if (NULL != fp)
			{
				Debug("Can open the file");
			}
			else
				Debug("Could not open the file");

			result = fread(Lzma, 1, srcLen, fp);

			if (result != srcLen)
				Debug("Reading error");

			fclose(fp);
	//	}*/
		/*int num3 = 0;
		while (num3 < DT->FI[s]->Compressed)
		{
			
		}*/

		unsigned char Prop[5];
		unsigned char Arr[13];
		std::vector<unsigned char> inBuf2;
		
		std::ifstream::pos_type size;
		std::ifstream file;
		file.open(DatFileInTemp, std::ios::in |std::ios::binary | std::ios::ate);

		size = file.tellg();
		std::vector<unsigned char> inBuf(size);
		file.seekg(0, std::ios::beg);
		file.read((char *)&inBuf[0], size);
		file.close();


		for (int index = 0; index < 13; index++)
			Arr[index] = (int)inBuf.at(DT->FI[s]->Offset + index);

		for (int index = DT->FI[s]->Offset + 13; index < DT->FI[s]->Offset + DT->FI[s]->Compressed; index++)
			inBuf2.push_back(inBuf[index]);

		for (int index = 0; index < 5; ++index)
			Prop[index] = Arr[index];
		long num6 = 0L;
		for (int index = 0; index < 8; ++index)
			num6 += (long)((int)Arr[index + 5] << 8 * index);
		unsigned char* Decomress = new unsigned char[destLen];

		int ok = LzmaUncompress(Decomress, &destLen, &inBuf2[0], &srcLen, Prop, LZMA_PROPS_SIZE);

		fp = fopen(File, "wb+");

		fwrite(Decomress, 1, destLen, fp);

		fclose(fp);

		delete DatFileInTemp;
		delete File;
	}

}

void Verify(VerifyArgument *param)
{
	//en ; de ; es ; fr ; ru ; 
	USING_NAMESPACE(CryptoPP)
	USING_NAMESPACE(Weak1)

	Downloader::SetVerifying(true);

	char *languagePackage = new char[3];
	char *path = new char[MAX_PATH];
	char *path1 = new char[MAX_PATH];
	char FilePath[128];
	char ServerPath[128] = { 0 };
	bool FullDownload = param->FullDownload;
	bool tos = false;
	short i = 0;
	FileInfo **FI = (FileInfo**)malloc(sizeof(FileInfo*)* 100);
	Downloader D;


	sprintf(ServerPath, "%s/", param->ServerPath);

	if (sizeof(param->Package) > NULL)
		memcpy(languagePackage, param->Package, 3);

	sprintf(FilePath, "%sindex.xml", ServerPath);
	char * IndexCharFile = D.GetIndexFile(FilePath);

	if (sizeof(IndexCharFile) < NULL)
	{
		Error("Error retrieving the index.xml file");
	}

	tinyxml2::XMLDocument indexFile;
	indexFile.Parse(IndexCharFile);
	free(IndexCharFile);

	sprintf(path, "%s\\Data", Launcher::GetGameDir());
	memcpy(path1, path, MAX_PATH);

	if (sizeof(languagePackage) > NULL)
		sprintf(path1, "%s\\%s", path, languagePackage);

	if (!Utils::FileExists(path1))
		CreateDirectory(path1, NULL);

	tinyxml2::XMLElement* ShardInfo = indexFile.FirstChildElement("index")->FirstChildElement("fileinfo");

	
	for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
	{
		char *path3 = 0;
		char path4[MAX_PATH] = { 0 };
		char *innerText = new char[MAX_PATH];
		char *File = new char[MAX_PATH];
		char *str = new char[10];
		char *Hash = 0;
		char *HashSvr = 0;
		MD5 md5;
		std::string HashOutput;

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
			FileSource f(File, true, new HashFilter(md5, new Base64Encoder(new StringSink(HashOutput))));
			Hash = const_cast<char*>(HashOutput.c_str());
			Hash[strlen(Hash) - 1] = '\0';
			HashSvr = const_cast<char*>(ShardInfo->FirstChildElement("hash")->GetText());

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
					FI[0]->LanguagePackage = false;
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
					FI[i - 1]->LanguagePackage = false;
					FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
					tos = true;
				}
			}
			else
			{
				Debug("FILE : %s Hash is the same\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
			}
		}
		else
		{
			i++;
			Debug("FILE : %s No file", innerText);
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
				FI[0]->LanguagePackage = false;
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
				FI[i - 1]->LanguagePackage = false;
				FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
				tos = true;
			}
		}

		delete[] innerText;
		delete[] File;
		delete[] str;
	}

	if (sizeof(languagePackage) > NULL)
		sprintf(ServerPath, "%s/%s", param->ServerPath, languagePackage);

	sprintf(FilePath, "%s/index.xml", ServerPath);
	IndexCharFile = D.GetIndexFile(FilePath);

	if (sizeof(IndexCharFile) < NULL)
	{
		Error("Error retrieving the index.xml file");
	}

	indexFile.Parse(IndexCharFile);
	free(IndexCharFile);

	if (sizeof(languagePackage) > NULL)
		sprintf(path1, "%s\\Sound\\Speech", path);

	if (!Utils::FileExists(path1))
		CreateDirectory(path1, NULL);

	ShardInfo = indexFile.FirstChildElement("index")->FirstChildElement("fileinfo");

	for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
	{
		char *path3 = 0;
		char path4[MAX_PATH] = { 0 };
		char *innerText = new char[MAX_PATH];
		char *File = new char[MAX_PATH];
		char *str = new char[10];
		char *Hash = 0;
		char *HashSvr = 0;
		MD5 md5;
		std::string HashOutput;

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
			FileSource f(File, true, new HashFilter(md5, new Base64Encoder(new StringSink(HashOutput))));
			Hash = const_cast<char*>(HashOutput.c_str());
			Hash[strlen(Hash) - 1] = '\0';
			HashSvr = const_cast<char*>(ShardInfo->FirstChildElement("hash")->GetText());

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
					FI[0]->LanguagePackage = true;
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
					FI[i - 1]->LanguagePackage = true;
					FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
					tos = true;
				}
			}
			else
			{
				Debug("FILE : %s Hash is the same\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
			}
		}
		else
		{
			i++;
			Debug("FILE : %s No file", innerText);
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
				FI[0]->LanguagePackage = true;
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
				FI[i - 1]->LanguagePackage = true;
				FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
				tos = true;
			}
		}

		delete[] innerText;
		delete[] File;
		delete[] str;
	}

	sprintf(ServerPath, "%s/Tracks", param->ServerPath);

	sprintf(FilePath, "%s/index.xml", ServerPath);
	IndexCharFile = D.GetIndexFile(FilePath);

	if (sizeof(IndexCharFile) < NULL)
	{
		Error("Error retrieving the index.xml file");
	}

	indexFile.Parse(IndexCharFile);
	free(IndexCharFile);

	sprintf(path1, "%s\\Tracks\\", path);

	if (!Utils::FileExists(path1))
		CreateDirectory(path1, NULL);

	ShardInfo = indexFile.FirstChildElement("index")->FirstChildElement("fileinfo");

	for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
	{
		char *path3 = 0;
		char path4[MAX_PATH] = { 0 };
		char *innerText = new char[MAX_PATH];
		char *File = new char[MAX_PATH];
		char *str = new char[10];
		char *Hash = 0;
		char *HashSvr = 0;
		MD5 md5;
		std::string HashOutput;

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
			FileSource f(File, true, new HashFilter(md5, new Base64Encoder(new StringSink(HashOutput))));
			Hash = const_cast<char*>(HashOutput.c_str());
			Hash[strlen(Hash) - 1] = '\0';
			HashSvr = const_cast<char*>(ShardInfo->FirstChildElement("hash")->GetText());

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
					FI[0]->LanguagePackage = false;
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
					FI[i - 1]->LanguagePackage = false;
					FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
					tos = true;
				}
			}
			else
			{
				Debug("FILE : %s Hash is the same\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
			}
		}
		else
		{
			i++;
			Debug("FILE : %s No file", innerText);
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
				FI[0]->LanguagePackage = false;
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
				FI[i - 1]->LanguagePackage = false;
				FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
				tos = true;
			}
		}

		delete[] innerText;
		delete[] File;
		delete[] str;
	}

	if (FullDownload)
	{
		sprintf(ServerPath, "%s/TracksHigh", param->ServerPath);

		sprintf(FilePath, "%s/index.xml", ServerPath);
		IndexCharFile = D.GetIndexFile(FilePath);

		if (sizeof(IndexCharFile) < NULL)
		{
			Error("Error retrieving the index.xml file");
		}

		indexFile.Parse(IndexCharFile);
		free(IndexCharFile);

		sprintf(path1, "%s\\TracksHigh\\", path);

		if (!Utils::FileExists(path1))
			CreateDirectory(path1, NULL);

		ShardInfo = indexFile.FirstChildElement("index")->FirstChildElement("fileinfo");

		for (ShardInfo; ShardInfo; ShardInfo = ShardInfo->NextSiblingElement())
		{
			char *path3 = 0;
			char path4[MAX_PATH] = { 0 };
			char *innerText = new char[MAX_PATH];
			char *File = new char[MAX_PATH];
			char *str = new char[10];
			char *Hash = 0;
			char *HashSvr = 0;
			MD5 md5;
			std::string HashOutput;

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
				FileSource f(File, true, new HashFilter(md5, new Base64Encoder(new StringSink(HashOutput))));
				Hash = const_cast<char*>(HashOutput.c_str());
				Hash[strlen(Hash) - 1] = '\0';
				HashSvr = const_cast<char*>(ShardInfo->FirstChildElement("hash")->GetText());

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
						FI[0]->LanguagePackage = false;
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
						FI[i - 1]->LanguagePackage = false;
						FI[i - 1]->ToSection = atoi(ShardInfo->FirstChildElement("section")->GetText());
						tos = true;
					}
				}
				else
				{
					Debug("FILE : %s Hash is the same\nHash: %s\nHashSvr: %s", innerText, Hash, HashSvr);
				}
			}
			else
			{
				i++;
				Debug("FILE : %s No file", innerText);
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
					FI[0]->LanguagePackage = false;
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
					FI[i - 1]->LanguagePackage = false;
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
		Debug("Verify : \npath : %s\nfile : %s\nSection : %d\nToSection : %d\noffset : %d\nlength : %d\ncompressed : %d\n", FI[s]->Path, FI[s]->File, FI[s]->Section, FI[s]->ToSection,  FI[s]->Offset, FI[s]->Lenght, FI[s]->Compressed);
	}

	DownloadThread *DT = new DownloadThread;

	DT->Path = (char *)malloc(MAX_PATH);
	strcpy(DT->Path, path);
	DT->Package = (char *)malloc(3);
	strcpy(DT->Package, languagePackage);
	DT->i = i;
	DT->FI = (FileInfo**)malloc(sizeof(FileInfo)* 100);
	memcpy(&DT->FI, &FI, sizeof(sizeof(FileInfo)* 100));
	DT->Url = (char *)malloc(256);
	strcpy(DT->Url, param->ServerPath);
	

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Download, (LPVOID)DT, 0, 0);

	delete path1;
	delete path;
	delete param;
	delete[] languagePackage;
}

void Downloader::StartVerificationAndDownload(bool FullD, char* Package, char* ServerPath)
{
	VerifyArgument *Vparam = new VerifyArgument;

	Vparam->ServerPath = (char *)malloc(256);
	memcpy(Vparam->ServerPath, ServerPath, 256);
	Vparam->Package = (char *)malloc(3);
	strcpy(Vparam->Package, Package);//en ; de ; es ; fr ; ru ; 
	Vparam->FullDownload = FullD;

	Info("Starting verify");
	ThreadV = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Verify, Vparam, 0, 0);

}

char *Downloader::GetIndexFile(char * url)
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
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, Progress_Func);

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
