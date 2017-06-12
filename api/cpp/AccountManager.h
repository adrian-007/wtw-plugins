/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "Globals.h"
#include "..\wtwPlugin.h"
#include "..\wtwOther.h"
#include "..\wtwAccountManager.h"
#include "Core.h"
#include "File.h"
#include "String.h"
	

namespace wtw 
{
	class LIBWTW_API CAccountManager : public CBaseObject
	{
	public:
		CAccountManager(const wchar_t * netClass, const wchar_t * netName);		
		virtual ~CAccountManager();

	public:
		WTW_PTR addAccount(
			int netId,
			const wchar_t * accountName, const wchar_t * accountInfo,
			DWORD flags = 0, const wchar_t * iconId = NULL);

	protected:
		virtual WTW_PTR onAccountList() = 0;
		virtual WTW_PTR onAccountAdd() = 0;

		virtual WTW_PTR onAccountDelete(const wtwACMAccount *pAccountInfo);
		virtual WTW_PTR onAccountInfo(const wtwACMAccount *pAccountInfo);
		virtual WTW_PTR onAccountEdit(const wtwACMAccount *pAccountInfo);

	private:
		static WTW_PTR onEvent(WTW_PARAM wP, WTW_PARAM lP, void *od);

	protected:
		CString m_netClass;
	};
};