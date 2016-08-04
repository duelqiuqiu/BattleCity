#pragma once

#include <irrlicht.h>
#include <functional>
#include <tuple>
#include <map>
#include <mutex>
#include <algorithm>
#include <vector>
namespace ygo
{
	class GlobalEventHandler : public irr::IEventReceiver
	{
		/*
		typedef std::function<bool(const irr::SEvent::SGUIEvent& eventInfo)> GUIEventCallBack;
		typedef std::function<bool(const irr::SEvent::SKeyInput& eventInfo)> KeyInputEventCallBack;
		typedef std::function<bool(const irr::SEvent::SMouseInput& eventInfo)> MouseInputEventCallBack;
		typedef std::tuple<irr::gui::EGUI_EVENT_TYPE, int> GUIEventTuple;
	
		std::mutex _mutex;

		struct GUIEventInfo
		{
			irr::gui::EGUI_EVENT_TYPE eventType; 
			int id; 
			GUIEventCallBack func;
		};

		

		std::map<unsigned int , GUIEventInfo> _guiEventCallbacks;
		std::map<unsigned int, KeyInputEventCallBack> _keyInputEventCallbacks;
		std::map<unsigned int, MouseInputEventCallBack> _mouseInputEventCallbacks;
		
		unsigned int _acc = 0;
		*/
		irr::IEventReceiver* _gameEventHandler = nullptr;
		irr::IEventReceiver* _userEventHandler = nullptr;
	public:
		/*
		unsigned int RegisterGUIEvent(irr::gui::EGUI_EVENT_TYPE eventType, int id, const GUIEventCallBack& func);

		void UnregisterEvent(unsigned int handlerID);

		unsigned int RegisterKeyInputEvent(const KeyInputEventCallBack& func);

		unsigned int RegisterMouseInputEvent(const MouseInputEventCallBack& func);

		*/

		irr::IEventReceiver* GetGameEventHandler();

		irr::IEventReceiver* GetUserEventHandler();

		void SetGameEventHandler(irr::IEventReceiver* h);

		void SetUserEventHandler(irr::IEventReceiver* h);

		virtual bool OnEvent(const irr::SEvent& event) override;

	};
}
