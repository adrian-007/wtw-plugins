#pragma once

#include "Core.h"
#include "String.h"
#include <wtwProtocol.h>
	

namespace wtw
{
	class LIBWTW_API CProtocol : public CBaseObject
	{
	public:
		CProtocol(CCore *pF, const wchar_t * netClass, const wchar_t * netName);
		virtual ~CProtocol();

	protected:
		int m_protoSID;
		wtw::CString m_netClass, m_netName, m_netGuid;
		CCore *m_pF;
		DWORD m_conStatus;

	protected:
		static WTW_PTR _onDeleteContact(WTW_PARAM wP, WTW_PARAM lP, void *d);
		static WTW_PTR _onAuthContact(WTW_PARAM wP, WTW_PARAM lP, void *d);
		static WTW_PTR _onMoveContact(WTW_PARAM wP, WTW_PARAM lP, void *d);

	protected:
		virtual WTW_PTR onDeleteContact(const wtwContactDef & ct);
		virtual WTW_PTR onAuthContact(const wtwContactDef & ct, DWORD flags);
		virtual WTW_PTR onMoveContact(const wtwContactDef & ct, const wtwMoveContactDef & groupInfo);

	public:
		HRESULT protocolRegister(const wchar_t * netDescription, wchar_t * pNetGuid, DWORD netCaps1 = WTW_PROTO_CAPS1_MASK_BASIC,
			DWORD netCaps2 = 0, DWORD flags = WTW_PROTO_FLAG_GEN_GUID_IF_NULL);
		HRESULT protocolUnregister();

		const wchar_t * getNetGuid();
	};
};