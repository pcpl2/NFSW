/**************************************************************
 *
 * Solution  : MultiVice
 * Project   : Client Launcher
 * License   : See LICENSE file in top level directory
 * File	     : HTTP.h
 * Author(s) : RootKiller <rootkiller.programmer@gmail.com>
 *
 **************************************************************/
#pragma once

#include <string>
#include <cstring>

#include <stdio.h>

#ifdef _WIN32
#include <windows.h>
//#	include <winsock2.h>
#else
#	include <netdb.h>
#	include <netinet/in.h>
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <arpa/inet.h>
#	include <asm/ioctls.h>
#	include <linux/fs.h>
#	include <fcntl.h>
#endif

#define DEFAULT_HTTP_CLIENT "NFSW/1.0"
#define DEFAULT_HTTP_PORT 80

enum eHTTPState
{
	// Default
	HTTP_WAITING_FOR_HOST, //
	HTTP_WAITING_FOR_TASK, //
	HTTP_WAITING_RESPONSE, //!If http client waiting for post/get response


	// Our
	HTTP_SHUTDOWN //!If http client going to shutdown!
};

typedef void (*pfnReceiveHandler)(char *, int);

class HTTP
{
private:
#ifdef _WIN32
	HANDLE m_hThread;

	HANDLE m_hMutex;
#else
	pthread_t m_hThread;

	pthread_mutex_t m_hMutex;
#endif

	char m_szClientName[128];

	char m_szHostName[60];
	unsigned short m_usPort;

	eHTTPState m_eState;

	int m_httpSocket;

	pfnReceiveHandler m_pfnReceiveHandler;
	
	bool _connect();
public:
	HTTP();
	~HTTP();

	bool setRequestServer(const char * szHostName, unsigned short usPort = DEFAULT_HTTP_PORT);

	inline void setReceiveHandler(pfnReceiveHandler _pfnReceiveHandler) { m_pfnReceiveHandler = _pfnReceiveHandler; }

	inline void setClientName(const char * szClientName) { strcpy(m_szClientName,szClientName); }

	bool get(const char * szPath);

	bool post(const char * szPath, const char * szData, const char * szContentType = "text/plain");

	static void RecvThread(void * pHTTP);

	inline eHTTPState getState() { return m_eState; }
	inline void setState(eHTTPState state) { m_eState = state; }
	inline int getSocket() { return m_httpSocket; }

#if _WIN32
	inline HANDLE getMutex() { return m_hMutex; }
#else
	inline pthread_mutex_t getMutex() { return m_hMutex; }
#endif

	void triggerHandler(char * szData, int iSize);
};
