#pragma once
#include "irrlicht.h"
#if _IRR_ANDROID_PLATFORM_
#include "CAndroidGUIEditBox.h"
#include "CAndroidGUIComboBox.h"
#include "CAndroidGUIListBox.h"
#include "CAndroidGUISkin.h"
#endif
class CGUIAdapter
{
public:
	static irr::gui::IGUIEditBox* addEditBox(const wchar_t* text, bool border, irr::gui::IGUIEnvironment *env, const irr::core::rect<irr::s32>& rectangle, irr::gui::IGUIElement* parent, irr::s32 id = 0)
	{
		IGUIEditBox* box;
#if _WIN32
		box = env->addEditBox(text, rectangle, border, parent, id);
#elif _IRR_ANDROID_PLATFORM_
		box = new irr::gui::CAndroidGUIEditBox(text, border, env, parent, id, rectangle);
#else
		static_assert(false, "");
#endif
		//box->setOverrideColor({ 255,0,0,0 });
		return box;
	}

	static irr::gui::IGUIListBox* addGUIListBox(irr::gui::IGUIEnvironment* env, irr::core::rect<irr::s32> rectangle,
		irr::gui::IGUIElement* parent, irr::s32 id, bool drawBack, int scrollbarSize)
	{
#if _WIN32
		return env->addListBox(rectangle, parent, id, drawBack);
#elif _IRR_ANDROID_PLATFORM_
		return new irr::gui::CAndroidGUIListBox(env, parent, id, rectangle, false ,  drawBack, false, scrollbarSize * 2);
		//return env->addListBox(rectangle, parent, id, drawBack);
#else
		return nullptr;
#endif
	}

	static irr::gui::IGUIComboBox* addComboBox(irr::gui::IGUIEnvironment *env, const irr::core::rect<irr::s32>& rectangle, irr::gui::IGUIElement* parent, irr::s32 id = 0)
	{
		irr::gui::IGUIComboBox* box;
#if _WIN32
		box = env->addComboBox(rectangle, parent, id);
#elif _IRR_ANDROID_PLATFORM_
		//return new irr::gui::CAndroidGUIComboBox(env, parent, id, rectangle);
		box = env->addComboBox(rectangle, parent, id);
#else
		static_assert(false, "");
#endif
		return box;
	}
};