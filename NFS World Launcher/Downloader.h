#pragma once

struct VerifyCommandArgument
{
	char *ServerPath;
	char *Package;
	char *PatchPath;
	bool StopOnFail;
	bool ClearHashes;
	bool WriteHashes;
	bool Download;
};

struct FileInfo
{
	char *path;
	char *file;
	int section;
	int offset;
	int lenght;
	int compressed;
};

struct DownloadThread
{
	short i;
	FileInfo **FI;
	char *path;
};

class Downloader
{
protected:
	HANDLE ThreadV;
	HANDLE ThreadD;
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

	int HashThreads;
	
	/*
	const int LZMAOutPropsSize = 5;
	const int LZMALengthSize = 8;
	const int LZMAHeaderSize = 13;
	const int HashThreads = 3;
	const int DownloadThreads = 3;
	const int DownloadChunks = 16;
	*/
public:
	Downloader();
	~Downloader();

	bool StopFlag;

	void StartVerificationAndDownload();

	static inline void SetVerifying(bool verifying){ Downloader D;  D.Verifying = verifying; }

	char * GetIndexFile(char * url, bool useCache);

};




