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

struct VerifyP
{
	char *ServerUrl;
	bool SetPackage;
	char *Package;
	char *PackageDir;
	short PackageSize;
	short Thread;
};

struct DownloadThread
{
	char *Path;
	char *Url;
};

class Downloader
{
protected:
	HANDLE ThreadV;
private:
	static Downloader *instance;
	static short FI_I;
	HANDLE Mutex;
public:
	Downloader();
	~Downloader();

	static Downloader& Get();

	void StartVerificationAndDownload(bool FullD, char *Package, char *ServerPath);

	static void Download(DownloadThread *DT);
	static void Verify(VerifyArgument *param);
	static void VerifyProcess(VerifyP *VP);

	static short AddFileToFI(char *Path, char *File, int Section, int Offset, int Lenght, int Compressed, bool SetPackage, short PackageSize, char *Package, int ToSection);
	static void ChangeToSectionInFI(short i, int ToSection);

	void LockMutex();
	void UnlockMutex();

	char * GetIndexFile(char *url);
};