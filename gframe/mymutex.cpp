#include "mymutex.h"
#include <assert.h>
#ifdef _WIN32
#include <windows.h>
#endif


Mutex::Mutex() {
}
Mutex::~Mutex() {
	assert(_locked == false);
}
void Mutex::Lock() {
	try {
		_mutex.lock();
	}
	catch (std::system_error ex)
	{
#ifdef _WIN32
		MessageBox(NULL, ex.what(), "Mutex Lock Operation Exception", MB_ICONEXCLAMATION);
		TerminateProcess(GetCurrentProcess(), 0);
#endif
	}
	_locked = true;
}
void Mutex::Unlock() {
	try {
		_mutex.unlock();
	}
	catch (std::system_error ex)
	{
#ifdef _WIN32
		MessageBox(NULL, ex.what(), "Mutex Unlock Operation Exception", MB_ICONEXCLAMATION);
		TerminateProcess(GetCurrentProcess(), 0);
#endif
	}
	_locked = false;

}
bool Mutex::TryLock() {
	return _mutex.try_lock();
}


