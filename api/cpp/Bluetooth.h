/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "Core.h"
#include "BaseObject.h"

namespace wtw 
{

	typedef struct 
	{

		unsigned __int64 btAddr;
		std::wstring radioName;

	} btInfo;

	typedef SOCKET	(* btPortOpen	 )	(ULONGLONG devAddr);
	typedef SOCKET	(* btPortOpenSrv )	(ULONGLONG devAddr, GUID guid, DWORD nTimeOut);
	typedef int		(* btPortClose	 )	(SOCKET s);
	typedef int		(* btPortRead	 )	(SOCKET s, char *pBuffer, int pBufferLen);
	typedef int		(* btPortWrite	 )	(SOCKET s, char *pBuffer, int pBufferLen);
	typedef int		(* btPortReset	 )	(SOCKET s);
	typedef int		(* btPortIsOpen )	(SOCKET s);
	typedef int		(* btEnumDevices)	(std::deque<btInfo> &pElements, int bScan);

	class LIBWTW_API CBluetooth : public CBaseObject
	{
	private:
		btPortOpen		_portOpen;
		btPortOpenSrv	_portOpenSrv;
		btPortClose		_portClose;
		btPortRead		_portRead;
		btPortWrite		_portWrite;
		//btPortReset	_portReset;
		//btPortIsOpen	_portIsOpen;
		btEnumDevices	_enumDevices;

	public:
		SOCKET			m_hSocket;

	public:
		CBluetooth();
		virtual ~CBluetooth();

		HRESULT openSerialPort(ULONGLONG devAddr);
		HRESULT openByClass(ULONGLONG devAddr, GUID guid, DWORD rcvTimeoutInMs);
		HRESULT close();
		HRESULT read(void *pBuffer, int bufferSize);
		HRESULT write(void *pBuffer, int bufferSize);
		HRESULT enumerate(std::deque<btInfo> &devs, int bScan);

	private:
		HMODULE m_hInstance;

	};

};