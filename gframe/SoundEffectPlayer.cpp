#include "SoundEffectPlayer.h"
#include <irrlicht.h>
#include <string>
#include <codecvt>
#include <clocale>
#include <vector>
#include "PlatformString.h"
#include "IFileManager.h"
#include <random>
#if _WIN32
#pragma comment(lib, "irrKlang.lib") 
#endif

using namespace irrklang;






class MySongTrack
{
	std::shared_ptr<ygo::MyISound> _irrSound;
	PlatformString _curTrackName;
public:

	enum class State
	{
		Playing,
		Finished,
		Paused,
	};

	MySongTrack(const PlatformString& name, std::shared_ptr<ygo::MyISound> irrSound)
	{
		_irrSound = irrSound;
	}
	~MySongTrack()
	{
	}
	State GetState()
	{
		if (_irrSound->GetIsFinished())
			return State::Finished;
		else if (_irrSound->GetIsPaused())
			return State::Paused;
		else
			return State::Playing;
	}

	void Stop()
	{
		_irrSound->Stop();
	}

	PlatformString GetTrackFilename()
	{
		return _curTrackName;
	}

};


class MySongTracks
{
private:
	std::vector<PlatformString> _playlist;
	std::shared_ptr<MySongTrack> _curTrack;

	std::shared_ptr<ygo::MyISoundEngine> _engine;

	MySongTracks(const MySongTracks&) = delete;
protected:
	std::shared_ptr<MySongTrack> PlayTrack(const PlatformString& trackFilename)
	{
		auto sound = _engine->PlayAsTrack(trackFilename);
		return std::make_shared<MySongTrack>(trackFilename, sound);
	}
public:
	MySongTracks(const std::vector<PlatformString>& playlist, std::shared_ptr<ygo::MyISoundEngine> engine)
	{
		_playlist = playlist;
		_engine = engine;
	}
	~MySongTracks()
	{
	}

	void NextTrack()
	{
		if (_playlist.size() > 0)
		{
			srand(time(0));
			auto idx = rand() % _playlist.size();
			_curTrack = PlayTrack(_playlist[idx]);
		}
	}

	void Update()
	{
		if (_curTrack && (_curTrack->GetState() == MySongTrack::State::Finished))
		{
			NextTrack();
		}
	}

	void Stop()
	{
		if (_curTrack)
		{
			_curTrack->Stop();
			_curTrack = nullptr;
		}
	}

};


SoundEffectPlayer::SoundEffectPlayer(std::shared_ptr<IFileManager> fileManager, std::shared_ptr<ygo::MyISoundEngine> soundEngine) :
	_fileManager(fileManager),
	_engine(soundEngine)
{
	_bgmTracks[BGMSoundEffect::None] = std::make_shared<MySongTracks>(GetSoundEffect(BGMSoundEffect::None), std::make_shared<ygo::EmptySoundEngine>());
	_bgmTracks[BGMSoundEffect::Menu] = std::make_shared<MySongTracks>(GetSoundEffect(BGMSoundEffect::Menu), _engine);
	_bgmTracks[BGMSoundEffect::Opening] = std::make_shared<MySongTracks>(GetSoundEffect(BGMSoundEffect::Opening), _engine);
	_bgmTracks[BGMSoundEffect::InSide] = std::make_shared<MySongTracks>(GetSoundEffect(BGMSoundEffect::InSide), _engine);
	_bgmTracks[BGMSoundEffect::MoreLp] = std::make_shared<MySongTracks>(GetSoundEffect(BGMSoundEffect::MoreLp), _engine);
	_bgmTracks[BGMSoundEffect::LowLp] = std::make_shared<MySongTracks>(GetSoundEffect(BGMSoundEffect::LowLp), _engine);
	_bgmTracks[BGMSoundEffect::Win] = std::make_shared<MySongTracks>(GetSoundEffect(BGMSoundEffect::Win), _engine);
	_bgmTracks[BGMSoundEffect::Lose] = std::make_shared<MySongTracks>(GetSoundEffect(BGMSoundEffect::Lose), _engine);
}

SoundEffectPlayer::~SoundEffectPlayer()
{
	_engine = nullptr;
}

void SoundEffectPlayer::doPlayerEnterEffect()
{
	_engine->Play(((GetSoundEffect(LobbySoundEffect::PlayerEnter))));
}

void SoundEffectPlayer::doShuffleCardEffect()
{
	_engine->Play(((GetSoundEffect(BattleSoundEffect::ShuffleCard))));
}

void SoundEffectPlayer::doNewTurnEffect()
{
	_engine->Play(((GetSoundEffect(BattleSoundEffect::NewTurn))));
}

void SoundEffectPlayer::doNewPhaseEffect()
{
	_engine->Play(((GetSoundEffect(BattleSoundEffect::NewPhase))));
}

void SoundEffectPlayer::doSetCardEffect()
{
	_engine->Play(((GetSoundEffect(BattleSoundEffect::SetCard))));
}

void SoundEffectPlayer::doSummonEffect()
{
	_engine->Play(((GetSoundEffect(BattleSoundEffect::Summon))));
}

void SoundEffectPlayer::doSpecialSummonEffect()
{
	_engine->Play(((GetSoundEffect(BattleSoundEffect::SpecialSummon))));
}

void SoundEffectPlayer::doFlipCardEffect()
{
	_engine->Play(((GetSoundEffect(BattleSoundEffect::FlipCard))));
}

void SoundEffectPlayer::doActivateEffect()
{
	_engine->Play(((GetSoundEffect(BattleSoundEffect::Activate))));
}

void SoundEffectPlayer::doDrawCardEffect() {
	_engine->Play(((GetSoundEffect(BattleSoundEffect::DrawCard))));
}

void SoundEffectPlayer::doDamageEffect() {
	_engine->Play(((GetSoundEffect(BattleSoundEffect::Damage))));
}

void SoundEffectPlayer::doGainLpEffect() {
	_engine->Play(((GetSoundEffect(BattleSoundEffect::GainLp))));
}

void SoundEffectPlayer::doEquipEffect() {
	_engine->Play(((GetSoundEffect(BattleSoundEffect::Equip))));
}

void SoundEffectPlayer::doAddCounterEffect() {
	_engine->Play(((GetSoundEffect(BattleSoundEffect::AddCounter))));
}

void SoundEffectPlayer::doRemoveCounterEffect() {
	_engine->Play(((GetSoundEffect(BattleSoundEffect::RemoveCounter))));
}

void SoundEffectPlayer::doAttackEffect() {
	_engine->Play(((GetSoundEffect(BattleSoundEffect::Attack))));
}

void SoundEffectPlayer::doCoinFlipEffect() {
	_engine->Play(((GetSoundEffect(BattleSoundEffect::CoinFlip))));
}

void SoundEffectPlayer::doDiceRollEffect() {
	_engine->Play(((GetSoundEffect(BattleSoundEffect::DiceRoll))));
}

void SoundEffectPlayer::doChatEffect() {
	_engine->Play(((GetSoundEffect(LobbySoundEffect::Chat))));
}

void SoundEffectPlayer::doDestroyEffect() {
	_engine->Play(((GetSoundEffect(BattleSoundEffect::Destroy))));
}

void SoundEffectPlayer::setSEEnabled(bool enabled) {
	if (enabled)
		_engine->SetSoundVolume(1.0f);
	else
		_engine->SetSoundVolume(0.0f);
}

void SoundEffectPlayer::doMenuItemMoveInEffect()
{
	_engine->Play(((GetSoundEffect(UISoundEffect::MenuItemMoveIn))));
}

void SoundEffectPlayer::doMenuItemClickEffect()
{
	_engine->Play(((GetSoundEffect(UISoundEffect::MenuItemClick))));
}

void SoundEffectPlayer::doCheckBoxChange()
{
	_engine->Play(((GetSoundEffect(UISoundEffect::CheckBoxChange))));
}

void SoundEffectPlayer::doCardPickUp()
{
	_engine->Play(((GetSoundEffect(UISoundEffect::CardPickUp))));
}

void SoundEffectPlayer::doCardAttached()
{
	_engine->Play(((GetSoundEffect(UISoundEffect::CardAttached))));
}

void SoundEffectPlayer::doCardMoveIn()
{
	_engine->Play(((GetSoundEffect(UISoundEffect::CardMoveIn))));
}

void SoundEffectPlayer::doSaveDeck()
{
	_engine->Play(((GetSoundEffect(UISoundEffect::SaveDeck))));
}

void SoundEffectPlayer::doBack()
{
	_engine->Play(((GetSoundEffect(UISoundEffect::Back))));
}

void SoundEffectPlayer::doStartGame()
{
	_engine->Play(((GetSoundEffect(LobbySoundEffect::StartGame))));
}

void SoundEffectPlayer::doReady()
{
	_engine->Play(((GetSoundEffect(LobbySoundEffect::Ready))));
}

void SoundEffectPlayer::doUnReady()
{
	_engine->Play(((GetSoundEffect(LobbySoundEffect::Unready))));
}

void SoundEffectPlayer::doPlayerExit()
{
	_engine->Play(((GetSoundEffect(LobbySoundEffect::PlayerExit))));
}

void SoundEffectPlayer::Update()
{
	std::lock_guard<std::mutex> guard(_bgmMutex);
	_bgmTracks[_curBGMSoundEffect]->Update();
}


void SoundEffectPlayer::doMenuBGM()
{
	std::lock_guard<std::mutex> guard(_bgmMutex);
	if (_curBGMSoundEffect != BGMSoundEffect::Menu)
	{
		_bgmTracks[_curBGMSoundEffect]->Stop();
		_curBGMSoundEffect = BGMSoundEffect::Menu;
		auto _curTracks = _bgmTracks[_curBGMSoundEffect];
		_curTracks->NextTrack();
	}
}

void SoundEffectPlayer::doOpeningBGM()
{
	std::lock_guard<std::mutex> guard(_bgmMutex);
	if (_curBGMSoundEffect != BGMSoundEffect::Opening)
	{
		_bgmTracks[_curBGMSoundEffect]->Stop();
		_curBGMSoundEffect = BGMSoundEffect::Opening;
		auto _curTracks = _bgmTracks[_curBGMSoundEffect];
		_curTracks->NextTrack();
	}
}

void SoundEffectPlayer::doInSideBGM()
{
	std::lock_guard<std::mutex> guard(_bgmMutex);
	if (_curBGMSoundEffect != BGMSoundEffect::InSide)
	{
		_bgmTracks[_curBGMSoundEffect]->Stop();
		_curBGMSoundEffect = BGMSoundEffect::InSide;
		auto _curTracks = _bgmTracks[_curBGMSoundEffect];
		_curTracks->NextTrack();
	}
}

void SoundEffectPlayer::doLowLpBGM()
{
	std::lock_guard<std::mutex> guard(_bgmMutex);
	if (_curBGMSoundEffect != BGMSoundEffect::LowLp)
	{
		_bgmTracks[_curBGMSoundEffect]->Stop();
		_curBGMSoundEffect = BGMSoundEffect::LowLp;
		auto _curTracks = _bgmTracks[_curBGMSoundEffect];
		_curTracks->NextTrack();
	}
}

void SoundEffectPlayer::doMoreLpBGM()
{
	std::lock_guard<std::mutex> guard(_bgmMutex);
	if (_curBGMSoundEffect != BGMSoundEffect::MoreLp)
	{
		_bgmTracks[_curBGMSoundEffect]->Stop();
		_curBGMSoundEffect = BGMSoundEffect::MoreLp;
		auto _curTracks = _bgmTracks[_curBGMSoundEffect];
		_curTracks->NextTrack();
	}
}

void SoundEffectPlayer::doBGMWin()
{
	std::lock_guard<std::mutex> guard(_bgmMutex);
	if (_curBGMSoundEffect != BGMSoundEffect::Win)
	{
		_bgmTracks[_curBGMSoundEffect]->Stop();
		_curBGMSoundEffect = BGMSoundEffect::Win;
		auto _curTracks = _bgmTracks[_curBGMSoundEffect];
		_curTracks->NextTrack();
	}
}

void SoundEffectPlayer::doBGMLose()
{
	std::lock_guard<std::mutex> guard(_bgmMutex);
	if (_curBGMSoundEffect != BGMSoundEffect::Lose)
	{
		_bgmTracks[_curBGMSoundEffect]->Stop();
		_curBGMSoundEffect = BGMSoundEffect::Lose;
		auto _curTracks = _bgmTracks[_curBGMSoundEffect];
		_curTracks->NextTrack();
	}
}

