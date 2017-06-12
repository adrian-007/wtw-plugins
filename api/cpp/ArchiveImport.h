/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "Globals.h"
#include "..\wtwPlugin.h"
#include "..\wtwArchImport.h"
#include "Core.h"
#include "File.h"
#include "String.h"
#include "..\wtwProtocol.h"
#include "BaseObject.h"

namespace wtw 
{
	namespace arch
	{
		WTW_PTR LIBWTW_API writeSMS(const wchar_t * phone,
			const wchar_t * text, int bIncoming, QWORD time, const wchar_t * extra = NULL, WTWFUNCTION cb = NULL, void *cbData = NULL);
		WTW_PTR LIBWTW_API writeChatMsg(wtwMessageDef *pMessage,
			const __int64 & chatId, const __int64 & tempId, DWORD writeFlags, WTWFUNCTION cb = NULL, void *cbData = NULL);
		WTW_PTR LIBWTW_API writeMsg(wtwMessageDef *pMessage,
			DWORD writeFlags, WTWFUNCTION cb = NULL, void *cbData = NULL);

		WTW_PTR LIBWTW_API transactionBegin();
		WTW_PTR LIBWTW_API transactionCommit();
		WTW_PTR LIBWTW_API transactionRollback();
	};

	class LIBWTW_API CArchiveImport : public CBaseObject
	{
	public:
		CArchiveImport(const wchar_t *id, const wchar_t * mechName, void *cbData);
		virtual ~CArchiveImport();

	protected:
		virtual WTW_PTR onImport(void *cbData, HWND hMainDlg) = 0;

	private:
		static WTW_PTR _onImport(WTW_PARAM wP, WTW_PARAM lP, void *cbData);

	public:
		WTW_PTR registerMechanism();
		WTW_PTR unregisterMechanism();

	protected:
		void setProgress(int percent);
		void setStatusText(const wchar_t* text);

	private:
		CString m_id, m_mechName;
		void *m_cbData;
		HWND m_hStatus;
		HWND m_hProgress;
	};
};