#include "image_manager.h"

namespace ygo {

ImageManager imageManager;

bool ImageManager::Initial() {
	tCover[0] = driver->getTexture("textures/cover.jpg");
	tCover[1] = driver->getTexture("textures/cover2.jpg");
	tUnknown = driver->getTexture("textures/unknown.jpg");
	tAct = driver->getTexture("textures/act.png");
	tAttack = driver->getTexture("textures/attack.png");
	tChain = driver->getTexture("textures/chain.png");
	tNegated = driver->getTexture("textures/negated.png");
	tNumber = driver->getTexture("textures/number.png");
	tLPBar = driver->getTexture("textures/lp.png");
	tLPFrame = driver->getTexture("textures/lpf.png");
	tMask = driver->getTexture("textures/mask.png");
	tEquip = driver->getTexture("textures/equip.png");
	tTarget = driver->getTexture("textures/target.png");
	tLim = driver->getTexture("textures/lim.png");
	tHand[0] = driver->getTexture("textures/f1.jpg");
	tHand[1] = driver->getTexture("textures/f2.jpg");
	tHand[2] = driver->getTexture("textures/f3.jpg");
	tBackGround = driver->getTexture("textures/bg.jpg");
	tBackGround_menu = driver->getTexture("textures/bg_menu.jpg");
	tBackGround_deck = driver->getTexture("textures/bg_deck.jpg");
	tField = driver->getTexture("textures/field2.png");
	tFieldTransparent = driver->getTexture("textures/field-transparent2.png"); 
	int i = 0;
	char buff[100];
	for (; i < 14; i++) {
		snprintf(buff, 100, "textures/extra/rscale_%d.png", i);
		tRScale[i] = driver->getTexture(path(buff).c_str());
	}
	for (i = 0; i < 14; i++) {
		snprintf(buff, 100, "textures/extra/lscale_%d.png", i);
		tLScale[i] = driver->getTexture(path(buff).c_str());
	}
	return true;
}
void ImageManager::SetDevice(irr::IrrlichtDevice* dev) {
	device = dev;
	driver = dev->getVideoDriver();
}
void ImageManager::ClearTexture() {
	for(auto tit = tMap.begin(); tit != tMap.end(); ++tit) {
		if(tit->second)
			driver->removeTexture(tit->second);
	}
	for(auto tit = tThumb.begin(); tit != tThumb.end(); ++tit) {
		if(tit->second)
			driver->removeTexture(tit->second);
	}
	tMap.clear();
	tThumb.clear();
}
void ImageManager::RemoveTexture(int code) {
	auto tit = tMap.find(code);
	if(tit != tMap.end()) {
		if(tit->second)
			driver->removeTexture(tit->second);
		tMap.erase(tit);
	}
}
irr::video::ITexture* ImageManager::GetTexture(int code) {
	if(code == 0)
		return tUnknown;
	auto tit = tMap.find(code);
	if(tit == tMap.end()) {
		static const std::string exts[] = { ".bpg", ".jpg" };
		char file[256];
		irr::video::ITexture* img = nullptr;
		for (const auto& ext : exts)
		{
			sprintf(file, "pics/%d%s", code, ext.c_str());
			img = driver->getTexture(file);
			if(img)
				break;
		}
		if (img == NULL) {
			tMap[code] = NULL;
			return GetTextureThumb(code);
		}
		else {
			tMap[code] = img;
			return img;
		}
	}
	if(tit->second)
		return tit->second;
	else
		return GetTextureThumb(code);
}
irr::video::ITexture* ImageManager::GetTextureThumb(int code) {
	if(code == 0)
		return tUnknown;
	auto tit = tThumb.find(code);
	if(tit == tThumb.end()) {
		static const std::string exts[] = { ".bpg", ".jpg" };
		char file[256];
		irr::video::ITexture* img = nullptr;
		for (const auto& ext : exts)
		{
			sprintf(file, "pics/thumbnail/%d%s", code, ext.c_str());
			img = driver->getTexture(file);
			if(img)
				break;
		}
		if (img == NULL) {
			tThumb[code] = NULL;
			return tUnknown;
		}
		else {
			tThumb[code] = img;
			return img;
		}
	}
	if(tit->second)
		return tit->second;
	else
		return tUnknown;
}
irr::video::ITexture* ImageManager::GetTextureField(int code) {
	if(code == 0)
		return NULL;
	auto tit = tFields.find(code);
	if(tit == tFields.end()) {
		char file[256];
		static const std::string exts[] = { ".bpg", ".png", ".jpg" };
		
		irr::video::ITexture* img = nullptr;
		for (const auto& ext : exts)
		{
			sprintf(file, "pics/field/%d%s", code, ext.c_str());
			img = driver->getTexture(file);
			if(img)
				break;
		}

		if (img == NULL) {
			tFields[code] = NULL;
			return NULL;
		}
		else {
			tFields[code] = img;
			return img;
		}
	}
	if(tit->second)
		return tit->second;
	else
		return NULL;
}
}
