/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include <windows.h>
#include "Core.h"
#include "BaseObject.h"

namespace wtw
{

	class LIBWTW_API CCriticalSection : public CBaseObject
	{
	public:
		CCriticalSection()
		{
			InitializeCriticalSection(&_cs);
		};

		virtual ~CCriticalSection()
		{
			Unlock();
			DeleteCriticalSection(&_cs);
		};

	public:
		void Lock()
		{
			EnterCriticalSection(&_cs);
		};

		void Unlock()
		{
			LeaveCriticalSection(&_cs);
		};

	private:
		CRITICAL_SECTION _cs;
	};

}