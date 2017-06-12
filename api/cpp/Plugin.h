/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#include "Core.h"
#include <deque>
#pragma once

#pragma warning( disable: 4251 ) // 

namespace wtw
{
	class LIBWTW_API CPluginBase
	{
	protected:
		CPluginBase();

	public:
		virtual int load(DWORD callReason);
		virtual int unload(DWORD callReason);

	protected:
		std::deque<HANDLE> m_hRegFuncs;
		std::deque<HANDLE> m_hHookedEvs;

	public:
		HRESULT addFunction(const wchar_t *name, WTWFUNCTION fn, void *ownerData);
		HRESULT hookEvent(const wchar_t *name, WTWHOOK cb, void *cbData);

		HRESULT cleanupFunctions();
		HRESULT cleanupEvents();
	};

};

#define WTW_PLUGIN_INFO(name,desc,copy,author,author_mail,www,version,updateUrl,cls,guid,opts) WTWPLUGINFO g_pluginInfo = {\
	sizeof(WTWPLUGINFO),\
	name, desc, copy, author, author_mail, www, \
	version, PLUGIN_API_VERSION, updateUrl, \
	cls, NULL, guid, 0, opts, \
	0, 0 }; \

#define WTW_PLUGIN_CLASS(mainClass) mainClass g_pluginInterface; wtw::CPluginBase *g_pluginInterfacePtr = &g_pluginInterface; 
