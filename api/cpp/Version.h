/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "..\wtwPlugin.h"
#include "Core.h"

namespace wtw
{
	class LIBWTW_API SystemVersion
	{
		friend void wtw::libWtwInit(WTWFUNCTIONS *pFunc);

	protected:
		static OSVERSIONINFOEX m_osve;

	public:
		static bool isEight();
		static bool isEightPlus();
		static bool isSeven();
		static bool isSevenPlus();
		static bool isVista();
		static bool isVistaPlus();
		static bool isXP();
		static bool isXPPlus();

		static DWORD getMajor();
		static DWORD getMinor();
		static DWORD getRevision();
		static DWORD getServicePackLevel();
		
		static bool isHomeEdition();
		
		static bool isServer();
		static bool isDomainController();
		static bool isWorkstation();
	};
};