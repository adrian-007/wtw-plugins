/*
*** Copyright (C) 2007-2014, K2T.eu
*/

#pragma once

#include "..\wtwPlugin.h"
#include "core.h"
#include "..\wtwContactList.h"
#include "BaseObject.h"
#include "String.h"

namespace wtw 
{
	/*
		ALL INTERFACE FUNCTIONS MUST BE THREAD SAFE!
	*/

	class LIBWTW_API CArchiveInterface abstract
	{
	public:
		struct archiveCallback
		{
			__wtwStdStructDefs(archiveCallback);
			int structSize;

			int ev;
			HRESULT result;
			__int64 threadId;
		};

		enum
		{
			CURRENT_INTERFACE_VERSION = 1
		};

		virtual int getInterfaceVersion() = 0;

	public:
		struct archOptimizeOptions
		{
			__wtwStdStructDefs(archOptimizeOptions);

			int structSize;

			int bRemoveDuplicates;
			int bRemoveOrphansFromIndex;
			int bRemoveOrphansFromData;
			int bDeleteArchive;
		};

		struct archUser
		{
			wtw::CString name;
			wtw::CString id;
			wtw::CString netClass;
		};

		struct archEntry
		{
			wtw::CString	text;
			wtw::CString	extraData;
			wtw::CString	subject;
			DWORD			flags;
			time_t			time;
			archUser		contact;
		};

		struct archListEntry
		{
			wtw::CString	str[8];
			time_t			date;
			QWORD			count;
		};

		struct archMarkerEntry
		{
			wtw::CString	label;
			archUser		contact;

			QWORD			id;
			time_t			date;
			QWORD			chatId;
		};

		enum
		{
			ARCH_FLAG_ENCRYPTED = 0x0001,
			ARCH_FLAG_REMOTE = 0x0002,
		};


		enum
		{
			COLUMN_CHAT_ID = 0,
			COLUMN_CHAT_LOGIN = 1,
			COLUMN_CHAT_NAME = 2,
			COLUMN_CHAT_DATE = 3,
			COLUMN_CHAT_OTHER = 4,
		};

		enum
		{
			COLUMN_CONFERENCE_ID = 0,
			COLUMN_CONFERENCE_NAME = 1,
			COLUMN_CONFERENCE_DATE = 2,
			COLUMN_CONFERENCE_OTHER = 3,
		};

		enum
		{
			COLUMN_MESSAGES_ID = 0,
			COLUMN_MESSAGES_LOGIN = 1,
			COLUMN_MESSAGES_NAME = 2,
			COLUMN_MESSAGES_DATE = 3,
			COLUMN_MESSAGES_OTHER = 4,
			COLUMN_MESSAGES_EXTRA = 5,
		};

		enum
		{
			COLUMN_SMS_ID = 0,
			COLUMN_SMS_LOGIN = 1,
			COLUMN_SMS_NAME = 2,
			COLUMN_SMS_DATE = 3,
			COLUMN_SMS_OTHER = 4,
			COLUMN_SMS_EXTRA = 5,
		};

	public:
		virtual HRESULT initArchive() = 0;
		virtual HRESULT optimize(const archOptimizeOptions &opts) = 0;
		virtual HRESULT shutdown() = 0;
		virtual HRESULT getArchFlags(DWORD & flags) = 0;
		virtual HRESULT removeMessagesOlderThan(const int days) = 0;
		virtual HRESULT triggerWrite() = 0;

	public:
		virtual HRESULT largeOperationBegin() = 0;
		virtual HRESULT largeOperationCommit() = 0;
		virtual HRESULT largeOperationAbort() = 0;

	public:
		virtual HRESULT conferenceDelete(const __int64 id) = 0;
		virtual HRESULT conferenceInsertMessage(
			__int64 & confId, const QWORD & windowId,
			const wtwMessageDef &pMsg, 
			WTWFUNCTION fn, void *od) = 0;
		virtual HRESULT conferenceGetList(
			std::deque <archListEntry> &listDeck,
			const wtw::CString & textFilter,
			const time_t & timeBegin, const time_t & timeEnd, 
			std::deque<archMarkerEntry> *pMarkers
			) = 0;
		virtual HRESULT conferenceGetUserList(
			std::deque <archUser> & data
			) = 0;
		virtual HRESULT conferenceDeleteMarker(const unsigned __int64 id) = 0;
		virtual HRESULT conferenceGet(const __int64 id, std::deque<archEntry> & list) = 0;

	public:
		virtual HRESULT chatDelete(const __int64 id) = 0;
		virtual HRESULT chatInsert(
			__int64 & chatId, const QWORD & windowId,
			const wtwMessageDef &pMsg, 
			WTWFUNCTION fn, void *od) = 0;
		virtual HRESULT chatInsertMarker(
			const __int64 & chatId, const QWORD & windowId,
			const wtw::CString & label,
			WTWFUNCTION fn, void *od) = 0;
		virtual HRESULT chatGetList(
			std::deque <archListEntry> &list,
			std::deque <archUser> *contactFilter,
			const wtw::CString & textFilter,
			const time_t & timeBegin, const time_t & timeEnd, 
			std::deque<archMarkerEntry> *pMarkers
			) = 0;
		virtual HRESULT chatGetUserList(
			std::deque <archUser> & data
			) = 0;
		virtual HRESULT chatDeleteMarker(const __int64 & id) = 0;
		virtual HRESULT chatGet(const __int64 & id, std::deque<archEntry> & list) = 0;
		virtual HRESULT chatGetEnd(const std::deque<archUser> & from, std::deque<archEntry> &list, int count, int hLimit) = 0;

	public:
		virtual HRESULT smsDelete(const __int64 &id) = 0;
		virtual HRESULT smsInsert(
			const wtwMessageDef &pMsg, const wtw::CString & extraData, 
			const DWORD flags,
			WTWFUNCTION fn, void *od) = 0;
		virtual HRESULT smsGetList(
			std::deque <archListEntry> &listDeck, 
			const wtw::CString & filterPhone, const wtw::CString & filterText,
			const time_t & timeBegin, const time_t & timeEnd) = 0;
		virtual HRESULT smsGet(const __int64 & id,
			archEntry &e
			) = 0;
		virtual HRESULT smsGetUserList(
			std::deque <archUser> & data
			) = 0;

	public:
		virtual HRESULT messageDelete(const __int64 & id) = 0;
		virtual HRESULT messageInsert(
			const wtwMessageDef &pMsg,
			WTWFUNCTION fn, void *od) = 0;
		virtual HRESULT messageList(
			std::deque <archListEntry> &listDeck,
			const std::deque <archUser> *contactFilter,
			const wtw::CString & textFilter,
			const time_t & timeBegin, const time_t & timeEnd
			) = 0;
		virtual HRESULT messageGet(const __int64 & id,
			archEntry &e
			) = 0;
		virtual HRESULT messageGetUserList(
			std::deque <archUser> & data
			) = 0;
	};
}