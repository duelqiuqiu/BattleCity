#pragma once

#include "IFileManager.h"
class Win32FileManager : public IFileManager
{
	
public:
	Win32FileManager();
	/*
	irr::core::ustring GetRealPath(const std::string& p);


	irr::core::ustring GetRealPath(const std::wstring& p);
*/
	virtual PlatformString GetRealPath(const PlatformString& p);

};