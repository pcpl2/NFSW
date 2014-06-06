#pragma once
class Utils
{
public:
	struct BufferStruct
	{
		char *buffer;
		size_t size;
	};

	class Timer
	{
	private:
		DWORD Start;
		DWORD Finish;
	public:
		void start() {
			Start = Finish = timeGetTime();
		}
		void stop() {
			Finish = timeGetTime();
		}
		DWORD elapsed() {
			return Finish - Start;
		}
	};


	static size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data);
	static size_t WriteDataCallback(void *ptr, size_t size, size_t nmemb, FILE *stream);
	static int FileExists(const char *szPath);
};