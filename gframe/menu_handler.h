#ifndef MENU_HANDLER_H
#define MENU_HANDLER_H

#include "config.h"

namespace ygo {

class MenuHandler: public irr::IEventReceiver {
	int id;

public:
	virtual bool OnEvent(const irr::SEvent& event);

	void ButtonClicked_ModeExit();

	void ButtonClicked_LanMode();

	void ButtonClicked_JoinHost();
	void ButtonClicked_JoinCancel();
	void ButtonClicked_LanRefresh();
	void ButtonClicked_CreateHost();
	void ButtonClicked_HostConfirm();
	void ButtonClicked_HostCancel();
	void ButtonClicked_HostPlayer_DuelList();
	void ButtonClicked_HostPlayer_Observer();
	void ButtonClicked_HostPlayerKick(irr::gui::IGUIElement* caller);
	void ButtonClicked_HostPlayerStart();
	void ButtonClicked_HostPlayerCancel();
	void ButtonClicked_ReplayMode();
	void ButtonClicked_SingleMode();
	void ButtonClicked_LoadReplay();
	void ButtonClicked_CancelReplay();
	void ButtonClicked_LoadSinglePlay();


	void ButtonClicked_DeckEdit();

	void ButtonClicked_SinglePlay();

};

}

#endif //MENU_HANDLER_H
