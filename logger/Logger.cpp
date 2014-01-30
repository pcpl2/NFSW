/////////////////////////////////////////////////////////
//
//	SDMP @ logger.cpp
//  Author: Eryk Dwornicki
//
/////////////////////////////////////////////////////////
#include <Logger.h>

FILE * Logger::s_pLogFile = 0;

void Logger::Init(const char *szFile)
{
	if(!s_pLogFile)
	{		
		if(s_pLogFile = fopen(szFile,"w+"))
		{		
			Info("Logger started!");
		} 
#ifdef DEBUG
		else
			printf("[FATAL ERROR] Cannot initialize logger (%s)\n", szFile);
#endif
	} else
		Error("Cannot initialize logger two times!");
}

void Logger::Log(ELogType type, const char *msg, ...)
{
	if(!s_pLogFile)
	{
#ifdef DEBUG
		printf("[FATAL ERROR] Cannot use Logger::Log - no log file set!\n");
#endif
		return;
	}

	va_list args;
	va_start(args,msg);

	time_t timeraw = time(NULL);
	struct tm * pTimeinfo = localtime(&timeraw);

	fprintf(s_pLogFile, "[%02d:%02d:%02d] ", pTimeinfo->tm_hour, pTimeinfo->tm_min, pTimeinfo->tm_sec);
#ifdef DEBUG
	printf("[%02d:%02d:%02d] ", pTimeinfo->tm_hour, pTimeinfo->tm_min, pTimeinfo->tm_sec);
#endif
	switch(type)
	{
	case LogType_Error:
		{
			fputs("<error> ",s_pLogFile);
#ifdef DEBUG
			printf("<error> ");
#endif
			break;
		}
	case LogType_Warning:
		{
			fputs("<warning> ",s_pLogFile);
#ifdef DEBUG
			printf("<warning> ");
#endif
			break;
		}
	case LogType_Debug:
		{
			fputs("<debug> ",s_pLogFile);
#ifdef DEBUG
			printf("<debug> ");
#endif
			break;
		}
	default:
		{
			fputs("<info> ",s_pLogFile);
#ifdef DEBUG
			printf("<info> ");
#endif
			break;
		}
	}	
	vfprintf(s_pLogFile, msg, args);
	fputc('\n', s_pLogFile);
#ifdef DEBUG
	vprintf(msg, args);
	printf("\n");
#endif
	va_end(args);
	fflush(s_pLogFile);
}

FILE * Logger::GetFile()
{
	return s_pLogFile;
}