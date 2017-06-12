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

#ifndef WTW_REMOTE_IMAGES_HPP
#define WTW_REMOTE_IMAGES_HPP

#define SERVICE_PROVIDER L"imgur.com"

class wtwRemoteImages
{
	static wtwRemoteImages* inst;
public:
	wtwRemoteImages(HINSTANCE hInstance);
	~wtwRemoteImages();

	static void create(wtwRemoteImages* m)
	{
		wtwRemoteImages::inst = m;
	}

	static void destroy()
	{
		delete wtwRemoteImages::inst;
		wtwRemoteImages::inst = 0;
	}

	static wtwRemoteImages* instance()
	{
		return wtwRemoteImages::inst;
	}

	static void showToolTip(const std::wstring& caption, const std::wstring& msg);

private:
	struct itemData
	{
		itemData() : netId(-1) { }

		std::string buffer;

		std::wstring id;
		std::wstring netClass;
		int netId;
	};

	static WTW_PTR externalFuncSendFile(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr);
	static WTW_PTR externalFuncQueryInfo(WTW_PARAM lParam, WTW_PARAM wParam, void* ptr);

	static WTW_PTR MenuCallback(WTW_PARAM wParam, WTW_PARAM lParam, void*);
	static WTW_PTR MenuRebuild(WTW_PARAM wParam, WTW_PARAM lParam, void*);

	void sendImage(const wchar_t* contactId, const wchar_t* netClass, int netId, const wchar_t* filePath);
	void onRequestComplete(const wchar_t* fileId, wtwHttpStatus* status, itemData* data);

	static WTW_PTR onHttpService_callback(WTW_PARAM wParam, WTW_PARAM lParam, void* ptr)
	{
		wtwHttpEvent* event = (wtwHttpEvent*)wParam;
		itemData* pBuffer = (itemData*)ptr;

		if(event == 0 || pBuffer == 0)
		{
			return 0;
		}

		switch(event->event)
		{
		case WTW_HTTP_EVENT_DATA:
			{
				wtwRawDataDef* data = (wtwRawDataDef*)lParam;

				if(data && data->pData && data->pDataLen > 0)
				{
					pBuffer->buffer.append(std::string(data->pData, data->pDataLen));
				}

				break;
			}
		case WTW_HTTP_EVENT_COMPLETE:
			{
				inst->onRequestComplete(event->fileId, (wtwHttpStatus*)lParam, pBuffer);

				if(pBuffer)
				{
					delete pBuffer;
				}

				break;
			}
		default:
			{
			}
		}

		return 0;
	}

	void getFile(wtwContactDef* c);
	void wtwRemoteImages::sendMessage(const wchar_t* id, const wchar_t* netClass, int netId, const std::wstring& msg, bool sendToContact);

	HINSTANCE hInstance;
	
	HANDLE extFuncHandle;
	HANDLE extFuncHandleFallback;
	HANDLE extFuncQueryInfo;

	HANDLE onMenuRebuild;
};

#endif // WTW_REMOTE_IMAGES_HPP

