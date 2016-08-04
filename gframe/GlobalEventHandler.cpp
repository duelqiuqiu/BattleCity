#include "GlobalEventHandler.h"


/*
bool ygo::GlobalEventHandler::OnEvent(const irr::SEvent& event)
{
	std::lock_guard<std::mutex> guard(_mutex);
	switch (event.EventType)
	{
	case irr::EEVENT_TYPE::EET_GUI_EVENT:
	{
		for (auto itor : _guiEventCallbacks)
		{
			if (event.EventType == itor.second.eventType && event.GUIEvent.Caller->getID() == itor.second.id)
				if (itor.second.func(event.GUIEvent) == true)
					break;
		}
		break;
	}
	case irr::EEVENT_TYPE::EET_KEY_INPUT_EVENT:
		for (auto itor : _keyInputEventCallbacks)
		{
			if (itor.second(event.KeyInput))
				break;;
		}
		break;
	case irr::EEVENT_TYPE::EET_MOUSE_INPUT_EVENT:
		for (auto itor : _mouseInputEventCallbacks)
		{
			if (itor.second(event.MouseInput))
				break;;
		}
		break;
	default:
		break;
	}
	return false;
}

unsigned int ygo::GlobalEventHandler::RegisterMouseInputEvent(const MouseInputEventCallBack& func)
{
	std::lock_guard<std::mutex> guard(_mutex);
	_mouseInputEventCallbacks[++_acc] = func;
	return _acc;
}

unsigned int ygo::GlobalEventHandler::RegisterKeyInputEvent(const KeyInputEventCallBack& func)
{
	std::lock_guard<std::mutex> guard(_mutex);
	_keyInputEventCallbacks[++_acc] = func;
	return _acc;
}

void ygo::GlobalEventHandler::UnregisterEvent(unsigned int handlerID)
{
	std::lock_guard<std::mutex> guard(_mutex);
	_guiEventCallbacks.erase(handlerID);
	_keyInputEventCallbacks.erase(handlerID);
	_mouseInputEventCallbacks.erase(handlerID);
}

unsigned int ygo::GlobalEventHandler::RegisterGUIEvent(irr::gui::EGUI_EVENT_TYPE eventType, int id, const GUIEventCallBack& func)
{
	std::lock_guard<std::mutex> guard(_mutex);
	GUIEventInfo info;
	info.eventType = eventType;
	info.id = id;
	info.func = func;
	_guiEventCallbacks[++_acc] = info;
	return _acc;
}
*/

bool ygo::GlobalEventHandler::OnEvent(const irr::SEvent& event)
{
	bool res = false;
	if (_userEventHandler)
		res = _userEventHandler->OnEvent(event);
	if (_gameEventHandler)
		res = _gameEventHandler->OnEvent(event);
	return false;
}

void ygo::GlobalEventHandler::SetUserEventHandler(irr::IEventReceiver* h)
{
	_userEventHandler = h;
}

void ygo::GlobalEventHandler::SetGameEventHandler(irr::IEventReceiver* h)
{
	_gameEventHandler = h;
}

irr::IEventReceiver* ygo::GlobalEventHandler::GetUserEventHandler()
{
	return _userEventHandler;
}

irr::IEventReceiver* ygo::GlobalEventHandler::GetGameEventHandler()
{
	return _gameEventHandler;
}
