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

#ifndef DNS_LOOKUP_HPP
#define DNS_LOOKUP_HPP

#include <ws2tcpip.h>
#include <deque>

class DNSLookup 
{
public:
	bool setDomain(const std::string& domain);
	const std::string& getDomain() const;
	
	std::string front();
	void pop_front();
	
	size_t size() const;
	void clear();

private:
	void lookup();

	typedef std::deque<std::string> Queue;
	Queue ips;

	std::string domain;
};

#endif
