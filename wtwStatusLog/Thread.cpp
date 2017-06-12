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

#include "stdinc.h"
#include "Thread.hpp"
#include <process.h>

/** mutex impl */
CriticalSection::CriticalSection() {
	InitializeCriticalSection(&cs);
}
CriticalSection::~CriticalSection() {
	DeleteCriticalSection(&cs);
}

void CriticalSection::lock() {
	EnterCriticalSection(&cs);
}

void CriticalSection::unlock() {
	LeaveCriticalSection(&cs);
}


Semaphore::Semaphore() throw() {
	hSemaphore = CreateSemaphore(NULL, 0, MAXLONG, NULL);
}

Semaphore::~Semaphore() throw() {
	CloseHandle(hSemaphore);
}

void Semaphore::signal() throw() { 
	ReleaseSemaphore(hSemaphore, 1, NULL); 
}

bool Semaphore::wait() throw() {
	return WaitForSingleObject(hSemaphore, INFINITE) == WAIT_OBJECT_0; 
}

bool Semaphore::wait(DWORD ms) throw() { 
	return WaitForSingleObject(hSemaphore, ms) == WAIT_OBJECT_0; 
}

/** thread impl */
Thread::Thread() {
	thread = INVALID_HANDLE_VALUE;
}

Thread::~Thread() {
	if(thread != INVALID_HANDLE_VALUE) {
		CloseHandle(thread);
		thread = INVALID_HANDLE_VALUE;
	}
}

void Thread::runThread() {
	join();
	thread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, &threadProc, this, CREATE_SUSPENDED, reinterpret_cast<unsigned int*>(&threadId)));
	if(thread != 0) {
		::ResumeThread(thread);
	}
}

void Thread::join() {
	if(thread == INVALID_HANDLE_VALUE) {
		return;
	}

	DWORD ret;
	do {
		ret = WaitForSingleObject(thread, 1);
	} while(ret != WAIT_OBJECT_0);

	CloseHandle(thread);
	thread = INVALID_HANDLE_VALUE;
	threadId = 0;
}

void Thread::sleep(int ms) {
	::Sleep(static_cast<DWORD>(ms));
}

unsigned int WINAPI Thread::threadProc(LPVOID param) {
	Thread* t = (Thread*)param;

	t->threadMain();

	_endthreadex(0);
	return 0;
}
