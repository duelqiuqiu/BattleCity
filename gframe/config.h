#ifndef __CONFIG_H
#define __CONFIG_H

#pragma once
#if _DEBUG
#define _IRR_STATIC_LIB_
#endif
#define _IRR_COMPILE_WITH_DX9_DEV_PACK
#ifdef _IRR_ANDROID_PLATFORM_

#include <android_native_app_glue.h>
#include <android_tools.h>
#endif
#ifdef _WIN32

#include <WinSock2.h>
#include <windows.h>
#include <ws2tcpip.h>
/*
#ifdef _MSC_VER
#define myswprintf _swprintf
#else
#define myswprintf swprintf
#endif
*/
#define socklen_t int

#else //_WIN32

#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>

#define SD_BOTH 2
#define SOCKET int
#define closesocket close
#define INVALID_SOCKET -1
#define SOCKET_ERROR -1
#define SOCKADDR_IN sockaddr_in
#define SOCKADDR sockaddr
#define SOCKET_ERRNO() (errno)

#endif
#if !_WIN32
#define ARRAYSIZE(arr) (sizeof (arr) / sizeof(*arr) / static_cast<size_t>(!(sizeof(arr) % sizeof(*(arr)))))
#endif

#ifdef _IRR_ANDROID_PLATFORM_
#include <wchar.h>
#include <xstring.h>
#define myswprintf(buf,fmt, ...) swprintf_x(buf, 4096, fmt, ##__VA_ARGS__)
#define myswprintf_(buf, bufsize, fmt, ...) swprintf_x(buf, bufsize, fmt, ##__VA_ARGS__)
#define _wtoi wtoi_x
#define mywcscat wcscat_x
#else
#define myswprintf(buf, fmt, ...) swprintf(buf, ARRAYSIZE(buf), fmt, ##__VA_ARGS__)
#define myswprintf_(buf, bufsize, fmt, ...) swprintf(buf, bufsize, fmt, ##__VA_ARGS__)
#define mywcscat wcscat
/*
inline int _wtoi(const wchar_t * s) {
	wchar_t * endptr;
	return (int)wcstol(s, &endptr, 10);
}
*/
#endif


#include <irrlicht.h>
#include <os.h>
#ifdef _IRR_ANDROID_PLATFORM_
#include <GLES/gl.h>
#include <GLES/glext.h>
#include <GLES/glplatform.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#include "CGUITTFont.h"
#include "CGUIImageButton.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#ifdef _IRR_ANDROID_PLATFORM_
#include <bufferio_android.h>
#else
#include "bufferio.h"
#endif
#include "mymutex.h"
#include "mysignal.h"
#include "mythread.h"
#include "../ocgcore/ocgapi.h"
#include "../ocgcore/card.h"

#ifdef _IRR_ANDROID_PLATFORM_
#include "os.h"
#endif

#if defined(_IRR_ANDROID_PLATFORM_)
#include <CustomShaderConstantSetCallBack.h>
#endif

using namespace irr;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;
using namespace os;

extern const unsigned short PRO_VERSION;
extern int enable_log;
extern bool exit_on_return;

#endif
