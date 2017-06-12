/*
** Copyright (C) 2007-2014, K2T.eu
*/

#ifndef __wtw__ctl_h__
#define __wtw__ctl_h__

#include "wtwPlugin.h"
#include "wtwContact.h"
#include "wtwGraph.h"
#include "wtwProtocol.h"

/*******************************************************************************
**
** Function: 
**		WTW_CTL_CONTACT_ADD
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwContactListItem*
**		lP - wP array size, or 0 -> 0 = one element
**
*/

#define WTW_CTL_CONTACT_ADD		L"WTW/CTL/AddContact"

/*******************************************************************************
**
** Function: 
**		WTW_CTL_CONTACT_FIND_EX
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwContactDef*		- in
**		lP - WTW_PTR itemHandle*- out
**
*/

#define WTW_CTL_CONTACT_FIND_EX	L"WTW/CTL/FndContactEx"

/*******************************************************************************
**
** Function: 
**		WTW_CTL_CONTACT_FINDALL
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwContactFindAll*
**		lP - NULL
**
*/

#define WTW_CTL_CONTACT_FINDALL	L"WTW/CTL/FndAll"

/*******************************************************************************
**
** Function: 
**		WTW_CTL_CONTACT_DELETE
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - itemHandle
**		lP - NULL
**
*/

#define WTW_CTL_CONTACT_DELETE	L"WTW/CTL/DelContact"

/*******************************************************************************
**
** Function: 
**		WTW_CTL_CONTACT_MODIFY
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - itemHandle
**		lP - wtwContactListItem*
**
*/

#define WTW_CTL_CONTACT_MODIFY	L"WTW/CTL/ModContact"

/*******************************************************************************
**
** Function: 
**		WTW_CTL_CONTACT_ENUM
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwContactListEnum*
**		lP - NULL
**
*/

#define WTW_CTL_CONTACT_ENUM	L"WTW/CTL/EnContacts"

/*******************************************************************************
**
** Function: 
**		WTW_CTL_CONTACT_GET
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - itemHandle
**		lP - wtwContactListItem*
**
*/

#define WTW_CTL_CONTACT_GET		L"WTW/CTL/GetContact"

/*******************************************************************************
**
** Function: 
**		WTW_CTL_GET_PRESENCE
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - wtwPresenceDef*, if lP != then You dont have to fill wP's contactData
**		lP - itemHandle, can be NULL
**
*/

#define WTW_CTL_GET_PRESENCE L"WTW/CTL/GetPresence"	

#define WTW_EVENT_ON_CONTACT_ADD L"WTW/OnContactAdd" // wP - wtwContactListItem*, lP - itemHandle
#define WTW_EVENT_ON_CONTACT_DEL L"WTW/OnContactDel" // wP - NULL, lP - itemHandle
#define WTW_EVENT_ON_CONTACT_MOD L"WTW/OnContactMod" // wP - NULL, lP - itemHandle (after modify)


/*******************************************************************************
**
** Function: 
**		WTW_CTL_CONTACT_ICON_ADD
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - itemHandle
**		lP - wtwIconSetDef*
**
*/

#define WTW_CTL_CONTACT_ICON_ADD L"WTW/CTL/AddContactIcon"

/*******************************************************************************
**
** Function: 
**		WTW_CTL_CONTACT_ICON_DELETE
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - itemHandle
**		lP - wtwIconSetDef*
**
*/

#define WTW_CTL_CONTACT_ICON_DELETE L"WTW/CTL/DelContactIcon"

/*******************************************************************************
**
** Function: 
**		WTW_CTL_CONTACT_ICON_MODIFY
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - itemHandle
**		lP - wtwIconSetDef*
**
*/

#define WTW_CTL_CONTACT_ICON_MODIFY L"WTW/CTL/M0dContactIcon"


/*******************************************************************************
**
** Function: 
**		WTW_CTL_ENABLE_DRAWING
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - (int) 1 - enable, 0 - disable
**		lP - NULL
**
*/

#define WTW_CTL_ENABLE_DRAWING	L"WTW/CTL/EnableDrawing"



#define WTW_CTL_SET_CONTACT_STATE	L"WTW/CTL/SetContactState" // zamiast wolac caly czas Mod do ikonki 
															 // mozemy uzyc tego do uaktualnienia stanu
															 // ale wczesniej trzeba wywolac reg_mappng
															 // aby  WTW wiedzial ktory index ikonki 
															 // jest ktory, wparam - presencedef* z ct info,
															 // lparam mzoe byc null, to wtw sam poszuka kontaktu,
															 // w innym wypadku podajemy tam handle do kontaktu

#define WTW_CTL_SET_OVERLAY_STATE_MAP L"WTW/CTL/SetOverlayState" // wP - OVL_, lP = contactHandle

#define WTW_CTL_REGISTER_MAPPING	L"WTW/CTL/RegisterMapping"	// do tlumaczenia indexów ikonek na
																// stany i odwrotnie
																// mapka jest automagicznie
																// rejestrowana dla protkolu
																// jesli ladowalismy caly set,
																// ale jak chcemy mozmy zrobic wlasna

#define WTW_CTL_SET_ALL_OFFLINE		L"WTW/CTL/SetAllOffline"	// przydatne przy roz³¹czeniu ;)
																// wparam = netClass, lparam netId;
#define WTW_CTL_DELETE_ALL			L"WTW/CTL/DeleteAll"		// przydatne przy wylaczeniu pluga
																// wparam = netClass, lparam netId;

struct wtwContactListItem 
{
	__wtwStdStructDefs(wtwContactListItem);

	int				structSize;

	wtwContactDef	contactData;

	const wchar_t *	itemText;
	const wchar_t *	itemGroup;

	DWORD			itemType;
	DWORD			itemState;
	void		  * itemHandle;			// przy dodawaniu dostajemy to spowrotem;

	int				flags;

	void *			ownerData;			// nieuzywane

	WTWFUNCTION		drawCallback;		// nieuzywane
	void *			cbData;				// nieuzywane

	wtwIconSet	  * iconToAdd;
};

struct wtwContactFindAll
{
	__wtwStdStructDefs(wtwContactFindAll);

	int			structSize;

	const wchar_t * contactId;
	const wchar_t * netClass;

	DWORD			flags;

	wtwContactListItem *pResults;
	int					maxResults;
	int					results;
};

#define CTL_ITEM_EVENT_GET_HEIGHT	0x0001	// masz tylko obliczyc cy (y, x, cx sa podane)
#define CTL_ITEM_EVENT_DRAW_ITEM	0x0002	// narysowac, mieszczac sie w x,y,cx,cy
#define CTL_ITEM_EVENT_LCLICK		0x0003	// x & y - cursosr position in client coodrs, cx, cxy = -1, hDC unavailable
#define CTL_ITEM_EVENT_RCLICK		0x0004	// x & y - cursosr position in client coodrs, cx, cxy = -1, hDC unavailable
#define CTL_ITEM_EVENT_DBLCLICK		0x0005	// x & y - cursosr position in client coodrs, cx, cxy = -1, hDC unavailable

struct wtwContactListItemEvent
{
	__wtwStdStructDefs(wtwContactListItemEvent);

	int				structSize;
	int				event;
	HDC				hDC;		// zwrocic w stanie oryginalnym!
	DWORD			flags;
	int				y;
	int				x;
	int				cx;
	int				cy;			// przy get height obliczyc to, 
								// przy draw dostajemy to co obliczylismy

	wtwContactDef*	pContact;
	void *			ownerData;

	WTWFUNCTION		fnDrawStandardElement;
	void *			cbDataDrawStandardElement; // przekazac jako trzeci parametr do fnDrawStandardElement
	void *			pRenderer;	// nic co byloby Wam potrzebne ;P
};

struct wtwContactListStdDraw
{
	__wtwStdStructDefs(wtwContactListStdDraw);

	int				structSize;

	wtwContactListItemEvent *pEvent;

	int				whatToDraw;
};

struct wtwContactListStdElementText
{
	__wtwStdStructDefs(wtwContactListStdElementText);

	int				structSize;

	const wchar_t * text;

	DWORD			x;
	DWORD			y;

	DWORD			cx;
	DWORD			cy;

	DWORD			flags;
};

#define	CSE_TEXT_FLAG_CALC_ONLY 0x0001
#define	CSE_TEXT_FLAG_MULTILINE 0x0002
#define	CSE_TEXT_FLAG_VCENTER	0x0004

#define CTL_STD_ELEMENT_BACKGROUND	0x00000001	// wP - wtwContactListStdDraw*
#define CTL_STD_ELEMENT_MAIN_TEXT	0x00000002	// wP - wtwContactListStdDraw*, lP - wtwContactListStdElementText*
#define CTL_STD_ELEMENT_DESC_TEXT	0x00000003	// wP - wtwContactListStdDraw*, lP - wtwContactListStdElementText*

#define WTW_CTL_SET_CUSTOM_SORT_FUNCTION L"WTW/SetCustomCTLSort"// wParam - adres funcji typu 
																// _wtwFnCustomSortOperator, albo NULL jesli 
																// chcemy funkcjê usun¹æ (przy wy³adowaniu wtyczki)

struct wtwContactListSortParam 
{
	__wtwStdStructDefs(wtwContactListSortParam);

	int					structSize;
	wtwContactListItem	item;
	wtwPresenceDef		presence;
	int					sortPos;
};

typedef int (__stdcall *_wtwFnCustomSortOperator)(
	const wtwContactListSortParam *i1,
	const wtwContactListSortParam *i2);	// efektywnie jest to operator "<", czyli jesi i1 jest mniejsze
									// od i2 to zwracamy "true", w innym wypadku "false"

#define WTW_CTL_FLAG_REPLACE_DUPLICATE	0x00000001 // przy tej fladze zastapi na liscie dane kontaktu o identycznym id, nid i nsid (poza stanem i opisem)
#define WTW_CTL_FLAG_DO_NOT_EXPAND		0x00000002
#define WTW_CTL_FLAG_UPDATE_STATE		0x00000004
#define WTW_CTL_FLAG_CLEAR_EXISTING		0x00000008
#define WTW_CTL_FLAG_SELECTED			0x00000010	// pozycja jest zaznaczona
#define WTW_CTL_FLAG_PINNED				0x00010000	// 
#define WTW_CTL_FLAG_ALWAYS_ONLINE		0x00020000	// 
#define WTW_CTL_FLAG_CHATROOM			0x00040000	// 

#define WTW_CTL_FLAG_PRESIDENT_MASK		0xFFFF0000	// 

#define	WTW_CTL_TYPE_GROUP		0x00000001	// grupa
#define	WTW_CTL_TYPE_METAC		0x00000002	// metakontant
#define	WTW_CTL_TYPE_ELEMENT	0x00000003	// kontakt
#define	WTW_CTL_TYPE_USER		0x00000004	// niezaimplementowane
#define	WTW_CTL_TYPE_SEPARATOR	0x00000005	// j/w

#define	WTW_CTL_STATE_MASK		0x0000FFFF
#define	WTW_CTL_STATE_SPECIAL	0x00000100
#define	WTW_CTL_STATE_ONLINE	0x00000200
#define	WTW_CTL_STATE_OFFLINE	0x00000300
#define	WTW_CTL_STATE_OFFLIST	0x00000400
#define	WTW_CTL_STATE_WAITAUTH	0x00000500
#define	WTW_CTL_STATE_NOAUTH	0x00000600
#define	WTW_CTL_STATE_UNKNOWN	0x00000700

///////////////////////////////////////////////////////////////////////////////

struct wtwAvatarSet 
{
	__wtwStdStructDefs(wtwAvatarSet);

	int				structSize;
	
	const wchar_t * filePath;		// sciezka do pliku, pelna
	int				avatarType;		// typ grafiki
	
	void		  * contactHandle;	// handle kontaktu, jesli null to ustawiamy net id, SID i ctID w contact info, 
	wtwContactDef * pContactData;	// jesli cti null, to handle musi byc podany

	DWORD			flags;			// opcje
	DWORD			magic;			// coœ magicznego, jesli 0 to cokolwiek bedziemoglo 
									// ustawic awatar, jesli rozne od zera, totylko jesli 
									// ta wartosc sie zgdza to awatar bedzie mogl byc
									// ustawiony;
	DWORD			source;			// skad sie wzial;
};

#define WTW_AVATAR_TYPE_PNG	0x0001
#define WTW_AVATAR_TYPE_JPG	0x0002
#define WTW_AVATAR_TYPE_GIF	0x0003
#define WTW_AVATAR_TYPE_BMP	0x0004
#define WTW_AVATAR_TYPE_ICO	0x0005

#define	WTW_AVATAR_SET			L"WTW/CTL/SetAvatar"
#define	WTW_AVATAR_GET			L"WTW/CTL/GetAvatar"
#define	WTW_AVATAR_GET_SOURCE	L"WTW/CTL/GetAvatarSource" // wParam - wtwContactDef*

#define	WTW_ON_AVATAR_SET		L"WTW/Event/SetAvatar" // wP - wtwAvatarSet*

#define AV_SOURCE_DEFAULT	0	// lub "none"
#define AV_SOURCE_EXTERNAL	1
#define AV_SOURCE_LOCAL		2

#define WTW_CTL_ENUM_GROUPS_HWND L"WTW/CTL/EnumGroupsHWND" // wParam - HWND ComboBox'a

struct wtwContactListEnumerate 
{
	__wtwStdStructDefs(wtwContactListEnumerate);

	int				structSize;

	WTWFUNCTION		enumFunct;		// dostanie contactListItem jako *wP
	void *			cbData;

	DWORD			flags;			// WTW_CTL_ENUMERATE_*
};

#define WTW_CTL_ENUMERATE_ALL			0x0000
#define WTW_CTL_ENUMERATE_ONLINE_ONLY	0x0001

#endif