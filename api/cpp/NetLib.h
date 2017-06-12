/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "..\wtwPlugin.h"
#include "Core.h"
#include "String.h"
#include <string>
#include "BaseObject.h"

namespace wtw 
{

	class LIBWTW_API CNetworkSocket
	{
	public:
		CNetworkSocket(const wchar_t *id);
		virtual ~CNetworkSocket();

	protected:
		CString m_socketId;
		
	private:
		static WTW_PTR onNetlibEvent(WTW_PARAM wP, WTW_PARAM lP, void *p);
	public:
		void destroy();
		int create(int bUseSSL);
		int close();
		int connect(QWORD devAddr, const wchar_t *classId);
		int connect(QWORD devAddr, const GUID & classId);
		int connect(const wchar_t *pHost, DWORD port);
		int connect(
			const wchar_t *pHost, 
			DWORD port, 
			const wchar_t *pProxyHost, 
			DWORD proxyPort,
			DWORD proxyType);

		int connectBySRV(const wchar_t *pSrvLoc, const wchar_t *pFallbackHost, DWORD fallbackPort);
		int startSSL();

		int send(const void *pData, int dataLen);

	protected:
		virtual void onConnect(int bError);
		virtual void onDestroy();
		virtual void onClose();
		virtual void onSendCompleted();
		virtual void onReceive(const char *pData, int pDataLen);
		virtual void onSSLStatus(int bSucceded);
		virtual void onError();

	};

};