/*
** Copyright (C) 2007-2014, K2T.eu
*/

#ifndef __wtw__cwin_h__
#define __wtw__cwin_h__

#include "wtwPlugin.h"
#include "wtwProtocol.h"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_CREATE_EX
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwConferenceWindow*
**		lP - WTW_PTR* ptr - return value, window pointer
**
** Remarks:
**		If window with specified id already exists, function returns S_FALSE
**		and sets ptr to the old existing window
**
*/

#define WTW_CONFERENCE_WINDOW_CREATE_EX		L"WTW/C9/Create"		// lp WTW_PTR* - handle nowego okna, wparam - wtwconfwindowcreate*

struct wtwConferenceWindow 
{
	__wtwStdStructDefs(wtwConferenceWindow);

	int				structSize;
	const wchar_t * windowId;

	WTWFUNCTION		windowCallback;
	void *			cbData;

	HINSTANCE		hInst;	// not required unless You want to use "destroy all" function

	DWORD			flags;	// WCWF_*
};

#define WCWF_AUTO_TITLE		0x0001	// set window title to tab name (on tab change)

struct wtwConferenceWindowEvent // passed as wP in window/event callback
{
	__wtwStdStructDefs(wtwConferenceWindowEvent);

	int				structSize;

	DWORD			event;

	const wchar_t *	windowId;
	WTW_PTR			windowPtr;

	const wchar_t * tabId;
	
	const void  *	param;
	POINT			pt;

	HWND			windowHwnd;
};

#define CONFERENCE_EVENT_SEND_MESSAGE		0x01 // lp - wtwMessageDef*
#define CONFERENCE_EVENT_WND_CLOSED			0x02 // lp - NULL
#define CONFERENCE_EVENT_CT_LDBLCLK			0x03 // lp - wtwConferenceContact* or NULL
#define CONFERENCE_EVENT_CT_RCLICK			0x04 // lp - wtwConferenceContact* or NULL
#define CONFERENCE_EVENT_CT_LCLICK			0x05 // lp - wtwConferenceContact* or NULL
#define CONFERENCE_EVENT_CMD_LCLICK			0x06 // lp - wtwConferenceCommandInfo *
#define CONFERENCE_EVENT_CMD_RCLICK			0x07 // lp - wtwConferenceCommandInfo *
#define CONFERENCE_EVENT_WND_DESTROYED		0x08 // lp - NULL
#define CONFERENCE_EVENT_WND_CREATED		0x09 // lp - NULL
#define CONFERENCE_EVENT_WND_SHOW			0x0A // lp - 1 - if shown, 0 - otherwise
#define CONFERENCE_EVENT_TAB_CREATED		0x0B // lp - NULL
#define CONFERENCE_EVENT_TAB_DESTROYED		0x0C // lp - NULL
#define CONFERENCE_EVENT_TAB_ON_SHOW		0x0E // lp - 1 - if shown, 0 - otherwise
#define CONFERENCE_EVENT_TAB_BEFORE_CLOSE	0x0F // lp - NULL
#define CONFERENCE_EVENT_LOG_COMMAND		0x10 // lp - wtwConferenceCommnad*

struct wtwConferenceTabEvent	// not used
{
	__wtwStdStructDefs(wtwConferenceTabEvent);

	int				structSize;
};

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_DESTROY
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - NULL
**
*/

#define WTW_CONFERENCE_WINDOW_DESTROY			L"WTW/C9/Destroy"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_DESTROY_DELAYED - destroys window after exiting 
**				from command callback (use if you want to destroy window in 
**				command callback)
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - NULL
**
*/

#define WTW_CONFERENCE_WINDOW_DESTROY_DELAYED	L"WTW/C9/DestroyDelayed"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_DESTROY_ALL
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - plugins hInstance
**		lP - NULL
**
*/

#define WTW_CONFERENCE_WINDOW_DESTROY_ALL	L"WTW/C9/DestroyAll"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_SET_TITLE
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what create returned in lP
**		lP - const wchar_t * - null terminated string
**
*/

#define WTW_CONFERENCE_WINDOW_SET_TITLE		L"WTW/C9/SetTitle"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_SET_ICON
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what create returned in lP
**		lP - const wchar_t* - iconId
**
*/

#define WTW_CONFERENCE_WINDOW_SET_ICON		L"WTW/C9/SetIcon"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_FIND_EX
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - const wchar_t* - conference window Id
**		lP - WTW_PTR* - returns here window pointer or null
**
*/

#define WTW_CONFERENCE_WINDOW_FIND_EX		L"WTW/C9/Find"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_SHOW_WINDOW
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - const wchar_t* - conference window Id
**		lP - CWSW_SHOW_*
**
*/

#define WTW_CONFERENCE_WINDOW_SHOW_WINDOW		L"WTW/C9/ShowWindow"	// wparam - to co zwrocilo create, lparam - 1/0, pokazac/ukryc

#define CWSW_HIDE				0x000000
#define CWSW_SHOW_NORMAL		0x000001
#define CWSW_SHOW_BACKGROUND	0x000002
#define CWSW_SHOW_MINIMZED		0x000003

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_HWNDFROMPTR_EX
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what create returned in lP
**		lP - HWND* - out
**
*/

#define WTW_CONFERENCE_WINDOW_HWNDFROMPTR_EX	L"WTW/C9/HWNDFromPtr"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_CONTACT_ADD
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what create returned in lP
**		lP - wtwConferenceContact* - function behaves like "modify" if it 
**				finds contact with specified id
**
*/

#define WTW_CONFERENCE_WINDOW_CONTACT_ADD		L"WTW/C9/Contact/Add"

struct wtwConferenceContact 
{
	__wtwStdStructDefs(wtwConferenceContact);

	int				structSize;
	
	wtwContactDef	contactInfo;

	const wchar_t * name;
	const wchar_t * iconId;
	const wchar_t * contactHint;

	const wchar_t * tabId;

	int				sortClass;
	void	*		userData;

	DWORD			flags;
};

#define WTW_CONFERENCE_CONTACT_FLAG_UPDATE_NAME 0x0001
#define WTW_CONFERENCE_CONTACT_FLAG_UPDATE_DATA 0x0002
#define WTW_CONFERENCE_CONTACT_FLAG_UPDATE_ICON 0x0004
#define WTW_CONFERENCE_CONTACT_FLAG_UPDATE_SCLS 0x0008
#define WTW_CONFERENCE_CONTACT_FLAG_UPDATE_ALL  0x000F
#define WTW_CONFERENCE_CONTACT_FLAG_AUTO_NAME	0x0010	// get name from contact list if contact is present there, if not use id as name

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_TAB_GET_CURRENT
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - wtwConferenceTab*
**
*/

#define WTW_CONFERENCE_TAB_GET_CURRENT		L"WTW/C9/Tab/GetCurSel"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_TAB_ADD
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - wtwConferenceTab*
**
*/

#define WTW_CONFERENCE_TAB_ADD				L"WTW/C9/Tab/Add"

struct wtwConferenceTab 
{
	__wtwStdStructDefs(wtwConferenceTab);

	int				structSize;

	const wchar_t *	tabId;
	const wchar_t *	tabName;
	const wchar_t *	tabIcon;

	int				tabClass; // CFTC_*

	DWORD			flags;

	HWND			hCustomWindow;
};

#define CFTF_VALID_NAME		0x00000001	// used for "modify"
#define CFTF_VALID_ICON		0x00000002
#define CFTF_VALID_CLASS	0x00000004	// not implemented
#define CFTF_VALID_FLAGS	0x00000008
#define CFTF_VALID_HWND		0x00000010	// not implemented

#define CFTF_MASK_FLAGS		0xFFFF0000

#define CFTF_MARK			0x00010000
#define CFTF_AUTO_MARK		0x00020000
#define CFTF_CLOSEABLE		0x00040000	// user can close tab
#define CFTF_DELAY_DELETE	0x00080000	// MUST be set if TAB_DELETE is called from window callback
#define CFTF_LOG_CMD_FIELD	0x00100000

#define CFTC_LOG			0x0001
#define CFTC_MULTI_USER		0x0002
#define CFTC_SINGLE_USER	0x0003
#define CFTC_USER			0x0004	// not implemented, before destroying window remove all custom tabs from it

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_TAB_DELETE
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - wtwConferenceTab*
**
*/

#define WTW_CONFERENCE_TAB_DELETE			L"WTW/C9/Tab/Delete"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_TAB_MODIFY
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - wtwConferenceTab*
**
*/

#define WTW_CONFERENCE_TAB_MODIFY			L"WTW/C9/Tab/Modify"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_TAB_EXISTS
**
** Return value: 
**		S_OK - present, S_FALSE - absent, E_* - error
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - wtwConferenceTab*
**
*/

#define WTW_CONFERENCE_TAB_EXISTS			L"WTW/C9/Tab/Exists"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_TAB_SWITCH_TO
**
** Return value: 
**		S_*, E_*
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - wtwConferenceTab*
**
*/

#define WTW_CONFERENCE_TAB_SWITCH_TO		L"WTW/C9/Tab/SwitchTo"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_CONTACT_DELETE
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - wtwConferenceContact*
**
*/

#define WTW_CONFERENCE_WINDOW_CONTACT_DELETE		L"WTW/C9/Contact/Delete"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_GET_CONTACTS
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - wtwConferenceContacts *
**
*/

#define WTW_CONFERENCE_WINDOW_GET_CONTACTS	L"WTW/C9/GetUsers"	

struct wtwConferenceContacts 
{
	__wtwStdStructDefs(wtwConferenceContacts);

	int						structSize;
	const wchar_t *			tabId;
	
	wtwConferenceContact *	pContacts;		// tyle struktur aby pomeiscic wszystkie kontaty
											// jesli bedzie za malo to po prostu nie skopiuje wszystkich
											// aby dostac ilosc potrzebych structow wywolac
											// _GET_USERS z contactCount = -1, fukcja zrwoci potrzebny rozmiar

	int						contactCount; 
};

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_ADD_MESSAGE
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - wtwConferenceMessage*
**
*/

#define WTW_CONFERENCE_WINDOW_ADD_MESSAGE	L"WTW/C9/AddMessage"  

struct wtwConferenceMessage
{
	__wtwStdStructDefs(wtwConferenceMessage);

	int				structSize;
	wtwMessageDef   message;

	const wchar_t * nick;
	const wchar_t *	tabId;

	DWORD			flags;

	const wchar_t *	whatifyHint;	// if notification will be show, this is secondary text for it
};

#define CONFERENCE_MESSAGE_FLAG_FLASH			0x0001 // flash window when message is added
#define CONFERENCE_MESSAGE_FLAG_SHOW_WINDOW		0x0004 // force window show
#define CONFERENCE_MESSAGE_FLAG_NO_WHATIFY		0x0008 // don't include message in unread count if window is not visible

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_IS_CT_PRESENT
**
** Return value: 
**		E_*, S_OK (on list), S_FALSE (not on list)
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - wtwConferenceContact * (with filled id field)
**
*/

#define WTW_CONFERENCE_WINDOW_IS_CT_PRESENT	L"WTW/C9/IsUserPresent"	

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_GET_CONTACT
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what "create ex" returned in lP
**		lP - wtwConferenceContact * (with filled id field)
**
*/

#define WTW_CONFERENCE_WINDOW_GET_CONTACT	L"WTW/C9/GetContact"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_CMD_ADD
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwConferenceCmdEntry*
**		lP - NULL
**
*/

#define WTW_CONFERENCE_CMD_ADD		L"WTW/C9/CmdAdd"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_CMD_MOD
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwConferenceCmdEntry*
**		lP - NULL
**
*/

#define WTW_CONFERENCE_CMD_MOD		L"WTW/C9/CmdMod"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_CMD_DEL
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwConferenceCmdEntry*
**		lP - NULL
**
*/

#define WTW_CONFERENCE_CMD_DEL		L"WTW/C9/CmdDel"

struct wtwConferenceCmdEntry 
{
	__wtwStdStructDefs(wtwConferenceCmdEntry);

	int				structSize;

	WTW_PTR			pWnd;		

	const wchar_t*	itemId;
	
	const wchar_t*	caption;
	const wchar_t*  toolTip;
	const wchar_t*	graphId;

	const wchar_t*	tabId;

	DWORD			itemType;
	DWORD			itemFlags;
	void		*	itemData;
};

struct wtwConferenceCmdInfo
{
	__wtwStdStructDefs(wtwConferenceCmdInfo);

	int				structSize;

	const wchar_t * itemId;
	void		*	itemData;
};

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_SET_USER_PTR
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what create returned in lP
**		lP - WTW_PARAM - ptr to set
**
*/

#define WTW_CONFERENCE_WINDOW_SET_USER_PTR		L"WTW/C9/SetUserPtr"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_WINDOW_GET_USER_PTR
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what create returned in lP
**		lP - WTW_PARAM* - ptr to get
**
*/

#define WTW_CONFERENCE_WINDOW_GET_USER_PTR		L"WTW/C9/GetUserPtr"

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_TAB_SET_USER_PTR
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what create returned in lP
**		lP - wtwConferenceTabPtr*
**
*/

#define WTW_CONFERENCE_TAB_SET_USER_PTR		L"WTW/C9/SetTabUserPtr"

struct wtwConferenceTabPtr
{
	__wtwStdStructDefs(wtwConferenceTabPtr);

	int				structSize;

	const wchar_t *	tabId;
	WTW_PTR			ptr;

};

/*******************************************************************************
**
** Function: 
**		WTW_CONFERENCE_TAB_GET_USER_PTR
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - WTW_PTR - what create returned in lP
**		lP - wtwConferenceTabPtr*
**
*/

#define WTW_CONFERENCE_TAB_GET_USER_PTR		L"WTW/C9/GetTabUserPtr"



struct wtwConferenceCommand
{
	__wtwStdStructDefs(wtwConferenceCommand);

	int				structSize;

	const wchar_t * str;

	DWORD			flags;
};


#endif