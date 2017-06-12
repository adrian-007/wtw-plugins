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

#ifndef WTW_PASTEBIN
#define WTW_PASTEBIN

class wtwPastebin
{
	static wtwPastebin* inst;
public:
	struct pasteItem
	{
		std::wstring format;
		std::wstring expire;
		bool priv;

		std::string data;
	};

	wtwPastebin(HINSTANCE hInstance);
	~wtwPastebin();

	static void create(wtwPastebin* m);
	static void destroy();
	static wtwPastebin* instance();

	void send(const wchar_t* id, const wchar_t* netClass, int netId, const pasteItem& pi);

private:
	struct item 
	{
		item()
		{
		}

		void sendUrl(const std::wstring& url);
		void sendMessage(const std::wstring& msg, bool sendToContact);
		std::wstring getContactName() const;

		std::wstring id;
		std::wstring netClass;
		int netId;

		std::wstring dataBuffer;
	};

	static void ShowToolTip(const std::wstring& caption, const std::wstring& msg);

	static WTW_PTR externalFuncSendFile(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr);
	static WTW_PTR MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void*);
	static WTW_PTR MenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void*);
	static WTW_PTR onHttpService_callback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr);

	void getFile(wtwContactDef* c);

	HINSTANCE hInstance;
	HANDLE extFuncHandle;
	HANDLE onMenuRebuild;
};

#endif
