/**************************************************************
 *
 * Solution  : MultiVice
 * Project   : Client Launcher
 * License   : See LICENSE file in top level directory
 * File	     : HTTP.cpp
 * Author(s) : RootKiller <rootkiller.programmer@gmail.com>
 *
 **************************************************************/
// NOTE: We need to add Linux support + some other stuffs like
// multi-task's (maybe just return task-id struct after make any request,
// and add to request handler task-id attribute - we need to think about that)
#include "HTTP.h"

HTTP::HTTP()
	: m_pfnReceiveHandler(0),
	  m_eState(HTTP_WAITING_FOR_HOST),
	  m_httpSocket(INVALID_SOCKET)
{
	// Client name
	strcpy(m_szClientName, DEFAULT_HTTP_CLIENT);
	
	// Setup socket
#ifdef _WIN32
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
#endif

	// Setup socket
	m_httpSocket = socket(AF_INET, SOCK_STREAM, 0);

	// Set socket non blocking
#ifdef _WIN32
    u_long arg=0;
    ioctlsocket(m_httpSocket, FIONBIO, &arg);
#else
	int flags = fcntl(m_httpSocket, F_GETFL, 0);
  	fcntl(m_httpSocket, F_SETFL, flags|O_NONBLOCK);
#endif
	
#ifdef _WIN32
	// Mutex
	m_hMutex = CreateMutex(NULL,FALSE,NULL);
	
	// Thread
	m_hThread = CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)HTTP::RecvThread, this, 0, NULL);

#else
	//Mutex
	pthread_mutex_init(&m_hMutex,  0);

	// Thread
	pthread_create(&m_hThread, 0, (void *(*)(void*))CHTTP::RecvThread, this);
#endif

	
}

HTTP::~HTTP()
{
	// Set state for shutdown
	m_eState = HTTP_SHUTDOWN;

	// Close socket (Wait for end of thread)
#ifdef _WIN32
	WaitForSingleObject(m_hThread,INFINITE);
	
	CloseHandle(m_hThread);
	
	CloseHandle(m_hMutex);
#else

	pthread_mutex_destroy(&m_hMutex);

	pthread_exit(&m_hThread);
#endif
	closesocket(m_httpSocket);
#ifdef _WIN32
	WSACleanup();
#endif
}

bool HTTP::_connect()
{
	// If our socket is valid just re-create it
	if(m_httpSocket != INVALID_SOCKET)
	{
		// Close current socket
		closesocket(m_httpSocket);

		// Setup socket
		m_httpSocket = socket(AF_INET, SOCK_STREAM, 0);
	}

	// Convert domain/ip to socket struct's.
	struct sockaddr_in remote;

	remote.sin_family = AF_INET;
	remote.sin_port = htons(m_usPort);

	LPHOSTENT hostEntry = gethostbyname(m_szHostName);
	if(!hostEntry)
	{
		unsigned int addr = inet_addr( m_szHostName );
        hostEntry = gethostbyaddr((char *)&addr, 4, AF_INET);
		
		if(!hostEntry)
			return false;
	}
#ifdef _WIN32
	remote.sin_addr.S_un.S_addr = *((int*)*hostEntry->h_addr_list);
#else
	remote.sin_addr.s_addr = *((int*)*hostEntry->h_addr_list);
#endif

	// Connect
	if(connect(m_httpSocket,(struct sockaddr*)&remote,sizeof(struct sockaddr)) == SOCKET_ERROR)
		return false;

	// Change httpclient state to waiting for next task
	m_eState = HTTP_WAITING_FOR_TASK;
	return true;
}

bool HTTP::setRequestServer(const char * szHostName, unsigned short usPort)
{
	// If we're waiting response return false because we can't connect to other server - it would
	// make some problems with receiving last response
	if(m_eState == HTTP_WAITING_RESPONSE)
		return false;

	// Grab our request server info
	strcpy(m_szHostName,szHostName);
	m_usPort = usPort;

	// Change httpclient state to waiting for next task
	m_eState = HTTP_WAITING_FOR_TASK;
	return true;
}

bool HTTP::get(const char * szPath)
{
	// Check current httpclient state
	if(m_eState == HTTP_WAITING_FOR_TASK)
	{		
		// Connect
		if(!_connect())
			return false;

		// Build GET request
		char szRequest[256];
		sprintf(szRequest,
			"GET %s HTTP/1.0\r\n" \
			"Host: %s\r\n" \
			"User-Agent: %s\r\n" \
			"Connection: Close\r\n\r\n", szPath, m_szHostName, m_szClientName);			

		// Change state for waiting reponse
		m_eState = HTTP_WAITING_RESPONSE;

		// Send builded request to a connected host
		if(send(m_httpSocket,szRequest,strlen(szRequest),0) < 0)
		{
			// If we cannot send it just wait for next task
			m_eState = HTTP_WAITING_FOR_TASK;		
			return false;
		}
		return true;
	}
	return false;
}

bool HTTP::post(const char * szPath, const char * szData, const char * szContentType /* = "text/plain" */ )
{
	// Check current httpclient state
	if(m_eState == HTTP_WAITING_FOR_TASK)
	{
		// Connect
		if(!_connect())
			return false;

		// Build GET request
		char szRequest[256];
		sprintf(szRequest,
			"POST %s HTTP/1.0\r\n" \
			"Host: %s\r\n" \
			"User-Agent: %s\r\n" \
			"Content-Type: %s\r\n" \
			"Content-Length: %d\r\n" \
			"Connection: Close\r\n\r\n" \
			"%s", szPath, m_szHostName, m_szClientName, szContentType, strlen(szData), szData);						
		
		// Change state for waiting reponse
		m_eState = HTTP_WAITING_RESPONSE;

		// Send builded request to a connected host
		if(send(m_httpSocket,szRequest,strlen(szRequest),0) < 0)
		{
			// If we cannot send it just wait for next task
			m_eState = HTTP_WAITING_FOR_TASK;		
			return false;
		}
		return true;
	}
	return false;
}

void HTTP::triggerHandler(char * szData, int iSize) 
{
	// Check state if we're not waiting response just leave this function
	if(m_eState != HTTP_WAITING_RESPONSE)
		// NOTE: Maybe add error /warning report here?
		return;

	// Call receive handler
	if(m_pfnReceiveHandler)
		m_pfnReceiveHandler(szData,iSize);		
}

void HTTP::RecvThread(void * pUser)
{
	// Grab CHTTP pointer
	HTTP * pHTTP = reinterpret_cast<HTTP*>(pUser);

	// While loop for process all http-client things
	while(pHTTP->getState() != HTTP_SHUTDOWN)
	{
		// Mutex OP #1
#ifdef _WIN32
		WaitForSingleObject(pHTTP->getMutex(),INFINITE);
#else
		pthread_mutex_lock(&pHTTP->getMutex());
#endif

		// Allocate char buffer array
		char szBuffer[4086];

		// Allocate buffer len
		int iBuffer = 0;

		// Loop for get each 512Bytes of received data
		while( ( iBuffer = recv(pHTTP->getSocket(),szBuffer,4086,0) ) > 0 )
		{
			// Make last buffer char as '0', e.g. prevent "strange" signs 
			szBuffer[iBuffer] = '\0';

			// Allocate parsed buffer
			char * szParsedBuffer = new char [iBuffer];
			memset(szParsedBuffer,0,sizeof(szParsedBuffer));

			// Some other variables
			bool bContent = false;
			int iAbs = 0;
			int iStart = 0;

			// Process headers (NOTE: just leave it - maybe add more expanded parsing to multi-threading option?)
			for(size_t i = 0; i < strlen(szBuffer); i++)
			{
				if(i > strlen(szBuffer)-1) break;

				if(bContent)
				{
					szParsedBuffer[iAbs] = szBuffer[i];
					iAbs ++;
				}

				if( ( (szBuffer[i] == '\r') && (szBuffer[i+1] == '\n') && (szBuffer[i+2] == '\r') && (szBuffer[i+3] == '\n') ) )				
				{
					i += 3;
					iStart = i;
					bContent = true;
				}
			}
			
			// Clear end of buffer for prevent strange signs
			szParsedBuffer[iBuffer-iStart-1] = '\0';

			// Trigger event handler
			pHTTP->triggerHandler(szParsedBuffer,iBuffer);

			// Clear `szParsedBuffer` but first check if it's allocated
			// NOTE: "Non-alloc" of szParsedBuffer is impossible but i think all crash preventions
			// are needed at all.
			if(szParsedBuffer){
				delete szParsedBuffer;
				szParsedBuffer = NULL;
			}
		}

		// Set state for waiting next task
		pHTTP->setState(HTTP_WAITING_FOR_TASK);		

		// Mutex OP #2
#ifdef _WIN32
		ReleaseMutex(pHTTP->getMutex());
#else
		pthread_mutex_unlock(&pHTTP->getMutex());
#endif
		Sleep(10);
	}
}
