/*
 * Copyright (C) 2009-2017 adrian_007, adrian-007 on o2 point pl
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef THREAD_H
#define THREAD_H

//#pragma warning(push, 3)
//#include <boost/thread/recursive_mutex.hpp>
//#include <boost/thread/locks.hpp>
//#pragma warning(pop)

//typedef boost::recursive_mutex	CriticalSection;
//typedef boost::lock_guard<boost::recursive_mutex> ScopedLock;

class Thread {
public:
	Thread();
	~Thread();

	void runThread();
	void join(DWORD ms = INFINITE);
	void sleep(DWORD ms);
	bool isThreadRunning();

	virtual void threadMain() = 0;
private:
	static unsigned int WINAPI threadProc(LPVOID param);

	DWORD threadId;
	HANDLE thread;
};

class CriticalSection {
public:
	CriticalSection()
	{
		::InitializeCriticalSection(&cs);
	}

	~CriticalSection()
	{
		::DeleteCriticalSection(&cs);
	}

	void lock()
	{
		::EnterCriticalSection(&cs);
	}

	void unlock()
	{
		::LeaveCriticalSection(&cs);
	}

private:
	CRITICAL_SECTION cs;
};

class Semaphore {
public:
	Semaphore() throw();
	~Semaphore() throw();

	void signal() throw();
	bool wait() throw();
	bool wait(DWORD ms) throw();

private:
	Semaphore(const Semaphore&);
	Semaphore& operator=(const Semaphore&);
	
	HANDLE hSemaphore;
};

class ScopedLock {
public:
	ScopedLock(CriticalSection& _cs) : cs(_cs) {
		cs.lock();
	}
	~ScopedLock() {
		cs.unlock();
	}
private:
	ScopedLock(const ScopedLock&);
	ScopedLock& operator=(const ScopedLock&);
	CriticalSection& cs;
};

#endif
