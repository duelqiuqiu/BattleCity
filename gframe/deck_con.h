#ifndef DECK_CON_H
#define DECK_CON_H

#include "config.h"
#include <unordered_map>
#include <vector>
#include "client_card.h"

#include "config.h"

namespace ygo {

class DeckBuilder: public irr::IEventReceiver {
	int last_hovered_code = 0;
public:
	virtual bool OnEvent(const irr::SEvent& event);

	void KeyInput_PressedESC();

	void KeyInput_PresedR(const irr::SEvent &event);

	void MouseWhell(const irr::SEvent &event);

	void MouseMoved(const irr::SEvent &event);

	void MiddleMouseLeftUp();

	void RightMouseLeftUp();

	void LeftMouseLeftUp();

	void LeftMousePressedDown(const irr::SEvent &event);

	void ComboBoxChanged_SortType();

	void ComboBoxChanged_MainType();

	void ComboBoxChanged_DBDecks();

	void ComboBoxChanged_DBLFList();

	void EditBoxEnter_KeyWord();

	void ScrollBarChanged_CardText();

	void ButtonClicked_YES();

	void ButtonClicked_SideOK();

	void ButtonClicked_CategoryOK();

	void ButtonClicked_StartFilter();

	void ButtonClicked_LeaveGame();

	void ButtonClicked_DeleteDeck();

	void ButtonClicked_SaveDeckAs();

	void ButtonClicked_SaveDeck();

	void ButtonClicked_ShuffleDeck();

	void ButtonClicked_SortDeck();

	void ButtonClicked_ClearDeck();

	void FilterCards();
	void ClearFilter();
	void ClearSearch();
	void SortList();
	
	long long filter_effect;
	unsigned int filter_type;
	unsigned int filter_type2;
	unsigned int filter_attrib;
	unsigned int filter_race;
	unsigned int filter_atktype;
	int filter_atk;
	unsigned int filter_deftype;
	int filter_def;
	unsigned int filter_lvtype;
	unsigned int filter_lv;
	unsigned int filter_scltype;
	unsigned int filter_scl;
	int filter_lm;
	int hovered_code;
	int hovered_pos;
	int hovered_seq;
	int click_pos;
	bool is_draging;
	int dragx;
	int dragy;
	size_t pre_mainc;
	size_t pre_extrac;
	size_t pre_sidec;
	code_pointer draging_pointer;
	
	std::unordered_map<int, int>* filterList;
	std::vector<code_pointer> results;
	wchar_t result_string[8];

	bool is_deleting = false;
	bool is_clearing = false;
};

}

#endif //DECK_CON
