class Utils
{
public:

	struct BufferStruct
	{
		char * buffer;
		size_t size;
	};

	static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);
	static size_t WriteDataCallback(void *ptr, size_t size, size_t nmemb, FILE *stream);
	static int FileExists(const char *szPath);
};