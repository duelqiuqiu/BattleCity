#include "data_manager.h"
#include "game.h"
#include <stdio.h>
#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem.hpp>
#include "PlatformString.h"
#include "IFileManager.h"
namespace ygo {

const wchar_t* DataManager::unknown_string = L"???";
wchar_t DataManager::strBuffer[4096];
DataManager dataManager;

std::tuple<bool, PlatformString> copyToRoot(const char* file)
{
	auto fs = mainGame->device->getFileSystem();
	auto filename = PlatformString::FromCharString(file);
	if (!fs->existFile(filename.ToPath()))
		return { false, L"" };
	auto realrealFilename = mainGame->fileManager->GetRealPath(filename);
	auto safetempFilename = boost::filesystem::path(filename.ToWString()).filename();
	auto realtempFilename = mainGame->fileManager->GetRealPath(PlatformString::FromUnicode(safetempFilename.wstring()));
#ifdef _WIN32
	std::ofstream ofs(realtempFilename.ToWString(), std::ofstream::binary);
#else
	std::ofstream ofs(realtempFilename.ToString(), std::ofstream::binary);
#endif
	if (!ofs.is_open())
		return{ false, L"" };
	auto arcFile = fs->createAndOpenFile(filename.ToPath());
	if (!arcFile)
		return{ false, L"" };
	char* buf = new char[arcFile->getSize()];
	auto bytesRead = arcFile->read(buf, arcFile->getSize());
	if (bytesRead != arcFile->getSize())
	{
		delete[] buf;
		ofs.close();
		return{ false, L"" };
	}

	ofs.write(buf, arcFile->getSize());
	ofs.flush();
	delete[] buf;
	ofs.close();
	return{ true, realtempFilename };
}

bool DataManager::LoadDB(const char* file) {
	auto copyResult = copyToRoot(file);
	if (!std::get<0>(copyResult))
		return false;

	sqlite3* pDB;
	if(sqlite3_open(std::get<1>(copyResult).ToUTF8().c_str(), &pDB) != SQLITE_OK)
		return Error(pDB);
	sqlite3_stmt* pStmt;
	const char* sql = "select * from datas,texts where datas.id=texts.id";
	if(sqlite3_prepare_v2(pDB, sql, -1, &pStmt, 0) != SQLITE_OK)
		return Error(pDB);
	CardDataC cd;
	CardString cs;
	for(int i = 0; i < 16; ++i) cs.desc[i] = 0;
	int step = 0, len = 0;
	do {
		step = sqlite3_step(pStmt);
		if(step == SQLITE_BUSY || step == SQLITE_ERROR || step == SQLITE_MISUSE)
			return Error(pDB, pStmt);
		else if(step == SQLITE_ROW) {
			cd.code = sqlite3_column_int(pStmt, 0);
			cd.ot = sqlite3_column_int(pStmt, 1);
			cd.alias = sqlite3_column_int(pStmt, 2);
			cd.setcode = sqlite3_column_int64(pStmt, 3);
			cd.type = sqlite3_column_int(pStmt, 4);
			cd.attack = sqlite3_column_int(pStmt, 5);
			cd.defense = sqlite3_column_int(pStmt, 6);
			unsigned int level = sqlite3_column_int(pStmt, 7);
			cd.level = level & 0xff;
			cd.lscale = (level >> 24) & 0xff;
			cd.rscale = (level >> 16) & 0xff;
			cd.race = sqlite3_column_int(pStmt, 8);
			cd.attribute = sqlite3_column_int(pStmt, 9);
			cd.category = sqlite3_column_int(pStmt, 10);
			_datas.insert(std::make_pair(cd.code, cd));
			len = BufferIO::DecodeUTF8((const char*)sqlite3_column_text(pStmt, 12), strBuffer);
			if(len) {
				cs.name = new wchar_t[len + 1];
				memcpy(cs.name, strBuffer, (len + 1)*sizeof(wchar_t));
			} else cs.name = 0;
			len = BufferIO::DecodeUTF8((const char*)sqlite3_column_text(pStmt, 13), strBuffer);
			if(len) {
				cs.text = new wchar_t[len + 1];
				memcpy(cs.text, strBuffer, (len + 1)*sizeof(wchar_t));
			} else {
				cs.text = new wchar_t[1];
				cs.text[0] = 0;
			}
			for(int i = 14; i < 30; ++i) {
				len = BufferIO::DecodeUTF8((const char*)sqlite3_column_text(pStmt, i), strBuffer);
				if(len) {
					cs.desc[i - 14] = new wchar_t[len + 1];
					memcpy(cs.desc[i - 14], strBuffer, (len + 1)*sizeof(wchar_t));
				} else cs.desc[i - 14] = 0;
			}
			_strings.insert(std::make_pair(cd.code, cs));
		}
	} while(step != SQLITE_DONE);
	sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	return true;
}
bool DataManager::LoadStrings(const char* file) {
	auto copyResult = copyToRoot(file);
	if (!std::get<0>(copyResult))
		return false;

	boost::filesystem::ifstream fp(std::get<1>(copyResult).ToWString());
	for(int i = 0; i < 2048; ++i)
		_sysStrings[i] = 0;
	char linebuf[256];
	char strbuf[256];
	int value;
	fp.seekg(0, boost::filesystem::ifstream::end);
	int fsize = fp.tellg();
	fp.seekg(0, boost::filesystem::ifstream::beg);
	//fgets(linebuf, 256, fp);
	while(!fp.eof()) {
		fp.getline(linebuf, 256);
		if(linebuf[0] != '!')
			continue;
		sscanf(linebuf, "!%s", strbuf);
		if(!strcmp(strbuf, "system")) {
			sscanf(&linebuf[7], "%d %240[^\n]", &value, strbuf);
			int len = BufferIO::DecodeUTF8(strbuf, strBuffer);
			wchar_t* pbuf = new wchar_t[len + 1];
			wcscpy(pbuf, strBuffer);
			_sysStrings[value] = pbuf;
		} else if(!strcmp(strbuf, "victory")) {
			sscanf(&linebuf[8], "%x %240[^\n]", &value, strbuf);
			int len = BufferIO::DecodeUTF8(strbuf, strBuffer);
			wchar_t* pbuf = new wchar_t[len + 1];
			wcscpy(pbuf, strBuffer);
			_victoryStrings[value] = pbuf;
		} else if(!strcmp(strbuf, "counter")) {
			sscanf(&linebuf[8], "%x %240[^\n]", &value, strbuf);
			int len = BufferIO::DecodeUTF8(strbuf, strBuffer);
			wchar_t* pbuf = new wchar_t[len + 1];
			wcscpy(pbuf, strBuffer);
			_counterStrings[value] = pbuf;
		} else if(!strcmp(strbuf, "setname")) {
			sscanf(&linebuf[8], "%x %240[^\t\n]", &value, strbuf);//using tab for comment
			int len = BufferIO::DecodeUTF8(strbuf, strBuffer);
			wchar_t* pbuf = new wchar_t[len + 1];
			wcscpy(pbuf, strBuffer);
			_setnameStrings[value] = pbuf;
		} else if(!strcmp(strbuf, "custom")) {
			sscanf(&linebuf[8], "%d %240[^\t\n]", &value, strbuf);//using tab for comment
			std::string tmpStr = strbuf;
			while(tmpStr.find("\\n") != -1)
				tmpStr = tmpStr.replace(tmpStr.find("\\n"), 2, "\n");
			int len = BufferIO::DecodeUTF8(tmpStr.c_str(), strBuffer);
			wchar_t* pbuf = new wchar_t[len + 1];
			wcscpy(pbuf, strBuffer);
			_customStrings[value] = pbuf;
			delete[] pbuf;
		}
	}
	fp.close();
	for(int i = 0; i < 255; ++i)
		myswprintf(numStrings[i], L"%d", i);
	return true;
}
bool DataManager::Error(sqlite3* pDB, sqlite3_stmt* pStmt) {
	BufferIO::DecodeUTF8(sqlite3_errmsg(pDB), strBuffer);
	if(pStmt)
		sqlite3_finalize(pStmt);
	sqlite3_close(pDB);
	return false;
}
bool DataManager::GetData(int code, CardData* pData) {
	auto cdit = _datas.find(code);
	if(cdit == _datas.end())
		return false;
	if(pData)
		*pData = *((CardData*)&cdit->second);
	return true;
}
code_pointer DataManager::GetCodePointer(int code) {
	return _datas.find(code);
}
bool DataManager::GetString(int code, CardString* pStr) {
	auto csit = _strings.find(code);
	if(csit == _strings.end()) {
		pStr->name = (wchar_t*)unknown_string;
		pStr->text = (wchar_t*)unknown_string;
		return false;
	}
	*pStr = csit->second;
	return true;
}
const wchar_t* DataManager::GetName(int code) {
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(csit->second.name)
		return csit->second.name;
	return unknown_string;
}
const wchar_t* DataManager::GetText(int code) {
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(csit->second.text)
		return csit->second.text;
	return unknown_string;
}
const wchar_t* DataManager::GetDesc(int strCode) {
	if(strCode < 10000)
		return GetSysString(strCode);
	int code = strCode >> 4;
	int offset = strCode & 0xf;
	auto csit = _strings.find(code);
	if(csit == _strings.end())
		return unknown_string;
	if(csit->second.desc[offset])
		return csit->second.desc[offset];
	return unknown_string;
}
const wchar_t* DataManager::GetSysString(int code) {
	if(code < 0 || code >= 2048 || _sysStrings[code] == 0)
		return unknown_string;
	return _sysStrings[code];
}

const wchar_t* DataManager::GetCustomString(int code) {
	return _customStrings[code].c_str();
}

const wchar_t* DataManager::GetVictoryString(int code) {
	auto csit = _victoryStrings.find(code);
	if(csit == _victoryStrings.end())
		return unknown_string;
	return csit->second;
}
const wchar_t* DataManager::GetCounterName(int code) {
	auto csit = _counterStrings.find(code);
	if(csit == _counterStrings.end())
		return unknown_string;
	return csit->second;
}
const wchar_t* DataManager::GetSetName(int code) {
	auto csit = _setnameStrings.find(code);
	if(csit == _setnameStrings.end())
		return NULL;
	return csit->second;
}
unsigned int DataManager::GetSetCode(const wchar_t* setname) {
	for(auto csit = _setnameStrings.begin(); csit != _setnameStrings.end(); ++csit)
		if(wcscmp(csit->second, setname) == 0)
			return csit->first;
	return 0;
}
const wchar_t* DataManager::GetNumString(int num, bool bracket) {
	if(!bracket)
		return numStrings[num];
	wchar_t* p = numBuffer;
	*p++ = L'(';
	BufferIO::CopyWStrRef(numStrings[num], p, 4);
	*p = L')';
	*++p = 0;
	return numBuffer;
}
const wchar_t* DataManager::FormatLocation(int location, int sequence) {
	if(location == 0x8) {
		if(sequence < 5)
			return GetSysString(1003);
		else if(sequence == 5)
			return GetSysString(1008);
		else
			return GetSysString(1009);
	}
	unsigned filter = 1;
	int i = 1000;
	for(; filter != 0x100 && filter != location; filter <<= 1)
		++i;
	if(filter == location)
		return GetSysString(i);
	else
		return unknown_string;
}
const wchar_t* DataManager::FormatAttribute(int attribute) {
	wchar_t* p = attBuffer;
	unsigned filter = 1;
	int i = 1010;
	for(; filter != 0x80; filter <<= 1, ++i) {
		if(attribute & filter) {
			BufferIO::CopyWStrRef(GetSysString(i), p, 16);
			*p = L'|';
			*++p = 0;
		}
	}
	if(p != attBuffer)
		*(p - 1) = 0;
	else
		return unknown_string;
	return attBuffer;
}
const wchar_t* DataManager::FormatRace(int race) {
	wchar_t* p = racBuffer;
	unsigned filter = 1;
	int i = 1020;
	for(; filter != 0x1000000; filter <<= 1, ++i) {
		if(race & filter) {
			BufferIO::CopyWStrRef(GetSysString(i), p, 16);
			*p = L'|';
			*++p = 0;
		}
	}
	if(p != racBuffer)
		*(p - 1) = 0;
	else
		return unknown_string;
	return racBuffer;
}
const wchar_t* DataManager::FormatType(int type) {
	wchar_t* p = tpBuffer;
	unsigned filter = 1;
	int i = 1050;
	for(; filter != 0x2000000; filter <<= 1, ++i) {
		if(type & filter) {
			BufferIO::CopyWStrRef(GetSysString(i), p, 16);
			*p = L'|';
			*++p = 0;
		}
	}
	if(p != tpBuffer)
		*(p - 1) = 0;
	else
		return unknown_string;
	return tpBuffer;
}
const wchar_t* DataManager::FormatSetName(unsigned long long setcode) {
	wchar_t* p = scBuffer;
	for(int i = 0; i < 4; ++i) {
		const wchar_t* setname = GetSetName((setcode >> i * 16) & 0xffff);
		if(setname) {
			BufferIO::CopyWStrRef(setname, p, 16);
			*p = L'|';
			*++p = 0;
		}
	}
	if(p != scBuffer)
		*(p - 1) = 0;
	else
		return unknown_string;
	return scBuffer;
}
int DataManager::CardReader(int code, void* pData) {
	if(!dataManager.GetData(code, (CardData*)pData))
		memset(pData, 0, sizeof(CardData));
	return 0;
}

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if (start_pos == std::string::npos)
		return false;
	str.replace(start_pos, from.length(), to);
	return true;
}


byte* DataManager::ScriptReader(const char* filename, int* readlen) {
	*readlen = 0;
	std::string stdpath(filename);
	static const std::string sep = "./";
	static const std::string emptystr = "";
	replace(stdpath, sep, emptystr);
	auto fs = mainGame->device->getFileSystem();
	auto filePtr = fs->createAndOpenFile(stdpath.c_str());
	byte* ret = nullptr;
	do
	{
		if (filePtr)
		{
			static byte buffer[0x10000];
			auto filesize = filePtr->getSize();
			if (filePtr->getSize() > sizeof(buffer))
				break;
			auto restSizeToRead = filesize;
			restSizeToRead -= filePtr->read(buffer, restSizeToRead);
			if (restSizeToRead != 0)
				break;
			*readlen = filesize;
			ret = buffer;
			
		}
		break;
	}while(true);
	if (filePtr)
	{
		filePtr->drop();
		filePtr = nullptr;
	}
	return ret;
}

}
