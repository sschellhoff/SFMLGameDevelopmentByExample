#pragma once
#define RUNNING_WINDOWS
#include <SFML/Network.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

namespace Utils{
	#ifdef RUNNING_WINDOWS
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <Shlwapi.h>
	inline std::string GetWorkingDirectory(){
		HMODULE hModule = GetModuleHandle(nullptr);
		if(hModule){
			char path[256];
			GetModuleFileName(hModule,path,sizeof(path));
			PathRemoveFileSpec(path);
			strcat_s(path,"\\");
			return std::string(path);
		}
		return "";
	}
	#elif defined RUNNING_LINUX
	#include <unistd.h>
	inline std::string GetWorkingDirectory(){
		char cwd[1024];
		if(getcwd(cwd, sizeof(cwd)) != nullptr){
			return std::string(cwd) + std::string("/");
		}
		return "";
	}
	#endif

	inline void ReadQuotedString(std::stringstream& l_stream, 
		std::string& l_string)
	{
		l_stream >> l_string;
		if (l_string.at(0) == '"'){
			while (l_string.at(l_string.length() - 1) != '"' || !l_stream.eof()){
				std::string str;
				l_stream >> str;
				l_string.append(" " + str);
			}
		}
		l_string.erase(std::remove(l_string.begin(), l_string.end(), '"'), l_string.end());
	}
}