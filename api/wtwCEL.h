#pragma once

// Custom Encryption Layer

#include "wtwPlugin.h"
#include "wtwContact.h"

struct wtwCELMessage
{
	__wtwStdStructDefs(wtwCELMessage);

	int			structSize;

	wtwContactDef contactData;
	
	const wchar_t *pProtocolGuid;
	
	void *		pText;		// wtw::CString

	DWORD		flags;
};

#define WTW_CEL_RET_OK			S_OK
#define WTW_CEL_RET_CONSUME		1
#define WTW_CEL_RET_ENCRYPTED	2
#define WTW_CEL_RET_DECRYPTED	3

#define WTW_CEL_MESSAGE_RECEIVED		L"WTW/CEL/MsgReceived"	// wParam = wtwCELMessage *
#define WTW_CEL_MESSAGE_BEFORE_SEND		L"WTW/CEL/MsgSend"		// wParam = wtwCELMessage *