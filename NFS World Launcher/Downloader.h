#pragma once

struct VerifyArgument
{
	char *ServerPath;
	char *Package;
	bool FullDownload;
};

struct FileInfo
{
	char *Path;
	char *File;
	char *Package;
	int Section;
	int ToSection;
	int Offset;
	int Lenght;
	int Compressed;
	bool SetPackage;
};

struct DownloadThread
{
	short i;
	FileInfo **FI;
	char *Path;
	char *Url;
};

class Downloader
{
protected:
	HANDLE ThreadV;
private:
	static Downloader *instance;

public:
	Downloader();
	~Downloader();

	static Downloader& Get();

	void StartVerificationAndDownload(bool FullD, char *Package, char *ServerPath);

	static void Download(DownloadThread *DT);
	static void Verify(VerifyArgument *param);

	char * GetIndexFile(char *url);
};




