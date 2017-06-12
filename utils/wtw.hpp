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

#ifndef WTW_T_HPP
#define WTW_T_HPP

#ifndef _WIN32
#error Platform not supported or windows.h not included before wtw.hpp
#endif // _WIN32

#ifndef WTW_PARAM
#error You must include WTW API headers before wtw.hpp
#endif // WTW_PARAM

#include <string>
#include <vector>
#include <algorithm>

#ifdef WTW_USE_UUID_GENERATOR
#include <rpc.h>
#pragma comment(lib, "rpcrt4.lib")
#endif

#include "singleton.hpp"

namespace wtw_t {
	inline bool preloadIcon(WTWFUNCTIONS* pFunc, HINSTANCE hInst, const wchar_t* iconId, const wchar_t* fileName, int resourceId = -1) {
		wtwGraphics icon;
		initStruct(icon);
		icon.graphId = iconId;
		icon.imageType = 0;
		icon.hInst = hInst;

		icon.flags = WTW_GRAPH_FLAG_RELATIVE_DEF_PATH;
		icon.filePath = fileName;

		HRESULT ret = static_cast<HRESULT>(pFunc->fnCall(WTW_GRAPH_LOAD, reinterpret_cast<WTW_PARAM>(&icon), 0));
		
		if(SUCCEEDED(ret) == false && resourceId != -1)
		{
			icon.resourceId = MAKEINTRESOURCEW(resourceId);
			icon.filePath = 0;
			icon.flags = 0;
			pFunc->fnCall(WTW_GRAPH_LOAD, reinterpret_cast<WTW_PARAM>(&icon), 0);

			ret = 1;
		}
		return ret != 0;
	}

#ifdef WTW_USE_UUID_GENERATOR
	inline std::wstring generateUUID() {
		std::wstring buf;
		UUID id;
		if(UuidCreate(&id) == RPC_S_OK) {
			RPC_WSTR p = 0;
			if(UuidToStringW(&id, &p) == RPC_S_OK) {
				buf = (LPWSTR)p;
				RpcStringFree(&p);
			}
		}
		return buf;
	}
#endif

	class api : public singleton<api>, public WTWFUNCTIONS {
	public:
		api(WTWFUNCTIONS* pFunc, HINSTANCE hInstance = 0) : WTWFUNCTIONS(*pFunc), singleton<api>(), hInst(hInstance) {
			singleton<api>::init(this);
		}

		HINSTANCE instance() {
			return hInst;
		}

		WTW_PTR call(const wchar_t* func) {
			return this->fnCall(func, 0, 0);
		}

		WTW_PTR call(const wchar_t* func, WTW_PARAM _1) {
			return this->fnCall(func, _1, 0);
		}

		WTW_PTR call(const wchar_t* func, WTW_PARAM _1, WTW_PARAM _2) {
			return this->fnCall(func, _1, _2);
		}

		template<typename T1>
		WTW_PTR call(const wchar_t* func, T1 _1) {
			return this->fnCall(func, (WTW_PARAM)(_1), 0);
		}

		template<typename T1, typename T2>
		WTW_PTR call(const wchar_t* func, T1 _1, T2 _2) {
			return this->fnCall(func, (WTW_PARAM)(_1), (WTW_PARAM)(_2));
		}

		void hookAttach(HANDLE h) {
			hooks.push_back(h);
		}

		void hookDetach(HANDLE h) {
			std::remove(hooks.begin(), hooks.end(), h);
		}

		void hookClearAll() {
			for(HookVector::iterator i = hooks.begin(); i != hooks.end(); ++i) {
				this->evUnhook(*i);
			}
			hooks.clear();
		}

	private:
		typedef std::vector<HANDLE> HookVector;
		friend class singleton<api>;
	
		~api() {
			hookClearAll();
		}
	
		HookVector hooks;
		HINSTANCE hInst;
	};

	class paths {
	public:
		static void getPath(WTWFUNCTIONS* pFunc, int type, std::wstring& outbuf, bool relative = false) {
			wtwDirectoryInfo di;

			if(relative)
			{
				di.flags |= WTW_DIRECTORY_FLAG_RELPATH;
			}
			else
			{
				di.flags |= WTW_DIRECTORY_FLAG_FULLPATH;
			}

			di.dirType = type;
			di.bi.bufferSize = -1;

			if(pFunc->fnCall(WTW_GET_DIRECTORY_LOCATION, di, 0) == S_OK && di.bi.bufferSize > 0)
			{
				di.bi.pBuffer = new wchar_t[di.bi.bufferSize];

				if(pFunc->fnCall(WTW_GET_DIRECTORY_LOCATION, di, 0) == S_OK)
				{
					outbuf = di.bi.pBuffer;
				}

				delete[] di.bi.pBuffer;
			}
		}
	};
} // namespace wtw

#endif // WTW_T_HPP
