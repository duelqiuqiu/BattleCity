#include "config.h"
#include "game.h"
#include "data_manager.h"
#include <event2/thread.h>
#include <boost/lexical_cast.hpp>
#include <locale>

#include "User.h"

#ifdef _IRR_ANDROID_PLATFORM_
#include "android_native_app_glue.h"
#endif

int enable_log = 0;
bool exit_on_return = false;

bool g_ProgramEnded = false;

#ifdef _IRR_ANDROID_PLATFORM_
void PostTextFromAndroid(const std::wstring& text)
{
	ygo::mainGame->gMutex.Lock();
	for (wchar_t c : text)
	{
		irr::SEvent event;
		memset(&event,0, sizeof(irr::SEvent));
		event.EventType = irr::EEVENT_TYPE::EET_KEY_INPUT_EVENT;
		event.KeyInput.PressedDown = true;
		event.KeyInput.Char = c;
		ygo::mainGame->device->postEventFromUser(event);
	}
	ygo::mainGame->gMutex.Unlock();
}
#endif


int char2int(char input)
{
	if (input >= '0' && input <= '9')
		return input - '0';
	if (input >= 'A' && input <= 'F')
		return input - 'A' + 10;
	if (input >= 'a' && input <= 'f')
		return input - 'a' + 10;
	throw std::invalid_argument("Invalid input string");
}

// This function assumes src to be a zero terminated sanitized string with
// an even number of [0-9a-f] characters, and target to be sufficiently large
void hex2bin(const char* src, char* target)
{
	while (*src && src[1])
	{
		*(target++) = char2int(*src) * 16 + char2int(src[1]);
		src += 2;
	}
}

#if _WIN32
#include "resource.h"
void SetIcon()
{
	HWND hwnd;
	if (ygo::mainGame->gameConf.use_d3d)
		hwnd = (HWND)ygo::mainGame->driver->getExposedVideoData().D3D9.HWnd;
	else
		hwnd = (HWND)ygo::mainGame->driver->getExposedVideoData().OpenGLWin32.HWnd;
	HICON hIcon = (HICON)LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICON1));
	// Set big icon (ALT+TAB)
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
	// Set little icon (titlebar) 
	SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
}
#endif

#ifdef _IRR_ANDROID_PLATFORM_
void android_main_entry(android_app* app) {
	//app->inputPollSource.process = android::process_input;
#else
int main(int argc, char* argv[]) {
#endif

	setlocale(LC_ALL, ""); // 一定要这句，否则IRR粘贴文字会乱码

#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);
	evthread_use_windows_threads();
#else
	evthread_use_pthreads();
#endif //_WIN32
	ygo::Game* _game = new ygo::Game();
	ygo::mainGame = _game;
#ifdef _IRR_ANDROID_PLATFORM_
	if(!ygo::mainGame->Initialize(app))
		return;
#else
	if(!ygo::mainGame->Initialize())
		return 0;

	SetIcon();
#endif


	std::wstring ipStrW = ygo::dataManager.GetCustomString(2074);
	auto ipStr = std::string(ipStrW.begin(), ipStrW.end());
	int port = boost::lexical_cast<int>(ygo::dataManager.GetCustomString(2075));
	auto roomport = boost::lexical_cast<unsigned short>(ygo::dataManager.GetCustomString(2083));
	User::Instance()->Init();
	User::Instance()->SetConnectAddress(ipStr, port, roomport);
	User::Instance()->InitGUI(ygo::mainGame->env);
	User::Instance()->InitSkin(ygo::mainGame->env);
	ygo::mainGame->device->setEventReceiver(&ygo::mainGame->globalEventHandler);
#ifndef _IRR_ANDROID_PLATFORM_
	for(int i = 1; i < argc; ++i) {
		/*command line args:
		 * -j: join host (host info from system.conf)
		 * -d: deck edit
		 * -r: replay */
		if(argv[i][0] == '-' && argv[i][1] == 'e') {
			ygo::dataManager.LoadDB(&argv[i][2]);
		} else if(!strcmp(argv[i], "-j") || !strcmp(argv[i], "-j2") || !strcmp(argv[i], "-j3") || !strcmp(argv[i], "-d") || !strcmp(argv[i], "-r") || !strcmp(argv[i], "-s")) {
			exit_on_return = true;
			irr::SEvent event;
			event.EventType = irr::EET_GUI_EVENT;
			event.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;

			if(!strcmp(argv[i], "-j")) {
				User::Instance()->HideLoginWindow();
				const char* tokenStr = getenv("TOKEN");
				uint32 gameid = boost::lexical_cast<std::uint64_t>(getenv("GAMEID"));
				byte token[16];
				hex2bin(tokenStr, (char*)token);
				User::Instance()->SetToken(token);
				ygo::mainGame->ebJoinIP->setID(gameid);
				event.GUIEvent.Caller = ygo::mainGame->btnJoinHost;
				//ygo::mainGame->device->setEventReceiver(&ygo::mainGame->menuHandler);
				ygo::mainGame->globalEventHandler.SetGameEventHandler(&ygo::mainGame->menuHandler);
				ygo::mainGame->device->postEventFromUser(event);
			}
			else if (!strcmp(argv[i], "-j2"))
			{
				const wchar_t* usernameStr = _wgetenv(L"USERNAME");
				const wchar_t* passwordStr = _wgetenv(L"PASSWORD");
				const wchar_t* roomIPStr = _wgetenv(L"ROOMIP");
				const wchar_t* roomPortStr = _wgetenv(L"ROOMPORT");
				uint32 gameid = boost::lexical_cast<std::uint64_t>(getenv("GAMEID"));
				ygo::mainGame->ebJoinIP->setText(roomIPStr);
				ygo::mainGame->ebJoinPort->setText(roomPortStr);
				ygo::mainGame->ebJoinIP->setID(gameid);

				User::Instance()->Username = usernameStr;
				User::Instance()->Password = passwordStr;
				irr::SEvent event2;
				event2.EventType = irr::EET_GUI_EVENT;
				event2.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
				event2.GUIEvent.Caller = User::Instance()->loginWindow->getElementFromId(4);
				dynamic_cast<LoginWindowEventReceiver*>(ygo::mainGame->globalEventHandler.GetUserEventHandler())->AutoLogin = true;
				dynamic_cast<LoginWindowEventReceiver*>(ygo::mainGame->globalEventHandler.GetUserEventHandler())->SkipMainMenu = true;
				ygo::mainGame->device->postEventFromUser(event2);
			}
			else if (!strcmp(argv[i], "-j3"))
			{
				exit_on_return = false;
				const wchar_t* usernameStr = _wgetenv(L"USERNAME");
				const wchar_t* passwordStr = _wgetenv(L"PASSWORD");

				User::Instance()->Username = usernameStr;
				User::Instance()->Password = passwordStr;
				irr::SEvent event2;
				event2.EventType = irr::EET_GUI_EVENT;
				event2.GUIEvent.EventType = irr::gui::EGET_BUTTON_CLICKED;
				event2.GUIEvent.Caller = User::Instance()->loginWindow->getElementFromId(4);
				dynamic_cast<LoginWindowEventReceiver*>(ygo::mainGame->globalEventHandler.GetUserEventHandler())->AutoLogin = true;
				dynamic_cast<LoginWindowEventReceiver*>(ygo::mainGame->globalEventHandler.GetUserEventHandler())->SkipMainMenu = false;
				ygo::mainGame->device->postEventFromUser(event2);
			}
			else if(!strcmp(argv[i], "-d")) {
				event.GUIEvent.Caller = ygo::mainGame->btnDeckEdit;
				ygo::mainGame->device->postEventFromUser(event);
			} else if(!strcmp(argv[i], "-r")) {
				event.GUIEvent.Caller = ygo::mainGame->btnReplayMode;
				ygo::mainGame->device->postEventFromUser(event);
				ygo::mainGame->lstReplayList->setSelected(0);
				event.GUIEvent.Caller = ygo::mainGame->btnLoadReplay;
				ygo::mainGame->device->postEventFromUser(event);
			} else if(!strcmp(argv[i], "-s")) {
				event.GUIEvent.Caller = ygo::mainGame->btnServerMode;
				ygo::mainGame->device->postEventFromUser(event);
				ygo::mainGame->lstSinglePlayList->setSelected(0);
				event.GUIEvent.Caller = ygo::mainGame->btnLoadSinglePlay;
				ygo::mainGame->device->postEventFromUser(event);
			}

		}
	}
#endif

	ygo::mainGame->MainLoop();
	g_ProgramEnded = true;
	User::Instance()->Destroy();
#ifdef _WIN32
	WSACleanup();
#else

#endif //_WIN32

	delete _game;
	ygo::mainGame = _game = nullptr;

#ifdef _IRR_ANDROID_PLATFORM_
	return;
#else
	return EXIT_SUCCESS;
#endif
}
