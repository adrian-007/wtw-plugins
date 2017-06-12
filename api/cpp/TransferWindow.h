/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "Core.h"
#include "..\wtwFileTransferManager.h"
#include "CriticalSection.h"
#include "BaseObject.h"

namespace wtw {

	class LIBWTW_API CTransferWindow : public CBaseObject
	{
	private:
		wtw::CCriticalSection m_objectLock;

	public:
		CTransferWindow();
		virtual ~CTransferWindow();

	protected:
		void *m_pWnd;
		WTWFUNCTION m_pFnUpdate;
		void *m_oD;

	private:
		static WTW_PTR onStaticWindowCallback(WTW_PARAM wP, WTW_PARAM, void *od);

	protected:
		virtual WTW_PTR onWindowCallback(wtwFTEvent *pEvent);

	public:
		void initializeWindow(
			const wchar_t *dialogTitle,
			wtwContactDef *pContact,
			HINSTANCE hInst,
			const wchar_t *initialStatus = L"");
		void enableCloseButton(int bEnable);
		void enableAbortButton(int bEnable);
		void setFileSize(ULONGLONG iSize, int bSetTotal);
		void setFileProgress(ULONGLONG iTransferred, int bSetTotal);
		void setStatusText(const wchar_t *ptr);
		void setFileLabel(const wchar_t *ptr);
		void setOpenTarget(const wchar_t *text, const wchar_t *target);
		void setStatus(DWORD transferStatus);
		virtual void drop();
		void setTotalSize(ULONGLONG iSize);
		void setTotalProgress(ULONGLONG iTransferred);
		void *getCtrlPtr();
	};
};