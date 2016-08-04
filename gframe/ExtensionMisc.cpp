#include "SoundEffectPlayer.h"
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <array>
#include "game.h"
static std::map<BattleSoundEffect, PlatformString> _battleSoundEffects =
{
	
	{ BattleSoundEffect::ShuffleCard, PlatformString::FromUTF8("sound/battle/shuffle.wav") },
	{ BattleSoundEffect::NewTurn, PlatformString::FromUTF8("sound/battle/nextturn.wav") },
	{ BattleSoundEffect::NewPhase, PlatformString::FromUTF8("sound/battle/phase.wav") },
	{ BattleSoundEffect::SetCard, PlatformString::FromUTF8("sound/battle/set.wav") },
	{ BattleSoundEffect::Summon, PlatformString::FromUTF8("sound/battle/summon.wav") },
	{ BattleSoundEffect::SpecialSummon, PlatformString::FromUTF8("sound/battle/specialsummon.wav") },
	{ BattleSoundEffect::FlipCard, PlatformString::FromUTF8("sound/battle/flip.wav") },
	{ BattleSoundEffect::Activate, PlatformString::FromUTF8("sound/battle/activate.wav") },
	{ BattleSoundEffect::DrawCard, PlatformString::FromUTF8("sound/battle/draw.wav") },
	{ BattleSoundEffect::Damage, PlatformString::FromUTF8("sound/battle/damage.wav") },
	{ BattleSoundEffect::GainLp, PlatformString::FromUTF8("sound/battle/gainlp.wav") },
	{ BattleSoundEffect::Equip, PlatformString::FromUTF8("sound/battle/equip.wav") },
	{ BattleSoundEffect::AddCounter, PlatformString::FromUTF8("sound/battle/addcounter.wav") },
	{ BattleSoundEffect::RemoveCounter, PlatformString::FromUTF8("sound/battle/removecounter.wav") },
	{ BattleSoundEffect::Attack, PlatformString::FromUTF8("sound/battle/attack.wav") },
	{ BattleSoundEffect::CoinFlip, PlatformString::FromUTF8("sound/battle/coinflip.wav") },
	{ BattleSoundEffect::DiceRoll,PlatformString::FromUTF8("sound/battle/diceroll.wav") },
	
	{ BattleSoundEffect::Destroy, PlatformString::FromUTF8("sound/battle/destroyed.wav") },

	

};

static std::map<UISoundEffect, PlatformString> _uiSoundEffects =
{
	{UISoundEffect::MenuItemMoveIn, PlatformString::FromUTF8("sound/ui/menuitemmovein.wav") },
	{ UISoundEffect::MenuItemClick, PlatformString::FromUTF8("sound/ui/menuitemclick.wav") },
	{ UISoundEffect::CheckBoxChange, PlatformString::FromUTF8("sound/ui/checkboxchange.wav") },
	{ UISoundEffect::CardPickUp, PlatformString::FromUTF8("sound/ui/cardpickup.wav") },
	{ UISoundEffect::CardAttached, PlatformString::FromUTF8("sound/ui/cardattached.wav") },
	{ UISoundEffect::CardMoveIn, PlatformString::FromUTF8("sound/ui/cardmovein.wav") },
	{ UISoundEffect::SaveDeck, PlatformString::FromUTF8("sound/ui/savedeck.wav") },
	{ UISoundEffect::Back, PlatformString::FromUTF8("sound/ui/back.wav") },


};
static std::map<LobbySoundEffect, PlatformString> _lobbySoundEffects =
{
	{ LobbySoundEffect::PlayerEnter, PlatformString::FromUTF8("sound/lobby/playerenter.wav") } ,
	{ LobbySoundEffect::Chat,PlatformString::FromUTF8("sound/lobby/chatmessage.wav") },
	{ LobbySoundEffect::StartGame,PlatformString::FromUTF8("sound/lobby/startgame.wav") },
	{ LobbySoundEffect::Ready,PlatformString::FromUTF8("sound/lobby/ready.wav") },
	{ LobbySoundEffect::Unready,PlatformString::FromUTF8("sound/lobby/unready.wav") },
	{ LobbySoundEffect::PlayerExit,PlatformString::FromUTF8("sound/lobby/playerexit.wav") },

};
static std::map<BGMSoundEffect, PlatformString> _bgmSoundEffects =
{
	{ BGMSoundEffect::Menu, PlatformString::FromUTF8("sound/bgm/menu") },
	{ BGMSoundEffect::Opening, PlatformString::FromUTF8("sound/bgm/opening") },
	{ BGMSoundEffect::MoreLp, PlatformString::FromUTF8("sound/bgm/morelp") },
	{ BGMSoundEffect::LowLp, PlatformString::FromUTF8("sound/bgm/lowlp") },
	{ BGMSoundEffect::InSide, PlatformString::FromUTF8("sound/bgm/inside") },

	{ BGMSoundEffect::Win, PlatformString::FromUTF8("sound/bgm/duelwin") },
	{ BGMSoundEffect::Lose, PlatformString::FromUTF8("sound/bgm/duellose") },
};


PlatformString SoundEffectPlayer::GetSoundEffect(BattleSoundEffect effect)
{
	return _battleSoundEffects[effect];
}

PlatformString SoundEffectPlayer::GetSoundEffect(LobbySoundEffect effect)
{
	return _lobbySoundEffects[effect];
}

PlatformString SoundEffectPlayer::GetSoundEffect(UISoundEffect effect)
{
	return _uiSoundEffects[effect];
}

std::vector<PlatformString> SoundEffectPlayer::GetSoundEffect(BGMSoundEffect effect)
{
	auto fs = ygo::mainGame->device->getFileSystem();
	auto folder = _bgmSoundEffects[effect].ToString();
	auto prevWorkingPath = fs->getWorkingDirectory();
	fs->changeWorkingDirectoryTo(folder.c_str());
	std::vector<PlatformString> vec;
	boost::to_lower(folder);
	
	static const std::array<std::string,3> acceptExtension = { ".mp3", ".ogg", ".wav" };

	irr::io::IFileList* files = fs->createFileList();
	if (files == nullptr)
		return vec;
	for (std::size_t i = 0; i < files->getFileCount(); i++)
	{
		if(files->isDirectory(i))
			continue;
		auto filename = PlatformString::FromPath(files->getFullFileName(i));
		auto cfilename = filename.ToString();
		boost::to_lower(cfilename);
		
		if (cfilename.find(folder) != -1)
		{
			auto extItor = std::find(acceptExtension.begin(), acceptExtension.end(), boost::filesystem::extension(cfilename));
			if(extItor != acceptExtension.end())
				vec.push_back(filename);
		}
	}
	files->drop();
	fs->changeWorkingDirectoryTo(prevWorkingPath);
	return vec;
}