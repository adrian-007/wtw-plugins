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
#include "TXTRecord.hpp"
#include <dns_sd.h>

void TXTRecord::setTXTField(Field type, const std::string& value) {
	std::string name = getFieldNameAsString(type);
	txt_record[name] = value;
}

std::string TXTRecord::getTXTField(Field type) const {
	std::string name = getFieldNameAsString(type);
	std::string ret;

	StringMap::const_iterator i = txt_record.find(name);
	if(i != txt_record.end())
		ret = i->second;
	return ret;
}

void TXTRecord::remTXTField(Field type) {
	std::string name = getFieldNameAsString(type);

	StringMap::iterator i = txt_record.find(name);
	if(i != txt_record.end())
		txt_record.erase(i);
}

bool TXTRecord::isTXTField(Field type) const {
	std::string name = getFieldNameAsString(type);
	return txt_record.find(name) != txt_record.end();
}

std::string TXTRecord::getFieldNameAsString(Field type) const throw(...) {
	switch(type) {
			case FIRST_NAME:		{ return "1st"; }
			case LAST_NAME:			{ return "last"; }
			case NICK:				{ return "nick"; }
			case EMAIL:				{ return "email"; }
			case JABBER_ID:			{ return "jid"; }
			case STATUS:			{ return "status"; }
			case STATUS_MSG:		{ return "msg"; }
			case P2P_PORT:			{ return "port.p2pj"; }
			case NODE:				{ return "node"; }
			case TXTVERS:			{ return "txtvers"; }
			case HASH_TYPE:			{ return "hash"; }
			case HASH:				{ return "phsh"; }
			case VERSION:			{ return "ver"; }
			case VC:				{ return "vc"; }
			default:				{ throw std::exception("bad field type"); }
	}
	return std::string();
}

void TXTRecord::resetTXTRecord() {
	txt_record.clear();
}

const StringMap& TXTRecord::get() const {
	return txt_record;
}

void TXTRecord::set(const StringMap& record) {
	txt_record = record;
}

void TXTRecord::setRAWData(const unsigned char* data, unsigned short len) {
	const void* rData;
	unsigned char rLen;

	resetTXTRecord();
	std::string name;

	for(int i = (int)FIRST_NAME; i < (int)FIELD_LAST; ++i) {
		try {
			name = getFieldNameAsString((Field)i);

			if(TXTRecordContainsKey(len, data, name.c_str())) {
				rData = TXTRecordGetValuePtr(len, data, name.c_str(), &rLen);
				std::string val((const char*)rData, rLen);
				txt_record[name] = val;
			}
		} catch(...) {
			// ignore...
		}
	}
}
