/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "Core.h"

namespace wtw
{
	// {7112E353-E8C1-4CB2-94A4-4136D7E17811}
	static const GUID WTW_CLASS_INTERNET_HTTP =
		{ 0x7112e353, 0xe8c1, 0x4cb2, { 0x94, 0xa4, 0x41, 0x36, 0xd7, 0xe1, 0x78, 0x11 } };
	// {EC067782-3DD8-4087-8204-AB9B09076B18}
	static const GUID WTW_CLASS_SETTINGS =
		{ 0xec067782, 0x3dd8, 0x4087, { 0x82, 0x4, 0xab, 0x9b, 0x9, 0x7, 0x6b, 0x18 } };
	// {248CAC5A-68A0-487A-98A3-FF79435BD9E6}
	static const GUID WTW_CLASS_SETTINGS_BINDER =
		{ 0x248cac5a, 0x68a0, 0x487a, { 0x98, 0xa3, 0xff, 0x79, 0x43, 0x5b, 0xd9, 0xe6 } };

	class LIBWTW_API CFactory
	{
	private:
		CFactory();

	public:
		static HRESULT createClassInstance(const GUID & guid, void **ppOut);
	};
};