#pragma once
#include"framework.h"

enum class LogType
{
	Warning = 0,
	Error = 1
};
class GameLogRecorder
{
private:
	
	vector<wstring> records;
public:
	void LogRecord(LogType type, const wchar_t* explain, const char* file,const int line, const char* func)
	{
		wstring temp = L"";
		switch (type)
		{
		case LogType::Warning:
			temp = L"Warning";
			break;
		case LogType::Error:
			temp = L"Error";
			break;
		default:
			break;
		}
		wchar_t temp2[512] = L"";
		swprintf_s(temp2, L"%s:%s   line: %d   file: %hs    func: %hs", temp.c_str(), explain, line,file,func);
		temp = temp2;
		records.push_back(temp);
	}
#define LogRecord(logType,wchar_explain) LogRecord(logType,wchar_explain,__FILE__,__LINE__,__FUNCTION__)
};