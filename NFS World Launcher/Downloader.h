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

class DownloadCommandArgument
{
public:
	char *Package;

	char *PatchPath;

	DownloadCommandArgument(char *package, char *patchPath)
	{
		DownloadCommandArgument::Package = package;
		DownloadCommandArgument::PatchPath = patchPath;
	}
};

class Downloader
{
protected:
	HANDLE Thread;
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

	void StartVerificationAndDownload(VerifyCommandArgument parameters);

	void VerificationAndDownload();

	char * GetHash(char *File);

	static inline void SetVerifying(bool verifying){ Downloader D;  D.Verifying = verifying; }

	tinyxml2::XMLDocument GetIndexFile(char * url, bool useCache);

};




