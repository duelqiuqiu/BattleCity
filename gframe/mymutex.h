#ifndef MUTEX_H
#define MUTEX_H


#include <mutex>


class Mutex {
public:
	Mutex();
	~Mutex();
	void Lock();
	void Unlock();
	bool TryLock();
private:
	bool _locked = false;
	std::mutex _mutex;
};


#endif // MUTEX_H
