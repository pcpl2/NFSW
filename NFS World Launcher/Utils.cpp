#include <includes.h>
#ifdef WIN32
#include <io.h>
#else
#include <unistd.h>
#endif
#include <sys/stat.h>

size_t Utils::WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
	size_t realsize = size * nmemb;

	struct BufferStruct * mem = (struct BufferStruct *) data;

	mem->buffer = (char*)realloc(mem->buffer, mem->size + realsize + 1);

	if (mem->buffer)
	{
		memcpy(&(mem->buffer[mem->size]), ptr, realsize);
		mem->size += realsize;
		mem->buffer[mem->size] = 0;
	}
	return realsize;
}

int Utils::FileExists(const char *szPath)
{
#ifdef WIN32
	return !access(szPath, 0);   
#else
	return !access(szPath, F_OK);   
#endif

}

size_t Utils::WriteDataCallback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
	size_t written;
	written = fwrite(ptr, size, nmemb, stream);
	return written;
}