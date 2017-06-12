/*
** Copyright (C) 2007-2014, K2T.eu
*/

#ifndef __wtw__protocol_h__
#define __wtw__protocol_h__

#pragma once

#include "wtwPlugin.h"
#include "wtwCaps.h"
#include "wtwContact.h"

#define WTW_MESSAGE_EXTRA_INFO_SUBJECT	L"subject"	// meesage subject
#define WTW_MESSAGE_EXTRA_INFO_UMID		L"umid"		// message unique id
#define WTW_MESSAGE_EXTRA_INFO_UCID		L"ucid"		// conversation unique id

struct wtwMessageExtInfo 
{
	__wtwStdStructDefs(wtwMessageExtInfo);

	int				structSize;

	const wchar_t * name;	// parameter name
	const wchar_t *	value;	// and it's value
};

struct wtwMessageExtInfoEx
{
	__wtwStdStructDefs(wtwMessageExtInfoEx);

	int				structSize;

	const wchar_t * name;	// parameter name
	const void *	value;	// 

	DWORD			flags;	//
};

struct wtwMessageDef 
{
	__wtwStdStructDefs(wtwMessageDef);

	int				structSize;

	wtwContactDef	contactData;	// from/to whom

	const void *	protocolData;
	const wchar_t * msgMessage;		// message body, it SHOULD NOT be null but it MAY be 

	DWORD			msgFlags;		
	QWORD			msgTime;		// when message was sent

	const wtwMessageExtInfo *extInfo;	// extra infos
	int				extInfoCount;		// how many infos we have

	wtwResourceInfo * exResource;		// mostly for XMPP message

	void *			ownerData;
};

inline const wchar_t * __valFromExtInfo(const wtwMessageDef *msg, const wchar_t *name, const wchar_t *def = NULL)
{
	const wchar_t *ret = def;

	if (msg->extInfo == NULL)
	{
		return ret;
	}

	const int shift = msg->extInfo->structSize;
	const char *p = reinterpret_cast<const char*>(msg->extInfo);

	for (int i = 0; i < msg->extInfoCount; ++i)
	{
		const wchar_t *n;
		const wchar_t *v;

		if (shift == sizeof(wtwMessageExtInfo))
		{
			const wtwMessageExtInfo *pe = reinterpret_cast<const wtwMessageExtInfo *>(p + (i * shift));
			n = pe->name;
			v = pe->value;
		}
		else
		{
			const wtwMessageExtInfoEx *pe = reinterpret_cast<const wtwMessageExtInfoEx *>(p + (i * shift));
			n = pe->name;
			v = static_cast<const wchar_t*>(pe->value);
		}

		if (n && !wcscmp(name, n))
		{
			ret = msg->extInfo[i].value;
			break;
		}
	}

	return ret;
}

struct wtwAvatarDef
{
	__wtwStdStructDefs(wtwAvatarDef);

	int				structSize;

	wtwContactDef	contactData;

	const wchar_t * avPath;				// file to the avatar
	DWORD			avType;				// and it's type (0 for autodetect)
};

struct wtwAvatarEvent 
{
	__wtwStdStructDefs(wtwAvatarEvent);

	int				structSize;

	const wchar_t * avFile;		// may be null

	const wchar_t *	sha1;		// file sha1 checksum, may be null
	const wchar_t *	md5;		// and md5 sum, may be null

	DWORD			event;		// what happened with avatar, WTW_AVATAR_EVENT_*
};

#define WTW_AVATAR_EVENT_DELETE		0x0001
#define WTW_AVATAR_EVENT_SETUP		0x0002

#define WTW_EVENT_AVATAR_CHANGED	L"WTW/OnAvatarChanged"	// wP - wtwAvatarEvent *

#define WTW_MESSAGE_FLAG_INCOMING		0x00000001
#define WTW_MESSAGE_FLAG_OUTGOING		0x00000002
#define WTW_MESSAGE_FLAG_CHAT_MSG		0x00000004
#define WTW_MESSAGE_FLAG_ENCRYPTED		0x00000008 	// marker for "do not store encrypted message to archive" option
#define WTW_MESSAGE_FLAG_CONFERENCE		0x00000010	// conference messages are not routed automagically into conference window
#define WTW_MESSAGE_FLAG_PICTURE		0x00000040	// msgMessage MUST BE base64 encoded picture data
#define WTW_MESSAGE_FLAG_NOHTMLESC		0x00000080	// do not replace <> with &lt; & &gt;
#define WTW_MESSAGE_FLAG_NOARCHIVE		0x00001000	// do not store in archive
#define WTW_MESSAGE_FLAG_TRANSFER_LINK	0x00002000	
#define WTW_MESSAGE_FLAG_FORCEDISP		0x00004000	// force message to be displayed immiedietly
#define WTW_MESSAGE_FLAG_NOAUTOLINK		0x00008000	// do not generate links for URL's, mail adresses - etc
#define WTW_MESSAGE_FLAG_NOTHEME		0x00010000	// we want to insert raw html/text inot chat window
#define WTW_MESSAGE_FLAG_NOWRITETRIG	0x00020000	// Do not trigger archive write operation, jus add message to queue
#define WTW_MESSAGE_FLAG_NOAUTORECV		0x00040000	// do not route message to chatwindow after hook calls
#define WTW_MESSAGE_FLAG_CUSTOM_VARS	0x00080000	// custom variables are present
#define WTW_MESSAGE_FLAG_OFFLINE		0x00100000	// delivered from offline storage
#define WTW_MESSAGE_FLAG_CEL			0x00200000	// encrypted message, do not route via message hooks
#define WTW_MESSAGE_FLAG_FORCENODISP	0x00400000	// don't disaply message, just pass it to archive

#define WTW_MESSAGE_FLAG_UTF8_MARK		0x10000000	// do not use, internal
#define WTW_MESSAGE_FLAG_DEFLATED		0x20000000	// do not use, internal
#define WTW_MESSAGE_FLAG_MARKER			0x40000000	// do not use, internal

#define WTW_MESSAGE_FLAG_WARNING		0x00000100	// warning class, warnings, errors and infos do not cause 
#define WTW_MESSAGE_FLAG_ERROR			0x00000200	// chat window to be opened, they are displayed only
#define WTW_MESSAGE_FLAG_INFO			0x00000400	// in already created windows (visible or not)
#define WTW_MESSAGE_FLAG_INFO_LINK		0x00000800	// this is link information

#define WTW_MESSAGE_FLAG_MASK_STATE		0x00000F00	// 

#define WTW_TRIGGER_ARCHIVE_WRITE	L"WTW/TriggerArchiveWrite"

struct wtwPresenceDef 
{
	__wtwStdStructDefs(wtwPresenceDef);

	int				structSize;

	wtwContactDef *	pContactData;	// this may be null

	int				curStatus;		// current user/protocol/etc status (for _BEFORE events)
	const wchar_t * curDescription;	// this may be null
	QWORD			curTimeStamp;	// precence time stamp (when it was set, if available)

	const void	  * exInfo;			// extra info, generated by the protocol plugin
									// valid only in _BEFORE events

	wtwResourceInfo * exResource;	// resource informations, for XMPP mostly

	DWORD			flags;
};

/* 
	These are for account status changes
*/

#define WTW_PRESENCE_FLAG_MENU			0x00000001L	// status was set via menu click
#define WTW_PRESENCE_FLAG_PREDEF		0x00000002L	// this is predefined status
#define WTW_PRESENCE_FLAG_USER			0x00000004L	// i have no idea what this means
#define WTW_PRESENCE_FLAG_PLUGIN		0x00000008L	// change made by external plugin
#define WTW_PRESENCE_FLAG_EXIT			0x00000010L	// core system is being shutdown, we are exiting
#define WTW_PRESENCE_FLAG_AUTO			0x00000020L	// automatic connection at startup
#define WTW_PRESENCE_FLAG_AA_ENTER		0x10000000L	// entering AutoAway mode
#define WTW_PRESENCE_FLAG_AA_EXIT		0x20000000L	// exiting z AA
#define WTW_PRESENCE_FLAG_LOCK_ENTER	0x40000000L	// computer locked
#define WTW_PRESENCE_FLAG_LOCK_EXIT		0x80000000L	// computer unlocked
#define WTW_PRESENCE_FLAG_SUSPEND		0x01000000L	// system is entering suspend mode
#define WTW_PRESENCE_FLAG_RESET			0x02000000L	// connection was reset
#define WTW_PRESENCE_FLAG_RESUME		0x04000000L	// resume after suspend

/*
	These are for contact presence
*/

#define WTW_PRESENCE_FLAG_NO_NOTIFY		0x00000040L	// do not generate whatify if applicable
#define WTW_PRESENCE_FLAG_NO_ARSM		0x00000080L	// No Automated Resource Status Management, generally - do not use
#define WTW_PRESENCE_FLAG_EX_ICONID		0x00000100L	// I have no idea what it does	
#define WTW_PRESENCE_FLAG_UNRELAYABLE	0x00000200L //

/*
	Contacts and protocol states, support for individual states is protocol dependent
*/

#define WTW_PRESENCE_UNDEF		0x8000L	// not defined, error, etc
#define WTW_PRESENCE_ONLINE		0x0001L	// online
#define WTW_PRESENCE_CHAT		0x0002L	// free for chat
#define WTW_PRESENCE_DND		0x0003L	// do not disturb
#define WTW_PRESENCE_AWAY		0x0004L	// away
#define WTW_PRESENCE_XA			0x0005L	// extended away
#define WTW_PRESENCE_INV		0x0006L	// invisible
#define WTW_PRESENCE_OFFLINE	0x0007L	// disconnected
#define WTW_PRESENCE_ERROR		0x0008L	// error
#define WTW_PRESENCE_BLOCK		0x0009L	// contact has blocked us
#define WTW_PRESENCE_NO_CHANGE	0xFFFFFFFFL	// do not change 

/*
	Icon overlays, for contact list only
*/

#define WTW_PRESENCE_OVL_NONE	0x0000L	// No overlay
#define WTW_PRESENCE_OVL_DESC	0x0001L	// Contact has status note
#define WTW_PRESENCE_OVL_NOL	0x0002L	// We are not present on contact's friend list

/*
	Raw data, protocol dependent
*/

struct wtwRawDataDef 
{
	__wtwStdStructDefs(wtwRawDataDef);

	int				structSize;

	const char *	pData;		// pointer to data
	int				pDataLen;	// data length
	DWORD			flags;		
};

#define WTW_RAW_FLAG_TEXT	0x0001 // pData is null terminated string, ansi encoded
#define WTW_RAW_FLAG_BIN	0x0002 // pData contains binary data
#define WTW_RAW_FLAG_UTF	0x0004 // combined with TEXT, string is utf-8 encoded
#define WTW_RAW_FLAG_UNI	0x0008 // Unicode text, pData is const wchar_t*
#define WTW_RAW_FLAG_OOB	0x0010 // OOB, Out Of band Data
#define WTW_RAW_FLAG_EXT	0x0020 // Data from plugin
#define WTW_RAW_FLAG_PRIV	0x0040 // Private segment
#define WTW_RAW_FLAG_USER	0x0080 // Data injected from protocol console

/*
	For fast access, these functions are not registerd in the system
*/

struct wtwProtocolInterface 
{
	__wtwStdStructDefs(wtwProtocolInterface);

	int			structSize;
	WTWFUNCTION pfAuthContact;	// wP - wtwContactDef*, lP - WTW_AUTH_OPT...
	WTWFUNCTION pfDeleteContact;// wP - wtwContactDef*
	WTWFUNCTION pfMoveContact;	// wP - wtwContactDef*, lP - wtwMoveContactDef*
	void *		cbData;
};

/*
	User wants to move contact from one group to another
*/

struct wtwMoveContactDef
{
	__wtwStdStructDefs(wtwMoveContactDef);

	int				structSize;
	const wchar_t * currectGroupName;
	const wchar_t * newGroupName;
};

/*
	Auth options for auth contact
*/

#define WTW_AUTH_OPTION_ADD_TO_LIST 0x00000001	// add contact to list
#define WTW_AUTH_OPTION_AUTH_GRANT	0x00000002	// grant auth, so it can see out presence
#define WTW_AUTH_OPTION_NO_AUTH		0x00000004	// reject auth
#define WTW_AUTH_OPTION_CANCEL		0x00000008	// do nothing, ignore

/*******************************************************************************
**
** Function: 
**		WTW_PF_UPDATE_PROTO_INFO
**
** Return value: 
**		S_OK, E_*
**
** Parameters:
**		wP - protocolLoginInfo*
**		lP - NULL
**
** Remarks:
**		Sets information (login/account name displayed in tooltips) about
**		our registered protocol
**
*/

#define WTW_PF_UPDATE_PROTO_INFO	L"WTW/PFSetInfo"

struct protocolLoginInfo 
{
	__wtwStdStructDefs(protocolLoginInfo);

	int				structSize;
	const wchar_t * loginID;
	const wchar_t * loginName;

	const wchar_t * netClass;
	int				netId;
};

WTW_PTR inline wtwSetProtoInfo(WTWFUNCTIONS *pF, 
							   const wchar_t * netClass, int netId, 
							   const wchar_t * lID, const wchar_t * lName)
{
	protocolLoginInfo pt;
	initStruct(pt);

	pt.loginID		= lID;
	pt.loginName	= lName;
	pt.netClass		= netClass;
	pt.netId		= netId;

	return pF->fnCall(WTW_PF_UPDATE_PROTO_INFO, (WTW_PARAM)&pt, 0);
}

struct wtwProtocolDef 
{
	__wtwStdStructDefs(wtwProtocolDef);

	int				structSize;

	const wchar_t * netClass;		// protocol class (GG, XMPP, TLEN, preferably frour capital letters
	const wchar_t * protoName;		// protocol name ("Gadu-Gadu", "Tlen.pl", etc)
	const wchar_t * protoDescr;		// shor description of the protocol

	int				netId;			// Network/account id, ths parameter is RETURNED by core system
									// this allows multiple accounts of one net class to exist
									// acconts/contacts are identified by pair netClass/netId;

	const wchar_t * netGUID;		// network GUID, should be generated when protocol is added first time,
									// and stored in protocol/account settings, this is used to identify
									// account if it's id has been changed (account was deleted, etc)
									// additional contact properties (phones, adresses, mail, and metacontact infos)
									// are recognizeg by guid

	DWORD			protoCaps1;		// protocol capabilities;
	DWORD			protoCaps2;		// protocol capabilities part 2;
	DWORD			flags;

	wtwProtocolInterface pInterface;// for fast access deleting, auth and moving to another group is here

	DWORD		  * protoState;		// pointer to DWORD that holds currect protocol state, this pointer
									// must be valid as long as protocol is registered
									// most of the states can be combined, flags are named WTW_PROTO_STATE_*
									// all access to this variable MUST BE done from main/gui thread,
									// cuz there is no locking present

	const wchar_t * protoIconId;	// if it's NULL then it means "<netClass>/Icon"

};

#define WTW_PROTO_STATE_CONNECTED	0x00000001	// we are connected to the service, not nessesairly authed
#define WTW_PROTO_STATE_ENCRYPTED	0x00000002	// connection is encrypted, safe, etc
#define WTW_PROTO_STATE_CONNECTING	0x00000004	// we are trying to connect, this may be present until "auth_ok" is set
#define WTW_PROTO_STATE_AUTH_OK		0x00000008	// logged in
#define WTW_PROTO_STATE_COMPRESSED	0x00000010	// compression is supported (and enabled) over data stream
#define WTW_PROTO_STATE_ROSTER_OK	0x00000020	// roster was dloaded, and it's now valid

/*
	Some siple helper functions to check in what state protocol is
*/

int __inline isConConnected(const DWORD & s)
{
	return s & WTW_PROTO_STATE_CONNECTED;
}

int __inline isConRosterValid(const DWORD & s)
{
	return s & WTW_PROTO_STATE_ROSTER_OK;
}

int __inline isConConnecting(const DWORD & s)
{
	return s & WTW_PROTO_STATE_CONNECTING;
}

int __inline isConAuthed(const DWORD & s)
{
	return s & WTW_PROTO_STATE_AUTH_OK;
}

int __inline isConEncrypted(const DWORD & s)
{
	return s & WTW_PROTO_STATE_ENCRYPTED;
}

int __inline isConCompressed(const DWORD & s)
{
	return s & WTW_PROTO_STATE_COMPRESSED;
}

/*
	Simple function used for protocol state changes
*/

void __inline protoModifyFlag(DWORD & status, DWORD stateAdd, DWORD stateRemove)
{
	status |= stateAdd;
	status &= ~stateRemove;
}

#define WTW_PROTO_FLAG_NO_MENU			0x00000001	// nie dodawac menu stanów
#define WTW_PROTO_FLAG_PSEUDO			0x00000002	// pseudo protokol, wtw bedzie olewal jego stany, etc ...
#define WTW_PROTO_FLAG_NO_PUBDIR		0x00000004	// nie dodawac zakladku "katalog" do wlasciwosci kontaktu
#define WTW_PROTO_FLAG_FILE_AUTOS		0x00000008	// zezwalamy na automatyzaje odbierania plikow
#define WTW_PROTO_FLAG_NO_RAW_DATA		0x00000010	// 
#define WTW_PROTO_FLAG_GEN_GUID_IF_NULL	0x00000020	// generate GUID for us if we pass NULL as netGUID
													// returned pointer (netGUID) will be const, and you
													// MUST NOT attempt to release it. It's stays valid 
													// until function that called PROTOCOL_ADD returs, 
													// so copy it's contents and save it in config;


#define WTW_PROTO_FUNC_ADD		L"WTW/ProtocolAdd"	// wParam = wtwProtocolDef*
#define WTW_PROTO_FUNC_DEL		L"WTW/ProtocolDel"	// wP - netClass, lP - SID

#define WTW_PROTO_EVENT_ON_ADD	L"WTW/OnProtocolAdd" // wParam = wtwProtocolDef*
#define WTW_PROTO_EVENT_ON_DEL	L"WTW/OnProtocolDel" // wP - netClass, lP - SID

#define WTW_ON_PROTOCOL_EVENT	L"WTW/OnProtocolEvent" // to jest event kochani, hook it

struct wtwProtocolInfo 
{
	__wtwStdStructDefs(wtwProtocolInfo);

	int				structSize;

	const wchar_t *	netClass;
	int				netId;
	const wchar_t *	guid;
	const wchar_t *	name;
	const wchar_t *	info;
	const wchar_t *	protoIconId;

	DWORD			flags;
};

/*******************************************************************************
**
** Function: 
**		WTW_PROTO_FUNC_ENUM
**
** Return value: 
**			on error:
**				E_OUTOFMEMORY, E_INVALIDARG 
**			on success:
**				> 0 (number of protocols is returned)
**
** Parameters:
**		wP - wtwProtocolInfo*
**		lP - sizeofarray(wP) or -1 if we want to know total protocol count
**
*/

#define WTW_PROTO_FUNC_ENUM		L"WTW/ProtocolEnum" 

/*******************************************************************************
**
** Function: 
**		WTW_PROTO_FUNC_GET
**
** Return value: 
**		on error:
**				E_*
**		on success:
**				S_OK
**
** Parameters:
**		wP - (const wchar_t*) - GUID
**		lP - (wtwProtocolInfo*)
**
*/

#define WTW_PROTO_FUNC_GET		L"WTW/ProtocolGet"		// wP - wchar_t* - GUID, lP - wtwProtocolInfo*; zwraca E_*/S_OK

/*******************************************************************************
**
** Function: 
**		WTW_PROTO_FUNC_GET_GUID_EX
**
** Return value: 
**		on error:
**				E_*
**		on success:
**				S_OK
**
** Parameters:
**		wP - (wtwProtocolInfo*) - with netClass and netId set
**		lP - (wtwBuffer*)		- with info about allocated pointer with enough space to hold the GUID
**
*/

#define WTW_PROTO_FUNC_GET_GUID_EX	L"WTW/ProtocolGetGUIDEx"

struct wtwProtocolEvent 
{
	__wtwStdStructDefs(wtwProtocolEvent);

	int				structSize;

	const wchar_t * netClass;		// protocol ID
	int				netId;			// protocol Second ID

	int				event;			// rodzaj zdarzenia
	int				type;			// typ

	wtwContactDef * pContactData;			// jesli s¹ dostepne sparsowane informacje o kontakcie inaczej NULL
	const wchar_t * metaGUID;		// jest kontakt jest w metakontakcie to tu jest id meta, jesli nie - null;
};


/*******************************************************************************
**
** Event types
**		Types come in packages, if one have _BEFORE type it has also _AFTER;
** 
** Remarks:
**		Events marked as _BEFORE can be blocked, if first plugin in chain will block 
**		the event, rest of the plugins will not receive it and _AFTER will not be fired;
**		This type of event should be used if plugin wants to change something/reject 
**		whole event;
**		Type _AFTER cannot be blocked and is "for information only", it MUST NOT be blocked;
**		_UNDEFINED behaves retty much like _AFTER; 
**
*/

#define	WTW_PEV_TYPE_BEFORE		0x0001 // przed przetworzeniem, mozna zablokowac zwracajac 1
#define	WTW_PEV_TYPE_AFTER		0x0002 // po przetworzeniu
#define	WTW_PEV_TYPE_UNDEFINED	0x0000 // zjawisko nie majace typu

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_RAW_DATA_RECV - data received from the network
**		WTW_PEV_RAW_DATA_SEND - data about to send into the network
**
** Should return: 
**		S_OK if it's type is _AFTER or _UNDEFINED, or S_OK/S_FALSE if it's
**		type is "_BEFORE";
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - wtwRawDataDef*
**
*/

#define WTW_PEV_RAW_DATA_RECV	0x0001
#define WTW_PEV_RAW_DATA_SEND	0x0002

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_PRESENCE_RECV - contact presence info received
**		WTW_PEV_PRESENCE_SEND - our presence info before send
**
** Should return: 
**		S_OK if it's type is _AFTER or _UNDEFINED, or S_OK/S_FALSE if it's
**		type is "_BEFORE";
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - wtwPresenceDef*
**
*/

#define WTW_PEV_PRESENCE_RECV	0x0003 
#define WTW_PEV_PRESENCE_SEND	0x0004 

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_MESSAGE_RECV - message received
**		WTW_PEV_MESSAGE_SEND - message is about to be sent
**
** Should return: 
**		S_OK if it's type is _AFTER or _UNDEFINED, or S_OK/S_FALSE if it's
**		type is "_BEFORE";
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - wtwMessageDef*
**
*/

#define WTW_PEV_MESSAGE_RECV	0x0005
#define WTW_PEV_MESSAGE_SEND	0x0006

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_NETWORK_LOGIN  - network logged in
**		WTW_PEV_NETWORK_LOGOUT - guess
**
** Should return: 
**		S_OK (type is always _UNDEFINED);
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - wtwPresenceDef* or NULL
**
*/

#define WTW_PEV_NETWORK_LOGIN	0x0008
#define WTW_PEV_NETWORK_LOGOUT	0x0009

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_AUTH_REQUEST  - an contact requests presence auth
**
** Should return: 
**		S_OK if it's type is _AFTER, or S_OK/S_FALSE if it's
**		type is "_BEFORE";
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - wtwMessageDef* or NULL
**
*/

#define WTW_PEV_AUTH_REQUEST	0x000A

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_ROSTER_DLOADED  - contact list has been downloaded
**
** Should return: 
**		S_OK (type is always _UNDEFINED);
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - NULL
**
*/

#define WTW_PEV_ROSTER_DLOADED	0x000B

#define WTW_PEV_XML_PROCESSED	0x0100	// it's blockable, it's internal, don't use ;)

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_ALERT_RECV
**		WTW_PEV_ALERT_SEND
**
** Should return: 
**		S_OK if it's type is _AFTER, or S_OK/S_FALSE if it's
**		type is "_BEFORE";
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - NULL
**
*/

#define WTW_PEV_ALERT_RECV			0x000D
#define WTW_PEV_ALERT_SEND			0x000F

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_TYPING_NOTIFY_RECV - guess
**		WTW_PEV_TYPING_NOTIFY_SEND - 
**
** Should return: 
**		S_OK if it's type is _AFTER, or S_OK/S_FALSE if it's
**		type is "_BEFORE";
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - NULL
**
*/

#define WTW_PEV_TYPING_NOTIFY_SEND	0x0010
#define WTW_PEV_TYPING_NOTIFY_RECV	0x000E

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_AVATAR_RECV - avatar received
**		WTW_PEV_AVATAR_SEND - (our) avatar is about to be sent
**
** Should return: 
**		S_OK if it's type is _AFTER, or S_OK/S_FALSE if it's
**		type is "_BEFORE";
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - wtwAvatarEvent*
**
*/

#define WTW_PEV_AVATAR_RECV			0x0011
#define WTW_PEV_AVATAR_SEND			0x0012

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_NEW_MAIL_NOTIFY - new mail notify
**
** Should return: 
**		S_OK if it's type is _AFTER or _UNDEFINED, or S_OK/S_FALSE if it's
**		type is "_BEFORE";
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - NULL
**
*/

#define WTW_PEV_NEW_MAIL_NOTIFY		0x0013

#define WTW_PEV_FILE_TRANSFER_REQ	0x0014 // lP - wtwProtocolFile*

struct wtwProtocolFile
{
	__wtwStdStructDefs(wtwProtocolFile);

	int				structSize;

	const wchar_t * fileName;	// moze byc NULL
	QWORD			fileSize;
};

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_CALL_RING	- incoming/outgoing call
**
** Should return: 
**		S_OK if it's type is _AFTER, or S_OK (user decides)/ S_FALSE (reject) if it's
**		type is "_BEFORE";
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - wtwProtocolCall*
**
** Remarks, flow:
**		_RING, _ACCEPT and maybe _END
**		if accepted: _START, _END
**		_END / _START can be called multiple times. For example when video stream can be started independly of audio stream;
**
*/

#define WTW_PEV_CALL_RING		0x0015

struct wtwProtocolCall
{
	__wtwStdStructDefs(wtwProtocolCall);

	int				structSize;

	DWORD			flags;
	
	DWORD			iContacts;
	wtwContactDef * pContacts;

	const wchar_t * callUniqueId;	// may be NULL

};

#define WTW_PROTOCOL_CALL_FLAG_AUDIO		0x0001
#define WTW_PROTOCOL_CALL_FLAG_VIDEO		0x0002
#define WTW_PROTOCOL_CALL_FLAG_INCOMING		0x0004	// call, not stream
#define WTW_PROTOCOL_CALL_FLAG_OUTGOING		0x0008	// call, not stream
#define WTW_PROTOCOL_CALL_FLAG_AUTOACCEPT	0x0100	// set it if you want to accept the call on users's behalf
#define WTW_PROTOCOL_CALL_FLAG_ACCEPTED		0x0200
#define WTW_PROTOCOL_CALL_FLAG_REJECTED		0x0400
#define WTW_PROTOCOL_CALL_FLAG_BLOCKED		0x0800
#define WTW_PROTOCOL_CALL_FLAG_ERROR		0x1000

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_CUSTOM - plugin custom event
**
** Should return: 
**		General scheme is: S_OK if it's type is _AFTER or _UNDEFINED, 
**		or S_OK/S_FALSE if it's type is "_BEFORE";
**		But it's all plugin dependent;
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - wtwProtocolCustomEvent *
**
*/

#define WTW_PEV_CUSTOM		0x0020

struct wtwProtocolCustomEvent
{
	__wtwStdStructDefs(wtwProtocolCustomEvent);

	int				structSize;

	const wchar_t * eventName;
	
	void *			lP;	// variable, usually depends on eventName and protocol source
	void *			wP; // as above
};


/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_CALL_START		- call has been accepted
**		WTW_PEV_CALL_END		- call ended
**		WTW_PEV_CALL_ACCEPT		- call was accepted/rejected
**
** Should return: 
**		S_OK, type is always "undefined"
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - wtwProtocolCall*
**
*/

#define WTW_PEV_CALL_ACCEPT		0x0016
#define WTW_PEV_CALL_START		0x0017
#define WTW_PEV_CALL_END		0x0018

/*******************************************************************************
**
** Hook event: 
**		WTW_PEV_RTT_EVENT_*	- real-time text event
**
** Should return: 
**		S_OK if it's type is _AFTER, or S_OK (user decides)/ S_FALSE (reject) if it's
**		type is "_BEFORE";
**
** Parameters:
**		wP - wtwProtocolEvent* (as in case of all protocol events)
**		lP - wtwRTTInfo*
**
** Remarks:
**		No need to send this event from event plugins, core will send notifies
**		when WTW_PF_SETUP_TN_RTT will be called
**
*/

#define WTW_PEV_RTT_EVENT_SEND	0x0019
#define WTW_PEV_RTT_EVENT_RECV	0x001A


HANDLE __inline wtwInstProtoFunc(
						WTWFUNCTIONS *pF,			// interface pointer
						const wchar_t * netClass,	// protocol class
						int				netId,		// protocol id
						const wchar_t * funcName,	// function name
						WTWFUNCTION		funcAddr,	// pointer to function, if it's not obvious
						void *			funcOwnerData
						)
{

	wchar_t sFuncName[255] = {0};
	swprintf(sFuncName, sizeof(sFuncName) / sizeof(wchar_t), L"%s/%d/%s", netClass, netId, funcName);
	return pF->fnCreate(sFuncName, funcAddr, funcOwnerData);

}

/*******************************************************************************
**
** Plugin protocol functions: 
**		Functions that should be registered via wtwInstProtoFunc, if plugin
**		supports it. For example: if we support protocol states
**		we should register WTW_PF_STATUS_SET and WTW_PF_STATUS_GET,
**		this will allow core and other plugins to change account/protocol status
**
** Should return: 
**		If everything is ok, S_OK, if something went wrong S_FALSE
**
*/

#define WTW_PF_STATUS_SET		L"PFStatusSet"		// Core (or plugin) will call it if it want to change account status
													// wP - wtwPresenceDef*
#define WTW_PF_STATUS_GET		L"PFStatusGet"		// Core (or plugin) will call it to obtain current account/protocol status
													// wP - wtwPresenceDef*
#define WTW_PF_MESSAGE_SEND		L"PFMessageSend"	// Core (or plugin) want to send a message (normal or chat)
													// wP - wtwMessageDef*
#define WTW_PF_ALERT_SEND		L"PFAlertSend"		// ?
#define WTW_PF_INIT_CALL_AUDIO	L"PFCallInitAudio"	// Initiate audio call
													// wP - wtwContactDef*
#define WTW_PF_INIT_CALL_VIDEO	L"PFCallInitVideo"	// Initiate video call
													// wP - wtwContactDef*
#define WTW_PF_END_CALL			L"PFCallEnd"		// End call
													// wP - wtwContactDef*

#define WTW_PF_RAW_DATA_SEND	L"PFRawSend"		// Send raw data do server
													// wP - wtwRawDataDef*
#define WTW_PF_RAW_DATA_FEED	L"PFRawFeed"		// Process raw data as it came from server
													// wP - wtwRawDataDef*

#define WTW_PF_TN_SEND			L"PFTNSend"			// wP - wtwContactDef*
													// lP - WTW_TYPING_STATUS_*
#define WTW_PF_TN_SEND_RTT		L"PFTNSendRTT"		// wP - wtwContactDef*
													// lP - wtwRTTInfo*

struct wtwRTTInfo
{
	__wtwStdStructDefs(wtwRTTInfo);
	int				structSize;

	union
	{
		const wchar_t * text;	// in case of _START, _ADD and _FINAL flags
		DWORD			param;	// in case of _BACKSPACE flag
	};

	DWORD			flags;
	
};




#define WTW_PF_GET_CT_SPEC_INFO		L"PFGetSpecInfo"	// get contact specyfic info, max img size, or smth

struct wtwProtocolContactSpecInfo
{
	__wtwStdStructDefs(wtwProtocolContactSpecInfo);

	int				structSize;

	union
	{
		int			iValue;
		void *		pValue;
		wchar_t *	sValue;
	};

	int				valueSize;
	DWORD			infoLevel;

	wtwContactDef	*pContact;					// for what contact? depending on implementation and level may be required or not
	DWORD			flags;
};

#define WTW_SPEC_INFO_LEVEL_IMAGESIZE	0x0001	// iValue will contain max img size in kiB that can be transferred




#define WTW_RTT_FLAG_START		0x0001
#define WTW_RTT_FLAG_FINAL		0x0002
#define WTW_RTT_FLAG_RESET		0x0004

#define WTW_RTT_FLAG_ADD		0x0010 // not used in send function call, wtw always send current full text version
#define WTW_RTT_FLAG_BACKSPACE	0x0020 // not used in send function call, wtw always send current full text version

#define WTW_TYPING_STATUS_STOP		0x00
#define WTW_TYPING_STATUS_START		0x01
#define WTW_TYPING_STATUS_PAUSE		0x02

#define WTW_PF_ASK_FOR_AUTH		L"WTW/PFAskForAuth" // wP - wtwContactDef*

#define WTW_PF_INIT_SEND_FILE	L"PFInitSendFile"

struct wtwPicture 
{
	__wtwStdStructDefs(wtwPicture);

	int				structSize;

	wtwContactDef	contactData;
	
	const wchar_t * fullFilePath;
};

#define SIS_OK					 0		// ok
#define SIS_ERROR				-1		// other error
#define SIS_FILE_TOO_BIG		-2		// file is too large
#define SIS_NO_CONNECTION		-3		// service not connected
#define SIS_NOT_SUPPORTED		-4		// contact does not support file reception
#define SIS_FILE_ERROR			-5		// file access error

#define WTW_PF_SEND_PICT		L"PFSendPicture"		// wP - wtwPicture*

// ret - 0 - blad jakis, 1 - ok, 2 - za duzy obraz

#define WTW_PF_SEND_IMGH		L"PFSendPictureByURL"

#define WTW_PF_DELETE_CONTACT	L"PFDeleteContact"	// wP - wtwContactDef*
#define WTW_PF_MOVE_CONTACT		L"PFMoveContact"	// wP - wtwContactDef, lP - wtwMoveContactDef*


#include "wtwVCard.h"

/*******************************************************************************
**
** Init functions: 
**		If You dont register functions from this group, default implementation
**		will be called. If You register function, your implementation will be 
**		called, regardles of function result;
**
** Example: 
**		If user doubleclick contact on contact list, Core checks if it's
**		plugin/protocol registered WTW_PF_INIT_CHAT function. If not, then
**		it opens chat window. If protocol function was registered, then it 
**		calls;
**
** Parameters:
**		wP - wtwContactInfo *
**		lP - NULL
**
*/

#define WTW_PF_INIT_CHAT		L"PFInitChat"
#define WTW_PF_INIT_MESSAGE		L"PFInitSendMessage"
#define WTW_PF_INIT_EDIT		L"PFInitEdit"
#define WTW_PF_INIT_PUBDIR		L"PFInitPubDir"
#define WTW_PF_INIT_ADD			L"PFInitAdd"
#define WTW_PF_INIT_SEND_PICT	L"PFInitSendPicture"


//////////////////////////////////////////////////////////////////////
//
// dlaczego funkcja a nie hookiem?
// bo hook i tak zostanei wywolany automatycznie, ale ta funkcja jest
// do czegos jeszcze, "karmi" ona komunikator danymi, jesli wiadomosc 
// nie zostanie odrzucona w TYPE_BEFORE to w TYPE_AFTER zostanie
// przetrawiona przez wtw (czyli wyswietlona, etc ...)
//
// wParam - wtwProtoEventDef *
// lParam - w zaleznosci co to za event ...
//
///

/*******************************************************************************
**
** Function WTW_PF_CALL_HOOKS: 
**		Use this function to notify Core and other plugins about protocol
**		events. Core processes received messages, presence states, etc.
**		Protocol console receives raw data to display via this function;
**
** Parameters:
**		wP - wtwProtocolEvent *
**		lP - event dependent, can be NULL
**
*/

#define WTW_PF_CALL_HOOKS		L"WTW/CallProtolHooks" //

//////////////////////////////////////////////////////////////////////
//
// W ramach informacji, jesli chcemy ustawic globalnie cos dla
// wszystkich protokolow to mozemy zrobic tu
//
///

/*******************************************************************************
**
** Function WTW_PF_WTW_STATUS_*: 
**		Functions for setting/retrieving global status
**
** Parameters:
**		wP - wtwPresenceDef *
**		lP - NULL
**
** Returns:
**		HRESULT (S_OK, E_*)
**
*/


#define WTW_PF_WTW_STATUS_GET	L"WTW/PFStatusGet"		// wP - wtwPresenceDef*
#define WTW_PF_WTW_STATUS_SET	L"WTW/PFStatusSet"		// wP - wtwPresenceDef*

/*******************************************************************************
**
** Function WTW_SET_CONTACT_TYPING_NOTIFY: 
**		Sets typing notify status for given contact, "typing" status is 
**		automatically cancelled after about 20secs
**
** Parameters:
**		wP - wtwContactDef *
**		lP - WTW_TYPING_STATUS_*
**
** Returns:
**		HRESULT (S_OK, E_*)
**
*/

#define WTW_SET_CONTACT_TYPING_NOTIFY	L"WTW/PFTNSet"

#define WTW_TYPING_STATUS_TYPING	1
#define WTW_TYPING_STATUS_INACTIVE	0

/*******************************************************************************
**
** Function: 
**		WTW_PF_WTW_SETUP_TN_RTT	- receive real-time text 
**
** Should return: 
**		S_OK
**		S_FALSE - no chat window, etc
**		E_ACCESSDENIED - event was blocked in protocol hooks
**		E_*
**
** Parameters:
**		wP - wtwContactDef* 
**		lP - wtwRTTInfo*
**
** Remarks:
**		You should not call protocol hooks before or after calling this function.
**		Function will call all hooks by herself
**
*/

#define WTW_PF_WTW_SETUP_TN_RTT	L"WTW/PFTNSetRTT"

/*******************************************************************************
**
** Function: 
**		WTW_SEND_GLOB_MESSAGE - sends message via first available protocol   
**
** Should return: 
**		S_OK
**		E_*
**
** Parameters:
**		wP - wtwMessageDef* 
**		lP - NULL
**
** Remarks:
**		if (valid(contactData.id) && valid(contactData.netClass) && valid(contactData.netId))
**		{
**			if (isInMetaContact(contactData))
**			{
**				return sendViaFirsAvailableAccountInMeta();
**			}
**			else
**			{
**				return sendViaSelectedProtocol();
**			}
**		}
**		else if (valid(contactData.uuid))
**		{
**			return sendViaFirsAvailableProtocolInMeta();
**		}
**		else if (valid(contactData.id) && valid(contactData.netClass))
**		{
**			return sendViaFirstAvailableProtocolOfNetClass();
**		}
**
*/

#define WTW_SEND_GLOB_MESSAGE L"WTW/PFSendMessage"

/*******************************************************************************
**
** Function: 
**		WTW_PF_GET_PROTOCOL_COUNT - how many protocols/accounts are registered
**
** Should return: 
**		S_OK	- no protocols
**		> 0		- protocol count
**		E_*		- error
**
** Parameters:
**		wP - NULL
**		lP - NULL
**
*/

#define WTW_PF_GET_PROTOCOL_COUNT	L"WTW/PFGetCount"

/*******************************************************************************
**
** Function: 
**		WTW_PF_GET_PROTOCOL_*PTR - internal, do not use
**
*/

#define WTW_PF_GET_PROTOCOL_IPTR	L"WTW/PFGetIPtr"
#define WTW_PF_GET_PROTOCOL_GPTR	L"WTW/PFGetGPtr"

/*******************************************************************************
**
** Event: 
**		WTW_ON_BEFORE_STATUS_SET - fired before global status change
**
** Should return: 
**		S_OK	- OK
**		S_FALSE	- block it
**
** Parameters:
**		wP - wtwNewGlobalStatus*
**		lP - NULL
**
*/

#define WTW_ON_BEFORE_STATUS_SET	L"WTW/BeforeDescSet"

typedef void (*_tReplaceDesc)(const wchar_t *pNewDesc, void *pWtwNewGlobalStatus);

struct wtwNewGlobalStatus
{
	__wtwStdStructDefs(wtwNewGlobalStatus);

	int				structSize;
	wtwPresenceDef	newStatus;

	_tReplaceDesc	replaceFunct;
	void *			pInternals;		// internal pointer, do not access
};

#endif