#pragma once

#include <string>
#include <fstream>
#include "PlatformString.h"
class IFileManager
{

public:

	virtual PlatformString GetRealPath(const PlatformString& p) = 0;

};