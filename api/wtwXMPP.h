/*
** Copyright (C) 2007-2014, K2T.eu
*/

#ifndef __wtw__xmpp_pep_h__
#define __wtw__xmpp_pep_h__

#include "wtwPlugin.h"
#include "wtwContact.h"

#define XMPP_PEP_EVENT_TYPE_USERTUNE	0x0001
#define XMPP_PEP_EVENT_TYPE_USERMOOD	0x0002

struct xmppPepEvent 
{
	__wtwStdStructDefs(xmppPepEvent);

	int				structSize;

	wtwContactDef	accountInfo;
	wtwContactDef	fromUser;

	int				eventType;
};

struct xmppPepEventSet 
{
	__wtwStdStructDefs(xmppPepEventSet);

	int				structSize;
	int				setType;

	WTWFUNCTION		callback;	// not implemented, ale jakby kiedys bylo to: wP - 1, serwer zwrocil ok, 0 - serwer zwrocil err
	void			*od;
};

struct xmppPepUserMood 
{
	__wtwStdStructDefs(xmppPepUserMood);

	int				structSize;

	const wchar_t * mood;
	const wchar_t * text;

	const wchar_t * id;
};

struct xmppPepUserTune 
{
	__wtwStdStructDefs(xmppPepUserTune);

	int				structSize;

	const wchar_t * track;
	const wchar_t * artist;
	const wchar_t * title;
	const wchar_t * source;
	const wchar_t * uri;
	const wchar_t * length;

	const wchar_t * id;
};

#define XMPP_PEP_EVENT_ARRIVED L"XMPP/OnPEPEvent" // event globalny;
#define XMPP_PFE_PEP_PUBLISH_EVENT L"PFEPublishData" // funkcja, zywana tak jak inne funkcje protoko³ów czyli w po³aczeniu z NetID i NetSID, 0 - bl¹d, 1 - ok

#endif