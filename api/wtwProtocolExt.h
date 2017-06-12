/*
** Copyright (C) 2007-2014, K2T.eu
*/

#ifndef __wtw__protocol_ext_h__
#define __wtw__protocol_ext_h__

#include "wtwProtocol.h"

struct wtwGgExtInfoV1 
{
	__wtwStdStructDefs(wtwGgExtInfoV1);

	int		structSize;

	DWORD	features;
	DWORD	ipAddress;
	DWORD	imageSize;
	DWORD	ipPort;
	DWORD	flags;
};

struct wtwTlenExtInfoV1 
{
	__wtwStdStructDefs(wtwTlenExtInfoV1);

	int				structSize;

	int				possiblyHidden;
	const wchar_t * avatarHash;
};

struct wtwXMPPExtInfoV1 
{
	__wtwStdStructDefs(wtwXMPPExtInfoV1);

	int				structSize;

	int				priority;
	const wchar_t * avatarHash;
	const wchar_t * lang;
};

struct wtwXMPPExtMsgInfoV1 
{
	__wtwStdStructDefs(wtwXMPPExtMsgInfoV1);

	int				structSize;

	int							extInfoCount;
	const wtwMessageExtInfoEx * extInfo;
	
	const void *	reserved1;
	DWORD			reserved2;
};


struct wtwGGIMToken		// as wP in wtwProtocomCustomEvent in "imtoken" event for GG network
{
	__wtwStdStructDefs(wtwGGIMToken);

	int				structSize;

	const wchar_t *	imToken;
	const wchar_t * uin;
};

struct wtwGGBirthInfo
{
	__wtwStdStructDefs(wtwGGBirthInfo);

	int				structSize;

	const wchar_t *	uin;
	QWORD			birthDate;
	const wchar_t *	rawBirthDate;
};

#endif