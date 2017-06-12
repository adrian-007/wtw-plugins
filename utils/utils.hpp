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

#ifndef WTW_UTILS_HPP
#define WTW_UTILS_HPP

#include <string>
#include <map>

namespace utils {
	class conv {
	public:
		static std::string getIpAsString(unsigned int ip) {
			in_addr a = { 0 };
			a.S_un.S_addr = ip;
			return inet_ntoa(a);
		}

		static unsigned int getIpAsInt(const std::string& ip) {
			return inet_addr(ip.c_str());
		}

		static std::string toString(int n) {
			char buf[32] = { 0 };
			_snprintf_s(buf, sizeof(buf), sizeof(buf)-1, "%d", n);
			return buf;
		}

		static std::string toString(unsigned int n) {
			char buf[32] = { 0 };
			_snprintf_s(buf, sizeof(buf), sizeof(buf)-1, "%u", n);
			return buf;
		}

		static std::string toString(__int64 n) {
			char buf[64] = { 0 };
			_snprintf_s(buf, sizeof(buf), sizeof(buf)-1, "%I64d", n);
			return buf;
		}

		static std::string toString(unsigned __int64 n) {
			char buf[64] = { 0 };
			_snprintf_s(buf, sizeof(buf), sizeof(buf)-1, "%I64u", n);
			return buf;
		}

		static std::wstring toStringW(int n) {
			wchar_t buf[32] = { 0 };
			_snwprintf_s(buf, sizeof(buf), sizeof(buf)-1, L"%d", n);
			return buf;
		}

		static std::wstring toStringW(unsigned int n) {
			wchar_t buf[32] = { 0 };
			_snwprintf_s(buf, sizeof(buf), sizeof(buf)-1, L"%u", n);
			return buf;
		}

		static std::wstring toStringW(__int64 n) {
			wchar_t buf[64] = { 0 };
			_snwprintf_s(buf, sizeof(buf), sizeof(buf)-1, L"%I64d", n);
			return buf;
		}

		static std::wstring toStringW(unsigned __int64 n) {
			wchar_t buf[64] = { 0 };
			_snwprintf_s(buf, sizeof(buf), sizeof(buf)-1, L"%I64u", n);
			return buf;
		}

		static inline std::string toString(unsigned short n) {
			return toString((unsigned int)n);
		}

		static inline std::string toString(short n) {
			return toString((int)n);
		}

		static inline std::wstring toStringW(unsigned short n) {
			return toStringW((unsigned int)n);
		}

		static inline std::wstring toStringW(short n) {
			return toStringW((int)n);
		}
	};
} // namespace utils

#endif
