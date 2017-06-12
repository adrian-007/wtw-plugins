/*
** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#define __UTILSAPI __cdecl

#include "Core.h"

/*
	Use ONLY if x has defined copy operator (!);
*/

#define WTW_NEW_GUID_ASSIGN_A(x) \
	{char *g; if(SUCCEEDED(wtw::CGuid::genGuid(&g))) { x = g; wtw::CGuid::release(g);} else {x="";}}
#define WTW_NEW_GUID_ASSIGN_W(x) \
	{wchar_t *g; if(SUCCEEDED(wtw::CGuid::genGuid(&g))) { x = g; wtw::CGuid::release(g);} else {x=L"";}}

namespace wtw 
{
	class LIBWTW_API CGuid
	{
	public:
		CGuid();

	protected:
		static void* _gen();

	public:
		static HRESULT genGuid(wchar_t ** regGuid);
		static HRESULT genGuid(char ** regGuid);
		static HRESULT release(wchar_t * regGuid);
		static HRESULT release(char * regGuid);
	};
}