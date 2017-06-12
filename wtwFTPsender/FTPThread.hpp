/*
 * Copyright (C) 2009-2017 adrian_007, adrian-007 on o2 point pl
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef FTP_THREAD_HPP
#define FTP_THREAD_HPP

#include <string>
#include <deque>

#include <windows.h>
#include "Thread.hpp"
#include "ftpclient/FTPClient.h"

class FTPSender : public Thread, private nsFTP::CFTPClient::CNotification {
public:	
	struct Contact {
		std::wstring id;
		std::wstring netID;
		int netSID;
	};

	void setServerInfo();
	void addToQueue(const Contact& c, const std::wstring& filePath);

	static void ShowToolTip(const std::wstring& caption, const std::wstring& msg);
private:
	class FileData {
	public:
		FileData(const Contact& c, const std::wstring file);

		void sendUrl();
		void sendMessage(const std::wstring& msg, bool sendToContact);
		std::wstring getContactName() const;

		std::wstring fileName;
		std::wstring filePath;
		std::wstring remotePath;
		std::wstring url;

		Contact cnt;
		wtwMessageDef lMsg;

	private:
		FileData(const FileData&);
	};

	void OnPreSendFile(const tstring& /*strSourceFile*/, const tstring& /*strTargetFile*/, long /*lFileSize*/);
	void OnPostSendFile(const tstring& /*strSourceFile*/, const tstring& /*strTargetFile*/, long /*lFileSize*/);
	void OnBytesSent(const nsFTP::TByteVector& /*vBuffer*/, long /*lSentBytes*/);
	void OnInternalError(const tstring& /*strErrorMsg*/, const tstring& /*strFileName*/, DWORD /*dwLineNr*/);
	void OnResponse(const nsFTP::CReply& /*Reply*/);
	void OnSendCommand(const tstring& /*strCommand*/);

	FileData* currentFile;
	typedef std::deque<FileData*> RequestQ;
	void threadMain();

	std::wstring srv;
	std::wstring username;
	std::wstring passwd;
	unsigned short port;
	unsigned int timeout;
	bool isPassive;

	long sentBytes;
	long fileSize;
	wchar_t tmpBuff[4096];

	RequestQ queue;
	CriticalSection cs;
};

#endif
