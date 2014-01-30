/////////////////////////////////////////////////////////
//
//	SDMP @ logger.h
//  Author: Eryk Dwornicki
//
/////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdarg.h>
#include <time.h> 

#pragma once

enum ELogType
{
	LogType_Info,
	LogType_Error,
	LogType_Warning,
	LogType_Debug,

	LogTypes_Count
};

class Logger
{
private:
	static FILE * s_pLogFile;
public:
	static void Init(const char * szFile);

	static void Log(ELogType type, const char *szMessage, ...);

	static FILE * GetFile();
};

#define Info(s,...) Logger::Log(LogType_Info,s,__VA_ARGS__)
#define Error(s,...) Logger::Log(LogType_Error,s,__VA_ARGS__)
#define Warning(s,...) Logger::Log(LogType_Warning,s,__VA_ARGS__)
#ifdef DEBUG
#	define Debug(s,...) Logger::Log(LogType_Debug,s,__VA_ARGS__)
#else
#	define Debug(s,...)
#endif