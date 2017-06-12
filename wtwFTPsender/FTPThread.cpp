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

#include "stdinc.hpp"

#include "FTPThread.hpp"

#include <ctime>
#include <map>

typedef std::map<std::wstring, std::wstring> StringMap;

void FTPSender::setServerInfo() {
	cfgGetStr(srv, SETTING_FTP_ADDRESS);
	cfgGetStr(username, SETTING_FTP_USERNAME);
	cfgGetStr(passwd, SETTING_FTP_PASSWORD);
	port = cfgGetInt(SETTING_FTP_PORT, 21);
	timeout = cfgGetInt(SETTING_FTP_TIMEOUT, 30);
	isPassive = cfgGetInt(SETTING_FTP_PASSIVE, 1) != 0;
}

void FTPSender::addToQueue(const Contact& c, const std::wstring& filePath) {
	FTPSender::FileData* fd = new FTPSender::FileData(c, filePath);
	if(fd->filePath.empty()) {
		delete fd;
		return;
	}

	cs.lock();
	queue.push_back(fd);

	if(isThreadRunning() == false) {
		runThread();
	}
	cs.unlock();
}

void FTPSender::ShowToolTip(const std::wstring& caption, const std::wstring& msg) {
	wtwTrayNotifyDef nt;
	initStruct(nt);
	nt.notifierId = L"wtwFTPsender";
	nt.textMain = caption.c_str();
	nt.textLower = msg.c_str();
	nt.iconId = PLUGIN_ICON;
	nt.graphType = WTW_TN_GRAPH_TYPE_SKINID;
	nt.flags = WTW_TN_FLAG_TXT_MULTILINE | WTW_TN_FLAG_HDR_MULTILINE;
	wtw->fnCall(WTW_SHOW_STANDARD_NOTIFY, reinterpret_cast<WTW_PARAM>(&nt), 0);
}

void FTPSender::threadMain() {
	nsFTP::CFTPClient client(nsSocket::CreateDefaultBlockingSocketInstance(), timeout);
	nsFTP::CLogonInfo info(srv, port, username, passwd);

	sentBytes = 0;
	currentFile = 0;

	client.AttachObserver(this);
	client.Login(info);

	cs.lock();
	FileData* fd = queue.front();
	queue.pop_front();
	cs.unlock();

	while(fd) {
		currentFile = fd;
		if(client.UploadFile(fd->filePath, fd->remotePath, false, nsFTP::CRepresentation(nsFTP::CType::Image()), isPassive)) {
			fd->sendUrl();
		} else {
			FTPSender::ShowToolTip(L"FTP Sender", L"Wyst�pi� b��d podczas wysy�ania pliku!");
		}

		delete fd;
		fd = 0;

		cs.lock();
		if(!queue.empty()) {
			fd = queue.front();
			queue.pop_front();
		}
		cs.unlock();
	}

	currentFile = 0;

	client.Logout();
	client.DetachObserver(this);
	/*HINTERNET hInt = wtwGetInet(wtw);

	cs.lock();
	FileData* fd = queue.front();
	queue.pop_front();
	cs.unlock();

	__LOG_F(wtw, 1, L"wtwFTP", L"connecting to %s (user: %s)", srv.c_str(), username.c_str());
	HINTERNET hConn = InternetConnect(hInt, srv.c_str(), port, username.c_str(), passwd.c_str(), INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE, 0);

	while(fd && hConn) {
		if(FtpPutFile(hConn, fd->filePath.c_str(), fd->remotePath.c_str(), FTP_TRANSFER_TYPE_BINARY, 0)) {
			cs.lock();
			fd->sendUrl();
		} else {
			DWORD err = GetLastError();
			std::wstring errMsg;
			errMsg += L"Wysy�anie pliku ";
			errMsg += fd->filePath;
			errMsg += L" nie powiod�o si�!";

			cs.lock();
			fd->sendMessage(errMsg, false);
			FTPSender::ShowToolTip(L"FTP Sender", L"Wys�anie pliku do " + fd->getContactName() + L" nie powiod�o si�!");
			
			__LOG_F(wtw, 1, L"wtwFTP", L"sending file failed: file = %s, error code = %d", fd->filePath.c_str(), err);
		}
		delete fd;
		fd = 0;
		if(!queue.empty()) {
			fd = queue.front();
			queue.pop_front();
		}
		cs.unlock();
	}

	if(hConn) {
		InternetCloseHandle(hConn);
		hConn = 0;
	}*/
}

/** file data impl */
FTPSender::FileData::FileData(const Contact& c, const std::wstring file) {
	cnt = c;
	initStruct(lMsg);
	lMsg.contactData.id = cnt.id.c_str();
	lMsg.contactData.netClass = cnt.netID.c_str();
	lMsg.contactData.netId = cnt.netSID;

	filePath = file;

	fileName;
	std::wstring::size_type i = file.rfind(L"\\");
	if(i != std::wstring::npos) {
		fileName = file.substr(i+1);
	} else {
		filePath.clear();
		FTPSender::ShowToolTip(L"FTP Sender", L"Podana �cie�ka do pliku jest niew�a�ciwa");
		return;
	}
	
	{
		time_t rawtime;
		tm* timeinfo;
		wchar_t buffer[4096] = { 0 };

		time(&rawtime);
		timeinfo = localtime(&rawtime);

		std::wstring format;
		cfgGetStr(format, SETTING_FTP_FILENAME_FORMAT, L"%d.%m.%y_%H.%M.%S_#filename#");
		wcsftime(buffer, sizeof(buffer), format.c_str(), timeinfo);
		format = buffer;

		std::wstring::size_type i = 0;
		while((i = format.find(L"#filename#", i)) != std::wstring::npos) {
			format.replace(i, 10, fileName);
			i += fileName.length();
		}

		while((i = format.find(L"/", i)) != std::wstring::npos)
		{
			format[i] = L'_';
			++i;
		}

		fileName = format;
	}

	cfgGetStr(remotePath, SETTING_FTP_REMOTE_DIR);
	if(wcslen(remotePath.c_str()) > 0) {
		if(remotePath[remotePath.size()-1] != L'/')
			remotePath += L"/";
		remotePath += fileName;
	} else {
		FTPSender::ShowToolTip(L"FTP Sender", L"�cie�ka do katalogu na serwerze jest niepoprawna!");
		filePath.clear();
		return;
	}

	cfgGetStr(url, SETTING_FTP_REMOTE_DIR_URL);
	if(wcslen(url.c_str()) > 0) {
		if(url[url.size()-1] != L'/')
			url += L"/";
	} else {
		FTPSender::ShowToolTip(L"FTP Sender", L"Link do katalogu na serwerze jest niepoprawny!");
		filePath.clear();
		return;
	}

	std::wstring msg = L"[FTP] Przygotowywanie " + fileName + L" do wys�ania na serwer...";
	sendMessage(msg, false);

	{ //uri encode
		StringMap chars;
		chars[L" "] = L"%20";
		chars[L"!"] = L"%21";
		chars[L"\""] = L"%22";
		chars[L"#"] = L"%23";
		chars[L"$"] = L"%24";
		//chars[L"%"] = L"%25";
		chars[L"&"] = L"%26";

		for(StringMap::iterator it = chars.begin(); it != chars.end(); ++it) {
			i = 0;
			while((i = fileName.find(it->first)) != std::wstring::npos) {
				fileName.replace(i, 1, it->second);
				i += it->second.length();
			}
		}
	}

	url += fileName;
	{
		std::wstring format;
		cfgGetStr(format, SETTING_FTP_MSG_FORMAT, L"Link do pliku: #url#");
		std::wstring::size_type i = 0;
		while((i = format.find(L"#url#", i)) != std::wstring::npos) {
			format.replace(i, 5, url);
			i += url.length();
		}
		url = format;
	}
	//__LOG_F(wtw, 1, L"wtwFTP", L"preparing: local path = %s, remote path = %s, id = %s", filePath.c_str(), remotePath.c_str(), cnt.id.c_str());
}

void FTPSender::FileData::sendUrl() {
	bool send = cfgGetInt(SETTING_FTP_SEND_AS_INFO, 0) == 0;

	std::wstring msg;

	if(!send)
		msg += L"[FTP] ";
	msg += url;

	sendMessage(msg, send);
	if(send)
		FTPSender::ShowToolTip(L"FTP Sender", L"Link do pliku na serwerze FTP wys�any do " + getContactName());
	else
		FTPSender::ShowToolTip(L"FTP Sender", L"Link do pliku na serwerze FTP gotowy dla " + getContactName());
}

void FTPSender::FileData::sendMessage(const std::wstring& msg, bool sendToContact) {
	lMsg.msgMessage = msg.c_str();
	lMsg.msgTime = time(0);

	if(sendToContact) {
		wchar_t buf[1024] = { 0 };
		wsprintf(buf, L"%s/%d/%s", cnt.netID.c_str(), cnt.netSID, WTW_PF_MESSAGE_SEND);
	
		lMsg.msgFlags = WTW_MESSAGE_FLAG_CHAT_MSG | WTW_MESSAGE_FLAG_OUTGOING;
		if(!wtw->fnCall(buf, reinterpret_cast<WTW_PARAM>(&lMsg), 0)) {
			wtw->fnCall(WTW_CHATWND_SHOW_MESSAGE, reinterpret_cast<WTW_PARAM>(&lMsg), 0);
		}
	} else {
		lMsg.msgFlags = WTW_MESSAGE_FLAG_CHAT_MSG | WTW_MESSAGE_FLAG_INFO;
		wtw->fnCall(WTW_CHATWND_SHOW_MESSAGE, reinterpret_cast<WTW_PARAM>(&lMsg), 0);
	}
}

std::wstring FTPSender::FileData::getContactName() const {
	wtwContactDef c;
	initStruct(c);
	c.id = cnt.id.c_str();
	c.netClass = cnt.netID.c_str();
	c.netId = cnt.netSID;

	WTW_PTR h = 0;
	
	wtw->fnCall(WTW_CTL_CONTACT_FIND_EX, (WTW_PARAM)&c, (WTW_PARAM)&h);
	
	if(h) {
		wtwContactListItem item;
		initStruct(item);
		wtw->fnCall(WTW_CTL_CONTACT_GET, h, (WTW_PARAM)&item);
		return item.itemText;
	} else {
		return cnt.id;
	}
}

void FTPSender::OnPreSendFile(const tstring& /*strSourceFile*/, const tstring& /*strTargetFile*/, long lFileSize) {
	fileSize = lFileSize;
}

void FTPSender::OnPostSendFile(const tstring& /*strSourceFile*/, const tstring& /*strTargetFile*/, long /*lFileSize*/) {
	fileSize = sentBytes = 0;
}

void FTPSender::OnBytesSent(const nsFTP::TByteVector& vBuffer, long lSentBytes) {
	sentBytes += lSentBytes;
	
	if(cfgGetInt(SETTING_FTP_SHOW_NOTIFY, 1) == 0) {
		return;
	}

	const wchar_t* msg = L"(nieznany)";
	
	if(currentFile != 0) {
		msg = currentFile->fileName.c_str();
	}
	swprintf(tmpBuff, L"%s: %.2f %%", msg, (sentBytes / (double)fileSize) * 100.0);

	FTPSender::ShowToolTip(L"Wysy�anie pliku na serwer FTP...", tmpBuff);
}

void FTPSender::OnInternalError(const tstring& strErrorMsg, const tstring& /*strFileName*/, DWORD /*dwLineNr*/) {
	FTPSender::ShowToolTip(L"B��d", strErrorMsg.c_str());
}

void FTPSender::OnResponse(const nsFTP::CReply& r) {
	__LOG_F(wtw, WTW_LOG_LEVEL_DEBUG, L"FTPI", L"%s", r.Value().c_str());
}

void FTPSender::OnSendCommand(const tstring& strCommand) {
	__LOG_F(wtw, WTW_LOG_LEVEL_DEBUG, L"FTPO", L"%s", strCommand.c_str());
}
