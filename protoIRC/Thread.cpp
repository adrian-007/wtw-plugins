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

#include "stdinc.hpp"
#include "Thread.h"
#include <process.h>

Thread::Thread() {
	thread = INVALID_HANDLE_VALUE;
}

Thread::~Thread() {
	join();
}

void Thread::runThread() {
	join();
	thread = reinterpret_cast<HANDLE>(_beginthreadex(0, 0, &threadProc, this, 0, reinterpret_cast<unsigned int*>(&threadId)));
}

void Thread::join(DWORD ms /*= INFINITE*/) {
	if(thread != INVALID_HANDLE_VALUE) {
		WaitForSingleObject(thread, ms);
		CloseHandle(thread);
		thread = INVALID_HANDLE_VALUE;
	}
}

void Thread::sleep(DWORD ms) {
	::Sleep(ms);
}

bool Thread::isThreadRunning() {
	return thread != INVALID_HANDLE_VALUE;
}

unsigned int WINAPI Thread::threadProc(LPVOID param) {
	Thread* t = reinterpret_cast<Thread*>(param);
	t->threadMain();
	return 0;
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
