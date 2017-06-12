#pragma once

#include "wtwPlugin.h"
#include "wtwContact.h"

/*******************************************************************************
**
** Function:
**		WTW_FILE_SHARE_MECH_REGISTER
**
** Return value:
**		S_OK, E_*
**
** Parameters:
**		wP - wtwFileShareMechanism*
**		lP - NULL
**
*/

#define WTW_FILE_SHARE_MECH_REGISTER L"FTM/Register"

/*******************************************************************************
**
** Function:
**		WTW_FILE_SHARE_MECH_UNREGISTER
**
** Return value:
**		S_OK, E_*
**
** Parameters:
**		wP - wtwFileShareMechanism*
**		lP - NULL
**
*/

#define WTW_FILE_SHARE_MECH_UNREGISTER L"FTM/Unregister"

struct wtwFileShareMechanism
{
	__wtwStdStructDefs(wtwFileShareMechanism);

	int				structSize;

	const wchar_t * id;			// mechanism/account unique id
	const wchar_t * name;		// name
	const wchar_t * info;		// some other info
	const wchar_t * netClass;	// network class to which this mechanism 
								// can send files or NULL if it supports all networks

	WTWFUNCTION		fnSend;	
	void *			fnSendData;

	const wchar_t * classGuid;	// unique id of transport class (one for all dropbox accunts, etc)
	
	void *			reserved1;
	void *			reserved2;

	DWORD			flags;
};

/*******************************************************************************
**
** Function:
**		WTW_FILE_SHARE_MECH_ENUM
**
** Return value:
**		S_OK, E_*
**
** Parameters:
**		wP - wtwFileShareMechanismEnum*
**		lP - NULL
**
** Remarks:
**		fnSend and fnSendData are not returned
**
*/

#define WTW_FILE_SHARE_MECH_ENUM L"FTM/Enum"

struct wtwFileShareMechanismEnum
{
	__wtwStdStructDefs(wtwFileShareMechanismEnum);

	int				structSize;

	wtwFileShareMechanism *	pMechs;
	int						iMechs;

	DWORD			flags;
};

/*******************************************************************************
**
** Function:
**		WTW_FILE_SHARE_MECH_SEND
**
** Return value:
**		S_OK, E_*
**
** Parameters:
**		wP - wtwFileShareSend
**		lP - NULL
**
*/

#define WTW_FILE_SHARE_MECH_SEND L"FTM/Send"

struct wtwFileShareSend
{
	__wtwStdStructDefs(wtwFileShareSend);

	int				structSize;

	wtwContactDef * pContacts;
	int				iContacts;

	const wchar_t * mechId;

	const wchar_t * filePath;

	WTWFUNCTIONS	cb;
	void *			cbData;

	void * reserved1;
	void * reserved2;

	DWORD			flags;
};

#define WFSM_FLAG_NO_UI				0x0001
#define WFSM_FLAG_SEND_AS_IMAGE		0x0002

struct wtwFileShareEvent
{
	__wtwStdStructDefs(wtwFileShareEvent);

	int				structSize;

	int				event;

	wchar_t *		text;
	void *			userPtr;

	int				progress;
	int				progressMax;

	HRESULT			opStatus;

	DWORD			flags;
};

#define FILE_SHARE_EVENT_INIT		0x01
#define FILE_SHARE_EVENT_COMPLETE	0x02
#define FILE_SHARE_EVENT_UPDATE		0x03

#define WFSE_FLAG_SET_PROGRESS		0x0001
#define WFSE_FLAG_SET_PROGRESS_MAX	0x0002
#define WFSE_FLAG_SET_TEXT			0x0004
#define WFSE_FLAG_SET_USER_PTR		0x0008
#define WFSE_FLAG_SET_OP_STATUS		0x0010
#define WFSE_FLAG_SET_TEXT_LINK		0x0020