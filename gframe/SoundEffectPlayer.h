#pragma once

#include <memory>
#include <mutex>
#include <map>
#include <vector>

#include "PlatformString.h"
namespace irrklang
{
	class ISoundEngine;
	class ISound;
}

namespace irr
{
	namespace io
	{
		class IFileSystem;
	}
}


enum class BattleSoundEffect
{

	ShuffleCard,
	NewTurn,
	NewPhase,
	SetCard,
	Summon,
	SpecialSummon,
	FlipCard,
	Activate,
	DrawCard,
	Damage,
	GainLp,
	Equip,
	AddCounter,
	RemoveCounter,
	Attack,
	CoinFlip,
	DiceRoll,
	Destroy,



};

enum class BGMSoundEffect
{
	None,
	Menu,
	Opening,
	LowLp,
	MoreLp,
	InSide,
	Win,
	Lose,
};

enum class UISoundEffect
{
	MenuItemMoveIn,
	MenuItemClick,
	CheckBoxChange,
	CardAttached,
	CardPickUp,
	CardMoveIn,
	SaveDeck,
	Back,
};

enum class LobbySoundEffect
{
	PlayerEnter,
	Chat,
	StartGame,
	Ready,
	Unready,
	PlayerExit,

};


namespace ygo
{

	class MyISound
	{
	public:
		virtual bool GetIsFinished() = 0;
		virtual bool GetIsPaused() = 0;
		virtual void Stop() = 0;
	};


	class MyISoundEngine
	{
	public:
		virtual std::shared_ptr<ygo::MyISound> Play(const PlatformString& filename) = 0;

		virtual std::shared_ptr<ygo::MyISound> PlayAsTrack(const PlatformString& filename) = 0;

		virtual void SetSoundVolume(float val) = 0;
	};


	class EmptySound : public ygo::MyISound
	{


		virtual bool GetIsFinished() override
		{
			return true;
		}

		virtual bool GetIsPaused() override
		{
			return false;
		}

		virtual void Stop() override
		{

		}

	};


	class EmptySoundEngine : public ygo::MyISoundEngine
	{


		virtual std::shared_ptr<ygo::MyISound> Play(const PlatformString& filename) override
		{
			return std::shared_ptr<EmptySound>();
		}

		virtual void SetSoundVolume(float val) override
		{

		}

		virtual std::shared_ptr<ygo::MyISound> PlayAsTrack(const PlatformString& filename) override
		{
			return std::shared_ptr<EmptySound>();
		}

	};

}
class IFileManager;
class MySongTracks;

class SoundEffectPlayer
{
	std::shared_ptr<ygo::MyISoundEngine> _engine;
	std::shared_ptr<IFileManager> _fileManager;
	BGMSoundEffect _curBGMSoundEffect = BGMSoundEffect::None;
	std::mutex _bgmMutex;
	std::map<BGMSoundEffect, std::shared_ptr<MySongTracks>> _bgmTracks;


public:
	SoundEffectPlayer(std::shared_ptr<IFileManager> fileManager, std::shared_ptr<ygo::MyISoundEngine> soundEngine);
	~SoundEffectPlayer();

	void doPlayerEnterEffect();

	void doShuffleCardEffect();

	void doNewTurnEffect();

	void doNewPhaseEffect();

	void doSetCardEffect();

	void doSummonEffect();

	void doSpecialSummonEffect();

	void doFlipCardEffect();

	void doActivateEffect();

	void doDrawCardEffect();

	void doDamageEffect();

	void doGainLpEffect();

	void doEquipEffect();

	void doAddCounterEffect();

	void doRemoveCounterEffect();

	void doAttackEffect();

	void doCoinFlipEffect();

	void doDiceRollEffect();

	void doChatEffect();

	void doDestroyEffect();

	void setSEEnabled(bool enabled);

	void doMenuBGM() ;

	void doMenuItemMoveInEffect() ;

	void doMenuItemClickEffect() ;

	void doCheckBoxChange() ;

	void doCardPickUp() ;

	void doCardAttached() ;

	void doCardMoveIn() ;

	void doSaveDeck() ;

	void doBack() ;

	void doStartGame() ;

	void doReady() ;

	void doUnReady() ;

	void doPlayerExit() ;

	void Update() ;

	void doOpeningBGM() ;

	void doInSideBGM() ;

	void doLowLpBGM() ;

	void doMoreLpBGM() ;

	void doBGMWin() ;

	void doBGMLose() ;


	PlatformString GetSoundEffect(BattleSoundEffect effect);
	PlatformString GetSoundEffect(UISoundEffect effect);
	PlatformString GetSoundEffect(LobbySoundEffect effect);
	std::vector<PlatformString> GetSoundEffect(BGMSoundEffect effect);
};