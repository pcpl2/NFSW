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
	int Section;
	int Offset;
	int Lenght;
	int Compressed;
	bool LanguagePackage;
};

struct DownloadThread
{
	short i;
	FileInfo **FI;
	char *Path;
};

class Downloader
{
protected:
	HANDLE ThreadV;
private:
	static Downloader instance;

	bool Verifying;
	bool DownloadQueueActive;
	bool Downloading;
	

	char *CurrentLocalVersion;
	char *CurrentServerVersion;
	char *DownloaderStartTime;

	long CompressedLength;
	long TotalToDownload;
	
	/*
	const int LZMAOutPropsSize = 5;
	const int LZMALengthSize = 8;
	const int LZMAHeaderSize = 13;
	*/
public:
	Downloader();
	~Downloader();

	void StartVerificationAndDownload(bool FullD);

	static inline void SetVerifying(bool verifying){ Downloader D;  D.Verifying = verifying; }

	char * GetIndexFile(char * url);

};




