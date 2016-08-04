#include "Win32FileManager.h"
#include <string>
#include <fstream>
#include <boost/filesystem.hpp>

Win32FileManager::Win32FileManager()
{

}

/*
irr::core::ustring Win32FileManager::GetRealPath(const std::string& p)
{
	return p.c_str();
}


irr::core::ustring Win32FileManager::GetRealPath(const std::wstring& p)
{
	return p.c_str();
}
*/

PlatformString Win32FileManager::GetRealPath(const PlatformString& p)
{
	return p;
}