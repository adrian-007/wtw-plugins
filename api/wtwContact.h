/*
** Copyright (C) 2007-2014, K2T.eu
*/

#ifndef __wtw__ct_h__
#define __wtw__ct_h__

#include "wtwPlugin.h"

struct wtwResourceInfo 
{
	__wtwStdStructDefs(wtwResourceInfo);

	int				structSize;

	const wchar_t * resource;		// dla wiadomosci, czy czegos co ma sam zasob oraz presence

	const wchar_t * description;	// do presence, o ile jest ustawion
	int				status;			// jw.
	int				priority;		// jw.
	QWORD			presenceTime;	// jw.
	const wchar_t * note;			// XEP 0310, or smth
};

struct wtwContactDef 
{
	__wtwStdStructDefs(wtwContactDef);

	int				structSize;

	const wchar_t * uuid;			// UUID kontaktu do ktorego przypisany jest kontakt,
									// wymagane tylko jesli operujemy na globalnych kontaktach
									// a nie na ich elementach

	const wchar_t * id;				// id (my@wtw.im)
	const wchar_t * netClass;		// net ID (XMPP)
	int				netId;			// net SID (to co zwrocil komunikator przy AddProtocol)

	DWORD			flags;

	const wchar_t * pName;			// used internally, w wiekszosci wypadkow null

	wtwResourceInfo * exResource;

};


#endif