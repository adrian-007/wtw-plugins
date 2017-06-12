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

#ifndef API_DATA_SOCKET_HPP
#define API_DATA_SOCKET_HPP

class ApiDataSocket
{
public:
	static bool trySend(WTWFUNCTIONS* fp, const wchar_t* netClass, int netId, const char* data, int len, int flags) {
		return tryData(fp, netClass, netId, data, len, flags, true);
	}

	static bool tryRecv(WTWFUNCTIONS* fp, const wchar_t* netClass, int netId, const char* data, int len, int flags) {
		return tryData(fp, netClass, netId, data, len, flags, false);
	}

private:
	static bool tryData(WTWFUNCTIONS* fp, const wchar_t* netClass, int netId, const char* data, int len, int flags, bool outgoing) {
		wtwProtocolEvent ev;
		initStruct(ev);

		ev.event = outgoing ? WTW_PEV_RAW_DATA_SEND : WTW_PEV_RAW_DATA_RECV; 
		ev.netClass = netClass;
		ev.netId = netId; 

		wtwRawDataDef rd;
		initStruct(rd);
		rd.pData = data;
		rd.pDataLen = len;
		rd.flags = flags;

		ev.type = WTW_PEV_TYPE_BEFORE; 
		if(fp->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&rd)) == 0)
		{
			ev.type = WTW_PEV_TYPE_AFTER; 
			fp->fnCall(WTW_PF_CALL_HOOKS, reinterpret_cast<WTW_PARAM>(&ev), reinterpret_cast<WTW_PARAM>(&rd));
			return true;
		}
		return false;
	}
};

#endif
