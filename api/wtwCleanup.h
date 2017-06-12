#pragma once

#include "wtwPlugin.h"

// profile cleanup mechanisms

#define WTW_CLEANUP_REGISTER_FUNC	L"WTW/CleanupRegister"
#define WTW_CLEANUP_UNREGISTER_FUNC	L"WTW/CleanupUnRegister"

struct wtwCleanupMech
{
	__wtwStdStructDefs(wtwCleanupMech);

	int				structSize;

	const wchar_t *	name;
	const wchar_t *	id;
	const wchar_t * cleanupMessage;
	const wchar_t * iconId;
	const wchar_t * groupName;

	WTWFUNCTION		callback;	// WARNING: This function MUST BE thread safe it will be called from NON-GUI thread
	void *			cbData;

	DWORD			flags;

	void *			ownerData;
};

struct wtwCleanupEvent
{
	__wtwStdStructDefs(wtwCleanupEvent);

	int				structSize;

	int				event;
	const wchar_t * id;
	void *			ownerData;

	QWORD			fsSize;
};

#define WTW_CLEANUP_EVENT_GET_SIZE	0x0001
#define WTW_CLEANUP_EVENT_PRECONFIG	0x0002
#define WTW_CLEANUP_EVENT_CLEANUP	0x0003