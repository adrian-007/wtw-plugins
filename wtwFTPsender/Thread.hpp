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

#ifndef THREAD_HPP
#define THREAD_HPP

class CriticalSection {
public:
	CriticalSection();
	~CriticalSection();

	void lock();
	void unlock();
private:
	CRITICAL_SECTION cs;
};

class Thread {
public:
	Thread();

	void runThread();
	void join();
	void sleep(int ms);
	bool isThreadRunning();

	virtual void threadMain() = 0;
private:
	static unsigned int WINAPI threadProc(LPVOID param);

	DWORD threadId;
	HANDLE thread;
};

#endif
