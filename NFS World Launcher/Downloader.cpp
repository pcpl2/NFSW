#include <includes.h>

Downloader::Downloader()
{

}

Downloader::~Downloader()
{


}

void Download(DownloadThread *DT)
{
	//http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client/Section1075.dat nfsw.exe

	short i = DT->i;
	char *DefaultTempPath = new char[MAX_PATH];
	char *TempPath = new char[MAX_PATH];

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
		char *TempPathFile = new char[MAX_PATH];
		char Url[256];
		size_t result;
		int FromSection;
		int val2 = 1;
		int num7;

		sprintf(TempPathFile, "%s\\%s\\", TempPath, DT->FI[s]->File);
		if (!Utils::FileExists(TempPathFile))
			CreateDirectory(TempPathFile, NULL);

		Debug("Download : \npath : %s\nfile : %s\nSection : %d\noffset : %d\nlength : %d\ncompressed : %d\n", DT->FI[s]->Path, DT->FI[s]->File, DT->FI[s]->Section, DT->FI[s]->Offset, DT->FI[s]->Lenght, DT->FI[s]->Compressed);
		
		num7 = DT->FI[s+1]->Section;
		FromSection = DT->FI[s]->Section;
		val2 = FromSection;
		if (DT->FI[s]->Offset == 0)
			--num7;

		for (; val2 <= num7; ++val2)
		{
			sprintf(DatFile, "Section%d.dat", val2);
			Debug("Section%d.dat", val2);

			if (DT->FI[s]->LanguagePackage)
			{
				sprintf(Url, "http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client/%s", DatFile);
			}
			else
			{
				sprintf(Url, "http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client/en/%s", DatFile);
			}

			sprintf(DatFileInTemp, "%s%s", TempPathFile, DatFile);
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
			
		}

		delete DatFile;

		if (strcmp(DT->FI[s]->Path, "") != 0)
			sprintf(File, "%s\\%s\\%s", DT->Path, DT->FI[s]->Path, DT->FI[s]->File);
		else
			sprintf(File, "%s\\%s", DT->Path, DT->FI[s]->File);

		SizeT srcLen = DT->FI[s]->Compressed - 13;

		SizeT destLen = DT->FI[s]->Lenght;

		unsigned char* Decomress = new unsigned char[srcLen];

		unsigned char* Lzma = new unsigned char[srcLen];

		fp = fopen(DatFileInTemp, "rb");

		result = fread(Lzma, 1, destLen, fp);

		if (result != destLen)
			Debug("Reading error");
		

		//props = (Byte)((p->pb * 5 + p->lp) * 9 + p->lc);
		
		unsigned char prop[5] = { 0 };

		memcpy(prop, fp, 5);
		
		fclose(fp);
		int ok = LzmaUncompress(Decomress, &srcLen, Lzma, &destLen, prop, LZMA_PROPS_SIZE);

		fp = fopen(File, "w+");

		fwrite(Decomress, 1, srcLen, fp);

		fclose(fp);

		delete DatFileInTemp;
		delete File;

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

void Verify(VerifyArgument *param)
{
	// add http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client/en/index.xml  //en ; de ; es ; fr ; ru ; 
	USING_NAMESPACE(CryptoPP)
	USING_NAMESPACE(Weak1)

	Downloader::SetVerifying(true);
	char ServerPath[128] = { 0 };
	char languagePackage[2] = { 0 };
	char FilePath[128];
	char *path = new char[MAX_PATH];
	char *path1 = new char[MAX_PATH];
	bool FullDownload = param->FullDownload;
	Downloader D;

	sprintf(ServerPath, "%s/", param->ServerPath);

	if (sizeof(param->Package) > NULL)
		memcpy(languagePackage, param->Package, 5);

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

	if (sizeof(languagePackage) > 0)
		sprintf(path1, "%s\\%s", path, languagePackage);

	if (!Utils::FileExists(path1))
		CreateDirectory(path1, NULL);

	tinyxml2::XMLElement* ShardInfo = indexFile.FirstChildElement("index")->FirstChildElement("fileinfo");
	FileInfo **FI = (FileInfo**)malloc(sizeof(FileInfo*) * 100);
	short i = 0;

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
				}
			}

			delete[] innerText;
			delete[] File;
			delete[] str;
		}
	}

	for (short s = 0; s < i; s++)
	{
		Debug("Verify : \npath : %s\nfile : %s\nSection : %d\noffset : %d\nlength : %d\ncompressed : %d\n", FI[s]->Path, FI[s]->File, FI[s]->Section, FI[s]->Offset, FI[s]->Lenght, FI[s]->Compressed);
	}

	DownloadThread *DT = new DownloadThread;

	DT->Path = (char *)malloc(MAX_PATH);
	strcpy(DT->Path, path);
	DT->i = i;
	DT->FI = (FileInfo**)malloc(sizeof(FileInfo)* 100);
	memcpy(&DT->FI, &FI, sizeof(sizeof(FileInfo)* 100));

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Download, (LPVOID)DT, 0, 0);

	delete path1;
	delete path;
	delete param;
}

void Downloader::StartVerificationAndDownload(bool FullD)
{
	VerifyArgument *Vparam = new VerifyArgument;

	Vparam->ServerPath = "http://static.cdn.ea.com/blackbox/u/f/NFSWO/1594/client";
	Vparam->Package = "en"; //en ; de ; es ; fr ; ru ; 
	Vparam->FullDownload = FullD;

	Info("Starting verify");
	ThreadV = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Verify, Vparam, 0, 0);

	time_t timeraw = time(NULL);
	struct tm * Timeinfo = localtime(&timeraw);

	//sprintf(DownloaderStartTime, "%d/%d/%d %d:%d:%d", Timeinfo->tm_mon, Timeinfo->tm_mday, Timeinfo->tm_year, Timeinfo->tm_hour, Timeinfo->tm_min, Timeinfo->tm_sec);
	//DownloadManager.ServerPath = parameters.ServerPath;
	//DownloadCommandArgument downloadCommandArgument = new DownloadCommandArgument(parameters.Package, parameters.PatchPath);
	//ThreadD = CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Download, (LPVOID)&downloadCommandArgument, 0, 0);
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
