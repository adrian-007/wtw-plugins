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

#ifndef XML_WRITER_HPP
#define XML_WRITER_HPP

#include <string>
#include <list>

namespace xml {
	namespace writer {
		class element {
			std::string tag;
			std::list<element> childs;
			int level;

			typedef std::list<std::pair<std::string, std::string> > AttribList;
			AttribList attribs;
			std::string data;

		public:
			element(const std::string& tag) : level(0) {
				this->tag = tag;
			}

			element& getChild(const std::string& tag) {
				childs.push_back(element(tag));
				childs.back().level = level + 1;
				return childs.back();
			}

			element& addAttrib(const std::string& name, int value) {
				char buf[128] = { 0 };
				sprintf(buf, "%d", value);
				return addAttrib(name, buf);
			}

			element& addAttrib(const std::string& name, const std::string& value) {
				for(AttribList::iterator i = attribs.begin(); i != attribs.end(); ++i) {
					if(i->first == name) {
						i->second = value;
						return *this;
					}
				}

				std::string val = value.c_str();
				std::string::size_type i = 0;
				const char fmt[] = "<&>\"";

				while((i = val.find_first_of(fmt, i)) != std::string::npos) {
					switch(val[i]) {
						case '<': val.replace(i, 1, "&lt;"); i+=4; break;
						case '&': val.replace(i, 1, "&amp;"); i+=5; break;
						case '>': val.replace(i, 1, "&gt;"); i+=4; break;
						case '"': val.replace(i, 1, "&quot;"); i+=6; break;
						default: i++;
					}
				}

				attribs.push_back(std::make_pair(name, val));
				return *this;
			}

			element& addData(const std::string& data) {
				this->data += data.c_str();
				return *this;
			}

			void print(std::string& out, bool wellFormatted = true) const {
				std::string startTag, endTag;
				
				if(wellFormatted) {
					for(int i = 0; i < level; ++i) {
						startTag += "\t";
						endTag += "\t";
					}
				}

				if(attribs.empty() == false) {
					startTag += "<" + tag;
					for(AttribList::const_iterator i = attribs.begin(); i != attribs.end(); ++i) {
						startTag += " " + i->first + "=\"" + i->second + "\"";
					}

					if(data.empty() == false) {
						startTag += ">";
						startTag += data;
						startTag += "</" + tag + ">";
						if(wellFormatted)
						{
							endTag += "\n";
						}
					} else if(childs.empty()) {
						startTag += "/>";
						if(wellFormatted)
						{
							endTag += "\n";
						}
					} else {
						startTag += ">";
						endTag += "</" + tag + ">";
						if(wellFormatted)
						{
							startTag += "\n";
							endTag += "\n";
						}
					}
				} else {
					if(data.empty() == false) {
						startTag += "<" + tag + ">";
						startTag += data;
						startTag += "</" + tag + ">";
						if(wellFormatted)
						{
							endTag += "\n";
						}
					} else if(childs.empty()) {
						startTag += "<" + tag + "/>";
						if(wellFormatted)
						{
							endTag += "\n";
						}
					} else {
						startTag += "<" + tag + ">";
						endTag += "</" + tag + ">";
						if(wellFormatted)
						{
							startTag += "\n";
							endTag += "\n";
						}
					}
				}
				out += startTag;
				for(std::list<element>::const_iterator i = childs.begin(); i != childs.end(); ++i) {
					i->print(out, wellFormatted);
				}
				out += endTag;
			}

			std::string getUtf8Header() const {
				return "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
			}
		};
	}
}

#endif
