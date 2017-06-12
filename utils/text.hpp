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

#ifndef TEXT_HELPER_HPP
#define TEXT_HELPER_HPP

namespace utils {
	class text {
	public:
		static std::wstring toWide(const std::string& str, unsigned int codePage = CP_UTF8)
		{
			std::wstring ret;
			if(str.empty() == false)
			{
				int size = MultiByteToWideChar(codePage, 0, str.c_str(), -1, 0, 0);
				if(size > 0)
				{
					ret.resize(size);
					size = MultiByteToWideChar(codePage, 0, str.c_str(), str.size(), &ret[0], (int)ret.size());

					if(size <= 0)
					{
						ret.clear();
					}
					else
					{
						ret.resize(size);
					}
				}
			}
			return ret;
		}

		static std::string fromWide(const std::wstring& str, unsigned int codePage = CP_UTF8)
		{
			std::string ret;
			if(str.empty() == false)
			{
				int size = WideCharToMultiByte(codePage, 0, str.c_str(), -1, 0, 0, 0, 0);
				if(size > 0)
				{
					ret.resize(size);
					size = WideCharToMultiByte(codePage, 0, str.c_str(), str.size(), &ret[0], (int)ret.size(), 0, 0);

					if(size <= 0)
					{
						ret.clear();
					}
					else
					{
						ret.resize(size);
					}
				}
			}
			return ret;
		}

		static void trimSpaces(std::wstring& str) {
			while(str.length() > 0 && ::iswspace(str[0]) != 0)
			{
				str.erase(0, 1);
			}

			while(str.length() > 0 && ::iswspace(str[str.length() - 1]) != 0)
			{
				str.erase(str.length()-1, 1);
			}
		}

		/*static std::string urlEscape(const std::string& c) {
			std::string escaped = "";
			size_t max = c.length();
			for(size_t i = 0; i < max; ++i) {
				if ( (48 <= c[i] && c[i] <= 57) ||//0-9
					 (65 <= c[i] && c[i] <= 90) ||//abc...xyz
					 (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
					 (c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'')
				) {
					escaped.append( &c[i], 1);
				} else {
					escaped.append("%");
					escaped.append(char2hex(c[i]));//converts char 255 to string "ff"
				}
			}
			return escaped;
		}*/

		static std::string urlEscape(const std::string& str, bool escapeReserved = true)
		{
			std::string ret;
			char buf[64];
			char c;

			for(std::string::size_type i = 0; i < str.length(); ++i)
			{
				c = str[i];

				if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9'))
				{
					ret += c;
				}
				else
				{
					switch(c)
					{
					// unreserved, non-alanum chars
					case '-':
					case '_':
					case '.':
					case '~':
						{
							ret += c;
							break;
						}
					// reserved, optional
					case '!':
					case '*':
					case '\'':
					case '(':
					case ')':
					case ';':
					case ':':
					case '@':
					case '&':
					case '=':
					case '+':
					case '$':
					case ',':
					case '/':
					case '?':
					case '#':
					case '[':
					case ']':
						{
							if(escapeReserved == false)
							{
								ret += c;
								break;
							}
						}
					default:
						{
							sprintf(buf, "%%%02X", static_cast<unsigned char>(c));
							ret += buf;
						}
					}
				}
			}

			return ret;
		}

		static std::wstring urlEscapeW(const std::wstring& c)
		{
			return toWide(urlEscape(fromWide(c)));
		}

		static void urlUnescape(std::string& s)
		{
			replaceAll<std::string>("&gt;", ">", s);
			replaceAll<std::string>("&lt;", "<", s);
			replaceAll<std::string>("&nbsp;", " ", s);
			replaceAll<std::string>("&quot;", "\"", s);
			replaceAll<std::string>("&amp;", "&", s);
		}

		static void urlUnescapeW(std::wstring& s)
		{
			replaceAll<std::wstring>(L"&gt;", L">", s);
			replaceAll<std::wstring>(L"&lt;", L"<", s);
			replaceAll<std::wstring>(L"&nbsp;", L" ", s);
			replaceAll<std::wstring>(L"&quot;", L"\"", s);
			replaceAll<std::wstring>(L"&amp;", L"&", s);
		}

		template<typename T>
		static void replaceAll(const T& what, const T& with, T& str) {
			T::size_type i = 0;

			while((i = str.find(what, i)) != T::npos) {
				str.replace(i, what.length(), with);
				i += with.length();
			}
		}

		template<typename T>
		static std::wstring stripFromTags(const T& start, const T& end, const T& str, long offset = 0)
		{
			T::size_type i = 0, j = 0;
			i = str.find(start, offset);
			j = str.find(end, offset);

			if(i != T::npos && j != T::npos)
			{
				i += start.length();
				return str.substr(i, j-i);
			}
			return T();
		}

	private:
		/*static std::string char2hex(char dec) {
			char dig1 = (dec&0xF0)>>4;
			char dig2 = (dec&0x0F);
			if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
			if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
			if ( 0<= dig2 && dig2<= 9) dig2+=48;
			if (10<= dig2 && dig2<=15) dig2+=97-10;

			std::string r;
			r.append( &dig1, 1);
			r.append( &dig2, 1);
			return r;
		}*/
	};
} // namespace utils

#endif
