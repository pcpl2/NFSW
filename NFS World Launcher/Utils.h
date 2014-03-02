class Utils
{
private:

public:

	static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);
	static size_t WriteDataCallback(void *ptr, size_t size, size_t nmemb, FILE *stream);
	static bool FileExists(const char *szPath);
};