#include "IrrCompileConfig.h"
#include "deck_manager.h"
#include "data_manager.h"
#include "game.h"
#include <algorithm>
#include "User.h"
#include "game.h"
#include "IFileManager.h"
#include <boost/filesystem.hpp>
#include <type_traits>
#include <fstream>
#include <boost/filesystem/fstream.hpp>
namespace ygo {

DeckManager deckManager;

std::tuple<bool, PlatformString> copyToRoot(const char* file);

void DeckManager::LoadLFList() {
	LFList* cur = NULL;
	auto fs = mainGame->device->getFileSystem();
	boost::filesystem::ifstream fp;
	const PlatformString lflists[] = { L"games/ocgtcg/lflist.conf" ,L"expansions/lflist.conf" };
	if (fs->existFile(lflists[1].ToPath()))
	{
		auto copyResult = copyToRoot(lflists[1].ToString().c_str());
		fp.open(std::get<1>(copyResult).ToWString());
	}
	else
	{
		auto copyResult = copyToRoot(lflists[0].ToString().c_str());
		fp.open(std::get<1>(copyResult).ToWString());
	}
	char linebuf[1024];
	wchar_t strBuffer[256];
	if (fp) {
		fp.seekg(0, boost::filesystem::ifstream::end);
		int fsize = fp.tellg();
		fp.seekg(0, boost::filesystem::ifstream::beg);
		int pos = 0;
		while(!fp.eof()) {
			fp.getline(linebuf, sizeof(linebuf)); // 如果linebuf缓冲区不够大，下面的tellg会返回-1，导致死循环
			pos = fp.tellg();
			if(linebuf[0] == '#')
				continue;
			int p = 0, sa = 0, code, count;
			if(linebuf[0] == '!') {
				sa = BufferIO::DecodeUTF8((const char*)(&linebuf[1]), strBuffer);
				while(strBuffer[sa - 1] == L'\r' || strBuffer[sa - 1] == L'\n' ) sa--;
				LFList newlist;
				_lfList.push_back(newlist);
				cur = &_lfList[_lfList.size() - 1];
				memcpy(cur->listName, (const void*)strBuffer, 40);
				cur->listName[sa] = 0;
				cur->content = new std::unordered_map<int, int>;
				cur->hash = 0x7dfcee6a;
				continue;
			}
			while(linebuf[p] != ' ' && linebuf[p] != '\t' && linebuf[p] != 0) p++;
			if(linebuf[p] == 0)
				continue;
			linebuf[p++] = 0;
			sa = p;
			code = atoi(linebuf);
			if(code == 0)
				continue;
			while(linebuf[p] == ' ' || linebuf[p] == '\t') p++;
			while(linebuf[p] != ' ' && linebuf[p] != '\t' && linebuf[p] != 0) p++;
			linebuf[p] = 0;
			count = atoi(&linebuf[sa]);
			if(cur == NULL) continue;
			(*cur->content)[code] = count;
			cur->hash = cur->hash ^ ((code << 18) | (code >> 14)) ^ ((code << (27 + count)) | (code >> (5 - count)));
		}
		fp.close();
	}
	LFList nolimit;
	myswprintf(nolimit.listName, L"N/A");
	nolimit.hash = 0;
	nolimit.content = new std::unordered_map<int, int>;
	_lfList.push_back(nolimit);
}
wchar_t* DeckManager::GetLFListName(int lfhash) {
	for(size_t i = 0; i < _lfList.size(); ++i) {
		if(_lfList[i].hash == (unsigned int)lfhash) {
			return _lfList[i].listName;
		}
	}
	return (wchar_t*)dataManager.unknown_string;
}
int DeckManager::CheckLFList(Deck& deck, int lfhash, bool allow_ocg, bool allow_tcg) {
	std::unordered_map<int, int> ccount;
	std::unordered_map<int, int>* list = 0;
	for(size_t i = 0; i < _lfList.size(); ++i) {
		if(_lfList[i].hash == (unsigned int)lfhash) {
			list = _lfList[i].content;
			break;
		}
	}
	if(!list)
		return 0;
	int dc = 0;
	if(deck.main.size() < 40 || deck.main.size() > 60 || deck.extra.size() > 15 || deck.side.size() > 15)
		return 1;
	for(size_t i = 0; i < deck.main.size(); ++i) {
		code_pointer cit = deck.main[i];
		if((!allow_ocg && (cit->second.ot == 0x1)) || (!allow_tcg && (cit->second.ot == 0x2)))
			return cit->first;
		if(cit->second.type & (TYPE_FUSION | TYPE_SYNCHRO | TYPE_XYZ | TYPE_TOKEN))
			return 1;
		int code = cit->second.alias ? cit->second.alias : cit->first;
		ccount[code]++;
		dc = ccount[code];
		auto it = list->find(code);
		if(dc > 3 || (it != list->end() && dc > it->second))
			return cit->first;
	}
	for(size_t i = 0; i < deck.extra.size(); ++i) {
		code_pointer cit = deck.extra[i];
		if((!allow_ocg && (cit->second.ot == 0x1)) || (!allow_tcg && (cit->second.ot == 0x2)))
			return cit->first;
		int code = cit->second.alias ? cit->second.alias : cit->first;
		ccount[code]++;
		dc = ccount[code];
		auto it = list->find(code);
		if(dc > 3 || (it != list->end() && dc > it->second))
			return cit->first;
	}
	for(size_t i = 0; i < deck.side.size(); ++i) {
		code_pointer cit = deck.side[i];
		if((!allow_ocg && (cit->second.ot == 0x1)) || (!allow_tcg && (cit->second.ot == 0x2)))
			return cit->first;
		int code = cit->second.alias ? cit->second.alias : cit->first;
		ccount[code]++;
		dc = ccount[code];
		auto it = list->find(code);
		if(dc > 3 || (it != list->end() && dc > it->second))
			return cit->first;
	}
	return 0;
}
void DeckManager::LoadDeck(Deck& deck, std::function<bool(int)> filter, int* codeBuffer, int mainc, int sidec)
{
	deck.clear();

	CardData cd;
	for (int i = 0; i < mainc; ++i) {
		int code = codeBuffer[i];
		if (filter(code))
			continue;
		if (!dataManager.GetData(code, &cd))
			continue;
		if (cd.type & TYPE_TOKEN)
			continue;
		else if (cd.type & 0x802040 && deck.extra.size() < 15) {
			deck.extra.push_back(dataManager.GetCodePointer(code));	//verified by GetData()
		}
		else if (deck.main.size() < 60) {
			deck.main.push_back(dataManager.GetCodePointer(code));
		}
	}
	for (int i = 0; i < sidec; ++i) {
		int code = codeBuffer[mainc + i];
		if (filter(code))
			continue;
		if (!dataManager.GetData(code, &cd))
			continue;
		if (cd.type & TYPE_TOKEN)
			continue;
		if (deck.side.size() < 15)
			deck.side.push_back(dataManager.GetCodePointer(code));	//verified by GetData()
	}
}
void DeckManager::LoadDeck(Deck& deck, int* dbuf, int mainc, int sidec) {
	deck.clear();
	int code;
	CardData cd;
	for(int i = 0; i < mainc; ++i) {
		code = dbuf[i];
		if(!dataManager.GetData(code, &cd))
			continue;
		if(cd.type & TYPE_TOKEN)
			continue;
		else if(cd.type & 0x802040 && deck.extra.size() < 15) {
			deck.extra.push_back(dataManager.GetCodePointer(code));	//verified by GetData()
		} else if(deck.main.size() < 60) {
			deck.main.push_back(dataManager.GetCodePointer(code));
		}
	}
	for(int i = 0; i < sidec; ++i) {
		code = dbuf[mainc + i];
		if(!dataManager.GetData(code, &cd))
			continue;
		if(cd.type & TYPE_TOKEN)
			continue;
		if(deck.side.size() < 15)
			deck.side.push_back(dataManager.GetCodePointer(code));	//verified by GetData()
	}
}
bool DeckManager::LoadSide(Deck& deck, int* dbuf, int mainc, int sidec) {
	std::unordered_map<int, int> pcount;
	std::unordered_map<int, int> ncount;
	for(size_t i = 0; i < deck.main.size(); ++i)
		pcount[deck.main[i]->first]++;
	for(size_t i = 0; i < deck.extra.size(); ++i)
		pcount[deck.extra[i]->first]++;
	for(size_t i = 0; i < deck.side.size(); ++i)
		pcount[deck.side[i]->first]++;
	Deck ndeck;
	LoadDeck(ndeck, dbuf, mainc, sidec);
	if(ndeck.main.size() != deck.main.size() || ndeck.extra.size() != deck.extra.size())
		return false;
	for(size_t i = 0; i < ndeck.main.size(); ++i)
		ncount[ndeck.main[i]->first]++;
	for(size_t i = 0; i < ndeck.extra.size(); ++i)
		ncount[ndeck.extra[i]->first]++;
	for(size_t i = 0; i < ndeck.side.size(); ++i)
		ncount[ndeck.side[i]->first]++;
	for(auto cdit = ncount.begin(); cdit != ncount.end(); ++cdit)
		if(cdit->second != pcount[cdit->first])
			return false;
	deck = ndeck;
	return true;
}
void DeckManager::LoadDeck(Deck& deck, std::function<bool(int)> filter, std::vector<char> dbuf)
{
	auto p = dbuf.data();
	int mainc = *(int*)p; p += sizeof(int);
	int sidec = *(int*)p; p += sizeof(int);
	LoadDeck(deck, filter, (int*)p, mainc, sidec);
}

bool DeckManager::LoadDeck(const wchar_t* file) {
	int sp = 0, ct = 0, mainc = 0, sidec = 0, code;
	if (file == NULL || file[0] == L'\0') {
		return false;
	}
	wchar_t deck[512];
	myswprintf(deck, mainGame->fileManager->GetRealPath( L"./deck/%ls.ydk").ToWString().c_str(), file);
	int cardlist[128];
	bool is_side = false;
#ifdef WIN32
	FILE* fp = _wfopen(deck, L"r");
#else
	char deckfn[256];
	BufferIO::EncodeUTF8(deck, deckfn);
	FILE* fp = fopen(deckfn, "r");
#endif
	if(!fp)
		return false;
	char linebuf[256];
	fseek(fp, 0, SEEK_END);
	int fsize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fgets(linebuf, 256, fp);
	while(ftell(fp) < fsize && ct < 128) {
		fgets(linebuf, 256, fp);
		if(linebuf[0] == '!') {
			is_side = true;
			continue;
		}
		if(linebuf[0] < '0' || linebuf[0] > '9')
			continue;
		sp = 0;
		while(linebuf[sp] >= '0' && linebuf[sp] <= '9') sp++;
		linebuf[sp] = 0;
		code = atoi(linebuf);
		cardlist[ct++] = code;
		if(is_side) sidec++;
		else mainc++;
	}
	fclose(fp);
	LoadDeck(current_deck, cardlist, mainc, sidec);
	User::Instance()->UpdateCardDeck(file, current_deck);
	return true;
}
bool DeckManager::SaveDeck(Deck& deck, const wchar_t* name) {
	wchar_t file[512]; 
#if WIN32
	myswprintf(file, L"./deck/%ls.ydk", name);
#else
	myswprintf(file, mainGame->fileManager->GetRealPath(L"./deck/%ls.ydk").ToWString().c_str(), name);
#endif

#ifdef WIN32
	FILE* fp = _wfopen(file, L"w");
#else
	char filefn[256];
	BufferIO::EncodeUTF8(file, filefn);
	FILE* fp = fopen(filefn, "w");
#endif
	if(!fp)
		return false;
	fprintf(fp, "#created by ...\n#main\n");
	for(size_t i = 0; i < deck.main.size(); ++i)
		fprintf(fp, "%d\n", deck.main[i]->first);
	fprintf(fp, "#extra\n");
	for(size_t i = 0; i < deck.extra.size(); ++i)
		fprintf(fp, "%d\n", deck.extra[i]->first);
	fprintf(fp, "!side\n");
	for(size_t i = 0; i < deck.side.size(); ++i)
		fprintf(fp, "%d\n", deck.side[i]->first);
	fclose(fp);
	User::Instance()->UpdateCardDeck(name, deck);
	return true;
}
bool DeckManager::DeleteDeck(Deck& deck, const wchar_t* name) {
	User::Instance()->DeleteDeck(name);
	wchar_t file[512];
	myswprintf(file, L"./deck/%ls.ydk", name);
#ifdef WIN32
	BOOL result = DeleteFileW(file);
	return !!result;
#else
	char filefn[256];
	BufferIO::EncodeUTF8(file, filefn);
	int result = unlink(filefn);
	return result == 0;
#endif
}
}
