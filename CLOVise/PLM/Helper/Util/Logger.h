#pragma once
/*
* Copyright 2020-2021 CLO-Vise. All rights reserved
*
* @file Logger.h
*
* @brief Class contains all logger functions wich are necessary by CLOVise to prins information in logs.
*
* @author GoVise
*
* @date 27-MAY-2020
*/
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <time.h>

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QTextStream>
#include <QFile>
#include "qfileinfo.h"

#include "CLOVise/PLM/Libraries/json.h"
#include "CLOVise/PLM/Helper/Util/Configuration.h"
#include "CLOVise/PLM/Helper/Util/DirectoryUtil.h"
#include "CLOVise/PLM/Helper/Util/Definitions.h"

using namespace std;
using json = nlohmann::json;

namespace Logger
{
	/*
	* Description - ConvertBytesToMB() method used to convert bytes to MB.
	* Parameter -  int.
	* Exception -
	* Return - long.
	*/
	inline long ConvertBytesToMB(int _bytes)
	{
		long long MegaBytes = 0;
		MegaBytes = _bytes / (1024 * 1000);
		return MegaBytes;
	}

	/*
	* Description - GetCurrentDateTime() method used to returns current Date & Time in string format.
	* Parameter - 
	* Exception -
	* Return - string.
	*/
	inline string GetCurrentDateTime()
	{
		time_t now = time(0);
		struct tm  time_struct;
		char  buffer[80];
		time_struct = *localtime(&now);
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &time_struct); //@To-Do Declare Date Format in Definitions and use here
		return string(buffer);
	}

	/*
	* Description - GetTimeStamp() method used to returns get time stamp in string format.
	* Parameter -
	* Exception -
	* Return - string.
	*/
	inline string GetTimeStamp()
	{
		time_t time0;
		time(&time0);
		struct tm  time_struct;
		char buffer[80];
		time_struct = *localtime(&time0);
		strftime(buffer, sizeof(buffer), "%H-%M-%S-%Y-%m-%d", &time_struct);
		return string(buffer);
	}


	/*
	* Description - CreateLogFileName() method used to creating log file name.
	* Parameter -  string.
	* Exception -
	* Return - 
	*/
	inline void CreateLogFileName(string _fileName)
	{
		string  newDirPath = DirectoryUtil::GetLogFilePath();
		string LOGPATH = newDirPath + _fileName + GetTimeStamp() + TEXT_FILE_EXTENSION;
		Configuration::GetInstance()->SetLogFileName(LOGPATH);
	}

	/*
	* Description - GetFileSize() method used to get file size.
	* Parameter -  string.
	* Exception -
	* Return - size_t.
	*/
	static size_t GetFileSize(const std::string& _path)
	{
		std::streampos begin, end;
		std::ifstream myFile(_path.c_str(), std::ios::binary);
		begin = myFile.tellg();
		myFile.seekg(0, std::ios::end);
		end = myFile.tellg();
		myFile.close();
		return ((end - begin) > 0) ? (end - begin) : 0;
	}

	/*
	* Description - Logger() method used to writes log messages to logger file.
	* Parameter -  string.
	* Exception -
	* Return - 
	*/
	inline void Logger(string _loggerMessage)
	{
		if (Configuration::GetInstance()->GetLoggerEnabled())
		{
			string logPath = Configuration::GetInstance()->GetLogFileName();
			long int logFilesize = GetFileSize(logPath);
			int logFilesizeInMB = ConvertBytesToMB(logFilesize);
			if (logFilesizeInMB > Configuration::GetInstance()->GetMaxLogFileSize())
			{
				CreateLogFileName("CLO-Vise_Log");
				logPath = Configuration::GetInstance()->GetLogFileName();
			}
			std::ofstream logfileopen(logPath.c_str(), std::ios_base::out | std::ios_base::app);
			string now = GetCurrentDateTime();
			logfileopen << now << '\t' << _loggerMessage << '\n';
			logfileopen.close();
		}
	}

	/*
	* Description - CreateLogFileName() method used to creating log file name.
	* Parameter - 
	* Exception -
	* Return -
	*/
	inline void CreateLogFileName()
	{
		DirectoryUtil::CreateDirectories();
		string  newDirPath = DirectoryUtil::GetLogFilePath();
		string LOGPATH = newDirPath + "CLO-Vise_Log" + GetTimeStamp() + TEXT_FILE_EXTENSION;
		Configuration::GetInstance()->SetLogFileName(LOGPATH);
	}
	
	/*
	* Description - Debug() method used to writes debug log messages to logger file.
	* Parameter -  string.
	* Exception -
	* Return -
	*/
	inline void Debug(string _loggerMessage)
	{
		if (Configuration::GetInstance()->GetDebugEnabled())
			Logger(_loggerMessage);
	}

	/*
	* Description - Error() method used to writes error log messages to logger file.
	* Parameter -  string.
	* Exception -
	* Return -
	*/
	inline void Error(string _loggerMessage)
	{
		if (Configuration::GetInstance()->GetErrorEnabled())
			Logger(_loggerMessage);
	}

	/*
	* Description - Info() method used to writes info log messages to logger file.
	* Parameter -  string.
	* Exception -
	* Return -
	*/
	inline void Info(string _loggerMessage)
	{
		if (Configuration::GetInstance()->GetInfoEnabled())
			Logger(_loggerMessage);
	}

	/*
	* Description - perfomance() method used to writes info log messages to logger file.
	* Parameter -  string.
	* Exception -
	* Return -
	*/
	inline void perfomance(string _loggerMessage)
	{
		if (Configuration::GetInstance()->GetPerfomanceEnabled())
			Logger(_loggerMessage);
	}

	/*
	* Description - RestAPIDebug() method used to writes info log messages to logger file.
	* Parameter -  string.
	* Exception -
	* Return -
	*/
	inline void RestAPIDebug(string _loggerMessage)
	{
		if (Configuration::GetInstance()->GetRestAPILogEnabled())
			Logger(_loggerMessage);
	}
}